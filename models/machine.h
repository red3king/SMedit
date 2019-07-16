#pragma once

#include <string>
#include <vector>

#include "entity.h"
#include "resourcelock.h"
#include "transition.h"
#include "state.h"


using std::string;
using std::vector;


class Machine : public Entity
{
    public:
        Machine(unsigned int id=0);
        Machine(const Machine& other);
        Machine& operator=(const Machine& other);
        Machine* clone() const;
        ~Machine();

        string name;
        bool run_on_start;

        vector<State*> states;
        vector<Transition*> transitions;
        vector<ResourceLock*> resourcelocks;

        State* get_state_by_id(unsigned int id);
        Transition* get_transition_by_id(unsigned int id);
        ResourceLock* get_resourcelock_by_id(unsigned int id);

    private:
        void _copy_from(const Machine& other);
};

