#pragma once


#define CHK_UNKNOWN -1

#include <gtkmm.h>

#include "saveload.h"
#include "signals.h"
#include "run_state.h"
#include "net/actionator.h"
#include "net/broadcast_events.h"
#include "models/project.h"
#include "gui/banner_display.h"
#include "historymanager/historymanager.h"
#include "controllers/run/machines_list_controller.h"


enum HashState { HS_UNKNOWN, HS_SAME, HS_DIFFERENT };


class RunController
{
    /*
    TODO 
     - ADD OPTIONS PANEL WITH
        - CONFIGURABLE STATE DELAY
        - CAMERA TRACKING 
    */

    public:
        RunController(HistoryManager* history_manager, ProjectInfo* project_info, Glib::RefPtr<Gtk::Builder> const& builder);

    private:
        HistoryManager* history_manager;
        ProjectInfo* project_info;
        RunningState* running_state;

        LineClient line_client;
        Actionator actionator;
        BroadcastEvents broadcast_events;       
        MachinesListController* machines_list_controller;
        BannerDisplayer banner_displayer;

        GUIContext* gui_context;
        Gtk::GLArea *gl_area;

        HashState hash_state;
        int32_t local_checksum, server_checksum;
        bool has_deployed_project, project_open, is_connected, is_connecting;
        bool state_synchronized;
        bool project_started, project_paused;
        Project deployed_project;

        Gtk::Entry *server_entry;
        Gtk::Label *server_hash_label, *project_hash_label;
        Gtk::Button *connect_button, *deploy_button, *start_button, *pause_button;

        // options controls
        Gtk::Entry *sopt_min_trans_time_entry;
        Gtk::CheckButton *lopt_auto_pan_cb, *lopt_open_spawned_cb;

        Gdk::RGBA rgb_same, rgb_unknown, rgb_different;

        void on_project_saved();
        void on_project_loaded();

        void on_project_open();
        void on_project_close();

        void on_connection_attempt_complete(ConnectionResult result);
        
        void on_get_hash_complete(Action* action);
        void on_state_synch_complete(Action* action);
        void on_start_stop_complete(Action* action);
        void on_pause_unpause_complete(Action* action);

        void on_disconnection(bool intentional);

        void on_connect_click();
        void on_deploy_click();
        void on_start_click();
        void on_pause_click();

        // options click callbacks
        void on_lopt_auto_pan_changed();
        void on_lopt_open_spawned_changed();
        void on_sopt_min_trans_time_changed();

        // display banner when things happen
        void banner_on_mach_create(int machine_id, int machine_def_id);
        void banner_on_mach_delete(int machine_id);

        void update_enabled();
        void begin_get_hash();

        Gdk::RGBA get_color(HashState state);
};

