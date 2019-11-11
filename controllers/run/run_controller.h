#pragma once


#define CHK_UNKNOWN -1

#include <gtkmm.h>

#include "saveload.h"
#include "signals.h"
#include "net/actionator.h"
#include "net/broadcast_events.h"
#include "models/project.h"
#include "historymanager/historymanager.h"


enum HashState { HS_UNKNOWN, HS_SAME, HS_DIFFERENT };


class RunController
{
    /*
    TODO 
     - ADD OPTIONS PANEL WITH
     -CONFIGURABLE STATE DELAY
     -CAMERA TRACKING 
    */

    public:
        RunController(HistoryManager* history_manager, ProjectInfo* project_info, Glib::RefPtr<Gtk::Builder> const& builder);

    private:
        HistoryManager* history_manager;
        ProjectInfo* project_info;

        LineClient line_client;
        Actionator actionator;
        BroadcastEvents broadcast_events;       

        HashState hash_state;
        int32_t local_checksum, server_checksum;
        bool has_deployed_project, project_open, is_connected, is_connecting;
        bool project_started, project_paused;
        Project deployed_project;

        Gtk::Entry *server_entry;
        Gtk::Label *server_hash_label, *project_hash_label;
        Gtk::Button *connect_button, *deploy_button, *start_button, *pause_button;

        Gdk::RGBA rgb_same, rgb_unknown, rgb_different;

        void on_project_saved();
        void on_project_loaded();

        void on_project_open();
        void on_project_close();

        void on_connection_attempt_complete(ConnectionResult result);
        
        void on_get_hash_complete(Action* action);
        void on_start_stop_complete(Action* action);

        void on_disconnection(bool intentional);

        void on_connect_click();
        void on_deploy_click();
        void on_start_click();
        void on_pause_click();


        void update_enabled();
        void begin_get_hash();

        Gdk::RGBA get_color(HashState state);
};

