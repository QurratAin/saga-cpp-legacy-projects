""" Bigjob Frontend

This Module contains the frontend of the Bigjob framework.

"""

import saga
import uuid
import Queue
import time
from threading import Thread

#
# BigJob/Diane implementation
#
from diane.bigjob_diane_frontend import BigjobDIANE
from bjsaga.bigjob_saga import bigjob as BigjobSAGA
from bjsaga.bigjob_saga import subjob

ADVERT_HOST = 'advert.cct.lsu.edu'

#
# Supported list of backends. Static for now.
#
class bigjob_type(object):
    Unknown = 0
    SAGA = 1
    DIANE = 2

#
# Isoldate uuid calls
# 
def getuuid():
    return uuid.uuid4()

def str2state(state):
    if state == 'New':
        return saga.job.job_state.New
    elif state == 'Running':
        return saga.job.job_state.Running
    elif state == 'Done':
        return saga.job.job_state.Done
    elif state == 'Canceled':
        return saga.job.job_state.Canceled
    elif state == 'Failed':
        return saga.job.job_state.Failed
    else:
        return saga.job.job_state.Unknown

    

#
# Description of UoW (for now just a saga job description)
# 
class uow_description(saga.job.description):
    pass


#
# Description of Bigjob (for now just a saga job description)
#
class bj_description(saga.job.description):
    pass

class Engine(Thread):
    def __init__(self, bj):

        print 'Troy Engine starting'

        Thread.__init__(self)
        self.status = -1
        self.bj = bj
        self.should_stop = False
        self.i = 0

    def run(self):
        while 1:
            print 'Engine loop'

            print 'Resources:', len(self.bj.resources)
            print 'UoW Queue:', self.bj.uow_q.qsize() 

            if self.i >= len(self.bj.resources):
                self.i = 0

            if self.bj.uow_q.qsize() > 0:
                u = self.bj.uow_q.get()
                u.run(self.bj.resources[self.i])
                #print u.uowd.get_vector_attribute('Arguments')
            else:
                time.sleep(5)

            self.i = self.i + 1
            
            if self.should_stop == True:
                break


