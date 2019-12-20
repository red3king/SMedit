from models.resource import *


class ResourceDef(object):

    @property 
    def ResourceSubclass(self):
        return self._subclass

    @property
    def name(self):
        return self._name

    @property
    def id(self):
        return self._id

    def __init__(self, json_dict, file_name_to_data):
        self._name = json_dict["name"]       # str
        self._id = json_dict["id"]           # uint

        self.code = file_name_to_data[self.name]

        local_dict = {}
        self._subclass = None

        exec(self.code, globals(), local_dict)
        
        for obj_name in local_dict:
            obj = local_dict[obj_name]
            
            if not isinstance(obj, type):
                continue

            if issubclass(obj, Resource) and obj not in PREDEFINED_RESOURCE_CLASSES:
                self._subclass = obj
                break

