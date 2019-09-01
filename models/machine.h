#pragma once

#include <string>
#include <vector>
#include "lib/json.hpp"

#include "entity.h"
#include "resourcelock.h"
#include "transition.h"
#include "state.h"


using std::string;
using std::vector;
using nlohmann::json;


class Project;

class Machine : public Entity
{
    public:
        Machine(unsigned int id=0);
        Machine(const Machine& other);
        Machine(json jdata);
        Machine* clone() const;
        ~Machine();

        string name;
        bool run_on_start;

        vector<State*> states;
        vector<Transition*> transitions;
        vector<ResourceLock*> resourcelocks;

        Entity* get_entity_by_id(unsigned int id);
        State* get_state_by_id(unsigned int id);
        Transition* get_transition_by_id(unsigned int id);
        ResourceLock* get_resourcelock_by_id(unsigned int id);

        json to_json();

    private:
};

