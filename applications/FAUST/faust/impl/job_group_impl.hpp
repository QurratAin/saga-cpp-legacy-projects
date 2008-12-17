/*
 *  job_group_impl.hpp 
 *  FAUST - Framework for Adaptive Ubiquitous Scalable Tasks
 *
 *  Created by Ole Weidner on 11/30/08.
 *  Copyright 2008 Center for Computation & Technology. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying 
 *  LICENSE file or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#ifndef FAUST_IMPL_JOB_GROUP_IMPL_HPP
#define FAUST_IMPL_JOB_GROUP_IMPL_HPP

#include <saga/saga.hpp>

#include <faust/impl/object_impl.hpp>

#include <faust/faust/exports.hpp>
#include <faust/faust/job.hpp>
#include <faust/faust/state.hpp>
#include <faust/faust/description.hpp>

namespace faust
{
  namespace impl
  {
    class FAUST_EXPORT job_group_impl : public faust::impl::object
      {
        
      private:
        
        std::string jobid_;
        
      public:
        
        job_group_impl();
        void run();
        bool wait(double timeout = -1.0);
        bool cancel(double timeout = -1.0);
        bool suspend();
        bool resume();
        std::string get_job_id();
        std::vector<std::string> list_jobs();
      };
  }
}

#endif /* FAUST_IMPL_JOB_GROUP_IMPL_HPP */