#pragma once

#include <string>

#include "boxentity.h"

using std::string;


enum StateType { INITIAL, CODE, RETURN, SPAWN, JOIN, RUN };


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
        bool initial;

    private:
};


