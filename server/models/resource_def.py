
class ResourceDef(object):

    def __init__(self, json_dict):
        self.name = json_dict["name"]       # str
        self.id = json_dict["id"]           # uint

        self.python_data = ""  # TODO
