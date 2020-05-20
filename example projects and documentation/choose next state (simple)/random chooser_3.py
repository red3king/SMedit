import random
from models.event import Event


def code_func(vars_dict, trigger_event=None):
    if random.randint(0,1):
        return Event("a")

    return Event("b")
