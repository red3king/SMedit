from abc import ABC, abstractmethod
from twisted.internet import reactor
from datetime import datetime

from utils import Signal


class Priority(object):
    JOIN = 10
    RUN_STATE_IMMEDIATE = 10
    EVENT = 9
    RUN_STATE_TIMEOUT = 8
    

class AsyncOperation(ABC):

    # Whenever something triggers code to run later in the twisted event loop,
    # an AsyncOperation (AOP) is used to represent that and control its execution
    # so when the user hits pause, everything can be easliy stopped

    def __init__(self):
        self.paused = False
        self.finished = Signal()  # no arg signal, call this when done to un-set srop from machine.

    def pause(self):       
        self.paused = True
        self.pause_impl()

    def unpause(self):
        self.paused = False
        self.unpause_impl()

    def start(self):
        self.start_impl()
    
    def stop(self):
        self.stop_impl()

    @abstractmethod
    def pause_impl(self):
        pass

    @abstractmethod
    def unpause_impl(self):
        pass

    @abstractmethod
    def start_impl(self):
        pass

    @abstractmethod
    def stop_impl(self):
        pass

    @abstractmethod
    def describe(self):
        pass


class StateResultOperation(AsyncOperation):
    # An AsyncOperation which is the result of executing a Machine's State

    ''' 
    Contains what the state is doing or going to do next. For instance:
        -could return cancellable object representing an in progress io activity
        -could return the name of the state to execute next
        -could return a timeout object which will go to a given state after time elasped, if nothing else happens
        -could return nothing and then the machine stays in this state unless a event moves it to the next one

    '''

    def __init__(self, machine):
        super().__init__()
        self.machine = machine

    @property
    @abstractmethod
    def priority(self):
        pass


class JoinSROP(StateResultOperation):

    def __init__(self, machine, result_var):
        super().__init__(machine)
        self.result_var = result_var

    def describe(self):
        return "srop-join"

    @property
    def priority(self, variable_name):
        return Priority.JOIN

    def pause_impl(self):
        # Since this is triggered by the machine calling notify_machine_finished(),
        # that machine will get paused by its own SROP, so this needs no pause code.
        pass

    def unpause_impl(self):
        pass

    def start_impl(self):
        pass

    def stop_impl(self):
        pass

    def notify_machine_finished(self, return_value):
        # save the return value into variable (is None when user defines no return value):
        self.machine.set_variable(self.result_var, return_value)
        self.finished()

        # srop if next (do the usual next state depending on outgoing transitions behavior)
        srop = self.machine.srop_if_next()
        if srop is not None:
            self.machine.run_srop(srop)


class RunLaterCancelAOP(AsyncOperation):
    '''
        AOP ABC for stuff which, if paused, should be re-started in the same way
        when the user unpauses.
        can either get to work immediately or do it on a timer

        Note: subclasses must call self.set_task_finished() in their task completion callbacks.
    '''

    S_DEFAULT = 0
    S_LAUNCHED = 1
    S_PAUSED = 2

    def __init__(self):
        super().__init__()

        self.state = self.S_DEFAULT

        self.seconds_elapsed = 0.0
        self.start_time = None

    @property
    def delay(self):
        return self.get_delay()

    def pause_impl(self):
        if self.state == self.S_LAUNCHED:
            self.cancel_task()
            self.state = self.S_PAUSED
            now = datetime.now()
            self.seconds_elapsed += (now - self.start_time).total_seconds()
            self.start_time = now

    def unpause_impl(self):
        self.start_impl()

    def start_impl(self):
        if self.state == self.S_PAUSED:
            delay = self.delay - self.seconds_elapsed
        else:
            delay = self.delay
            self.start_time = datetime.now()

        self.launch_task(delay)
        self.state = self.S_LAUNCHED

    def stop_impl(self):
        self.pause_impl()
        self.tear_down_for_stop()

    def tear_down_for_stop(self):
        # override to do teardown when stop() is called if needed.
        pass 

    def set_task_finished(self):
        self.state = self.S_DEFAULT
        self.seconds_elapsed = 0
        self.start_time = None        
        self.finished()
        
    @abstractmethod
    def launch_task(self, seconds_wait):
        '''put launch code here. seconds_wait will be self.delay or less if task was previously held up'''
        pass

    @abstractmethod
    def cancel_task(self):
        pass

    @abstractmethod
    def get_delay(self):
        '''when to schedule the callback, in seconds. zero = schedule for immediate execution'''
        pass


