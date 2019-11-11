from models.state_def import StateType, StateDef
from models.transition_def import TransitionDef


class MachineDef(object):

    def __init__(self, machine_json, file_name_to_data):
        self.spec = machine_json

        self.state_defs = []                                # list<StateDef>
        self.transition_defs = []                           # list<TransitionDef>
        self.resourcelock_defs = []                         # list<ResourceLockDef>
        self.initial_state_def = None                       # StateDef

        self.id = machine_json["id"]                        # uint
        self.name = machine_json["name"]                    # str
        self.run_on_start = machine_json["run_on_start"]    # bool

        for state_json in machine_json["states"]:
            state_def = StateDef(state_json, file_name_to_data)
            
            if state_def.type == StateType.INITIAL:
                self.initial_state_def = state_def

            self.state_defs.append(state_def)

        for transition_json in machine_json["transitions"]:
            self.transition_defs.append(TransitionDef(transition_json))


