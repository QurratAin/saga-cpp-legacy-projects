# Package: saga
# Module: monitoring
# Description: The module which specifies the classes concerning Callbacks, Metrics, Monitorable and Steerable in saga
# Specification and documentation can be found in section 3.9, page 114-139 of the GFD-R-P.90 document
# Author: P.F.A. van Zoolingen, Computer Systems Section, Faculty of Exact Science (FEW), Vrije Universiteit, Amsterdam, The Netherlands.

from saga.error import NotImplemented
from saga.attributes import Attributes
from saga.object import Object, ObjectType

from org.ogf.saga.monitoring import MonitoringFactory
import org.ogf.saga.monitoring.Metric
import org.ogf.saga.monitoring.Callback
import org.ogf.saga.task.Task
import org.ogf.saga.task.TaskContainer
import org.ogf.saga.stream.Stream
import org.ogf.saga.stream.StreamService
import org.ogf.saga.job.Job
import org.ogf.saga.job.JobSelf

class Callback(object):
    """Callbacks are used for asynchronous notification of metric changes (events) """
    
    def cb(self, monitorable, metric, context):
        #in monitorable mt, in metric metric, in context ctx, out bool keep
        """
        Asynchronous handler for metric changes.
        @summary: Asynchronous handler for metric changes.
        @param monitorable: the saga monitorable object which causes the callback invocation
        @type monitorable: L{Monitorable<saga.monitoring.Monitorable>}
        @param metric: the metric causing the callback invocation
        @type metric: L{Metric<saga.monitoring.Metric>}
        @param context: the context associated with the entity causing the callback
        @type context: L{Context<saga.context.Context>}
        @return: indicator if callback stays registered
        @rtype: bool
        @PreCondition: the passed context is authenticated.
        @PostCondition: if cb returns True, the callback stays registered, and will be invoked again on the next metric update.
        @PostCondition: if cb returns False, the callback gets unregistered, and will not be invoked again on metric updates, unless it gets re-added by the user.
        @Raise NotImplemented:
        @raise AuthorizationFailed:
        @Note: "metric" is the Metric the callback is invoked on, that means that this metric
               recently changed. Note that this change is semantically defined by the metric, e.g. the
               string of the "value" attribute of the metric might have the same value in two subsequent
               invocations of the callback.
        @note: "monitorable" is the Monitorable object the metric "metric" belonges to.
        @note: "context" is the Context which allows the callback to authorize the metric change.
               If the cb method decides not to authorize this particular invocation, it raises an "AuthorizationFailed" exception.
        @note: if no context is available, a context of type "Unknown" is passed, with no attributes
               attached. Note that this can also indicate that a non-authenticated party connected.
        @note: a callback can be added to a metric multiple times. A "False" return value (no keep) will
               remove only one registration, and keep the others.
        @note: a callback can be added to multiple metrics at the same time. A "False" return (no keep) will
               only remove the registration on the metric the callback was invoked on.
        @note: the application must ensure appropriate locking of callback instances which are used with multiple metrics.
        @note: a callback added to exactly one metric exactly once is guaranteed to be active at most once at
               any given time. That implies that the SAGA implementation queues pending requests
               until a callback invocation is finished.

        """
        raise NotImplemented, "cb() is not implemented in this object. Classes extending Callback must implement their own cb()"

