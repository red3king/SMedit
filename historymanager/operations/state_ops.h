#pragma once

#include "historymanager/operation.h"
#include "models/state.h"


class StateChgOperation : public Operation  // abc for state modification
{
    public:
        StateChgOperation(Machine* machine, State* state);
        unsigned int execute(Project& project);
        virtual void execute_impl(State* state)=0;
        
    private:
        unsigned int machine_id, state_id;
};


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


class OpStateType : public StateChgOperation
{
    public:
        OpStateType(Machine* machine, State* state, StateType type);
        OpStateType* clone();
        void execute_impl(State* state);

    private:
        StateType type;
};


class OpStateName : public StateChgOperation
{
    public:
        OpStateName(Machine* machine, State* state, string name);
        OpStateName* clone();
        void execute_impl(State* state);

    private:
        string name;
};


class OpStateCode : public StateChgOperation
{
    public:
        OpStateCode(Machine* machine, State* state, string code);
        OpStateCode* clone();
        void execute_impl(State* state);

    private:
        string code;
};

