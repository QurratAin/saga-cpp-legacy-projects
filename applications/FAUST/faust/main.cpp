/*
 *  main.cpp
 *  FAUST - Framework for Adaptive Ubiquitous Scalable Tasks
 *  Website: https://macpro01.cct.lsu.edu/trac/faust
 *
 *  Created by Ole Weidner <oweidner@cct.lsu.edu> on 11/22/08.
 *  Copyright 2008-2009 Center for Computation & Technology. 
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying 
 *  LICENSE file or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#include <iostream>
#include <faust/faust.hpp>

int main (int argc, char* argv[])
{
  std::vector<std::string> dir_ids, dir_path, dir_dev_space_total_cmd, env,
  dir_dev_space_used_cmd, dir_quota_total_cmd, dir_quota_used_cmd;
  
  // A directory description
  dir_ids.push_back("my_work_dir");
  dir_path.push_back("/home/oweidner");
  dir_dev_space_total_cmd.push_back("echo \"scale=0; `df . | awk '/\\// {print $2}'` * 512/1024/1024\" | bc");
  dir_dev_space_used_cmd.push_back("echo \"scale=0; `df . | awk '/\\// {print $3}'` * 512/1024/1024\" | bc");
  dir_quota_total_cmd.push_back("echo \"0\"");
  dir_quota_used_cmd.push_back("echo \"0\"");
  
  faust::resource_description localhost_rd;
  
  localhost_rd.set_attribute("identifier", "localhost");
  localhost_rd.set_attribute("faust_agent_submit_url",  "fork://localhost/");
  localhost_rd.set_attribute("faust_agent_binary_path", "/Users/oweidner/Work/FAUST/build/Debug/faust_agent");	
  localhost_rd.set_attribute("saga_root_path",          "/usr/local/saga-1.2.1/");
  
  localhost_rd.set_vector_attribute("dir_id", dir_ids);
  localhost_rd.set_vector_attribute("dir_path", dir_path);
  localhost_rd.set_vector_attribute("dir_dev_space_total_cmd", dir_dev_space_total_cmd);
  localhost_rd.set_vector_attribute("dir_dev_space_used_cmd", dir_dev_space_used_cmd);
  localhost_rd.set_vector_attribute("dir_quota_total_cmd", dir_quota_total_cmd);
  localhost_rd.set_vector_attribute("dir_quota_used_cmd", dir_quota_used_cmd);
  
  localhost_rd.write_to_file("/tmp/test.faust");
  
  faust::resource_description queenbee_rd;
  queenbee_rd.set_attribute("identifier", "queenbee.loni.org");

  
  queenbee_rd.set_attribute("identifier", "queenbee.loni.org");
  queenbee_rd.set_attribute("faust_agent_submit_url",  "gram://qb1.loni.org/jobmanager-fork");
  queenbee_rd.set_attribute("faust_agent_binary_path", "/work/oweidner/FAUST/agent/faust_agent");	
  queenbee_rd.set_attribute("saga_root_path",          "/work/oweidner/megajobs");
  
  env.push_back("LD_LIBRARY_PATH=/usr/local/compilers/GNU/gcc-4.2.0/lib64:/usr/local/packages/jdk1.6.0_06/lib:/usr/local/packages/mvapich-1.0-intel10.1/lib:/usr/local/compilers/Intel/intel_fc_10.1/lib:/usr/local/compilers/Intel/intel_cc_10.1/lib:/usr/local/compilers/Intel/mkl-10.0/lib/em64t:/home/packages/globus/globus-4.0.8-r2/lib:/work/oweidner/megajobs/lib/");
  queenbee_rd.set_vector_attribute("environment", env);
  
  while(1) 
  {
		try 
    {
      //// two new persitent faust instances
      //
			//faust::resource queenbee  (queenbee_rd, true);
			faust::resource localhost (localhost_rd, true);
      
      faust::resource_description rm_recx = localhost.get_description();
      std::vector<std::string> attr1x_ = rm_recx.list_attributes();
      
      faust::resource_monitor rm_rec2x = localhost.get_monitor();
      std::vector<std::string> attr2x_ = rm_rec2x.list_attributes();
      
							
			//// test reconnect
      //
			faust::resource localhost_reconnect(localhost_rd.get_attribute("identifier"));
			
      
      /*faust::resource_description localhost_rd_rec = localhost_reconnect.get_description();
			std::vector<std::string> attr_ = localhost_rd_rec.list_attributes();
			std::vector<std::string>::const_iterator it;
			for(it = attr_.begin(); it != attr_.end(); ++it)
			{
				std::cout << "localhost description attribute: " << (*it) << std::endl;
			}
      */
      
      faust::resource_description rm_rec = localhost_reconnect.get_description();
      std::vector<std::string> attr1_ = rm_rec.list_attributes();

      faust::resource_monitor rm_rec2 = localhost_reconnect.get_monitor();
      std::vector<std::string> attr2_ = rm_rec2.list_attributes();
      
      /*std::vector<std::string>::const_iterator it1;
			for(it1 = attr1_.begin(); it1 != attr1_.end(); ++it1)
			{
				std::cout << "monitor attribute: " << (*it1) << ": " << std::flush;
        if(rm_rec.attribute_is_vector(*it1)) {
          std::vector<std::string> tmp(rm_rec.get_vector_attribute(*it1));
          for(int i=0; i < tmp.size(); ++i)
            std::cout << tmp.at(i) << ", " << std::flush;
          std::cout << std::endl;
        }
        else
          std::cout << rm_rec.get_attribute(*it1) << std::endl;
			}*/
			
      //// test reconnect
      //
			faust::resource queenbee_reconnect(queenbee_rd.get_attribute("identifier"));
			//faust::resource_description queenbee_rd_rec = queenbee_reconnect.get_description();
			//std::vector<std::string> attr2_ = queenbee_rd_rec.list_attributes();
			//std::vector<std::string>::const_iterator it2;
			//for(it2 = attr2_.begin(); it2 != attr2_.end(); ++it2)
			//{
			//	std::cout << "queenbee attribute: " << (*it2) << std::endl;
			//}
       
			
			//queenbee_reconnect.set_persistent(false);
			localhost_reconnect.set_persistent(false);
		}
		catch(faust::exception const & e) {
			std::cout << "FAUST EXCEPTION: " << e.what() << std::endl;
			exit(1);
		}
		
  }
  return 0;
}
