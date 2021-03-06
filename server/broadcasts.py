from abc import ABC, abstractmethod


class BroadcastType(object):
    MACHINE_STATE_CHANGE = 0
    MACHINE_CREATE = 1
    MACHINE_DELETE = 2
    PRINT_MESSAGE = 3


class Broadcast(ABC):
    
    @property
    @abstractmethod
    def broadcast_type(self):
        pass

    @abstractmethod
    def to_dict_impl(self):
        pass

    def to_dict(self):
        return {
            "type": self.broadcast_type,
            "data": self.to_dict_impl()
        }



class MachineCreateBroadcast(Broadcast):
    def __init__(self, machine_id, def_id):
        self.machine_id = machine_id
        self.def_id = def_id

    @property
    def broadcast_type(self):
        return BroadcastType.MACHINE_CREATE

    def to_dict_impl(self):
        return {
            "machine_id": self.machine_id,
            "def_id": self.def_id
        }


class MachineDeleteBroadcast(Broadcast):
    def __init__(self, machine_id):
        self.machine_id = machine_id

    @property
    def broadcast_type(self):
        return BroadcastType.MACHINE_DELETE

    def to_dict_impl(self):
        return {
            "machine_id": self.machine_id
        }


class StateChangeBroadcast(Broadcast):
    def __init__(self, machine_id, state_id, vars_dict):
        self.machine_id = machine_id
        self.state_id = state_id
        self.vars_dict = vars_dict

    @property
    def broadcast_type(self):
        return BroadcastType.MACHINE_STATE_CHANGE

    def to_dict_impl(self):
        broadcast_dict = {
            "machine_id": self.machine_id,
            "state_id": self.state_id,
            "vars": self.vars_dict
        }

        return broadcast_dict


class PrintMessageBroadcast(Broadcast):
    def __init__(self, machine_id, state_id, log_level, message):
        self.log_level = log_level
        self.machine_id = machine_id
        self.state_id = state_id
        self.message = message

    @property
    def broadcast_type(self):
        return BroadcastType.PRINT_MESSAGE

    def to_dict_impl(self):
        broadcast_dict = {
            "machine_id": self.machine_id,
            "state_id": self.state_id,
            "message": self.message,
            "level": self.log_level
        }
        
        return broadcast_dict

