#pragma once

#include <string>

#include "entity.h"
#include "state.h"

using std::string;


enum TransitionType { EVENT, TIMEOUT, CATCHALL };


class Transition : public Entity 
{
    /*
     *  Transition creation - how does the user draw a transition?:
     *      - user presses t or clicks new button
     *      - if no states present, display modal "hey create a state first" & exit process
     *      - mouse cursor changes or something like "click first state" appears
     *      - ui goes into highlight nearest nearby state UIMODE
     *      - user clicks first state
     *      - ui goes into preview-transition-to-mouse-cursor UIMODE
     *      - "click second state"
     *      - user clicks second state
     *      - transition is now created between the two states, with default values. 
     *      - new transition is selected in the selected item editor window/panel/thing so user can configure it
     *
     *
     *  Modifying a transition's properties:
     *      - user moves mouse near enough to a Transition
     *      - transistion highlights
     *      - user clicks again, transition now appears in the selected item window
     *
     *
     *  Modifying a transition's endpoints/states:
     *      - user moves near enough to a transition endpoint
     *      - endpoint highlights
     *      - user clicks on endpoint
     *      - endpoint is now following the mouse
     *      - states nearby are highlighted
     *      - user clicks again when near a state to put down the transition endpoint
     *
     *
     *  Deleting a transition:
     *      - select, click delete button
     *      - or, select, click delete key
     *
     * */

    public:
        Transition(unsigned int id=0);

        TransitionType type;
        string event_name;  // only used for EVENT type

        State* from_state;
        State* to_state;

        float x0, y0, x1, y1;

        bool from_connected();
        bool to_connected();
        bool any_connected();

    private:
};


