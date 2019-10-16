#include "run_controller.h"
#include "utils.h"
#include "net/actions/all.h"
#include <gdkmm/rgba.h>

#include "log.h"



RunController::RunController(HistoryManager* history_manager, ProjectInfo* project_info, Glib::RefPtr<Gtk::Builder> const& builder) : actionator(&line_client) , rgb_same("green") , rgb_unknown("white") , rgb_different("red")
{
    project_open = false;
    is_connected = false;
    has_deployed_project = false;
    is_connecting = false;
    hash_state = HS_UNKNOWN;

    local_checksum = CHK_UNKNOWN;
    server_checksum = CHK_UNKNOWN;
    this->history_manager = history_manager;
    this->project_info = project_info;

    builder->get_widget("server_name_entry", server_entry);
    builder->get_widget("server_hash_label", server_hash_label);
    builder->get_widget("project_hash_label", project_hash_label);
    builder->get_widget("connect_button", connect_button);
    builder->get_widget("deploy_button", deploy_button);
    builder->get_widget("start_button", start_button);
    builder->get_widget("pause_button", pause_button);

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


void RunController::on_project_loaded()
{
    local_checksum = project_info->get_hash();
    update_enabled();
}


void RunController::on_project_saved()
{
    local_checksum = project_info->get_hash();
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


void RunController::on_connection_attempt_complete(ConnectionResult result)
{
    is_connected = result == CR_SUCCESS;
    is_connecting = false;

    update_enabled();
    begin_get_hash();
}


void RunController::on_get_hash_complete(Action* action)
{
    GetHash* get_hash = (GetHash*) action;

    if(get_hash->status == AS_SUCCESS)
        server_checksum = get_hash->project_hash;
    else
        server_checksum = CHK_UNKNOWN;

    update_enabled();
}


void RunController::on_disconnection(bool intentional)
{
    is_connected = false;
    is_connecting = false;
    server_checksum = CHK_UNKNOWN;
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


void RunController::on_deploy_click()
{
    auto new_project = new NewProjectAction();

    for(int i=0; i<project_info->get_num_files(); i++)
    {
        string filename = project_info->get_filename(i);
        string data = project_info->get_filedata(i);
        auto upload = new AddFileAction(filename, data);
        new_project->add_next_action(upload);
    }

    auto hash_cb = boost::bind(&RunController::on_get_hash_complete, this, _1);
    auto get_hash = new GetHash(hash_cb);
    new_project->add_next_action(get_hash);
    actionator.submit_action(new_project);
}


string make_hash_label(int hash)
{
    if(hash == CHK_UNKNOWN)
        return "unknown";

    return int_to_hex_string(hash);
}


void RunController::begin_get_hash()
{
    auto cb = boost::bind(boost::mem_fn(&RunController::on_get_hash_complete), this, _1);
    auto action = new GetHash(cb);
    actionator.submit_action(action);
}


void RunController::update_enabled()
{
    // logic
    HashState new_hash_state = HS_UNKNOWN;
    if(is_connected && local_checksum != CHK_UNKNOWN && server_checksum != CHK_UNKNOWN)
        new_hash_state = local_checksum == server_checksum ? HS_SAME : HS_DIFFERENT;

    if(new_hash_state != hash_state)
    {
        if(hash_state == HS_SAME)
        {
            //enable stuff
        }
        else if(hash_state == HS_DIFFERENT)
        {
            //disable stuff
        }
    }

    hash_state = new_hash_state;

    // widget updates
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

    auto color = get_color(hash_state);
    project_hash_label->override_color(color);
    server_hash_label->override_color(color);

    bool hash_match = hash_state == HS_SAME;
    start_button->set_sensitive(is_connected && hash_match);
    pause_button->set_sensitive(is_connected && hash_match);
}


Gdk::RGBA RunController::get_color(HashState hash_state)
{
    // get color for hash labels
    switch(hash_state)
    {
        case HS_SAME:
            return rgb_same;
        case HS_DIFFERENT:
            return rgb_different;
        default:
            return rgb_unknown;
    }
}