#
# Bigjob class
#
#class Bigjob(api.base.Bigjob): # make it dependend on api again
class Bigjob(object):

    """ This class represents the Bigjob. """

    def __init__(self, bj_type=None, rm=None, job_desc=None, context=None):
        """ Create a Bigjob object.

            Keyword arguments:
            bj_type -- Backend type
            rm -- URL pointing to resource management backend
            job_desc -- SAGA job description
            context -- security context

            TODO: name/label?

            Return value:
            If resource_url and job_desc are supplied, return a job service.
            Otherwise return None

        """

        self.resources = []
        self.uow_q = Queue.Queue()

        self.__uuid = getuuid()
        if bj_type != None and rm != None and job_desc != None:
            self.add_resource(bj_type, rm, job_desc, context)

        self.engine = Engine(self)
        self.engine.start()


    def add_resource(self, bj_type, rm, job_desc, context=None):
        """ Add a (list of) resource(s) to the Bigjob

            Keyword arguments:
            bj_type -- Backend type
            resource_url -- URL pointing to resource management backend
            job_desc -- SAGA job description
            context -- a security context
        """


        #
        # Advert
        #
        if bj_type == bigjob_type.SAGA:
            bj = BigjobSAGA(ADVERT_HOST)
            bj.bj_type = bj_type

            resource_url = rm
            number_nodes = job_desc.get_attribute('NumberOfProcesses')
            queue = job_desc.get_attribute('Queue')
            project = None
            workingdirectory = job_desc.get_attribute('WorkingDirectory')
            userproxy = None # userproxy (not supported yet due to context issue w/ SAGA)
            walltime = job_desc.get_attribute('WallTimeLimit')
            processes_per_node = job_desc.get_attribute('ProcessesPerHost')

            # local
            bigjob_agent = '/home/marksant/proj/bigjob/branches/bigjob_overhaul/bjsaga/bigjob_agent_launcher.sh' 
            # gram

            bj.start_pilot_job(resource_url,
                bigjob_agent,
                number_nodes,
                queue,
                project,
                workingdirectory, 
                userproxy,
                walltime,
                processes_per_node)
            
            self.resources.append(bj)
        #
        # DIANE
        #
        elif bj_type == bigjob_type.DIANE:
            bj = BigjobDIANE()
            bj.bj_type = bj_type

            resource_url = rm
            number_nodes = job_desc.get_attribute('NumberOfProcesses')
            queue = job_desc.get_attribute('Queue')
            project = None
            #workingdirectory = "gsiftp://oliver1.loni.org/work/marksant/diane"
            workingdirectory = job_desc.get_attribute('WorkingDirectory')
            userproxy = None # userproxy (not supported yet due to context issue w/ SAGA)
            walltime = job_desc.get_attribute('WallTimeLimit')
            processes_per_node = job_desc.get_attribute('ProcessesPerHost')
            bigjob_agent = workingdirectory

            bj.start_pilot_job(resource_url,
                bigjob_agent,
                number_nodes,
                queue,
                project,
                workingdirectory, 
                userproxy,
                walltime,
                processes_per_node)

            self.resources.append(bj)

        else:
            raise saga.exception.no_success("Unknown Backend type")


    def list_resources(self):
        """ Return the resources that are managed by this BigJob.  

            Keyword arguments:
            None

            Return value:
            A list of resources of type job_service
        """

        pass

    def remove_resource(self, rm):
        """ Remove the resource from the Bigjob

            Keyword arguments:
            resource -- a resource description
        """
        pass
         
    def get_capabilities(self, rm=None):
        """ Return the capabilities that this Bigjob or resource provides.
       
            Keyword arguments:

            resource -- The (optional) resource to act upon.
        """
        pass
    
    def list_uows(self, rm):        
        """ Return the list of UoWs that are assigned to this Bigjob. """
        pass

    def resize_resource(self, rm):        
        """ (re-)Calibrate the resouce in the Bigjob to the given 'size'.
        
            Keyword arguments:
            resource -- The resource to act upon.
        """
        pass

    def assign_uow(self, uow):
        """ Assign a UoW to this Bigjob.

            Keyword argument:
            uow -- The Unit of Work from the application
            XXX

        """
        u = UoW(self, uow)

        self.uow_q.put(u)

        return u

    def cancel(self, rm=None):        
        """ Cancel the a resource in the BigJob.

            Keyword arguments:
            rm -- The optional resource(s) to act upon.
        """

        for r in self.resources:
            r.cancel()

        print 'Cancel the BigJob'
        self.engine.should_stop = True
        self.engine.join()


#
# Unit of Work class
# 
class UoW(object):
    """ This class represents the Unit of Work concept in the Bigjob framework. """

    def __init__(self, bj, uowd, requirements=None):
        """ Create a new Unit of Work.

            Keyword arguments:
            bj -- the bigjob this corresponds to
            uowd -- the description of this UoW
            requirements -- the (optional) application requirements for this UoW

        """

        self.bj = bj
        self.state = saga.job.job_state.Unknown
        self.uowd = uowd

    def run(self, rm):

        if rm.bj_type == bigjob_type.SAGA:
            print 'This is an SAGA UoW'

            self.sj = subjob(ADVERT_HOST)
            self.uuid = self.sj.uuid
            print 'pilot_url:', rm.pilot_url
            self.sj.submit_job(rm.pilot_url, self.uowd)
            self.state = saga.job.job_state.Running

            self.rm = rm

        elif rm.bj_type == bigjob_type.DIANE:
            print 'This is a DIANE UoW'

            self.uuid = rm.submit_job(self.uowd)
            self.state = saga.job.job_state.Running

            self.rm = rm

        else:
            print 'This is an unknown UoW'


    def get_description(self):        
        """ Return the description of this UoW. """
        pass

    def get_requirements(self):        
        """ Return the requirements that this UoW has. """
        pass

    def get_state(self):        
        """ Return the state of the UoW. """

        if self.state == saga.job.job_state.Unknown:
            return self.state

        if self.rm.bj_type == bigjob_type.SAGA:
            return str2state(self.sj.get_state())

        elif self.rm.bj_type == bigjob_type.DIANE:
            return self.rm.get_job_state(self.uuid)

        else:
            print 'This is an unknown UoW'



    def get_input(self):        
        """ Return the input(s) of this UoW. """
        pass

    def get_output(self):        
        """ Return the output(s) of this UoW. """
        pass

    def get_error(self):        
        """ Return the error(s) of this UoW. """
        pass

    def cancel(self):
        """ Cancel the Task. """
        pass
