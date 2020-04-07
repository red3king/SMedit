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
        return self.__class__.__name__ == "InitialState"

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
    
    def feed_event(self, event):
        if event is not None:
            return self.machine.feed_event(event)

        return None
    

class CustomState(State):

    @staticmethod
    @abstractmethod
    def get_name():
        pass    
    
    def get_config_value(self, name):
        return self.state_def.get_custom_config_value(name)
    
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
    def get_configuration_definition():
        
        '''
        Return list of parameter definitions.
        
        ValueType.REF and ANY not allowed here.
        
        example:
        
        return [
            {
                "type": ValueType.STR,
                "name": "post_url"
            },
            {
                "type": ValueType.FLOAT,
                "name": "timeout_sec"
            }
        ]
        
        '''
        pass


class CustomStateQuick(CustomState):
    # Simple custom state which works like a Code state
    
    def execute_impl(self, vars_dict, trigger_event):
        event = self.code_func(vars_dict, trigger_event)
        self.feed_event(event)
        return self.srop_if_next()


    
