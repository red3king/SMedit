#include <gdkmm/rgba.h>

#include "gui/animation_timer.h"
#include "run_controller.h"
#include "utils.h"
#include "net/actions/all.h"
#include "log.h"
#include "options.h"
#include "gui/gui_state.h"  // for GAM_RUN 
#include "gui/gui_context.h"


RunController::RunController(HistoryManager* history_manager, ProjectInfo* project_info, Glib::RefPtr<Gtk::Builder> const& builder) : actionator(&line_client, &broadcast_events) , rgb_same("green") , rgb_unknown("white") , rgb_different("red")
{
    project_open = false;
    is_connected = false;
    has_deployed_project = false;
    is_connecting = false;
    project_started = false;
    project_paused = false;
    state_synchronized = false;
    hash_state = HS_UNKNOWN;

    local_checksum = CHK_UNKNOWN;
    server_checksum = CHK_UNKNOWN;

    running_state = new RunningState(broadcast_events);
    machines_list_controller = new MachinesListController(builder, broadcast_events, running_state);

    this->history_manager = history_manager;
    this->project_info = project_info;

    builder->get_widget("run_gl_area", gl_area);
    builder->get_widget("server_name_entry", server_entry);
    builder->get_widget("server_hash_label", server_hash_label);
    builder->get_widget("project_hash_label", project_hash_label);
    builder->get_widget("connect_button", connect_button);
    builder->get_widget("deploy_button", deploy_button);
    builder->get_widget("start_button", start_button);
    builder->get_widget("pause_button", pause_button);

    builder->get_widget("lopt_auto_pan", lopt_auto_pan_cb);
    builder->get_widget("lopt_open_spawned", lopt_open_spawned_cb);
    builder->get_widget("sopt_min_trans_time", sopt_min_trans_time_entry);

    gui_context = new GUIContext(gl_area, history_manager, GAM_RUN, running_state, &banner_displayer);
    animation_timer = new AnimationTimer(gui_context);
    
    lopt_auto_pan_cb->set_active(options.get_auto_pan());
    lopt_open_spawned_cb->set_active(options.get_open_spawned());

    lopt_auto_pan_cb->signal_toggled().connect(sigc::mem_fun(*this, &RunController::on_lopt_auto_pan_changed));
    lopt_open_spawned_cb->signal_toggled().connect(sigc::mem_fun(*this, &RunController::on_lopt_open_spawned_changed));

    signals.project_save.connect(sigc::mem_fun(this, &RunController::on_project_saved));
    signals.project_load.connect(sigc::mem_fun(this, &RunController::on_project_loaded));
    signals.project_open.connect(sigc::mem_fun(this, &RunController::on_project_open));
    signals.project_close.connect(sigc::mem_fun(this, &RunController::on_project_close));

    line_client.connection_attempt_signal.connect(sigc::mem_fun(this, &RunController::on_connection_attempt_complete));
    line_client.disconnection_signal.connect(sigc::mem_fun(this, &RunController::on_disconnection));

    connect_button->signal_clicked().connect(sigc::mem_fun(this, &RunController::on_connect_click));
    deploy_button->signal_clicked().connect(sigc::mem_fun(this, &RunController::on_deploy_click));
    start_button->signal_clicked().connect(sigc::mem_fun(this, &RunController::on_start_click));
    pause_button->signal_clicked().connect(sigc::mem_fun(this, &RunController::on_pause_click));

    broadcast_events.machine_created.connect(sigc::mem_fun(this, &RunController::banner_on_mach_create));
    broadcast_events.machine_deleted.connect(sigc::mem_fun(this, &RunController::banner_on_mach_delete));

    update_enabled();
}


void RunController::banner_on_mach_create(int machine_id, int machine_def_id)
{
    RunningMachine& mach = running_state->get_running_machine(machine_id);
    banner_displayer.display_banner(mach.name + " spawned");       
}


void RunController::banner_on_mach_delete(int machine_id)
{
    RunningMachine& mach = running_state->get_running_machine(machine_id);
    banner_displayer.display_banner(mach.name + " terminated");
}


void RunController::on_lopt_auto_pan_changed()
{
    bool value = lopt_auto_pan_cb->get_active();
    options.set_auto_pan(value);
}


void RunController::on_lopt_open_spawned_changed()
{
    bool value = lopt_open_spawned_cb->get_active();
    options.set_open_spawned(value);
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
    state_synchronized = false;

    update_enabled();
    
    if(is_connected)
        begin_get_hash();
}


