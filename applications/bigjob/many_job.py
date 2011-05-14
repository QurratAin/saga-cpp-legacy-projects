#!/usr/bin/env python

"""Module many_job.

This Module is used to launch a set of bigjobs.

"""
import pdb
import sys
import os
sys.path.append(os.path.dirname( __file__ ))
import getopt
import saga
import time
import uuid
import socket
import traceback
import bigjob
import Queue
import threading
import logging
import time
import math
import operator
import copy

# Log everything, and send it to stderr.
logging.basicConfig(level=logging.DEBUG)

DEFAULT_ADVERT_HOST="advert.cct.lsu.edu"

class many_job_service(object):

    def __init__(self, bigjob_list, advert_host):
        """ accepts resource list as key/value pair:
            ( {"resource_url" : "gram://qb1.loni.org/jobmanager-pbs", "number_nodes" : "32", "allocation" : "loni_stopgap2", "queue" : "workq", "bigjob_agent": "$(HOME)/src/REMDgManager/bigjob/advert_launcher.sh",  "walltime":1000},
              {"resource_url" : "gram://qb1.loni.org/jobmanager-pbs", "number_nodes" : "32", "allocation" : "loni_stopgap2", "queue" : "workq", "bigjob_agent": "$(HOME)/src/REMDgManager/bigjob/advert_launcher.sh", "walltime":1000})
        """        
        self.uuid = uuid.uuid1()
        
        if advert_host==None:
            self.advert_host=DEFAULT_ADVERT_HOST   
        else:
            self.advert_host=advert_host

        # list of resource dicts (1 dict per resource) 
        # will also store state of bigjob
        self.bigjob_list=copy.deepcopy(bigjob_list) 
       
        # state variable storing state of sub-jobs 
        self.active_subjob_list = []
        self.subjob_bigjob_dict = {}

        # queue contains unscheduled subjobs        
        self.subjob_queue = Queue.Queue()
        
        # submit bigjobs to resources
        self.__init_bigjobs()
        
        # thread which tries to resubmit jobs
        self.stop=threading.Event()
        self.rescheduler_thread=threading.Thread(target=self.__reschedule_subjobs_thread)
        self.rescheduler_thread.start()
        
        # last queue Size
        self.last_queue_size = 0
        self.submisssion_times=[]

    def __init_bigjobs(self):
        """ start on specified resources a bigjob """
        self.bigjob_list = self.__schedule_bigjobs()
        for i in self.bigjob_list:
            self.__start_bigjob(i)


    def __start_bigjob(self, bj_dict):
        """ private method - starts a bigjob on the defined resource """
        gram_url = bj_dict["resource_url"]
        logging.debug("start bigjob at: " + gram_url)
        bj = bigjob.bigjob(self.advert_host)
        ppn="1"
        if ("processes_per_node" in bj_dict):
            ppn=bj_dict["processes_per_node"]
        else:
            bj_dict["processes_per_node"]="1"
        bj.start_pilot_job(gram_url,
                           bj_dict["bigjob_agent"],
                           bj_dict["number_nodes"],
                           bj_dict["queue"],
                           bj_dict["allocation"],
                           bj_dict["working_directory"], 
                           None,
                           bj_dict["walltime"],
                           ppn)
        bj_dict["bigjob"]=bj # store bigjob for later reference in dict
        bj_dict["free_cores"]=int(bj_dict["number_nodes"])*int(ppn)
        bj_dict["to_be_terminated"]=False
        # lock for modifying the number of free nodes
        bj_dict["lock"] = threading.Lock()

    def add_resource(self, resource_dictionary):
        """ adds bigjob described in resource_dictionary to resources """
        dict = copy.deepcopy(resource_dictionary);
        self.__start_bigjob(dict)
        self.bigjob_list.append(dict)
        
    
    def remove_resource(self, bigjob):
        """ remove bigjob from resource list of manyjob """
        # mark bigjob for termination (after all sub-jobs in bj are 
        # finished        
        bigjob["to_be_terminated"]=True
        
 
    def __cleanup_resources(self):
        """ called periodically from scheduling thread
            terminates big-jobs which are marked and don't have 
            any running sub-jobs
        """
        # iterate over copy of list, but remove from orig list
        for i in self.bigjob_list[:]:
            if i["to_be_terminated"]==True:
                bj = i["bigjob"]
                total_cores = int(i["processes_per_node"])*int(i["number_nodes"])
                if  i["free_cores"]==total_cores and not i.has_key("bj_stopped"):
                    logging.debug("***Stop BigJob: " + str(bj.pilot_url))
                    # release resources of pilot job
                    bj.stop_pilot_job()
                    i["bj_stopped"]=True
                    #self.bigjob_list.remove(i)

    
    def get_resources(self):
        """ returns list with bigjob dictionaries
            for each managed bigjob 1 dictionary exists 
        """
        return self.bigjob_list


    def __schedule_bigjobs(self):
        """ prioritizes bigjob_list (bigjob with shortest expected delay will have index 0) """
        # no scheduling for now (start bigjob in the user specified order)
        return self.bigjob_list

    def create_job (self, job_description):
        subjob = sub_job(self, job_description, self.advert_host)        
        return subjob

    def __run_subjob(self, subjob):
        # select appropriate bigjob
        bigjob_info = self.__schedule_subjob(subjob)
        job = subjob.job 
        if job == None:
            #create new subjob
            #bj = bigjob_info["bigjob"]
            job = bigjob.subjob(self.advert_host)

        if bigjob_info == None:
            return job

        # create subjob on bigjob
        bj = bigjob_info["bigjob"]
        st = time.time()
        job.submit_job(bj.pilot_url, subjob.job_description)
        self.submisssion_times.append(time.time()-st)

        # store reference of subjob for further bookkeeping    
        self.active_subjob_list.append(job)
        self.subjob_bigjob_dict[subjob] = bigjob_info
        return job

    def queue_subjob(self, subjob):
        self.subjob_queue.put(subjob)
        job = bigjob.subjob(self.advert_host)
        return job

    def __schedule_subjob (self, subjob):
        """ find resource (bigjob) for subjob
            returns bigjob object """
        for i in self.bigjob_list:
            bigjob = i["bigjob"]
            lock = i["lock"]
            lock.acquire()
            free_cores = i["free_cores"]
            bigjob_url = bigjob.pilot_url
            state = bigjob.get_state_detail()
            logging.debug("Big Job: " + bigjob_url + " Cores: " + "%s"%free_cores + "/" 
                          + str(int(i["processes_per_node"])*int(i["number_nodes"])) 
                          + " State: " + state + " Terminated: " + str(i["to_be_terminated"]))
            if (state.lower() == "running" and free_cores >= int(subjob.job_description.number_of_processes) 
                and i["to_be_terminated"]==False):
                free_cores = i["free_cores"]
                free_cores = free_cores - int(subjob.job_description.number_of_processes)
                i["free_cores"]=free_cores
                lock.release()
                return i 

            lock.release()

        # no resource found
        self.subjob_queue.put(subjob)
        logging.debug("found no active resource for sub-job => (re-) queue it")
        return None        

    

    def __free_resources(self, subjob):
        """free resources taken by subjob"""
        if(self.subjob_bigjob_dict.has_key(subjob)):
            logging.debug("job: " + str(subjob) + " done - free resources")
            bigjob = self.subjob_bigjob_dict[subjob]
            lock = bigjob["lock"]
            lock.acquire()
            free_cores = bigjob["free_cores"]
            free_cores = free_cores + int(subjob.job_description.number_of_processes)
            bigjob["free_cores"]=free_cores
            del(self.subjob_bigjob_dict[subjob])
            lock.release()
            print "Freed resource - new state: Big Job: " +  bigjob["bigjob"].pilot_url + " Cores: " + "%s"%free_cores + "/" + str(int(bigjob["processes_per_node"])*int(bigjob["number_nodes"])) 
    
    def __reschedule_subjobs_thread(self):
        """ periodically checks subjob_queue for unscheduled subjobs
            if a unscheduled job exists it is scheduled
        """

        # XXX Hack to prevent multiple subjobs
        # Need real solution (and understanding)
        time.sleep(5)

        while True and self.stop.isSet()==False:
            logging.debug("Reschedule Thread")
            self.__cleanup_resources()
            subjob = self.subjob_queue.get()  
            # check whether this is a real subjob object  
            if isinstance(subjob, sub_job):
                self.__run_subjob(subjob)
                if self.last_queue_size == self.subjob_queue.qsize() or self.__get_total_free_cores()==0:
                    time.sleep(2) # sleep 30 s        

        logging.debug("Re-Scheduler terminated")


    def __get_free_cores(self, bigjob):
        """ return number of free cores if bigjob is active """
        #pdb.set_trace()
        if (bigjob["bigjob"].get_state_detail().lower()=="running" 
            and bigjob["to_be_terminated"]==False):
            return bigjob["free_cores"]

        return 0            

    def __get_total_free_cores(self):
        """ get's the total number of free cores from all active  bigjobs """
        free_cores = map(self.__get_free_cores, self.bigjob_list)
        #print "Free cores: " + str(free_cores)
        if len(free_cores)>0:
            total_free_cores = reduce(lambda x, y: x + y, free_cores)
            logging.debug("free_cores: " + str(free_cores) + " total_free_cores: " + str(total_free_cores))
            return total_free_cores
        return 0

    def cancel(self):
        logging.debug("Cancel re-scheduler thread")
        self.stop.set()
        # put object in queue to unlock the get() operation
        self.subjob_queue.put("dummy")
        self.rescheduler_thread.join()        
        logging.debug("Cancel many-job: kill all bigjobs")
        for i in self.bigjob_list:
            bigjob = i["bigjob"]
            bigjob.cancel()
        self.print_stats(self.submisssion_times, "Submission Times")

    def print_stats(self, times, description):
        n = len(times)
        sum = reduce(operator.add, times)
        mean = sum/n
        variance=0
        if n > 1:
            for i in times:
                variance += (i - mean)**2
            variance /= (n-1)
            variance = math.sqrt(variance)
        print description + " Average: " + str(mean) + " Stdev: " + str(variance)

    def __repr__(self):
        return str(self.uuid)

    def __del__(self):
        self.cancel()
                    

