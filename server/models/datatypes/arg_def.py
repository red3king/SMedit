from common import ValueType


class ArgDef(object):
    def __init__(self, arg_def_json):
        self.spec = arg_def_json
        
        self.variable_name = arg_def_json["variable_name"]  # string
        self.value_type = arg_def_json["value_type"]  # ValueType enum



