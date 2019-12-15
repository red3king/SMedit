from common import ValueType


class LVOV(object):
    
    def __init__(self, lvov_json):
        self.spec = lvov_json

        self.type = lvov_json["type"]  # ValueType enum
        self.value = None              # type depends on self.type

        valstr = lvov_json["value"]

        if self.type == ValueType.VT_INT:
            self.value = int(valstr)

        elif self.type == ValueType.VT_FLOAT:
            self.value = float(valstr)

        else:   # VT_REF, VT_STR
            self.value = valstr

    def evaluate(self, machine):
        if self.type == ValueType.VT_REF:
            return machine.get_variable(self.value)

        return self.value

