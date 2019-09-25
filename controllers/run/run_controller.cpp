#include "run_controller.h"
#include "utils.h"
#include "net/actions/all.h"
#include "log.h"


RunController::RunController(HistoryManager* history_manager, Glib::RefPtr<Gtk::Builder> const& builder) : actionator(&line_client)
{
    project_open = false;
    is_connected = false;
    has_deployed_project = false;
    is_connecting = false;

    local_checksum = CHK_NONE;
    server_checksum = CHK_UNKNOWN;
    this->history_manager = history_manager;

    builder->get_widget("server_name_entry", server_entry);
    builder->get_widget("server_hash_label", server_hash_label);
    builder->get_widget("project_hash_label", project_hash_label);
    builder->get_widget("connect_button", connect_button);
    builder->get_widget("deploy_button", deploy_button);

    signals.project_save.connect(sigc::mem_fun(this, &RunController::on_project_saved));
    signals.project_load.connect(sigc::mem_fun(this, &RunController::on_project_loaded));
    signals.project_open.connect(sigc::mem_fun(this, &RunController::on_project_open));
    signals.project_close.connect(sigc::mem_fun(this, &RunController::on_project_close));

    line_client.connection_attempt_signal.connect(sigc::mem_fun(this, &RunController::on_connection_attempt_complete));
    line_client.disconnection_signal.connect(sigc::mem_fun(this, &RunController::on_disconnection));

    connect_button->signal_clicked().connect(sigc::mem_fun(this, &RunController::on_connect_click));
    deploy_button->signal_clicked().connect(sigc::mem_fun(this, &RunController::on_deploy_click));

    update_enabled();
}


void RunController::on_project_loaded(uint16_t hash)
{
    local_checksum = hash;
    update_enabled();
}


void RunController::on_project_saved(uint16_t hash)
{
    local_checksum = hash;
    update_enabled();
}


void RunController::on_project_open()
{
    project_open = true;
    update_enabled();
}


void RunController::on_project_close()
{
    project_open = false;
    update_enabled();
}

/*
 * on connect
 *      get server checksum
 *          fail? disconnect
 *      
 *      update_enabled
 */

void RunController::on_connection_attempt_complete(ConnectionResult result)
{
    is_connected = result == CR_SUCCESS;
    is_connecting = false;
    update_enabled();
}


void RunController::on_disconnection(bool intentional)
{
    is_connected = false;
    is_connecting = false;
    update_enabled();
}


void RunController::on_connect_click()
{
    is_connecting = true;
    update_enabled();

    if(is_connected)
        line_client.begin_disconnect();

    else
        line_client.begin_connect(server_entry->get_text());
}


void RunController::on_echo_complete(Action* echo)
{
    if(echo->status == AS_SUCCESS)
        log("ECHO SUCCESS!!!!!!");
    else
        log("ECHO FAILURE.. ");
}


void RunController::on_deploy_click()
{
    ActionCB cb = boost::bind(&RunController::on_echo_complete, this, _1);
    auto echo = new Echo(cb);
    actionator.submit_action(echo);
}


string make_hash_label(int hash)
{
    if(hash == CHK_NONE)
        return "no project";

    if(hash == CHK_UNKNOWN)
        return "unknown";

    return int_to_hex_string(hash);
}


void RunController::update_enabled()
{
    server_entry->set_sensitive(project_open && !is_connected && !is_connecting);
    deploy_button->set_sensitive(project_open && is_connected && !is_connecting);
    
    server_hash_label->set_text(make_hash_label(server_checksum));
    project_hash_label->set_text(make_hash_label(local_checksum));

    string connect_text;
    bool connect_enabled = !is_connecting && project_open;

    if(is_connecting)
        connect_text = is_connected ? "disconnecting..." : "connecting...";
    else
        connect_text = is_connected ? "disconnect" : "connect";

    connect_button->set_label(connect_text);
    connect_button->set_sensitive(connect_enabled);
}

/*
 *
 * update_enabled      
 *
 *      if !has_deployed_project
 *          if server_checksum == local_checksum and they are not CHK_NONE:
 *              has_deployed_project = true
 *              deployed_project = hm.current_project
 *
 *      connect/disconnect - always enabled
 *      deploy - enabled if connected
 *      command buttons - enabled if connected and has_deployed_project is true
 *      server name entry - enabled if not connected
 *
 *
 *      ============
 *
 *
 *      on deploy
 *          (do deploy)
 *          has_deployed_project = true
 *          deployed_project = hm.current_project
 *          update_enabled
 *
 *      on connect 
 *          get server chksum
 *              fail? disconnect
 *          match?
 *              has_deployed_project = true
 *              deployed_project = hm.current_project
 *          mismatch
 *              has_deployed_project = false
 *
 *          update_enabled
 *      
 *      on disconnect
 *          has_deployed_project = false
 *          deployed_project = Project()
 *          update_enabled
 *
 *      on save
 *          local_checksum = new_crc
 *          update_enabled
 *
 *      on load
 *          local_checksum = new_crc
 *          update_enabled
 *      
 *
 *
 *
 *
 */
