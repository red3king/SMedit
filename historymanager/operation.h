#pragma once

#include "models/project.h"

class Operation
{
    public:
        
        Operation();

        virtual unsigned int execute(Project& project)=0;
        virtual bool may_collapse_impl(Operation& other);
        virtual void collapse(Operation& other);

        bool may_collapse(Operation& other);

        virtual Operation* clone()=0;
        virtual ~Operation()=0;

    private:    
};


class MachineRelatedOperation : public Operation
{
    public:
        
        MachineRelatedOperation(Machine* machine);
        unsigned int machine_id;
};


template<typename T> inline bool is_instance(Operation* operation)
{
    return dynamic_cast<T*>(operation);
}


inline bool is_machine_related_operation(Operation* operation)
{
    return is_instance<MachineRelatedOperation>(operation);
}