void RunController::on_get_hash_complete(Action* action)
{
    GetHash* get_hash = (GetHash*) action;

    if(get_hash->status == AS_SUCCESS)
    {
        server_checksum = get_hash->project_hash;
        hash_state = local_checksum == server_checksum ? HS_SAME : HS_DIFFERENT; 
       
        if(hash_state == HS_SAME) 
        {
            deployed_project = project_info->saved_project;
            running_state->set_project(&deployed_project);

            auto synch_cb = boost::bind(&RunController::on_state_synch_complete, this, _1);
            auto state_synch = new StateSynchAction(synch_cb);
            actionator.submit_action(state_synch);
        }

        else
        {
            deployed_project = Project();
            running_state->set_project(&deployed_project);
        }
    }

    else
    {
        hash_state = HS_UNKNOWN;
        server_checksum = CHK_UNKNOWN;
        display_action_error(action);
        deployed_project = Project();
        running_state->set_project(&deployed_project); // TODO maybe use nullptr to signify no project?
    }

    update_enabled();
}


void RunController::on_state_synch_complete(Action* action)
{
    StateSynchAction* synch = (StateSynchAction*) action;

    if(synch->status != AS_SUCCESS)
    {
        display_action_error(action);
        return;
    }

    project_started = synch->project_started;
    project_paused = synch->project_paused;
    running_state->initial_state_synch(synch->current_running_machines);
    broadcast_events.set_enabled(true);
    
    state_synchronized = true;
    

    update_enabled();
}


void RunController::on_disconnection(bool intentional)
{
    is_connected = false;
    is_connecting = false;
    state_synchronized = false;
    server_checksum = CHK_UNKNOWN;
    broadcast_events.set_enabled(false);
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


//////  Deploy Project  //////

void RunController::on_deploy_click()
{
    string missing_files;
    if(project_info->any_missing_files(missing_files))
    {
        display_error("Can't deploy. The following resource(s) are missing files:" + missing_files);
        return;
    }

    auto new_project = new NewProjectAction();

    for(int i=0; i<project_info->get_num_files(); i++)
    {
        string filename = project_info->get_filename(i);
        string data = project_info->get_filedata(i);
        auto upload = new AddFileAction(filename, data);
        new_project->add_next_action(upload);
    }
    
    auto load_project = new LoadProjectAction();
    new_project->add_next_action(load_project);

    auto hash_cb = boost::bind(&RunController::on_get_hash_complete, this, _1);
    auto get_hash = new GetHash(hash_cb);
    new_project->add_next_action(get_hash);
    actionator.submit_action(new_project);
}


//////  Start / Stop project behavior  //////

void RunController::on_start_click()
{
    // TODO - learn what the _1 and _2 are for
    // are these placeholders for when your function takes many arguments which you don't care
    // about passing in? Or something else? Can I compile this without passing _1 here??

    auto start_stop_cb = boost::bind(&RunController::on_start_stop_complete, this, _1);
    Action* action;
    
    if(project_started)
        action = new StopProjectAction(start_stop_cb);
    else
        action = new StartProjectAction(start_stop_cb);

    actionator.submit_action(action);
}


void RunController::on_start_stop_complete(Action* action)
{
    if(action->status != AS_SUCCESS)
    {
        display_action_error(action);
        return;
    }

    project_started = !project_started;
    banner_displayer.display_banner(project_started ? "started" : "stopped");
    update_enabled();
}


////// Pause / Unpause project behavior  //////

void RunController::on_pause_click()
{
    auto pause_unpause_cb = boost::bind(&RunController::on_pause_unpause_complete, this, _1);
    Action* action;
    
    if(project_paused)
       action = new UnpauseProjectAction(pause_unpause_cb);
    else
       action = new PauseProjectAction(pause_unpause_cb);

    actionator.submit_action(action);
}


void RunController::on_pause_unpause_complete(Action* action)
{
    if(action->status != AS_SUCCESS)
    {
        display_action_error(action);
        return;
    }

    project_paused = !project_paused;
    banner_displayer.display_banner(project_paused ? "paused" : "unpaused");
    update_enabled();
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
    start_button->set_sensitive(state_synchronized && is_connected && hash_match);
    start_button->set_label(project_started ? "stop" : "start");
    pause_button->set_sensitive(state_synchronized && is_connected && hash_match);
    pause_button->set_label(project_paused ? "unpause" : "pause"); 
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

