#pragma once

#include "lib/json.hpp"
#include <sigc++/sigc++.h>


using nlohmann::json;
using std::string;


enum BroadcastType
{
    BT_MACHINE_STATE_CHANGE = 0,
    BT_MACHINE_CREATE = 1,
    BT_MACHINE_DELETE = 2,
    BT_PRINT_MESSAGE = 3
};


class BroadcastEvents
{
    public:
        BroadcastEvents();

        sigc::signal<void, int, int, json> machine_state_changed;  // machine_id, state_id, state vars
        sigc::signal<void, int, int> machine_created;              // machine_id, def_id
        sigc::signal<void, int> machine_deleted;                   // machine_id
        sigc::signal<void, int, int, int, string> print_message;        // machine_id, state_id, level, message

        void handle_broadcast(json broadcast);
        void set_enabled(bool enabled);

    private:
        bool enabled;
        
        void handle_machine_state_changed(json data);
        void handle_machine_created(json date);
        void handle_machine_deleted(json data);
        void handle_print_message(json data);
};
