from models.datatypes.lvov import LVOV


class Arg(object):

    def __init__(self, arg_json):
        self.spec = arg_json

        self.variable_name = arg_json["variable_name"]  # str
        self.argument_value = LVOV(arg_json["argument_value"])  # lvov

