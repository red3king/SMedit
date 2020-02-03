#pragma once

#include "historymanager/operation.h"
#include "models/custom_state_class.h"


class CustomStateClassChgOperation : public Operation
{
    public:
        CustomStateClassChgOperation(CustomStateClass *custom_state_class);
        unsigned int execute(Project& project);
        virtual void execute_impl(CustomStateClass *resource)=0;

    protected:
        unsigned int customstateclass_id;
};


class OpCustStateCreate : public Operation
{
    public:
        OpCustStateCreate(string name, string path);
        OpCustStateCreate* clone();
        unsigned int execute(Project& project);
        
    private:
        string name, path;
};


class OpCustStateDelete : public Operation
{
    public: 
        OpCustStateDelete(CustomStateClass *to_delete);
        OpCustStateDelete *clone();
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

for line in custom_state_class_opgen.make_header():
    cog.outl(line)

]]]*/


class OpCustomStateClassPath : public CustomStateClassChgOperation
{
    // AUTOGENERATED, DO NOT MODIFY
    public:
        OpCustomStateClassPath(CustomStateClass* customstateclass, string path);
        OpCustomStateClassPath* clone();

        void execute_impl(CustomStateClass* customstateclass);
        bool may_collapse_impl(Operation& other);
        void collapse(Operation& other);

    private:
        string path;
};


class OpCustomStateClassName : public CustomStateClassChgOperation
{
    // AUTOGENERATED, DO NOT MODIFY
    public:
        OpCustomStateClassName(CustomStateClass* customstateclass, string name);
        OpCustomStateClassName* clone();

        void execute_impl(CustomStateClass* customstateclass);
        bool may_collapse_impl(Operation& other);
        void collapse(Operation& other);

    private:
        string name;
};


class OpCustomStateClassTransitionDefs : public CustomStateClassChgOperation
{
    // AUTOGENERATED, DO NOT MODIFY
    public:
        OpCustomStateClassTransitionDefs(CustomStateClass* customstateclass, vector<CustomTransitionDef> transition_defs);
        OpCustomStateClassTransitionDefs* clone();

        void execute_impl(CustomStateClass* customstateclass);
        bool may_collapse_impl(Operation& other);
        void collapse(Operation& other);

    private:
        vector<CustomTransitionDef> transition_defs;
};

//[[[end]]]

