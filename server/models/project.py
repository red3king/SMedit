import binascii

from utils import Signal
from common import InadequateUserException
from broadcasts import MachineCreateBroadcast, MachineDeleteBroadcast
from models.project_def import ProjectDef
from models.resource import Resource
from models.machine import Machine
from models.states import StateFactory


class Project(object):

    def __init__(self):
        self.files = {}  # maps filename to string file data
        self.project_def =  None 

        self.running_machines = []
        self.child_to_parent = {}       # dict<machine, machine> who launched who
        
        self.held_locks = {}            # dict<resource, machine> which machines have which locks
        self.pending_locks = []         # list<tuple<machine, list<resource>> which machines are waiting for which locks

        self.resources = {}             # dict<resource id, resource>
        self._loaded = False
        self.broadcast_signal = Signal()
        self.machine_id_counter = 0
        self.state_factory = None

    @property
    def loaded(self):
        return self._loaded

    def on_broadcast(self, broadcast):
        self.broadcast_signal(broadcast)

    def on_machine_finished(self, machine, return_value):
        if machine in self.child_to_parent:
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
        child.start()
        return child.id

    def on_machine_attempt_locks(self, machine, resource_ids_list):
        # acquire as many of the requested locks as possible
        # if all are acquired, return true, machine may proceed
        # otherwise, it must wait
        
        acquire_later = []
        
        for id in resource_ids_list:
            resource = self.resources[id]
            
            if resource in self.held_locks:
                acquire_later.append(resource)
                continue
            
            self.held_locks[resource] = machine
        
        all_acquired = len(acquire_later) == 0
        
        if not all_acquired:
            self.pending_locks.append((machine, acquire_later))
        
        return all_acquired
    
    def on_machine_release_locks(self, machine, resource_ids_list):
        self._on_machine_release_locks(machine, resource_ids_list)
    
    def _on_machine_release_locks(self, machine, resource_ids_list):
        # release locks
        for rid in resource_ids_list:
            resource = self.resources[rid]
            del self.held_locks[resource]
        
        # let next machine(s) in line acquire locks until we run out
        all_acquired = True
        
        while len(self.pending_locks) > 0 and all_acquired:
            next_machine, resource_list = self.pending_locks[0]
            
            for resource in resource_list:
                if resource in self.held_locks:
                    all_acquired = False
                    continue
                
                self.held_locks[resource] = next_machine
            
            if all_acquired:
                self.pending_locks = self.pending_locks[1:]
                next_machine.notify_pending_locks_acquired()

    def on_event(self, event):
        for machine in self.running_machines:
            machine.do_feed_event(event)

    def create_machine(self, machine_def, machine_args=None):
        # Creates a machine, but does not start running it
        self.machine_id_counter += 1
        machine = Machine(self.state_factory, machine_def, self.machine_id_counter, machine_args)

        machine.broadcast_signal += self.on_broadcast
        machine.finished_signal += self.on_machine_finished
        machine.spawn_request_signal += self.on_machine_spawn_request
        machine.attempt_acquire_locks_signal += self.on_machine_attempt_locks
        machine.release_locks_signal += self.on_machine_release_locks

        broadcast = MachineCreateBroadcast(machine.id, machine_def.id)
        self.broadcast_signal(broadcast)
        self.running_machines.append(machine)
        return machine

    def delete_machine(self, machine):
        machine.broadcast_signal -= self.on_broadcast
        machine.finished_signal -= self.on_machine_finished
        machine.spawn_request_signal -= self.on_machine_spawn_request
        machine.attempt_acquire_locks_signal -= self.on_machine_attempt_locks
        machine.release_locks_signal -= self.on_machine_release_locks        

        broadcast = MachineDeleteBroadcast(machine.id)
        self.broadcast_signal(broadcast)
        self.running_machines.remove(machine)
        
        # release any held locks
        to_release = []
        for k,v in self.held_locks.items():
            if v == machine:
                to_release.append(k.id)
                
        self._on_machine_release_locks(machine, to_release)

    def create_initial_machines(self):
        for machine_def in self.project_def.machine_defs:
            if machine_def.run_on_start:
                self.create_machine(machine_def)
 
    def load(self):
        # load defs
        project_def = ProjectDef(self.files)
        self.project_def = project_def

        # resources
        for resource_def in project_def.resource_defs:
            resource = Resource.create(resource_def)
            resource.event_signal += self.on_event
            self.resources[resource.id] = resource
        
        # custom state classes
        self.state_factory = StateFactory()
        self.state_factory.load_custom_state_classes(self.project_def.get_custom_state_defs())
        
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

        for resource in self.resources.values():
            resource.pause()

    def unpause(self):
        '''resumes project execution'''
        for machine in self.running_machines:
            machine.unpause()

        for resource in self.resources.values():
            resource.unpause()

    def start(self):
        self.create_initial_machines()

        for machine in self.running_machines:
            machine.start()
        
        for resource in self.resources.values():
            resource.start()

    def stop(self):
        for machine in self.running_machines:
            machine.stop()
            self.delete_machine(machine)

        for resource in self.resources.values():
            resource.stop()

    def dump_state(self):
        machines_list = []
        
        for machine in self.running_machines:
            machines_list.append(machine.dump_state())

        return machines_list
