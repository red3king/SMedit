


class TransitionType(object):
    EVENT = 0
    TIMEOUT = 1
    CATCHALL = 2


class TransitionDef(object):

    def __init__(self, transition_json):
        self.spec = transition_json

        self.id = transition_json["id"]                         # uint
        self.type = transition_json["type"]                     # TransitionType enum
        self.event_name = transition_json["event_name"]         # str
        self.timeout = transition_json["timeout"]               # float

    @property
    def from_state_id(self):                                    # uint
        return self.spec["from_state"]

    @property
    def to_state_id(self):
        return self.spec["to_state"]                            # uint
