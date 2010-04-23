""" Example application demonstrating job submission via bigjob 
    advert_job implementation of BigJob is used
"""

import saga
import os
import bigjob
import time
import pdb

advert_host = "fortytwo.cct.lsu.edu"

""" Test Job Submission via Advert """
if __name__ == "__main__":

    ##########################################################################################
    # Start BigJob
    # Parameter for BigJob
    bigjob_agent = os.getcwd() + "/bigjob_agent_launcher.sh" # path to agent
    #bigjob_agent = "/bin/echo"
    nodes = 8 # number nodes for agent
    lrms_url = "gram://eric1.loni.org/jobmanager-pbs" # resource url
    workingdirectory=os.getcwd() +"/agent"  # working directory for agent
    userproxy = None # userproxy (not supported yet due to context issue w/ SAGA)

    # start pilot job (bigjob_agent)
    print "Start Pilot Job/BigJob: " + bigjob_agent + " at: " + lrms_url
    bj = bigjob.bigjob(advert_host)
    bj.start_pilot_job(lrms_url,
                            bigjob_agent,
                            nodes,
                            None,
                            None,
                            workingdirectory, 
                            userproxy,
                            None)
    print "Pilot Job/BigJob URL: " + bj.pilot_url + " State: " + str(bj.get_state())

    ##########################################################################################
    # Submit SubJob through BigJob
    jd = saga.job.description()
   # jd.executable = "/bin/date"
    jd.executable = "/work/athota1/new_bigjob/bigjob/async_agent.py"
    jd.number_of_processes = "8"
    jd.spmd_variation = "single"
    jd.arguments = [""]
    jd.working_directory = os.getcwd() 
    jd.output = "stdout.txt"
    jd.error = "stderr.txt"
    
    sj = bigjob.subjob(advert_host)
    sj.submit_job(bj.pilot_url, jd)
    
    # busy wait for completion
    while 1:
        state = str(sj.get_state())
        energy = str(sj.get_energy())
	print "state: " + state
        print "energy: " + energy
	if(state=="Failed" or state=="Done"):
            break
        time.sleep(10)

    ##########################################################################################
    # Cleanup - stop BigJob
    bj.cancel()