class CallbackProxy(org.ogf.saga.monitoring.Callback):
    pythonCallbackObject = None
    
    def __init__(self, **impl):
        if "pythonCallbackObject" in impl:
            if not isinstance(impl["pythonCallbackObject"], Callback):
                raise BadParameter, "Parameter impl[\"pythonCallbackObject\"] is not a subclass of Callback. Type: " + str(impl["pythonCallbackObject"].__class__)
            self.pythonCallbackObject = impl["pythonCallbackObject"]

    def cb(self, monitorable, metric, context):  
        from saga.task import Task, TaskContainer
        #from saga.stream import Stream, StreamService
        #from saga.job import Job, JobSelf
        #TODO: Uncomment this
        
        print "CallbackProxy.cb called:"
        print " check 1",
        if metric is not None:
            tempMetric = Metric("","","","","","",delegateObject = metric)
        else:
            tempMetric = None
        
        print "2",
        
        if context is not None:
            tempContext = Context(delegateObject = context)
        else:
            tempContext = None
            
        print "3",
        tempMonitorable = None

        if  isinstance (monitorable, org.ogf.saga.task.Task):
            print "TASK",
            try:
                tempMonitorable = Task(delegateObject = monitorable)
            except Exception, e:
                print str(e.__class__),":", str(e)
            print "/TASK",
        
        elif isinstance(monitorable, org.ogf.saga.task.TaskContainer):
            print "TASKCONTAINER",
            tempMonitorable = TaskContainer(delegateObject = monitorable)
        
        elif isinstance(monitorable, org.ogf.saga.stream.StreamService):
            print "STREAMSERVICE",
            tempMonitorable = StreamService(delegateObject = monitorable)
        
        elif isinstance(monitorable, org.ogf.saga.stream.Stream):
            print "STREAM",
            tempMonitorable = Stream(delegateObject = monitorable)
        
        elif isinstance(monitorable, org.ogf.saga.job.Job):
            print "JOB",
            tempMonitorable = Job(delegateObject = monitorable)
        
        elif isinstance(monitorable, org.ogf.saga.job.JobSelf):
            print "JOBSELF",            
            tempMonitorable = Jobself(delegateObject = monitorable)

        else:
            print "ELSE",
            #TODO: Check if CallbackProxy fallback is needed
            message = "CallbackProxy: unknown monitorable object was passed from Java Implementation. Type: "
            message = message + str(monitorable.__class__) + " Not passed to pythonObject.cb() " 
            print message
        if tempMonitorable is not None:
            print "CALL MADE"
            self.pythonCallbackObject.cb(tempMonitorable, tempMetric, tempContext)
            print "CALL FINISHED"
            return
      
