from abc import ABC, abstractmethod

from inspect import isclass
from models.state import CustomState


class CustomStateClassDef(ABC):
    
    def __init__(self, csc_def_json, file_name_to_data):
        self.spec = csc_def_json
        self.id = csc_def_json["id"]
        self.name = csc_def_json["name"]
        
        local_dict = {}
        self.class_code = file_name_to_data[self.get_py_filename()]
        self.snap = globals()
        exec(self.class_code, self.snap)
        
        for varname in self.snap:
            val = self.snap[varname]
            
            if not isclass(val):
                continue
            
            if issubclass(val, CustomState):
                self._classname = varname
        
        def instantiator(machine, state_def):
            tmp = locals()
            exec('instance = ' + self._classname + '(machine, state_def)', self.snap, tmp)
            return tmp['instance']
        
        self.ClassObject = instantiator
        
    def get_py_filename(self):
        return "csc_" + str(self.id) + ".py"
