#pragma once

#include "historymanager/operation.h"
#include "models/state.h"


class OpStateCreate : public Operation
{
    public:
        OpStateCreate(Machine* machine, float x, float y);
        unsigned int execute(Project& project);
        OpStateCreate* clone();

    private:
        unsigned int machine_id;
        float x, y;
};


class OpStateDelete : public Operation
{
    public:
        OpStateDelete(Machine* machine, State* state);
        unsigned int execute(Project& project);
        OpStateDelete* clone();
    
    private:
        unsigned int machine_id, state_id;
};


class OpStateMove : public Operation
{
    public:
        OpStateMove(Machine* machine, State* state, float x, float y);
        unsigned int execute(Project& project);
        OpStateMove* clone();

        bool may_collapse_impl(Operation& other);
        void collapse(Operation& other);

    private:
        unsigned int machine_id, state_id;       
        float x, y;
};