class Metric(Object, Attributes):
    """A metric represents an entity / value to be monitored."""
    delegateObject = None
    callbacks = {}

    def __init__(self, name, desc, mode, unit, mtype, value, **impl):
    #in string name, in string desc, in string mode, in string unit, in string type, in string value, out metric          obj);
        """
        Initializes the Metric object.
        @summary: Initializes the Metric object.
        
        @param name: name of the metric
        @type name: string
        @param desc: description of the metric
        @type desc: string
        @param mode: mode of the metric
        @type mode: string
        @param unit: unit of the metric value
        @type unit: string
        @param mtype: type of the metric
        @type mtype: string
        @param value: initial value of the metric
        @type value: string

        @PostCondition: callbacks can be registered on the metric.
        @Raise  NotImplemented:
        @raise BadParameter:
        @raise Timeout:
        @raise NoSuccess:
        @note: a metric is not attached to a session, but can be used in different sessions.
        @note: the string arguments given are used to initialize the attributes of the metric.
        @note: the constructor ensures that metrics are always initialized completely. All changes to
         attributes later will always result in an equally valid metric.
        @note: incorrectly formatted "value" parameter, invalid "mode" and "type" parameter, and empty
         required parameter (all but "unit") will cause a BadParameter exception.
        @note: a "Timeout" or "NoSuccess" exception indicates that the backend could not create that specific metric.
        """
        if "delegateObject" in impl:
            if not isinstance(impl["delegateObject"], org.ogf.saga.monitoring.Metric):
                raise BadParameter, "Parameter impl[\"delegateObject\"] is not a org.ogf.saga.monitoring.Metric. Type: " + str(impl["delegateObject"].__class__)
            self.delegateObject = impl["delegateObject"]
            return
        super(Metric, self).__init__()
        if type(name) is not str:
            raise BadParameter, "Parameter name is not a string. Type: " + str(type(name))
        if type(desc) is not str:
            raise BadParameter, "Parameter desc is not a string. Type: " + str(type(desc))
        if type(mode) is not str:
            raise BadParameter, "Parameter mode is not a string. Type: " + str(type(mode))
        if type(unit) is not str:
            raise BadParameter, "Parameter unit is not a string. Type: " + str(type(unit))
        if type(mtype) is not str:
            raise BadParameter, "Parameter mtype is not a string. Type: " + str(type(mtype))
        if type(value) is not str:
            raise BadParameter, "Parameter value is not a string. Type: " + str(type(value))        
        try:
            self.delegateObject = MonitoringFactory.createMetric(name, desc, mode, unit, type, value) 
        except org.ogf.saga.error.SagaException, e:
            raise self.convertException(e)
      
    #callback handling
    def add_callback(self, cb): 
        #in callback cb, out int cookie);
        """
        Add asynchronous notifier callback to watch metric changes.
        @summary: Add asynchronous notifier callback to watch metric changes.
        @param cb: Callback to be added
        @type cb: L{Callback<saga.monitoring.Callback>}
        @return: handle for this callback, called a cookie, to be used for removal
        @rtype: int
        @PreCondition: the metric is not "Final".
        @PostCondition: the callback is invoked on metric changes.
        @Permission: Read
        @Raise NotImplemented:
        @raise IncorrectState:
        @raise PermissionDenied:
        @raise AuthorizationFailed:
        @raise AuthenticationFailed:
        @raise Timeout:
        @raise NoSuccess:
        @Note: "IncorrectState" is raised if the metric is "Final".
        @note: the "callback" method on cb will be invoked on any change of the metric (not only when its value changes)
        @note: if the "callback" method returns true, the callback is kept registered; if it returns
                 false, the callback is called, and is un-registered after completion. If the
                 callback raises an exception, it stays registered.
        @note: the returned cookie uniquely identifies the callback, and can be used to remove it.
        @note: A "Timeout" or "NoSuccess" exception is raised if the implementation cannot invoke the
                 callback on metric changes.
        @note: a backend MAY limit the ability to add callbacks - the method may hence cause an
                 "AuthenticationFailed", "AuthorizationFailed" or "PermissionDenied" exception to be raised.

        """
        if not isinstance(cb, Callback):
            raise BadParameter, "Parameter cb is not a subclass of Callback. Type: " + str(cb.__class__)
        try:
            delegateCallback = CallbackProxy(pythonCallbackObject=cb)
            cookie = self.delegateObject.addCallback(delegateCallback)
            self.callbacks[cookie] = delegateCallback
        except org.ogf.saga.error.SagaException, e:
            raise self.convertException(e) 
        return cookie              
      
    def remove_callback(self, cookie):
        #in int cookie
        """
        Remove a callback from a metric.
        @summary: Remove a callback from a metric.
        @param cookie: handle identifying the callback to be removed
        @type cookie: int
        @PreCondition: the callback identified by "cookie" is registered for that metric.
        @PostCondition: the callback identified by "cookie" is not active, nor invoked ever again.
        @Permission: Read
        @Raise NotImplemented:
        @Raise BadParameter:
        @Raise PermissionDenied:
        @Raise AuthorizationFailed:
        @Raise AuthenticationFailed:
        @Raise Timeout:
        @Raise NoSuccess:
        @Note: if a callback is active at the time of removal, the call blocks until
               that callback returns. The callback is not activated anew during or after that block.
        @note: if the callback was removed earlier, or was unregistered by returning false, this call does nothing.
        @note: the removal only affects the cb identified by "cookie", even if the same callback was
               registered multiple times.
        @note: if the cookie was not created by adding a callback to this object instance, a BadParameter is raised.
        @note: a "Timeout" or "NoSuccess" exception is raised if the backend cannot guarantee that the callback gets successfully removed.
        @note: note that the backend allows the removal of the callback, if it did allow its addition -
               hence, no authentication, autorization or permission faults are to be expected.

        """
        if type(cookie) is not int:
            raise BadParameter, "Parameter cookie is not an int. Type: " + str(type(cookie))
        try:
            self.delegateObject.removeCallback(name, cookie)
        except org.ogf.saga.error.SagaException, e:
            raise self.convertException(e)
        del self.callbacks[cookie]
        #TODO: check if multiple names can exist with same cookie! Remove old proxies!
              
              
    #actively signal an event
    def fire(self):
        """
        Push a new metric value to the backend.
        @summary: Push a new metric value to the backend.
        @PreCondition: the metric is not "Final"
        @PreCondition: the metric is "ReadWrite"
        @PostCondition: callbacks registered on the metric are invoked.
        @Permission: Write
        @Raise NotImplemented:
        @Raise IncorrectState:
        @Raise PermissionDenied:
        @Raise AuthorizationFailed:
        @Raise AuthenticationFailed:
        @Raise Timeout:
        @Raise NoSuccess:
        @Note: "IncorrectState" is raised if the metric is "Final".
        @note: "PermissionDenied" is raised if the metric is not "ReadWrite". That also holds for a once
                 writable metric which was flagged "Final". To catch race conditions on this exceptions,
                 the application should try/raise the fire().
        @note: it is not necessary to change the value of a metric in order to fire it.
        @note: "set_attribute ("value", "...") on a metric does NOT imply a fire. Hence the value can be
              changed multiple times, but unless fire() is explicitly called, no consumer will notice.
        @note: if the application invoking fire() has callbacks registered on the metric, these callbacks are invoked.
        @note: "AuthenticationFailed", "AuthorizationFailed" or "PermissionDenied" may get raised if the
              current session is not allowed to fire this metric.
        @note: a "Timeout" or "NoSuccess" exception signals that the implementation could not communicate
              the new metric state to the backend.

        """
        try:
            self.delegateObject.fire()
        except org.ogf.saga.error.SagaException, e:
            raise self.convertException(e)
        
