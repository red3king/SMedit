#pragma once

#include <string>
#include <vector>
#include <stdexcept>

#include "boxentity.h"


#define STS_INITIAL "Initial"
#define STS_CODE "Code"
#define STS_RETURN "Return"
#define STS_SPAWN "Spawn"
#define STS_JOIN "Join"
#define STS_RUN "Run"


using std::string;
using std::vector;
class Transition;


enum StateType { INITIAL, CODE, RETURN, SPAWN, JOIN, RUN };

string state_type_to_string(StateType type);
StateType string_to_state_type(string input);


class State : public BoxEntity
{
    /*
     *
     *  Creating a state:
     *      - user clicks button or s key
     *      - mouse now has a mini state icon thingy on it
     *      - user clicks on worksheet
     *      - state appears there and is selected in the selected item window
     *
     *
     *  Moving a state:
     *      - user moves over state
     *      - mouse now has a "move it" cursor
     *      - user clicks and drags to move state. state is moving with mouse, x and y and all attached transitions     *           updating
     *      - when the user lets go, nothing happens because the state was following the cursor the whole time
     *
     *
     *  Modifying a state:
     *      - user clicks on state
     *      - it's selected, appears in selected item window
     *
     *  Deleting a state:
     *      - user selects state,
     *      - and either clicks the delete button or the delete key
     *      - since transitions cannot be dangling, a modal pops up to confirm that associated transitions
     *          will be deleted. (if they can be dangling, no need for this modal)
     *
    * */


    public:
        State(unsigned int id=0);

        string name;
        StateType type;
        string code;    // only used in CODE type 

        vector<Transition*> incoming_transitions, outgoing_transitions;

        void add_transition(Transition* transition, bool incoming);
        void remove_transition(Transition* transition, bool incoming);
        void update_transition_positions();

    private:
};


