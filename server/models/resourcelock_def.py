

class ResourceLockDef(object):
    
    def __init__(self, resourcelock_json):
        self.id = resourcelock_json["id"]
        self.resource_id = resourcelock_json["resource"]
        
        self.x = resourcelock_json["x"]
        self.y = resourcelock_json["y"]
        self.w = resourcelock_json["w"]
        self.h = resourcelock_json["h"]
        

def box_contains(box_entity_def, x, y):
    
    if x < box_entity_def.x:
        return False
    
    if x > box_entity_def.x + box_entity_def.w:
        return False
    
    if y < box_entity_def.y:
        return False
    
    if y > box_entity_def.y + box_entity_def.h:
        return False
    
    return True


def resourcelock_contains_state(resourcelock_def, state_def):
    state_center_x = state_def.x + state_def.w / 2
    state_center_y = state_def.y + state_def.h / 2
    
    return box_contains(resourcelock_def, state_center_x, state_center_y)

