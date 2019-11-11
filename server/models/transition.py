from abc import ABC, abstractmethod

from models.transition_def import TransitionType


class Transition(ABC):

    @property
    def from_state_id(self):
        return self.transition_def.from_state_id

    @property
    def to_state_id(self):
        return self.transition_def.to_state_id

    @property
    def type(self):
        return self.transition_def.type

    @property
    def is_event_transition(self):
        return self.__class__ == EventTransition

    def __init__(self, transition_def):
        self.transition_def = transition_def


class EventTransition(Transition):

    def __init__(self, transition_def):
        super().__init__(transition_def)

    def match_event(self, event):
        return self.transition_def.event_name == event.name


class TimeoutTransition(Transition):

    @property
    def timeout(self):
        return self.transition_def.timeout

    def __init__(self, transition_def):
        super().__init__(transition_def)


class CatchallTransition(Transition):
    def __init__(self, transition_def):
        super().__init__(transition_def)


class TransitionFactory(object):
    @classmethod
    def make_transition(cls, transition_def):
        if transition_def.type == TransitionType.EVENT:
            return EventTransition(transition_def)

        elif transition_def.type == TransitionType.TIMEOUT:
            return TimeoutTransition(transition_def)

        elif transition_def.type == TransitionType.CATCHALL:
            return CatchallTransition(transition_def)

        else:
            raise Exception("unknown TransitionType")


