#include "resource_ops.h"
#include "signals.h"


// Create

OpResourceCreate::OpResourceCreate(string name, string path)
{
    this->name = name;
    this->path = path;
}


unsigned int OpResourceCreate::execute(Project& project)
{
    Resource* resource = new Resource(project.get_next_id());
    resource->name = name;
    resource->path = path;
    project.resources.push_back(resource);
    
    signals.fire_gui_signal(RESOURCE, CREATE, resource->id);
    return resource->id;
}


OpResourceCreate* OpResourceCreate::clone()
{
    return new OpResourceCreate(*this);
}


// Delete

OpResourceDelete::OpResourceDelete(Resource* to_delete)
{
    to_delete_id = to_delete->id;
}


unsigned int OpResourceDelete::execute(Project& project)
{
    int rindex = project.get_rindex_by_id(to_delete_id);

    signals.fire_gui_signal(RESOURCE, PRE_DELETE, to_delete_id);

    delete project.resources[rindex];
    project.resources.erase(project.resources.begin() + rindex);
    return to_delete_id;
}


OpResourceDelete* OpResourceDelete::clone()
{
    return new OpResourceDelete(*this);
}
