from models.state import State
from models.transition_def import TransitionType
from models.event import Event
from common import ValueType

import random
from abc import *


class RandomChooser(CustomState):
    @staticmethod
    def get_name():
        return "Random Chooser"
        
    @staticmethod
    def get_outgoing_transition_defs():
        return [
            {
                "type": TransitionType.EVENT,
                "event_name": "A",
                "timeout": 0
            },
            {
                "type": TransitionType.EVENT,
                "event_name": "B",
                "timeout": 0
            }
        ]

    @staticmethod
    def get_configuration_definition():
        return [
            {
                "type": ValueType.VT_INT,
                "name": "percent A"
            }
        ]


    def execute_impl(self, vars_dict, trigger_event):      
        percent_a = self.get_config_value("percent A")

        if random.randint(0, 100) < percent_a:
            event = Event("A")
        else:
            event = Event("B")
            
        return self.feed_event(event)
