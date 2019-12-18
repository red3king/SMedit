#include "broadcast_events.h"
#include "log.h"


BroadcastEvents::BroadcastEvents()
{
    enabled = false;
}


void BroadcastEvents::set_enabled(bool enabled)
{
    this->enabled = enabled;
}


void BroadcastEvents::handle_broadcast(json broadcast)
{
    if(!enabled)
        return;

    json resp;

    try { resp  = broadcast["resp"]; }
    catch (nlohmann::detail::type_error& ex) { log("Broadcast missing resp field"); return; }

    int broadcast_type;
    json data;

    try { broadcast_type = resp["type"]; data = resp["data"]; }
    catch (nlohmann::detail::type_error& ex) { log("Broadcast resp missing data/type field"); return; }

    switch(broadcast_type)
    {
        case BT_MACHINE_STATE_CHANGE:
            handle_machine_state_changed(data);
            break;

        case BT_MACHINE_CREATE:
            handle_machine_created(data);
            break;

        case BT_MACHINE_DELETE:
            handle_machine_deleted(data);
            break;

        default:
            log("unknown broadcast type " + std::to_string(broadcast_type));
            return;
    }
}


void BroadcastEvents::handle_machine_state_changed(json data)
{
    json vars;
    int machine_id;
    int state_id;

    try
    {
        machine_id = data["machine_id"];
        state_id = data["state_id"];
        vars = data["vars"];
    }

    catch(nlohmann::detail::type_error& ex) { log("handle_machine_state_changed missing field"); return; }
    machine_state_changed.emit(machine_id, state_id, vars);
}


void BroadcastEvents::handle_machine_created(json data)
{
    int machine_id, def_id;

    try
    {
        machine_id = data["machine_id"];
        def_id = data["def_id"];
    }

    catch(nlohmann::detail::type_error& ex) { log("handle_machine_created missing field"); return; }
    machine_created.emit(machine_id, def_id);
}


void BroadcastEvents::handle_machine_deleted(json data)
{
    int machine_id;

    try { machine_id = data["machine_id"]; }
    catch(nlohmann::detail::type_error& ex) { log("handle_machine_deleted missing field"); return; }
    machine_deleted.emit(machine_id);
}


