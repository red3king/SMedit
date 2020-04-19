from abc import ABC, abstractmethod

from utils import Signal
from broadcasts import StateChangeBroadcast
from models.transition import TransitionFactory
from models.transition_def import TransitionType
from models.srop import *


class Machine(object):

    def __init__(self, state_factory, machine_def, machine_id, machine_args=None):
        self.state_factory = state_factory
        self.machine_def = machine_def
        self.current_state = None
        self.vars_dict = {} 
        self.current_operation = None
        self.states_map = {}  # dict<state, dict<transition, state>>
        self.initial_state = None
        self.id = machine_id
        self.child_results = {}  # dict<int, object> mapping pid to return value

        self.broadcast_signal = Signal()              # (Broadcast)
        self.finished_signal = Signal()               # (Machine (self), object return val)
        self.spawn_request_signal = Signal()          # (machine (self), string name of task, dict<str, object> args)
        self.attempt_acquire_locks_signal = Signal()  # (list<resource ids>)
        self.release_locks_signal = Signal()          # (list<resource ids>)
        
        # construct State objects
        id_to_state = {}
        for state_def in machine_def.state_defs:
            state = self.state_factory.make_state(self, state_def)
            self.states_map[state] = {}
            id_to_state[state.id] = state

            if state.is_initial:
                self.initial_state = state

        # construct Transition objects
        transitions = []
        for transition_def in machine_def.transition_defs:
            transition = TransitionFactory.make_transition(transition_def)
            transitions.append(transition)

        # build states_map
        for from_state in self.states_map.keys():
            state_transitions = filter(lambda t: t.from_state_id == from_state.id, transitions)
            self.states_map[from_state] = {trans: id_to_state[trans.to_state_id] for trans in state_transitions}

        if machine_args is not None:
            for var_name in machine_args:
                val = machine_args[var_name]
                self.set_variable(var_name, val)

    def dump_state(self):
        return {
            "id": self.id,
            "machine_def_id": self.machine_def.id,
            "current_state_def_id": self.current_state.id
        }

    def log(self, message):
        print(self.describe() + ": " + message)

    def describe(self):
        return "Machine-" + str(self.id) + "-[" + self.machine_def.name + "]"

    def run_srop(self, srop):
        self.log("run srop " + srop.describe())

        if self.current_operation is not None:
            # if new srop has priority, stop/replace current one.
            if self.current_operation.priority > srop.priority:
                return
            
            self.current_operation.pause()

        self.current_operation = srop
        srop.finished += self.on_srop_finished
        srop.start() 

    def on_srop_finished(self):
        # un-set current operation when done so lower priority srop 
        # may start afterwards
        self.log("srop finished: " + self.current_operation.describe())
        self.current_operation = None

    def feed_event(self, event):
        states = self.get_next_states(self.current_state, TransitionType.EVENT)

        for transition in states:
            if transition.match_event(event):
                state = states[transition]
                return EventRunNextStateROP(self, state)

        return None

    def notify_child_returned(self, child_id, return_value):
        child_pid_match = False

        if self.current_state.is_joining:
            child_pid = self.get_variable(self.current_state.join_pid_variable)
            child_pid_match = child_id == child_pid
        
        if child_pid_match:
            # if we are joining for this child, eat the return value and go to next state
            # in this case, current_operation will be a JoinSrop
            self.current_operation.notify_machine_finished(return_value)    

        else:
            # otherwise, save the return value for the eventual join later.
            self.child_results[child_id] = return_value

    def notify_pending_locks_acquired(self):
        # if we are waiting for locks, the current operation is a run next state operation
        # which should be notified
        self.current_operation.notify_pending_locks_acquired()

    def get_child_result(self, child_id):
        if child_id in self.child_results:
            val = self.child_results[child_id]
            del self.child_results[child_id]
            return val

        return None

    def start(self):
        self.log("start()")

        if self.current_operation is not None:
            raise Exception("wtf already started??")
        
        srop = RunNextStateROP(self, self.initial_state)
        self.run_srop(srop)
    
    def stop(self):
        self.log("stop()")
        if self.current_operation is not None:
            self.current_operation.stop()

    def pause(self):
        self.log("pause()")
        if self.current_operation is not None:
            self.current_operation.pause()
    
    def unpause(self):
        self.log("unpause()")
        if self.current_operation is not None:
            self.current_operation.unpause()

    def set_next_state(self, next_state):
        self.log("set_next_state(" + next_state.describe() + ")")
        self.current_state = next_state

    def update(self):
        self.log("update()")
        
        #moved below so broadcast for entering terminal/return state
        # comes before we execute it, otherwise the state change signal 
        # gets disconnected before sending the signal.
        #srop = self.current_state.execute(self.vars_dict)
        
        broadcast = StateChangeBroadcast(self.id, self.current_state.id, self.vars_dict)
        self.broadcast_signal(broadcast)
        
        srop = self.current_state.execute(self.vars_dict)

        return srop

    def get_next_states(self, from_state, transition_type=None):
        '''return map of { transition, state } for outgoing transitions to possible next states
        specifying transition_type will restrict to that kind of outgoing transition only.
        '''

        if transition_type == None:
            return self.states_map[from_state]

        result = {}
        for trans in self.states_map[from_state].keys():
            if trans.type == transition_type:
                result[trans] = self.states_map[from_state][trans]

        return result

    def srop_if_next(self):
        '''if a state has an immediate successor (no waiting for events or timeout, etc), 
        return SROP to that state. If timeout, return SROP for that'''
        states = self.get_next_states(self.current_state)

        successor_state = None
        timeout_state = None

        self.log(str(states))

        for transition in states:
            state = states[transition]

            if transition.type == TransitionType.TIMEOUT:
                timeout_state = state

            elif transition.type == TransitionType.CATCHALL:
                successor_state = state

        if successor_state is not None:
            return RunNextStateROP(self, successor_state)

        elif timeout_state is not None:
            return RunNextStateROP(self, timeout_state, transition.timeout)

        return None
   
    def set_variable(self, variable_name, variable_value):
        self.vars_dict[variable_name] = variable_value

    def get_variable(self, variable_name):
        return self.vars_dict[variable_name]

    def send_spawn_request(self, task_name_str, task_args_dict):
        self.log("send_spawn_request(" + task_name_str + ")")
        return self.spawn_request_signal(self, task_name_str, task_args_dict)

    def send_finished_signal(self, return_value):
        self.log("send_finished_signal()")
        self.finished_signal(self, return_value)

    def send_lock_request_signal(self, resource_ids_list):
        self.log("send_lock_request_signal()")
        return self.attempt_acquire_locks_signal(self, resource_ids_list)
    
    def send_release_locks_signal(self, resource_ids_list):
        self.log("send_release_locks_signal()")
        self.release_locks_signal(self, resource_ids_list)
    