class Monitorable(object):
    """SAGA objects which provide metrics and can thus be monitored extend the Monitorable class"""
    delegateObject = None
    callbacks = {}

    def list_metrics(self):
        #return array<string> names
        """
        List all metrics associated with the object.
        @summary: List all metrics associated with the object.
        @return: names identifying the metrics associated with the object instance
        @rtype: tuple
        @Permission: Query
        @Raise NotImplemented:
        @raise PermissionDenied:
        @raise AuthorizationFailed:
        @raise AuthenticationFailed:
        @raise Timeout:
        @raise NoSuccess:
        @Note: several SAGA objects are required to expose certain metrics (e.g. "task.state"). However,
                 in general that assumption cannot be made, as implementations might be unable to provide
                 metrics. In particular, listed metrics might actually be unavailable.
        @note: no order is implied on the returned tuple
        @note: the returned tuple is guaranteed to have no double entries (names are unique)
        @note: an "AuthenticationFailed", "AuthorizationFailed" or "PermissionDenied"
                 exception indicates that the current session is not allowed to list the available metrics.
        @note: a "Timeout" or "NoSuccess" exception indicates that the backend was not able to list the available metrics.
        """
        try:
            retval = self.delegateObject.listMetrics()
            return(tuple(retval))
        except org.ogf.saga.error.SagaException, e:
            raise self.convertException(e)
     
    def get_metric(self, name):
         #in string name, return metric metric
        """
        Returns a metric instance, identified by name.
        @summary: Returns a metric instance, identified by name.
        @param name: name of the metric to be returned
        @type name: string
        @return: metric instance identified by name
        @rtype: L{Metric<Metric>}
        @Permission: Query
        @Raise NotImplemented:
        @raise DoesNotExist:
        @raise PermissionDenied:
        @raise AuthorizationFailed:
        @raise AuthenticationFailed:
        @raise Timeout:
        @raise NoSuccess:
        @Note: multiple calls of this method with the same value for name return multiple identical
              instances (copies) of the metric.
        @note: a "DoesNotExist" exception indicates that the backend does not know the metric with the given name.
        @note: an "AuthenticationFailed", "AuthorizationFailed" or "PermissionDenied"
              exception indicates that the current session is not allowed to obtain the named metric.
        @note: a "Timeout" or "NoSuccess" exception indicates that the backend was not able to return the named metric.

        """
        if type(name) is not str:
            raise BadParameter, "Parameter name is not a string. Type: " +str(type(name))
        try:
            javaObject = self.delegateObject.getMetric(name)
            return Metric("","","","","","",delegateObject=javaObject)
        except org.ogf.saga.error.SagaException, e:
            raise self.convertException(e)
     
    def add_callback(self, name, cb):
        #in string name, in callback cb, out int cookie
        """
        Add a callback to the specified metric.
        @summary: Add a callback to the specified metric.
        @param name: identifies the metric to which cb is to be added
        @type name: string
        @param cb: the Callback object to be registered
        @return: handle for callback removal called a cookie
        @rtype: int
        @PostCondition: the callback is registered on the metric.
        @Permission: Read on the metric.
        @Raise NotImplemented:
        @raise DoesNotExist:
        @raise IncorrectState:
        @raise PermissionDenied:
        @raise AuthorizationFailed:
        @raise AuthenticationFailed:
        @raise Timeout:
        @raise NoSuccess:
        @Note: notes to the add_callback method of the metric class apply.

        """
        if type(name) is not str:
            raise BadParameter, "Parameter name is not a string. Type: " +str(type(name))
        if isinstance(cb, Callback) is False:
            raise BadParameter, "Parameter cb is not a subclass of Callback. Type: " + str(type(cb))
        try:
            delegateCallback = CallbackProxy(pythonCallbackObject=cb)
            cookie = self.delegateObject.addCallback(name, delegateCallback )
            self.callbacks[cookie] = delegateCallback
        except org.ogf.saga.error.SagaException, e:
            raise self.convertException(e)
        return cookie
     
    def remove_callback(self, name, cookie):
        #in string name, in int cookie
        """
        Remove a callback from the specified metric
        @summary: Remove a callback from the specified metric.
        @param name: identifies the metric for which cb is to be removed
        @type name: string
        @param cookie: identifies the callback to be removed
        @type cookie: int
        @PreCondition: the callback was registered on the metric.
        @Permission: read on the metric.
        @Raise NotImplemented:
        @raise BadParameter:
        @raise DoesNotExist:
        @raise PermissionDenied:
        @raise AuthorizationFailed:
        @raise AuthenticationFailed:
        @raise Timeout:
        @raise NoSuccess:
        @Note: notes to the remove_callback method of the metric class apply
        """
        if type(name) is not str:
            raise BadParameter, "Parameter name is not a string. Type: " +str(type(name))
        if type(cookie) is not int:
            raise BadParameter, "Parameter cookie is not an int. Type: " + str(type(cookie))
        try:
            self.delegateObject.removeCallback(name, cookie)
        except org.ogf.saga.error.SagaException, e:
            raise self.convertException(e)
        # del self.callbacks[cookie]
        #TODO: check if multiple names can exist with same cookie! Remove old proxies!
   
