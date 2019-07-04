#pragma once

#include <string>
#include <vector>

#include "transition.h"
#include "state.h"
#include "entity.h"

using std::string;
using std::vector;


class Machine : public Entity
{
    public:
        Machine(unsigned int id=0);

        string name;
        bool run_on_start;

        vector<State> states;
        vector<Transition> transitions;

    private:
};

