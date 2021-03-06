#pragma once

#include "historymanager/operation.h"
#include "models/resource.h"


class ResourceOperation : public Operation 
{ 
    public:
        ResourceOperation(Resource* resource);
        unsigned int resource_id;
};


inline bool is_resource_operation(Operation* operation)
{
    return is_instance<ResourceOperation>(operation);
}


class ResourceChgOperation : public ResourceOperation
{
    public:
        ResourceChgOperation(Resource* resource);
        unsigned int execute(Project& project);
        virtual void execute_impl(Resource* resource) = 0;
};


class OpResourceCreate : public ResourceOperation
{
    public:
        OpResourceCreate(string name, string path);
        OpResourceCreate* clone();
        unsigned int execute(Project& project);
        
    private:
        string name, path;
};


class OpResourceDelete : public ResourceOperation
{
    public: 
        OpResourceDelete(Resource* to_delete);
        OpResourceDelete* clone();
        unsigned int execute(Project& project);

    private:
        unsigned int to_delete_id;
};


// MACHINE GENERATED CODE BELOW, DO NOT EDIT
// USE COG TO REGENERATE

/*[[[cog

import os
import sys
sys.path.append(os.getcwd())
from codegen.op_specs import *

for line in resource_opgen.make_header():
    cog.outl(line)

]]]*/


class OpResourcePath : public ResourceChgOperation
{
    // AUTOGENERATED, DO NOT MODIFY
    public:
        OpResourcePath(Resource* resource, string path);
        OpResourcePath* clone();

        void execute_impl(Resource* resource);
        bool may_collapse_impl(Operation& other);
        void collapse(Operation& other);

    private:
        string path;
};


class OpResourceName : public ResourceChgOperation
{
    // AUTOGENERATED, DO NOT MODIFY
    public:
        OpResourceName(Resource* resource, string name);
        OpResourceName* clone();

        void execute_impl(Resource* resource);
        bool may_collapse_impl(Operation& other);
        void collapse(Operation& other);

    private:
        string name;
};

//[[[end]]]

