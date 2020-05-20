from models.resource import *


class ResourceDef(object):

    @property
    def name(self):
        return self._name

    @property
    def id(self):
        return self._id

    def create(self):
        # create instance of subclass
        codestr = 'instance = ' + self._subclassname + '(resource_def)'
        self.snap['resource_def'] = self
        exec(codestr, self.snap)
        return self.snap['instance']

    def __init__(self, json_dict, file_name_to_data):
        self._name = json_dict["name"]       # str
        self._id = json_dict["id"]           # uint
        self.code = file_name_to_data[self.name]

        self.snap = globals().copy()
        exec(self.code, self.snap)
        
        for name in self.snap:
            val = self.snap[name]
            
            if not isinstance(val, type):
                continue
            
            if issubclass(val, Resource) and val not in PREDEFINED_RESOURCE_CLASSES:
                self._subclassname = name
