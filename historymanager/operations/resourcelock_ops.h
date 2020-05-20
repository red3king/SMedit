#pragma once

#include "historymanager/operation.h"
#include "models/resourcelock.h"


class ResourceLockOperation : public MachineRelatedOperation
{
    public:
        ResourceLockOperation(Machine* machine, ResourceLock* resource_lock);
        unsigned int resourcelock_id;
};


class OpResourceLockCreate : public ResourceLockOperation
{
    public:
        OpResourceLockCreate(Machine* machine, Resource* resource, float x, float y);
        OpResourceLockCreate* clone();
        unsigned int execute(Project& project);

    private:
        float x, y;    
        unsigned int resource_id;
};


class OpResourceLockDelete : public ResourceLockOperation
{
    public:
        OpResourceLockDelete(Machine* machine, ResourceLock* lock);
        OpResourceLockDelete* clone();
        unsigned int execute(Project& project);
    
    private:
};


class OpResourceLockMove : public ResourceLockOperation
{
    public:
        OpResourceLockMove(Machine* machine, ResourceLock* resourcelock, float x, float y);
        OpResourceLockMove* clone();

        unsigned int execute(Project& project);
        bool may_collapse_impl(Operation& other);
        void collapse(Operation& other);

    private:
        float x, y;
};
