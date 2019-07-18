#include "resourcelock_ops.h"
#include "signals.h"


// Create

OpResourceLockCreate::OpResourceLockCreate(Machine* machine, Resource* resource, float x, float y)
{
    this->x = x;
    this->y = y;
    resource_id = resource->id;
    machine_id = machine->id;
}


unsigned int OpResourceLockCreate::execute(Project& project)
{
    Resource* resource = project.get_resource_by_id(resource_id);
    Machine* machine = project.get_machine_by_id(machine_id);

    ResourceLock* lock = new ResourceLock(project.get_next_id());
    lock->resource = resource;
    lock->x = x;
    lock->y = y;
    lock->w = 200;
    lock->h = 200;

    machine->resourcelocks.push_back(lock);
    signals.fire_gui_signal(RESOURCELOCK, CREATE, lock->id);
    return lock->id;
}


OpResourceLockCreate* OpResourceLockCreate::clone()
{
    return new OpResourceLockCreate(*this);
}


// Delete

OpResourceLockDelete::OpResourceLockDelete(Machine* machine, ResourceLock* lock)
{
    machine_id = machine->id;
    resourcelock_id = lock->id;
}


unsigned int OpResourceLockDelete::execute(Project& project)
{
    Machine* machine = project.get_machine_by_id(machine_id);
    
    int i;
    ResourceLock* lock;

    for(i=0; i<machine->resourcelocks.size(); i++)
    {
        lock = machine->resourcelocks[i];

        if(lock->id == resourcelock_id)
            break;
    }

    signals.fire_gui_signal(RESOURCELOCK, PRE_DELETE, lock->id);

    delete lock;
    machine->resourcelocks.erase(machine->resourcelocks.begin() + i);
    return resourcelock_id;
}


OpResourceLockDelete* OpResourceLockDelete::clone()
{
    return new OpResourceLockDelete(*this);
}
