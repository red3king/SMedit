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
        exec(self.class_code, globals(), local_dict)
        self.ClassObject = None
        
        for varname in local_dict:
            obj = local_dict[varname]
           
            if not isclass(obj):
                continue
            
            if issubclass(obj, CustomState):
                self.ClassObject = obj
                break
        
    def get_py_filename(self):
        return "csc_" + str(self.id) + ".py"
