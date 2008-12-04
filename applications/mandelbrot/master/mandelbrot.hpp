
#ifndef MANDELBROT_HPP
#define MANDELBROT_HPP

#include <saga/saga.hpp>

#include "output_base.hpp"
#include "output_x11.hpp"

// this class encapsulates a master/worker paradigm.  We start
// njobs_ jobs, and distribute work items to them.
class mandelbrot 
{
  private:

    std::string   odev_;     // name of output device
    output_base * dev_;      // output device
    unsigned int  njobs_;    // number of jobs to use
    bool          running_;  // flag true if compute jobs are running

    std::vector <saga::job::service> job_services_;    // list of saga job services
    std::vector <saga::job::job>     jobs_;            // list of saga jobs
    std::string                      job_bucket_name_; // name of job directory
    saga::advert::directory          job_bucket_;      // advert container for jobs 

    // jobs startup
    void job_startup (void);

  public:
    mandelbrot (std::string odev  = "x11",  // default output device
                int         njobs = 1);     // default number of client jobs
    ~mandelbrot (void);

    void compute    (void);
};

#endif // MANDELBROT_HPP

