#pragma once

#include "lib/json.hpp"
#include <sigc++/sigc++.h>


using nlohmann::json;


enum BroadcastType
{
    BT_MACHINE_STATE_CHANGE = 0,
    BT_MACHINE_CREATE = 1,
    BT_MACHINE_DELETE = 2
};


class BroadcastEvents
{
    public:
        BroadcastEvents();

        sigc::signal<void, uint32_t, uint32_t, json> machine_state_changed;  // machine_id, state_id, state vars
        sigc::signal<void, uint32_t, uint32_t> machine_created;              // machine_id, def_id
        sigc::signal<void, uint32_t> machine_deleted;                        // machine_id

        void handle_broadcast(json broadcast);

    private:
        void handle_machine_state_changed(json data);
        void handle_machine_created(json date);
        void handle_machine_deleted(json data);
};
