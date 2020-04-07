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
    signals.fire_model_changed(RESOURCELOCK, CREATE, lock->id);
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

    signals.fire_model_changed(RESOURCELOCK, PRE_DELETE, lock->id);

    delete lock;
    machine->resourcelocks.erase(machine->resourcelocks.begin() + i);
    return resourcelock_id;
}


OpResourceLockDelete* OpResourceLockDelete::clone()
{
    return new OpResourceLockDelete(*this);
}


// Move

OpResourceLockMove::OpResourceLockMove(Machine* machine, ResourceLock* resourcelock, float x, float y)
{
    machine_id = machine->id;
    resourcelock_id = resourcelock->id;
    this->x = x;
    this->y = y;
}


OpResourceLockMove* OpResourceLockMove::clone()
{
    return new OpResourceLockMove(*this);
}


unsigned int OpResourceLockMove::execute(Project& project)
{
    Machine* machine = project.get_machine_by_id(machine_id);
    ResourceLock* lock = machine->get_resourcelock_by_id(resourcelock_id);
    lock->x = x;
    lock->y = y;
    signals.fire_model_changed(RESOURCELOCK, MODIFY, lock->id);
    return resourcelock_id;
}


bool OpResourceLockMove::may_collapse_impl(Operation& other)
{
    OpResourceLockMove& other_lm = dynamic_cast<OpResourceLockMove&>(other);
    return other_lm.resourcelock_id == resourcelock_id;
}


void OpResourceLockMove::collapse(Operation& other)
{
    OpResourceLockMove& other_lm = dynamic_cast<OpResourceLockMove&>(other);
    x = other_lm.x;
    y = other_lm.y;
}
