#pragma once

#include "historymanager/operation.h"
#include "models/transition.h"


class OpTransitionCreate : public Operation
{
    public:
        OpTransitionCreate(Machine* machine, float x0, float y0, float x1, float y1);
        unsigned int execute(Project& project);
        OpTransitionCreate* clone();

    private:
        unsigned int machine_id;
        float x0, y0, x1, y1;
};


class OpTransitionDelete : public Operation
{
    public:
        OpTransitionDelete(Machine* machine, Transition* transition);
        unsigned int execute(Project& project);
        OpTransitionDelete* clone();

    private:
        unsigned int machine_id, transition_id;
};
