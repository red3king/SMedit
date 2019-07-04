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
