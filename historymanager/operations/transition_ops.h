#pragma once

#include "historymanager/operation.h"
#include "models/transition.h"


class TransitionOperation : public MachineRelatedOperation
{
    public:
        TransitionOperation(Machine* machine, Transition* transition);
        unsigned int transition_id;
};


class TransitionChgOperation : public TransitionOperation  // abstract
{
    public:
        TransitionChgOperation(Machine* machine, Transition* transition);
        unsigned int execute(Project& project);
        virtual void execute_impl(Transition* transition)=0;
};


Transition *create_transition(Project& project, unsigned int machine_id, bool is_child, float x0, float y0, float x1, float y1, int type);


class OpTransitionCreate : public TransitionOperation
{
    public:
        OpTransitionCreate(Machine* machine, float x0, float y0, float x1, float y1);
        unsigned int execute(Project& project);
        OpTransitionCreate* clone();

    private:
        float x0, y0, x1, y1;
};



class OpTransitionDelete : public TransitionOperation
{
    public:
        OpTransitionDelete(Machine* machine, Transition* transition);
        unsigned int execute(Project& project);
        OpTransitionDelete* clone();
};


void delete_transition(Machine *machine, Transition *transition);


class OpTransitionMove : public TransitionOperation
{
    public:
        OpTransitionMove(Machine* machine, Transition* transition, float x0_new, float y0_new);
        unsigned int execute(Project& project);
        OpTransitionMove* clone();

        bool may_collapse_impl(Operation& other);
        void collapse(Operation& other);

    private:
        float x0_new, y0_new;
};


class OpTransitionEndpointMove : public TransitionOperation
{
    public:
        OpTransitionEndpointMove(Machine* machine, Transition* transition, bool is_endpoint_0, float x_new, float y_new);
        OpTransitionEndpointMove(Machine* machine, Transition* transition, bool is_endpoint_0, State* new_state);
        OpTransitionEndpointMove* clone();

        unsigned int execute(Project& project);
        bool may_collapse_impl(Operation& other);
        void collapse(Operation& other);

    private:
        unsigned int new_state_id;
        bool is_endpoint_0;
        float x_new, y_new;
};  


// MACHINE GENERATED CODE BELOW, DO NOT EDIT
// USE COG TO REGENERATE

/*[[[cog

import os
import sys
sys.path.append(os.getcwd())
from codegen.op_specs import *

for line in transition_opgen.make_header():
    cog.outl(line)

]]]*/


class OpTransitionType : public TransitionChgOperation
{
    // AUTOGENERATED, DO NOT MODIFY
    public:
        OpTransitionType(Machine* machine, Transition* transition, TransitionType type);
        OpTransitionType* clone();

        void execute_impl(Transition* transition);
        bool may_collapse_impl(Operation& other);
        void collapse(Operation& other);

    private:
        TransitionType type;
};


class OpTransitionTimeout : public TransitionChgOperation
{
    // AUTOGENERATED, DO NOT MODIFY
    public:
        OpTransitionTimeout(Machine* machine, Transition* transition, float timeout);
        OpTransitionTimeout* clone();

        void execute_impl(Transition* transition);
        bool may_collapse_impl(Operation& other);
        void collapse(Operation& other);

    private:
        float timeout;
};


class OpTransitionEventName : public TransitionChgOperation
{
    // AUTOGENERATED, DO NOT MODIFY
    public:
        OpTransitionEventName(Machine* machine, Transition* transition, string event_name);
        OpTransitionEventName* clone();

        void execute_impl(Transition* transition);
        bool may_collapse_impl(Operation& other);
        void collapse(Operation& other);

    private:
        string event_name;
};

//[[[end]]]



