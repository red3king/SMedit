def code_func(vars_dict, trigger_event=None):
    import random
    from models.event import Event

    if random.randint(0,1):
        return Event("a")

    return Event("b")
