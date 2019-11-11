from abc import ABC, abstractmethod
from models.srop import *
from models.state_def import StateType


class State(ABC):
    def __init__(self, machine, state_def):
        self.state_def = state_def
        self.machine = machine

    def describe(self):
        return self.__class__.__name__ + " " + str(self.id)

    @property 
    def id(self):
        return self.state_def.id

    @property
    def is_initial(self):
        return self.__class__ == InitialState

    def execute(self, vars_dict, trigger_event=None):
        # do stuff, then return SROP if next state else return None
        return self.execute_impl(vars_dict, trigger_event)

    def srop_if_next(self):
        return self.machine.srop_if_next()

    @abstractmethod
    def execute_impl(self, vars_dict, trigger_event):
        pass


class StateFactory(object):
    
    @classmethod
    def make_state(cls, machine, state_def):
        typ = state_def.type

        if typ == StateType.INITIAL:
            return InitialState(machine, state_def)

        elif typ == StateType.CODE:
            return CodeState(machine, state_def)

        elif typ == StateType.RETURN:
            return ReturnState(machine, state_def)

        elif typ == StateType.SPAWN:
            return SpawnState(machine, state_def)

        elif typ == StateType.JOIN:
            return JoinState(machine, state_def)

        else:
            raise Exception("unknown state type")


class InitialState(State):
    def __init__(self, machine, state_def):
        super().__init__(machine, state_def)

    def execute_impl(self, vars_dict, trigger_event):
        return self.srop_if_next()


class CodeState(State):
    def __init__(self, machine, state_def):
        super().__init__(machine, state_def)
        
        local_dict = {}
        exec(self.code, globals(), local_dict)
        self.code_func = local_dict["code_func"]

    @property
    def code(self):
        return self.state_def.code

    def execute_impl(self, vars_dict, trigger_event):
        # Code states can return events, which will be fed to this machine only.
        # This way, the code state can:
        #   - choose its successor in code
        #   - send data in the event for the next state to process via trigger_event, if desired.

        event = self.code_func(vars_dict, trigger_event)

        if event is not None:
            self.machine.feed_event(event)

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
    def join_result_variable(self):
        return self.state_def.join_result_variable

    def execute_impl(self, vars_dict, trigger_event):
        pid = self.machine.get_variable(self.join_pid_variable)
        return_val = self.machine.get_child_result(pid)
    
        if return_val is None:
            # if the child hasn't finished, return JoinSROP to wait for it
            return JoinSROP(self.machine, self.join_pid_variable, self.join_result_variable)

        # otherwise, assign return value and go to next state
        self.machine.set_variable(join_result_variable, return_val)
        return self.srop_if_next()


class SpawnState(JoinState):

    # Subclass from JoinState to run its execute_impl if synchronous

    def __init__(self, machine, state_def):
        super().__init__(machine, state_def)

    @property
    def launch_synchronous(self):
        return self.state_def.launch_synchronous

    def execute_impl(self, vars_dict, trigger_event):
        # if async, fire spawn event, and srop if next
        # if synch, fire spawn event, do whatever JoinState does
       
        self.machine.send_spawn_request(task_name, task_args)

        if not self.launch_synchronous:
            return self.srop_if_next()
       
        else:
            return super().execute_impl(vars_dict, trigger_event)


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

