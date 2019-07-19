#pragma once

#include "historymanager/operation.h"
#include "models/resourcelock.h"


class OpResourceLockCreate : public Operation
{
    public:
        OpResourceLockCreate(Machine* machine, Resource* resource, float x, float y);
        OpResourceLockCreate* clone();
        unsigned int execute(Project& project);

    private:
        float x, y;
        unsigned int machine_id, resource_id;         
};


class OpResourceLockDelete : public Operation
{
    public:
        OpResourceLockDelete(Machine* machine, ResourceLock* lock);
        OpResourceLockDelete* clone();
        unsigned int execute(Project& project);
    
    private:
        unsigned int machine_id, resourcelock_id;
};


class OpResourceLockMove : public Operation
{
    public:
        OpResourceLockMove(Machine* machine, ResourceLock* resourcelock, float x, float y);
        OpResourceLockMove* clone();

        unsigned int execute(Project& project);
        bool may_collapse_impl(Operation& other);
        void collapse(Operation& other);

    private:
        unsigned int machine_id, resourcelock_id;
        float x, y;
};
