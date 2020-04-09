#pragma once

#include "boxentity.h"
#include "resource.h"
#include "state.h"


class ResourceLock : public BoxEntity
{
    /*
     *  Create resourcelock:
     *      - user clicks button or presses r key
     *      - if no Resources exist, modal error pops up
     *      - else, mouse changes into resourcelock icon
     *      - user click-drags to create resourcelock : endpoints at button down and button release
     *      - the resourcelock is now in the selected item window so the user can change its type
     *
     *  Move resourcelock:
     *      - user moves mouse into center area of resourcelock
     *      - mouse icon becomes move icon
     *      - click and drag to move
     *
     *  Resize resourcelock:
     *      - user moves mouse into edge / corner of resourcelock
     *      - mouse icon becomes expand icon
     *      - click and drag to expand
     *
     *  Delete resourcelock:
     *      - click to select
     *      - dleete key or delete button
     *
     *
     *
     * */


    public:
        ResourceLock(unsigned int id=0);
        ResourceLock(json jdata);

        json to_json();

        Resource* resource;

        bool contains_state(State* state);
        
        EntityType get_entity_type();
        
    private:  
};