class TwistedRLCAOP(RunLaterCancelAOP):
    '''
    RunLaterCancelAOP for running/scheduling code inside Twisted's reactor
    '''

    def __init__(self):
        super().__init__()
        self.call_id = None

    def launch_task(self, seconds_wait):
        if self.call_id is not None:
            raise Exception("wtf")

        self.call_id = reactor.callLater(seconds_wait, self.callback)

    def cancel_task(self):
        self.call_id.cancel()
        self.call_id = None

    def callback(self):
        self.callback_impl()
        self.set_task_finished()
        self.post_finished()

    @abstractmethod
    def post_finished(self):
        pass

    @abstractmethod
    def callback_impl(self):
        pass


class ResourceTRLCAOP(TwistedRLCAOP):
    # AOP for use by resources
    # Unlike AOPs for state transitions, this will keep running every delay_seconds
    # until stop or pause is called.
    
    def __init__(self, delay_seconds):
        super().__init__()
        self.tick = Signal()    # no params
        self.delay_seconds = delay_seconds

    def post_finished(self):
        self.stop()
        self.start()

    def tear_down_for_stop(self):
        self.call_id = None

    def callback_impl(self):
        self.tick()

    def get_delay(self):
        return self.delay_seconds

    def describe(self):
        return "resource-trlcaop"


class RunNextStateROP(StateResultOperation, RunLaterCancelAOP):
    '''
    RunLaterCancelAOP for running/scheduling code inside Twisted's reactor
    '''

    def __init__(self, machine, next_state, timeout=0, trigger_event=None):
        RunLaterCancelAOP.__init__(self)
        StateResultOperation.__init__(self, machine)
        
        self.next_state = next_state
        self.timeout = timeout
        self.call_id = None
        self.trigger_event = trigger_event

    def launch_task(self, seconds_wait):
        if self.call_id is not None:
            raise Exception("wtf")

        self.call_id = reactor.callLater(seconds_wait, self.callback)

    def cancel_task(self):
        self.call_id.cancel()
        self.call_id = None

    def callback(self):
        current_state = self.machine.current_state
        
        if current_state is None:
            current_lock_ids = set([])
            
        else:
            current_lock_ids = set(current_state.required_resource_ids)
        
        next_lock_ids = set(self.next_state.required_resource_ids)
        
        release_lock_ids = list(current_lock_ids - next_lock_ids)
        acquire_lock_ids = list(next_lock_ids - current_lock_ids)
        
        self.machine.send_release_locks_signal(release_lock_ids)
        can_proceed = self.machine.send_lock_request_signal(acquire_lock_ids)
        
        # LOH i think the issue is that:
        # when the event from the resource comes in,
        # it cancels the currently-running srop
        # the current srop is a state change srop that didn't call finish(), 
        # so it stayed current ( ? )
        
        if not can_proceed:
            return
        
        self.go_next_state()
    
    def go_next_state(self):
        self.machine.set_next_state(self.next_state)
        next_srop = self.machine.update(self.trigger_event)
        
        self.set_task_finished()

        if next_srop is not None:
            self.machine.run_srop(next_srop)

    def get_delay(self):
        return self.timeout

    def describe(self):
        return "srop-next-state" + "->" + self.next_state.describe()

    @property
    def priority(self):
        return Priority.RUN_STATE_IMMEDIATE if self.timeout == 0.0 else Priority.RUN_STATE_TIMEOUT

    def notify_pending_locks_acquired(self):
        # all locks have finally been acquired, time to run the next state
        self.go_next_state()
        
        
class EventRunNextStateROP(RunNextStateROP):
    @property
    def priority(self):
        # lower priority than join SROP so the join state won't exit 
        # until the child machine has finished, even when events come
        # through
        return Priority.EVENT

        
