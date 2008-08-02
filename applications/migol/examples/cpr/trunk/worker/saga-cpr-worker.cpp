/*
 *  saga-cpr-worker.cpp
 *  saga_applications
 *
 *  Created by luckow on 18.06.08.
 */

#include <saga/saga.hpp>
#include <saga/saga/cpr.hpp>
#include <boost/thread/xtime.hpp>
#include <boost/thread.hpp>
#include "boost/filesystem/operations.hpp"
#include "boost/filesystem/path.hpp"

#define CHECKPOINT_NAME "remd_checkpoint"
#define CHECKPOINT_CHK_INTERVALL 60
#define MAX_URL 255

namespace fs = boost::filesystem;

void run_application(std::string launch_command);
saga::url build_url(fs::path file);
bool check_file(fs::path file, std::vector<saga::url> urls);
std::string get_hostname(void);
void update_checkpoints(std::string checkpoint_dir);
void print_registered_checkpoints();

int main (int argc, char* argv[])
{
    std::cout<<"start saga-cpr-worker"<<std::endl;  
   
    if (argc < 3 || argc > 4){
        std::cout << "Usage: " << argv[0] << "\"<executable parameter>\" <jobtype> <checkpoint dir>"<<std::endl;
        std::cout << "Example:\n " << argv[0] << "\"/usr/local/packages/namd-2.6-mvapich-1.0-intel10.1/namd2 NPT.conf\" mpi /home/luckow/NAMD"<<std::endl;
        exit(1);
    }

    std::string exe = std::string(argv[1]);
    std::string job_type, pbs_nodefile, checkpoint_dir(".");
    if(argc==3){
        job_type = std::string(argv[2]);
        if(job_type=="MPI"||job_type=="mpi"){
            pbs_nodefile.assign(saga::safe_getenv("PBS_NODEFILE"));
        }
    }
    if(argc==4){
        checkpoint_dir = std::string(argv[3]);
    }
      
    // Init Migol/Monitoring
    // uses per default Application Information Service (AIS) configured in 
    // the $SAGA_LOCATON/share/saga/saga_adaptor_migol_cpr.ini
    // required for all subsequent CPR calls (otherwise exception is thrown)
    saga::cpr::service js;
        
    std::ostringstream command_stream;
    if (pbs_nodefile!="" && (job_type=="MPI" || job_type=="mpi")){
        command_stream <<"mpirun -machinefile "<<
        pbs_nodefile << " " << argv[1];
    } else {
        command_stream << argv[1];
        
    }
    //execute command in separate thread
    std::string command = command_stream.str();
    boost::thread application_thread(TR1::bind(run_application, command));
    
    // This loop monitors the application threads and checks whether new checkpoint 
    // files exist
    while(!application_thread.timed_join(boost::posix_time::seconds(CHECKPOINT_CHK_INTERVALL))) {    
        std::cout<<"Check for application thread..."<<std::endl;
        update_checkpoints(checkpoint_dir);        
    }        
    
    std::cout<<"finished saga-cpr-worker"<<std::endl;   
}


/** parse checkpoint directory for new files and register files with AIS **/
void update_checkpoints(std::string checkpoint_dir){
    unsigned long file_count = 0;
	unsigned long err_count = 0;
    
    //get current files from AIS
    saga::url url(CHECKPOINT_NAME);
    saga::cpr::checkpoint chkpt(url);
    std::vector<saga::url> lfns;
    lfns = chkpt.list_files();
    std::cout << "Received files: " <<std::endl;
    for (int i = 0; i < lfns.size(); i++)
    {
        std::cout << lfns[i] << std::endl;
    }
    std::cout<<"Checkpoint dir: " << checkpoint_dir <<std::endl;
    fs::path full_path(fs::system_complete( fs::path(checkpoint_dir, fs::native)));
    if (fs::is_directory(checkpoint_dir))
    {
        std::cout << "current files in " << checkpoint_dir << "\n";
        fs::directory_iterator end_iter;
        for (fs::directory_iterator dir_itr( full_path);
             dir_itr != end_iter;
             ++dir_itr )
        {
            try
            {
                if (fs::is_regular(dir_itr->status()) )
                {
                    ++file_count;
                    fs::path p = dir_itr->path();
                    bool exists = check_file(p, lfns);
                    std::cout << "File: " << p << " exists: "<< ((exists==0) ? "false":"true") <<"\n";
                    if(!exists){
                        saga::url u = build_url(p);
                        std::cout<< "Create file: " << u <<std::endl;;
                        chkpt.add_file(u);
                    } else {
                        SAGA_VERBOSE (SAGA_VERBOSE_LEVEL_INFO)
                        {
                        std::cout<< "No new checkpoint"<<std::endl;
                        }
                    }
                }
            } catch ( const std::exception & ex )
            {
                ++err_count;
                std::cout << dir_itr->path().leaf() << " " << ex.what() << std::endl;
            }
        }
    }
    //print_registered_checkpoints();
}

/* build gsisftp:// urls for referencing of checkpoint files */
saga::url build_url(fs::path file){
	saga::url url("gsiftp://" + get_hostname() + "/" + file.string());
	std::cout<<url.get_string();
	return url;
}

void print_registered_checkpoints(){
    saga::url url(CHECKPOINT_NAME);
    saga::cpr::checkpoint chkpt(url);
    std::vector<saga::url> files = chkpt.list_files();
    std::cout << "Received files: " <<std::endl;
    for (unsigned int i = 0; i < files.size(); i++)
    {
            std::cout << files[i] << std::endl;
    }    
}

/* runs application process */
void run_application(std::string launch_command){
    SAGA_VERBOSE (SAGA_VERBOSE_LEVEL_INFO)
    {
        std::cout<< "Start application: " << launch_command.c_str() <<std::endl;
    }
    system(launch_command.c_str());   
    SAGA_LOG_DEBUG("Finished application");
    
}

/** check whether file exists **/
bool check_file(fs::path file, std::vector<saga::url> urls){
    for (int i = 0; i < urls.size(); i++)
    {
        fs::path local_file = urls[i].get_path();       
        //std::cout << "AIS URL: " + local_file.string() << " Local URL: " << file.string() << std::endl;
        if(file==local_file){
            return true;
        }
    }
    return false;
}

/** get hostname **/
std::string get_hostname(void)
{
    char buffer[MAX_URL] = { '\0' };
    gethostname(buffer, sizeof(buffer));
    return std::string(buffer);
}
