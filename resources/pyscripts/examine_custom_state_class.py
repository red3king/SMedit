import json
from models.state import CustomState

'''
    Execs user's custom state class definition files,
    and returns information about the custom state class within.
'''

def execute(custom_state_class_path):
    
    fh = open(custom_state_class_path, "r")
    data = fh.read()
    fh.close()
    
    global_dict = globals()
    
    # Not passing a local dict means code is executed at module level,
    # how it should be for these classes loaded from files,
    # assuming I understand correctly: 
    # https://docs.python.org/3/library/functions.html#exec
    
    exec(data, global_dict)
        
    for varname in global_dict:
        var = global_dict[varname]
        
        if isinstance(var, type) and issubclass(var, CustomState) and var is not CustomState:
            return analyze_cust_state(var)
            
    raise Exception("no custom state class found in " + custom_state_class_path)
    
    
def analyze_cust_state(CustomStateSubclass):
    jdict = {
        "name": CustomStateSubclass.get_name(),
        "outgoing_transitions": CustomStateSubclass.get_outgoing_transition_defs(),
        "configuration_definition": CustomStateSubclass.get_configuration_definition()
    }
    
    return json.dumps(jdict)
