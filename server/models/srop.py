from abc import ABC, abstractmethod
from twisted.internet import reactor
from datetime import datetime

from utils import Signal


class Priority(object):
    JOIN = 10
    RUN_STATE_IMMEDIATE = 10
    EVENT = 9
    RUN_STATE_TIMEOUT = 8
    

class StateResultOperation(ABC):
    ''' 
    Contains what the state is doing or going to do next. For instance:
        -could return cancellable object representing an in progress io activity
        -could return the name of the state to execute next
        -could return a timeout object which will go to a given state after time elasped, if nothing else happens
        -could return nothing and then the machine stays in this state unless a event moves it to the next one

    '''

    def __init__(self, machine):
        self.paused = False
        self.machine = machine
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

    @property
    @abstractmethod
    def priority(self):
        pass

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


class JoinSROP(StateResultOperation):

    def __init__(self, machine, pid_var, result_var):
        super().__init__(machine)
        self.pid_var = pid_var
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


class RunLaterCancelROP(StateResultOperation):
    '''
        ROP ABC for stuff which, if paused, should be re-started in the same way
        when the user unpauses.
        can either get to work immediately or do it on a timer

        Note: subclasses must call self.set_task_finished() in their task completion callbacks.
    '''

    S_DEFAULT = 0
    S_LAUNCHED = 1
    S_PAUSED = 2

    def __init__(self, machine):
        super().__init__(machine)

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


class TwistedRLCROP(RunLaterCancelROP):
    '''
    RunLaterCancelROP for running/scheduling code inside Twisted's reactor
    '''

    def __init__(self, machine):
        super().__init__(machine)
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


class RunNextStateROP(TwistedRLCROP):

    def __init__(self, machine, next_state, timeout=0):
        super().__init__(machine)
        self.next_state = next_state
        self.timeout = timeout

    def callback_impl(self):
        self.machine.set_next_state(self.next_state)
        self.next_srop = self.machine.update()

    def post_finished(self):
        if self.next_srop is not None:
            self.machine.run_srop(self.next_srop)

    def get_delay(self):
        return self.timeout

    def describe(self):
        return "srop-next-state" + "->" + self.next_state.describe()

    @property
    def priority(self):
        return Priority.RUN_STATE_IMMEDIATE if self.timeout == 0.0 else Priority.RUN_STATE_TIMEOUT


class EventRunNextStateROP(RunNextStateROP):
    @property
    def priority(self):
        # lower priority than join SROP so the join state won't exit 
        # until the child machine has finished, even when events come
        # through
        return Priority.EVENT

