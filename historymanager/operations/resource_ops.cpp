#include "resource_ops.h"
#include "signals.h"


// Abstract change

ResourceChgOperation::ResourceChgOperation(Resource* resource)
{
    resource_id = resource->id;
}


unsigned int ResourceChgOperation::execute(Project& project)
{
    Resource* resource = project.get_resource_by_id(resource_id);
    execute_impl(resource);
    return resource_id;
}


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


// Path change

OpResourcePath::OpResourcePath(Resource* resource, string path) : ResourceChgOperation(resource)
{
    this->path = path;
}


OpResourcePath* OpResourcePath::clone()
{
    return new OpResourcePath(*this);
}


void OpResourcePath::execute_impl(Resource* resource)
{
    resource->path = path;
}


// Name change

OpResourceName::OpResourceName(Resource* resource, string name) : ResourceChgOperation(resource)
{
    this->name = name;
}


OpResourceName* OpResourceName::clone()
{
    return new OpResourceName(*this);
}


void OpResourceName::execute_impl(Resource* resource)
{
    resource->name = name;
}

