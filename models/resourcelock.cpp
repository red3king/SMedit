#include "resourcelock.h"


ResourceLock::ResourceLock(unsigned int id) : BoxEntity(id) { }


ResourceLock::ResourceLock(json jdata) : BoxEntity(jdata) { }


json ResourceLock::to_json()
{
    json jdata = BoxEntity::to_json();
    jdata["resource"] = resource->id;
    return jdata;
}
