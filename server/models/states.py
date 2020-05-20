from abc import ABC, abstractmethod

from common import ValueType
from models.srop import *
from models.state_def import StateType
from models.state import *


class StateFactory(object):
    
    def __init__(self):
        self._registry = {}
        self._reset_registry()
    
    def _reset_registry(self):
        self._registry = {
                StateType.INITIAL: InitialState,
                StateType.CODE: CodeState,
                StateType.RETURN: ReturnState,
                StateType.SPAWN: SpawnState,
                StateType.JOIN: JoinState,
            }
        
    def load_custom_state_classes(self, custom_state_class_def_list):
        for csc_def in custom_state_class_def_list:
            self._registry[csc_def.id] = csc_def.ClassObject
    
    def make_state(self, machine, state_def):
        typ = state_def.type
        
        if typ not in self._registry:
            raise Exception("unknown state type " + str(typ))
        
        Class = self._registry[typ]
        return Class(machine, state_def)
        


class InitialState(State):
    # NOTE: State.is_initial() checks the __name__ of state.__class__ to see if it's "InitialState"
    
    def __init__(self, machine, state_def):
        super().__init__(machine, state_def)

    def execute_impl(self, vars_dict, trigger_event):
        return self.srop_if_next()


class CodeState(State):
    def __init__(self, machine, state_def):
        super().__init__(machine, state_def)
        self.snap = globals().copy()
        exec(self.code, self.snap)

    @property
    def code(self):
        return self.state_def.code

    def execute_impl(self, vars_dict, trigger_event):
        # Code states can return events, which will be fed to this machine only.
        # This way, the code state can:
        #   - choose its successor in code
        #   - send data in the event for the next state to process via trigger_event, if desired.
        
        self.snap['vars_dict'] = vars_dict
        self.snap['trigger_event'] = trigger_event
        exec('event = code_func(vars_dict, trigger_event)', self.snap)
        del self.snap['vars_dict']
        del self.snap['trigger_event']

        event = self.snap['event']
        srop = self.feed_event(event)
        
        if srop is not None:
            return srop
        
        return self.srop_if_next()


class JoinState(State):
    '''
        fire the join event
            this marks this machine as waiting for another machine in the project
            return JoinSrop, which basically does nothing
            when the project sees that the other machine has finished, it calls a method on the machine,
                which runs the joinsrop, 
    '''

    def __init__(self, machine, state_def):
        super().__init__(machine, state_def)

    @property
    def join_pid_variable(self):
        return self.state_def.join_pid_variable
    
    @property 
    def is_joining(self):
        return True

    @property
    def join_result_variable(self):
        return self.state_def.join_result_variable

    def execute_impl(self, vars_dict, trigger_event):
        pid = self.machine.get_variable(self.join_pid_variable)
        return_val = self.machine.get_child_result(pid)
    
        if return_val is None:
            # if the child hasn't finished, return JoinSROP to wait for it
            return JoinSROP(self.machine, self.join_result_variable)

        # otherwise, assign return value and go to next state
        self.machine.set_variable(self.join_result_variable, return_val)
        return self.srop_if_next()


class SpawnState(State):
    '''
        If async:
            -launch task, store child pid in launch_result_variable
            -srop if next to next state

        If synch:
            -launch task, don't exit
            -task return value gets stored in launch_result_variable
            -then srop if next
    '''
    
    def __init__(self, machine, state_def):
        super().__init__(machine, state_def)

    @property
    def launch_synchronous(self):
        return self.state_def.launch_synchronous

    @property
    def is_joining(self):
        return self.launch_synchronous

    @property
    def join_pid_variable(self):
        return '__SpawnState_join_pid_variable__'

    @property
    def task_name(self):
        return self.state_def.launch_task_name

    @property
    def task_args(self):
        return self.state_def.launch_args

    @property
    def result_variable(self):
        return self.state_def.launch_result_variable

    def execute_impl(self, vars_dict, trigger_event):
        
        # Turn self.task_name and self.task_args from LVOV / Arg objects
        # into concrete values given the current machine's variables:
        task_name_str = self.task_name.evaluate(self.machine)        
        task_args_dict = {}

        for arg in self.task_args:
            task_args_dict[arg.variable_name] = arg.argument_value.evaluate(self.machine)
        
        # launch
        child_pid = self.machine.send_spawn_request(task_name_str, task_args_dict)

        if not self.launch_synchronous:
            self.machine.set_variable(self.result_variable, child_pid)
            return self.srop_if_next()
       
        else:
            self.machine.set_variable(self.join_pid_variable, child_pid)
            return JoinSROP(self.machine, self.result_variable)


class ReturnState(State):
    # Terminal state. May return a value, or None
    
    def __init__(self, machine, state_def):
        super().__init__(machine, state_def)

    @property
    def has_return_value(self):
        return self.state_def.has_return_value

    @property
    def return_value_lvov(self):
        return self.state_def.return_value

    def execute_impl(self, vars_dict, trigger_event):
        return_value = None

        if self.has_return_value:
            val = self.return_value_lvov.value

            if self.return_value_lvov.type == ValueType.VT_REF:
                # return contents of variable
                return_value = self.machine.get_variable(val)

            else:
                # Return a constant
                return_value = val

        self.machine.send_finished_signal(return_value)

