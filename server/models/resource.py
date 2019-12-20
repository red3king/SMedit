from twisted.internet import task, reactor
from abc import ABC, abstractmethod

from utils import Signal
from models.event import Event
from models.srop import ResourceTRLCAOP


class Resource(ABC):

    @staticmethod
    def create(resource_def):
        Subclass = resource_def.ResourceSubclass
        return Subclass(resource_def)

    @property
    def name(self):
        return self.resource_def.name

    @property
    def id(self):
        return self.resource_def.id

    def __init__(self, resource_def):
        self.resource_def = resource_def
        self.event_signal = Signal()    # (Event)

    def pause(self):
        # temporarily stop doing stuff
        self.pause_impl()

    def unpause(self):
        # stop temporarily stop doing stuff
        self.unpause_impl()

    def start(self):
        # begin doing stuff
        self.start_impl()

    def stop(self):
        # stop doing stuff, clear state back to initial
        # should be able to be started again as if new
        self.stop_impl()

    def fire_event(self, name, data=None):
        if data is None:
            data = {}
        event = Event(name, data)
        self.event_signal(event)

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



class PeriodicScanResource(Resource):
    
    def __init__(self, resource_def):
        super().__init__(resource_def)
        self.aop = ResourceTRLCAOP(self.duration_milliseconds/1000.0)
        self.aop.tick += self.on_tick

    def on_tick(self):
        self.on_tick_impl()

    @abstractmethod
    def on_tick_impl(self):
        pass

    @property
    @abstractmethod
    def duration_milliseconds(self):
        pass

    def start_impl(self):
        self.aop.start() 
    
    def stop_impl(self):
        self.aop.stop()

    def pause_impl(self):
        self.aop.pause()

    def unpause_impl(self):
        self.aop.unpause()


PREDEFINED_RESOURCE_CLASSES = [Resource, PeriodicScanResource]
