import binascii

from utils import Signal
from common import InadequateUserException
from broadcasts import MachineCreateBroadcast, MachineDeleteBroadcast
from models.project_def import ProjectDef
from models.resource import Resource
from models.machine import Machine


class Project(object):

    def __init__(self):
        self.files = {}  # maps filename to string file data
        self.project_def =  None 

        self.running_machines = []
        self.child_to_parent = {}       # dict<machine, machine> who launched who

        self.resources = []
        self._loaded = False
        self.broadcast_signal = Signal()
        self.machine_id_counter = 0

    @property
    def loaded(self):
        return self._loaded

    def on_broadcast(self, broadcast):
        print("PROJECT GOT BCAST = " + str(broadcast))
        self.broadcast_signal(broadcast)

    def on_machine_finished(self, machine, return_value):
        if return_value is not None:
            parent = self.child_to_parent[machine]
            parent.notify_child_returned(machine.id, return_value)
        
        self.delete_machine(machine)

        if machine in self.child_to_parent:
            del self.child_to_parent[machine]
        else:
            pass  # happens when an initial machine is finished, has no parent.

    def on_machine_spawn_request(self, machine, machine_name, machine_args):
        machine_def = self.project_def.get_machine_def(machine_name)
        child = self.create_machine(machine_def, machine_args)
        self.child_to_parent[child] = machine 
        child.unpause()

    def create_machine(self, machine_def, machine_args=None):
        # Creates a machine, but does not start running it
        self.machine_id_counter += 1
        machine = Machine(machine_def, self.machine_id_counter, machine_args)

        machine.broadcast_signal += self.on_broadcast
        machine.finished_signal += self.on_machine_finished
        machine.spawn_request_signal += self.on_machine_spawn_request
        #machine.join_signal += self.on_machine_join

        broadcast = MachineCreateBroadcast(machine.id, machine_def.id)
        self.broadcast_signal(broadcast)
        self.running_machines.append(machine)
        return machine

    def delete_machine(self, machine):
        machine.broadcast_signal -= self.on_broadcast
        machine.finished_signal -= self.on_machine_finished
        machine.spawn_request_signal -= self.on_machine_spawn_request
        #machine.join_signal -= self.on_machine_join

        broadcast = MachineDeleteBroadcast(machine.id)
        self.broadcast_signal(broadcast)
        self.running_machines.remove(machine)

    def load(self):
        # load defs
        project_def = ProjectDef(self.files)
        self.project_def = project_def

        # resources
        for resource_def in project_def.resource_defs:
            self.resources.append(Resource(resource_def))
    
        # machines
        for machine_def in project_def.machine_defs:
            if machine_def.run_on_start:
                self.create_machine(machine_def)
        
        self._loaded = True

    def add_file(self, name, data):
        if self.loaded:
            raise InadequateUserException("project was already loaded, cannot add file.")

        self.files[name] = data

    def calculate_hash(self):
        hash_val = 0
    
        for data in self.files.values():
            data_bytes = data.encode("ascii")
            hash_val ^= binascii.crc32(data_bytes)
        
        return hash_val

    def pause(self):
        '''stops project temporarily'''
        for machine in self.running_machines:
            machine.pause()

        for resource in self.resources:
            resource.pause()

    def unpause(self):
        '''resumes project execution'''
        for machine in self.running_machines:
            machine.unpause()

        for resource in self.resources:
            resource.unpause()

    def start(self):
        
        #self.unpause()
        for machine in self.running_machines:
            machine.start()

        for resource in self.resources:
            resource.start()

    def stop(self):
        for machine in self.running_machines:
            machine.stop()

        for resource in self.resources:
            resource.stop()

