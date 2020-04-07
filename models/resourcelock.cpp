#include "resourcelock.h"
#include "utils.h"


ResourceLock::ResourceLock(unsigned int id) : BoxEntity(id) { }


ResourceLock::ResourceLock(json jdata) : BoxEntity(jdata) { }


json ResourceLock::to_json()
{
    json jdata = BoxEntity::to_json();
    jdata["resource"] = resource->id;
    return jdata;
}


bool ResourceLock::contains_state(State* state)
{
    float state_ctx, state_cty;
    state->get_center(state_ctx, state_cty);
    return point_in_box(state_ctx, state_cty, x, y, w, h);
}