class Steerable(Monitorable):
    """SAGA objects which can be steered by changing their metrics implement the steerable interface"""
    delegateObject = None

    def add_metric(self, metric):
        #in metric metric, out bool success
        """
        Add a metric instance to the application instance.
        @summary: Add a metric instance to the application instance.
        @param metric: metric to be added
        @type metric: L{Metric}
        @return: indicator if the metric was added successfully 
        @rtype: bool
        @PostCondition: the metric can be accessed from this application, and possibly from other applications.
        @Permission: Write
        @Raise NotImplemented:
        @Raise AlreadyExists:
        @Raise IncorrectState:
        @Raise PermissionDenied:
        @Raise AuthorizationFailed:
        @Raise AuthenticationFailed:
        @Raise Timeout:
        @Raise NoSuccess:
        @Note: a metric is uniquely identified by its name attribute - no two metrics with the same name can be added.
        @Note: any callbacks already registered on the metric stay registered (the state of metric is not changed)
        @Note: an object being steerable does not guarantee that a metric can in fact be added -- the
                 returned bool indicates if that particular metric could be added.
        @Note: an "AuthenticationFailed", "AuthorizationFailed" or "PermissionDenied"
                 exception indicates that the current session is not allowed to add metrics to the steerable.
        @Note: a "Timeout" or "NoSuccess" exception indicates that the backend was not able to add the  metric.
        @Note: if a metric with the same name is already known for the object, an "AlreadyExists" exception is raised.
        @Note: if the steerable instance does not support the addition of new metrics, i.e. if only the
                 default metrics can be steered, an "IncorrectState" exception is raised.
        """
        if type(metric) is not Metric:
            raise BadParameter, "Parameter metric is not a Metric. Type: " + str(type(metric))
        try:
            retval =  self.delgateObject.addMetric(metric.delegateObject)
            if retval == 1:
                return True
            else: 
                return False
        except java.lang.Exception:
            raise self.convertException(e)

    def remove_metric(self, name):
        #in string name
        """
        Remove a metric instance.
        @summary: Remove a metric instance.
        @param name: identifies the metric to be removed
        @type name: string
        @PostCondition: all callbacks registered on that metric are unregistered.
        @PostCondition: the metric is not available anymore.
        @Permission: Write
        @Raise NotImplemented:
        @Raise DoesNotExist:
        @Raise IncorrectState:
        @Raise PermissionDenied:
        @Raise AuthorizationFailed:
        @Raise AuthenticationFailed:
        @Raise Timeout:
        @Raise NoSuccess:
        @Note: only previously added metrics can be removed; default metrics (saga defined or implementation
                 specific) cannot be removed; attempts to do so raise a BadParameter exception.
        @Note: an "AuthenticationFailed", "AuthorizationFailed" or "PermissionDenied"
                 exception indicates that the current session is not allowed to remove the metrics from the steerable.
        @Note: a "Timeout" or "NoSuccess" exception indicates that the backend was not able to remove the metric.
        @Note: if a metric with that name is not known for the object, a "DoesNotExist" exception is raised.
        @Note: if a steerable instance does not support the removal of some metric, e.g. if a metric
                 needs to be always present, an "IncorrectState" exception is raised.
                 For example, the "state" metric on a steerable job cannot be removed.

        """
        if type(name) is not str:
            raise BadParameter, "Parameter name is not a string. Type: " + str(type(name))
        try:
            self.delgateObject.removeMetric(name)
        except java.lang.Exception:
            raise self.convertException(e)

    def fire_metric(self, name):
        #in string name
        """
        Push a new metric value to the backend.
        @summary: Push a new metric value to the backend.
        @param name: identifies the metric to be fired
        @type name: string
        @Permission: Write
        @Raise NotImplemented:
        @Raise DoesNotExist:
        @Raise IncorrectState:
        @Raise PermissionDenied:
        @Raise AuthorizationFailed:
        @Raise AuthenticationFailed:
        @Raise Timeout:
        @Raise NoSuccess:
        @Note: notes to the fire method of the metric class apply
        @Note: fire can be called for metrics which have been added with add_metric(), and for predefined metrics
        @Note: an "AuthenticationFailed", "AuthorizationFailed" or "PermissionDenied" exception indicates that the current session
                 is not allowed to fire the metric.
        @Note: a "Timeout" or "NoSuccess" exception indicates that the backend was not able to fire the metric.
        @Note: if a metric with that name is not known for the object, a "DoesNotExist" exception is raised.
        @Note: an attempt to fire a metric which is "ReadOnly" results in an "IncorrectState" exception.
        @Note: an attempt to fire a "Final" metric results in an "IncorrectState" exception.

        """
        if type(name) is not str:
            raise BadParameter, "Parameter name is not a string. Type: " + str(type(name))
        try:
            self.delgateObject.fireMetric(name)
        except java.lang.Exception:
            raise self.convertException(e)
        raise NotImplemented, "fire_metric() is not implemented in this object"
