from abc import ABC, abstractmethod

from models.datatypes.arg_def import ArgDef
from models.datatypes.lvov import LVOV
from models.datatypes.arg import Arg


class StateType(object):
    INITIAL = 0
    CODE = 1
    RETURN = 2
    SPAWN = 3
    JOIN = 4


class StateDef(ABC):

    def __init__(self, state_json, file_name_to_data):
        self.spec = state_json

        self.code = ""  # code states only

        self.id = state_json["id"]
        self.type = state_json["type"]                                          # enum, above
        self.name = state_json["name"]                                          # str
        self.initial_args = []                                                  # list<ArgDef>
        self.has_return_value = state_json["has_return_value"]                  # bool
        self.return_value = LVOV(state_json["return_value"])                    # LVOV
        self.launch_synchronous = state_json["launch_synchronous"]              # bool
        self.launch_task_name = LVOV(state_json["launch_task_name"])            # LVOV
        self.launch_args = []                                                   # list<ARG>
        self.launch_result_variable = state_json["launch_result_variable"]      # str
        self.join_pid_variable = state_json["join_pid_variable"]                # str
        self.join_result_variable = state_json["join_result_variable"]          # str

        self.incoming_transition_defs = []
        self.outgoing_transition_defs = []

        # initial_args
        for arg_def_json in state_json["initial_args"]:
            self.initial_args.append(ArgDef(arg_def_json))

        # launch args
        for arg_json in state_json["launch_args"]:
            self.launch_args.append(Arg(arg_json))
         
        # if code, look up python file from file_name_to_data
        if self.type == StateType.CODE:
            self.code = file_name_to_data[self.get_code_filename()]

    def get_code_filename(self):
        # only used for Code states. return name of py file containing execute_impl
        return self.name + "_" + str(self.id) + ".py"

    def add_incoming_tdef(self, transition_def):
        self.incoming_transition_defs.append(transition_def)

    def add_outgoing_tdef(self, transition_def):
        self.outgoing_transition_defs.append(transition_def)

