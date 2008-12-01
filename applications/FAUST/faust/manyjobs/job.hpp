/*
 *  job.hpp
 *  FAUST - Framework for Adaptive Ubiquitous Scalable Tasks
 *
 *  Created by Ole Weidner on 11/22/08.
 *  Copyright 2008 Center for Computation & Technology. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying 
 *  LICENSE file or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#ifndef FAUST_MANYJOBS_JOB_HPP
#define FAUST_MANYJOBS_JOB_HPP

#include <saga/saga.hpp>
#include <faust/faust/defines.hpp>

#include <faust/manyjobs/state.hpp>
#include <faust/manyjobs/service.hpp>
#include <faust/manyjobs/description.hpp>

namespace faust
{
  namespace manyjobs {

    /*! \brief The %job provides the manageability interface to a %job 
     *         instance submitted through a manyjob %service instance. 
     *         It can't be instanciated directly, only thourgh the create_job
     *         factory method. 
     */
    class FAUST_EXPORT job : public saga::object
    {
    
    friend class service;
      
    private:

      job();
      
      std::string jobid_;
      
      
      service * service_;
      void set_service(service * sp) { service_ = sp; } 
      
    public:
      /*! \brief Tries to properly kill this %job instance.
       *  
       */      
      faust::manyjobs::state state_;
      ~job();
      
      /*! \brief Returns this %job instance's %job ID.
       *  
       */      
      std::string get_job_id();
      
      /*! \brief Tries to run this %job instance. 
       *
       */
      void run();
      
      /*! \brief Tries to wait for this %job instance to complete.
       *
       */
      bool wait(double timeout = -1.0);

      /*! \brief Tries to cancel this %job instance.
       *
       */
      bool cancel(double timeout = -1.0);

      /*! \brief Tries to suspend this %job instance.
       *
       */
      bool suspend();

      /*! \brief Tries to resume this %job instance.
       *
       */
      bool resume();
      
      /*! \brief Tries to return this %job instance's state.
       *
       */
      faust::manyjobs::state get_state();
      
      /*! \brief Returns this %job instance's description.
       *
       */
      faust::manyjobs::description get_description();

    };
  }
}

#endif /* FAUST_MANYJOBS_JOB_HPP */
