
#ifndef SAGA_MANDELBROT_HPP
#define SAGA_MANDELBROT_HPP

#include <saga/saga.hpp>

#include "ini.hpp"
#include "client.hpp"
#include "output_base.hpp"
#include "job_starter.hpp"
#include "../version.hpp"


// this class encapsulates a master/worker paradigm.  We start
// the jobs, and distribute work items to them.

class mandelbrot 
{
  private:

    std::vector <output_base *>      odevs_;            // output devices

    std::string                      pwd_;              // working directory
    std::string                      ini_file_;         // control file
    mb_util::ini::ini                ini_;              // control data
    std::string                      job_bucket_name_;  // name of job directory
    saga::advert::directory          job_bucket_;       // advert container for new jobs 
    saga::advert::directory          done_bucket_;      // advert container for finished jobs 
    boost::shared_ptr <job_starter>  js_;               // client manager

    std::vector <boost::shared_ptr <client> > clients_; // saga jobs

    // jobs startup
    void job_startup    (void);
    void paint_it_black (saga::advert::entry ad,
                         std::string         msg);

    // all job buckets are created under that advert directory, by
    // appending the session uid and the client jobnum:
    // saga::advert::directory (<advert_dir>/<session_id>/<client_id>
    std::string advert_dir_;

    // mandelbrot algorithm parameters
    double plane_x_0_;
    double plane_y_0_;
    double plane_x_1_;
    double plane_y_1_;
    int limit_;
    int escape_;

    // work item definitions (box == work item)
    int img_size_x_;
    int img_size_y_;
    int box_size_x_;
    int box_size_y_;
    int box_num_x_;
    int box_num_y_;

    // flags
    bool debug_;

  public:
    mandelbrot   (void);
    ~mandelbrot  (void);

    int compute  (void);
};

#endif // SAGA_MANDELBROT_MANDELBROT_HPP

