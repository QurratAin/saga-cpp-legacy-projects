/*
 *  service_impl.cpp 
 *  FAUST - Framework for Adaptive Ubiquitous Scalable Tasks
 *
 *  Created by Ole Weidner on 11/30/08.
 *  Copyright 2008 Center for Computation & Technology. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying 
 *  LICENSE file or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#include <faust/faust/defines.hpp>
#include <faust/faust/manyjobs/job.hpp>
#include <faust/faust/manyjobs/job_group.hpp>
#include <faust/faust/manyjobs/resource.hpp>
#include <faust/faust/manyjobs/description.hpp>

#include <faust/impl/manyjobs/service_impl.hpp>

using namespace faust::impl::manyjobs;

////////////////////////////////////////////////////////////////////////////////
// CONSTRUCTOR
//
service_impl::service_impl (std::vector<faust::manyjobs::resource> resources, int num_jobs)
{
  using namespace saga::job;
  
  // Initialize log service for this instance
  std::string identifier(FW_NAME);
  identifier.append(" manyjobs::service");
  log_ = new detail::logwriter(identifier, std::cout);
  
  std::string msg("Starting new job servie instance." );
  log_->write(msg, LOGLEVEL_INFO);
  
  // check if the given hosts, queues, projects are valid!
  unsigned int fails = 0;
  
  std::vector<faust::manyjobs::resource>::iterator i;
  for(i = resources.begin(); i != resources.end(); ++i)
  {
    saga::url contact((*i).contact);
    saga::job::service sjs;
    
    // Try to initialize the job service - if it fails, remove this entry
    // from the list.
    try {
      std::string msg("Checking resource availability: " + contact.get_url());
      log_->write(msg, LOGLEVEL_INFO);
      
      sjs = saga::job::service(contact);
    }
    catch(saga::exception const & e)
    {
      ++fails;
      
      if(DEBUG) log_->write(e.what(), LOGLEVEL_DEBUG);
      std::string msg("Cannot connect with resource: " + contact.get_url());
      msg.append(". Removing entry from resource list." );
      log_->write(msg, LOGLEVEL_ERROR); 
      
      continue; // we don't want to try queueing if this stage already fails!
    }
    
    // Try to queue a sample dummy job to see if the queue and project 
    // informations a valid. If it fails, remove this entry from the list.
    try {
      std::string msg("Queuing sample job on: " + (*i).contact.get_url());
      msg.append(" (queue="+(*i).queue+", project="+(*i).project+")");
      log_->write(msg, LOGLEVEL_INFO);
      
      saga::job::description jd;
      jd.set_attribute (attributes::description_executable,  "/bin/date");
      jd.set_attribute (attributes::description_interactive, saga::attributes::common_false);
      jd.set_attribute (attributes::description_queue,       (*i).queue);
      std::vector<std::string> project; project.push_back((*i).project);
      jd.set_vector_attribute (attributes::description_job_project, project);
      
      saga::job::job j = sjs.create_job(jd);
      j.run(); 
      sleep(2); // GRAM needs some time otherwise it will reply with an error! 
      //std::cout << j.get_job_id() << std::endl;
      j.cancel();
    }
    catch(saga::exception const & e)
    {
      ++fails;
      
      if(DEBUG) log_->write(e.what(), LOGLEVEL_DEBUG);
      std::string msg("Cannot queue a sample job on: " + (*i).contact.get_url());
      msg.append(". Removing entry from resource list." );
      std::cout << e.what() << std::endl;
      log_->write(msg, LOGLEVEL_ERROR);  
    }
    
    // Host description seems to work properly. Add it to our
    // internal host list.
    resources_.insert(resources_pair((*i).contact.get_url(), (*i)));
  }
  
  // if we don't have any working execution hosts, abort!
  if(fails == resources.size()) {
    log_->write("No usable resources available. Aborting.", LOGLEVEL_FATAL); 
    exit(-1);
    // FATAL -> THROW Exception! 
  }
  
}

////////////////////////////////////////////////////////////////////////////////
// DESTRUCTOR
service_impl::~service_impl ()
{
  delete log_;
}

////////////////////////////////////////////////////////////////////////////////
// 
std::vector<std::string> service_impl::list_jobs(void)
{
  std::vector<std::string> job_ids;
  
  joblist_map::const_iterator ci;
  for(ci = joblist_.begin(); ci != joblist_.end(); ++ci)
  {
    job_ids.push_back(ci->first);
  }
  
  return job_ids;
}

////////////////////////////////////////////////////////////////////////////////
// 
faust::manyjobs::resource service_impl::get_resource(std::string contact)
{
  if(resources_.find(contact) == resources_.end())
  {
    // THROW SOME EXCEPTION
  }
  
  return resources_[contact];
}

////////////////////////////////////////////////////////////////////////////////
// 
std::vector<std::string> service_impl::list_resources(void)
{
  std::vector<std::string> res;
  
  resources_map::const_iterator ci;
  for(ci = resources_.begin(); ci != resources_.end(); ++ci)
  {
    res.push_back(ci->first);
  }
  
  return res;
}

////////////////////////////////////////////////////////////////////////////////
// 
faust::manyjobs::job_group 
service_impl::create_job_group(std::vector<faust::manyjobs::description> job_descs)
{
  faust::manyjobs::job_group ret;
  
  // joblist_.insert(joblist_pair(g.get_job_id(), g));
  std::string msg("Registering new job_group instance: " + ret.get_job_id());
  log_->write(msg, LOGLEVEL_INFO);
  
  return ret;
}

////////////////////////////////////////////////////////////////////////////////
// 
faust::manyjobs::job_group 
service_impl::create_job_group(std::vector<faust::manyjobs::description> job_descs, 
                               std::string dep_job_id, 
                               faust::manyjobs::state job_state)
{
  faust::manyjobs::job_group ret;
  
  // joblist_.insert(joblist_pair(g.get_job_id(), g));
  std::string msg("Registering new job_group instance: " + ret.get_job_id());
  log_->write(msg, LOGLEVEL_INFO);
  
  return ret;
}


////////////////////////////////////////////////////////////////////////////////
// 
faust::manyjobs::job
service_impl::create_job(faust::manyjobs::description job_descs)
{
  faust::manyjobs::job ret;
  
  joblist_.insert(joblist_pair(ret.get_job_id(), ret));
  std::string msg("Registering new job_group instance: " + ret.get_job_id());
  log_->write(msg, LOGLEVEL_INFO);
  
  return ret;
}

////////////////////////////////////////////////////////////////////////////////
// 
faust::manyjobs::job
service_impl::create_job(faust::manyjobs::description job_descs,
                         std::string dep_job_id, 
                         faust::manyjobs::state job_state)
{
  faust::manyjobs::job ret;
  
  joblist_.insert(joblist_pair(ret.get_job_id(), ret));
  std::string msg("Registering new job_group instance: " + ret.get_job_id());
  log_->write(msg, LOGLEVEL_INFO);
  
  return ret;
}

////////////////////////////////////////////////////////////////////////////////
// 
faust::manyjobs::job
service_impl::get_job(std::string job_id)
{
  
}

