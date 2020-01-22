#pragma once

#include "historymanager/operation.h"
#include "models/custom_state_class.h"


class CustStateChgOperation : public Operation
{
    public:
        CustStateChgOperation(CustomStateClass *custom_state_class);
        unsigned int execute(Project& project);
        virtual void execute_impl(CustomStateClass *resource)=0;

    protected:
        unsigned int csc_id;
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



//[[[end]]]