class sub_job():
    """ Class for controlling individual sub-jobs """
    
    def __init__(self, manyjob, job_description, advert_host):
        # store bigjob for reference
        self.manyjob=manyjob
        
        # init sub-job via advert
        self.job_description = job_description
        self.job = None

    def run(self):
         # select appropriate bigjob
        self.job = self.manyjob.queue_subjob(self)

    def get_state(self):     
        try:
            state = self.job.get_state()
            if self.__has_finished(state) == True:
                self.manyjob.__free_resources(self)
            return state
        except:
            pass
        return "Unknown"

    def cancel(self):
        return self.job.cancel()

    def wait(self):
        while 1:
            try:
                state = self.get_state()
                logging.debug("wait: state: " + state)
                if self.__has_finished(state) == True:
                    break
                time.sleep(2)
            except (KeyboardInterrupt, SystemExit):
                raise
            except:
                pass

    def __has_finished(self, state):
        state = state.lower()
        if state=="done" or state=="failed" or state=="canceled":
            return True
        else:
            return False

    def __del__(self):
        pass

    def __repr__(self):        
        return str(self.job)

""" Test Job Submission via ManyJob abstraction """
if __name__ == "__main__":
    try:
        print "Test ManyJob"
        # create job description
        jd = saga.job.description()
        jd.executable = "/bin/date"
        jd.number_of_processes = "1"
        jd.spmd_variation = "single"
        jd.arguments = [""]
        jd.working_directory = "/home/luckow"
        jd.output = "output.txt"
        jd.error = "error.txt"
        # submit via mj abstraction
        #resource_list =  ( {"resource_url" : "gram://qb1.loni.org/jobmanager-pbs", "number_nodes" : "128", "allocation" : "<your allocation>", "queue" : "workq", "bigjob_agent": "$(HOME)/src/REMDgManager/bigjob/advert_launcher.sh"},
        #                   {"resource_url" : "gram://qb1.loni.org/jobmanager-pbs", "number_nodes" : "64", "allocation" : "<your allocation>", "queue" : "workq", "bigjob_agent": "$(HOME)/src/REMDgManager/bigjob/advert_launcher.sh"})
        resource_list = []
        resource_list.append({"resource_url" : "gram://qb1.loni.org/jobmanager-pbs", "number_nodes" : "16", "allocation" : "<your allocation>", "queue" : "workq", "bigjob_agent": os.getcwd() + "/bigjob_agent_launcher.sh"})
        print "Create manyjob service " 
        mjs = many_job_service(resource_list, None)
        print "Create sub-job using manyjob " + str(mjs) 
        subjob = mjs.create_job(jd)
        print "Run sub-job"
        subjob.run()
        print "Wait for termination"
        subjob.wait()
        print "Sub-job state: " + str(subjob.get_state())
        mjs.cancel()
    except:
        try:
            if mjs != None:
                mjs.cancel()
        except:
            pass
