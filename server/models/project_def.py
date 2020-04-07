import json

from common import InadequateUserException
from models.resource_def import ResourceDef
from models.machine_def import MachineDef
from models.custom_state_class_def import CustomStateClassDef


class ProjectDef(object):

    def __init__(self, file_name_to_data):
        self.files = file_name_to_data
        self.spec = {}
        self.resource_defs = []
        self.machine_defs = []
        self.csc_defs = []
        
        if "project.json" not in self.files:
            raise InadequateUserException("cant init project def: project.json missing")

        self.spec = json.loads(file_name_to_data["project.json"])
        
        # Resources
        for resource_json in self.spec["resources"]:
            self.resource_defs.append(ResourceDef(resource_json, file_name_to_data))
        
        # Custom State Classes
        for custom_state_json in self.spec["custom_state_classes"]:
            self.csc_defs.append(CustomStateClassDef(custom_state_json, file_name_to_data))
        
        # Machines
        for machine_json in self.spec["machines"]:
            self.machine_defs.append(MachineDef(machine_json, file_name_to_data))

    def get_custom_state_defs(self):
        return self.csc_defs

    def get_machine_def(self, name):
        for machine_def in self.machine_defs:
            if machine_def.name == name:
                return machine_def

