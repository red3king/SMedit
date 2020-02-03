from abc import ABC, abstractmethod

from common import ValueType


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

    @property
    def is_joining(self):  # overridden where true
        return False

    def execute(self, vars_dict, trigger_event=None):
        # do stuff, then return SROP if next state else return None
        return self.execute_impl(vars_dict, trigger_event)

    def srop_if_next(self):
        return self.machine.srop_if_next()

    @abstractmethod
    def execute_impl(self, vars_dict, trigger_event):
        pass
    

class CustomState(State):
    @staticmethod
    @abstractmethod
    def get_outgoing_transition_defs():
        '''
        Return list of transition def dicts (same as TransitionDef's transition_json 
        but without the id field ), eg:
        
        return [
            {
                "type": TransitionType.EVENT,
                "event_name": "on success",
                "timeout": 0
            },
            {
                "type": TransitionType.EVENT,
                "event_name": "on failure",
                "timeout": 0
            }
        ]
        '''
        pass

    @staticmethod
    @abstractmethod
    def get_name():
        pass
