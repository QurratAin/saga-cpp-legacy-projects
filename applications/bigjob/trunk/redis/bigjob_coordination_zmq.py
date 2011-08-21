'''
Encapsulates coordination and communication specifics of bigjob
'''

import logging
import threading
import datetime
import sys
import os
import pickle
import pdb
import zmq
import traceback
import Queue
from zmq.eventloop import ioloop, zmqstream
import zlib, cPickle as pickle

if sys.version_info < (2, 5):
    sys.path.append(os.path.dirname( os.path.abspath( __file__) ) + "/../ext/uuid-1.30/")
    sys.stderr.write("Warning: Using unsupported Python version\n")
    
logging.debug(str(sys.path))
import uuid

SERVER_IP="localhost"
SERVER_PORT=0


class message:    
    def __init__(self, command, key, value):
        self.command = command
        self.key = key
        self.value = value

    def __repr__(self):
        return ("command: %s, key: %s, value: %s "%(self.command, self.key, self.value))

class bigjob_coordination_zmq(object):
    '''
    Encapsulates communication and coordination
    Implementation based on ZMQ 
    '''
    def __init__(self, server=SERVER_IP, server_port=SERVER_PORT, server_connect_url=None):
        '''
        Constructor
        set server and server_port to create a service (server)
        set server_connect_url to connect to a service (client)
        '''  
        self.stopped = False
        self.has_stopped=False            
        # set up ZMQ client / server communication
        self.context = zmq.Context()
       
                       
        if server_connect_url==None: # role = Server
            # start eventloop
            self.startup_condition = threading.Condition()
            self.eventloop_thread=threading.Thread(target=self.__server, args=(server, server_port))
            self.eventloop_thread.start()
            self.startup_condition.acquire()
            self.startup_condition.wait()                       
        else: # role client
            self.address = server_connect_url
            #self.address = "tcp://"+server+":"+str(server_port)
        
        self.client_socket = self.context.socket(zmq.REQ)
        self.client_socket.connect(self.address)
        logging.debug("Connect to service at: " + self.address)
                
        # state managed by server
        self.pilot_states = {}
        self.job_ids = []
        self.jobs = {}
        self.job_states = {}
        self.new_job_queue = Queue.Queue()
        
        self.resource_lock = threading.RLock()
       
        logging.debug("C&C ZMQ system initialized")
        
    def get_address(self):
        """ return handle to c&c subsystems """
        return self.address
        
    #####################################################################################
    # Pilot-Job State
    def set_pilot_state(self, pilot_url, new_state, stopped=False):     
        logging.debug("BEGIN update state of pilot job to: " + str(new_state))
        self.resource_lock.acquire()   
        self.stopped=stopped     
        msg = message("set_pilot_state", pilot_url, {"state":str(new_state), "stopped":str(stopped)})
        self.client_socket.send_pyobj(msg, zmq.NOBLOCK)        
        self.client_socket.recv_pyobj()
        self.resource_lock.release()    
        logging.debug("END update state of pilot job to: " + str(new_state))
        
    def get_pilot_state(self, pilot_url):
        logging.debug("BEGIN get_pilot_state: %s lock: %s" % (pilot_url, str(self.resource_lock)))
        self.resource_lock.acquire()
        msg = message("get_pilot_state", pilot_url, "")
        self.client_socket.send_pyobj(msg, zmq.NOBLOCK)
        result = self.client_socket.recv_pyobj()
        self.resource_lock.release()        
        logging.debug("END get_pilot_state: %s lock: %s" % (pilot_url, str(self.resource_lock)))
        return result.value
    
    def get_jobs_of_pilot(self, pilot_url):
        """ returns array of job_url that are associated with a pilot """
        """ local only - used only by manager """
        return self.job_ids
    
    def delete_pilot(self, pilot_url):
        """ local only - used only by manager """
        # stop everything
        self.stopped=True
        self.eventloop_thread.join()
        logging.debug("Has stopped: " + str(self.has_stopped))
        self.__shutdown()
        
    
    #####################################################################################
    # Sub-Job State    
    def set_job_state(self, job_url, new_state):
        logging.debug("set_job_state")
        self.resource_lock.acquire()
        msg = message("set_job_state", job_url, new_state)
        self.client_socket.send_pyobj(msg, zmq.NOBLOCK)
        self.client_socket.recv_pyobj()
        self.resource_lock.release()        
        
    def get_job_state(self, job_url):
        logging.debug("get_job_state")
        self.resource_lock.acquire()
        msg = message("get_job_state", job_url, "")
        self.client_socket.send_pyobj(msg, zmq.NOBLOCK)
        result = self.client_socket.recv_pyobj()
        self.resource_lock.release()
        return result.value      
        
    #####################################################################################
    # Sub-Job Description
    def set_job(self, job_url, job_dict):        
        """ local only - used only by manager """
        self.job_ids.append(job_url)
        self.jobs[job_url] = job_dict   
        self.job_states[job_url]="Unknown"     
    
    def get_job(self, job_url):       
        if self.jobs.has_key(job_url)==False:
            logging.debug("get_job: " + str(self.resource_lock))
            self.resource_lock.acquire()
            logging.debug("get_job (lock acquired): " + str(self.resource_lock))
            msg = message("get_job", job_url, "")
            self.client_socket.send_pyobj(msg)
            result = self.client_socket.recv_pyobj()
            self.jobs[job_url] = result.value
            logging.debug("received job: "  + str(result.value))
            self.resource_lock.release()
        return self.jobs[job_url] 
    
    def delete_job(self, job_url):
        self.job_ids.remove(job_url)
        del self.jobs[job_url]
        del self.job_ids[job_url]
    
    
    #####################################################################################
    # Distributed queue for sub-jobs
    def queue_job(self, pilot_url, job_url):
        """ queue new job to pilot """
        logging.debug("queue_job " + str(self.resource_lock))
        #pdb.set_trace()
        self.resource_lock.acquire()
        msg = message("queue_job", "", job_url)
        self.client_socket.send_pyobj(msg, zmq.NOBLOCK)
        self.client_socket.recv_pyobj()
        self.resource_lock.release()                
        
    def dequeue_job(self, pilot_url):
        """ dequeue to new job  of a certain pilot """
        logging.debug("dequeue_job " + str(self.resource_lock))
        self.resource_lock.acquire()
        msg = message ("dequeue_job", pilot_url, "")
        self.client_socket.send_pyobj(msg, zmq.NOBLOCK)
        result = self.client_socket.recv_pyobj()
        self.resource_lock.release()
        return result.value
    
    
    
    #####################################################################################
    # Private functions    
    
    
    def __server_handler(self, messageList):
        """ server for managing job / pilot job states via ZMQ """
        msg = pickle.loads(messageList[0])
        logging.debug("Message received: " + str(msg))
        self.__handle_message(msg, self.stream)
        
    
    def __handle_message(self, msg, reply_socket):
        try:
            command = msg.command        
            if command == "set_pilot_state":
                self.pilot_states[msg.key] = msg.value
                reply_socket.send_pyobj("")
                #self.service_socket.send("")            
            elif command == "get_pilot_state":
                result = message ("", "", self.pilot_states[msg.key])
                reply_socket.send_pyobj(result, zmq.NOBLOCK)                
            elif command == "set_job_state":
                self.job_states[msg.key] = msg.value
                reply_socket.send_pyobj("", zmq.NOBLOCK)       
            elif command == "get_job_state":
                result=message("", "", self.job_states[msg.key])
                reply_socket.send_pyobj(result, zmq.NOBLOCK)            
            elif command == "get_job":
                result = message("","", self.jobs[msg.key])
                reply_socket.send_pyobj(result, zmq.NOBLOCK)
            elif command == "queue_job":                
                self.new_job_queue.put(msg.value)
                reply_socket.send_pyobj("", zmq.NOBLOCK)                       
            elif command == "dequeue_job":
                new_job=None
                try:
                    new_job = self.new_job_queue.get(False)
                except:                
                    pass
                result = message("","", new_job)
                reply_socket.send_pyobj(result, zmq.NOBLOCK)
            else:
                logging.debug("sending default reply")
                reply_socket.send_pyobj("", zmq.NOBLOCK)
        except:
            traceback.print_exc(file=sys.stderr)
    
    
    def __server(self, server, server_port):
        """ server for managing job / pilot job states via ZMQ """
        service_socket = self.context.socket(zmq.REP)
        if SERVER_PORT==0: # random port
            server_port = service_socket.bind_to_random_port("tcp://*")    
            self.address = "tcp://"+server+":"+str(server_port)                
        elif server == "localhost":
            self.server_address = "tcp://*:"+str(server_port)
            self.address = "tcp://"+server+":"+str(server_port)
            service_socket.bind(self.server_address)
        else:
            self.server_address = "tcp://"+server+":"+str(server_port)
            self.address = self.server_address
            service_socket.bind(self.server_address)
        logging.debug("Starting service at: " + self.address)
        
        self.startup_condition.acquire()
        self.startup_condition.notify()   
        self.startup_condition.release()
        while self.stopped == False:
            #logging.debug("Waiting for messages...")
            msg = service_socket.recv_pyobj()
            #logging.debug("Message received: " + str(msg))
            self.__handle_message(msg, service_socket)
            logging.debug("Message handled: " + str(msg) + " stopped = " + str(self.stopped))
            #pdb.set_trace()
        logging.debug("__server thread stopped: " + str(self.stopped))
        self.has_stopped = True
        #service_socket.close()
            
    def __loop(self):
        self.loop = ioloop.IOLoop.instance()
        #self.loop.add_handler(self.service_socket, self.__server_handler, zmq.POLLIN)
        self.stream = zmqstream.ZMQStream(self.service_socket, self.loop)
        self.stream.on_recv(self.__server_handler)
        logging.debug("Start event loop")
        self.loop.start()
        
    def __shutdown(self):
        logging.debug("shutdown ZMQ")
        try:     
            #self.client_socket.close()            
            #self.service_socket.close()
            #self.context.term()
            pass
        except:
            pass       
            
            