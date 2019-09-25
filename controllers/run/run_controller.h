#pragma once


#define CHK_UNKNOWN -2
#define CHK_NONE -1

#include <gtkmm.h>

#include "signals.h"
#include "net/actionator.h"
#include "models/project.h"
#include "historymanager/historymanager.h"


class RunController
{
/*

    Project server_project
    uint32 local_hash, server_hash

    disconnected
        enable server entry
        disabled other butttons

    connected
        disabled server entry
        enabled other buttons


        save&deploy, (un)pause, start/stop:
            these are allmost the same, user clicks once and you do one thing or set of things
                they may fail and display some sort of error, or succeed

            
            save&deploy button:
            
            project command buttons (start/stop, pause/unpause, etc)
                enabled if connected and hashes match

            server hash - get on connect
            last saved project hash:

        log controller:
            react to a specific kind of message from the server
            maybe also print messages from the system (eg cant start b/c no code loaded)

        machine state:
            set_machine(machine or none)
                called when user clicks on runningn machine
            also reacts to messages from the server, but only those pertaining to the selected machine
            
      

        running machines:
            client requests list of running machines on connect to populate this
            afterwards, is changed by machine create/destroy event messages

            on click:
                tell machine state
                tell selected machine


        gui context:
            on running mahcine select
                set that machine

            on running machine update event
                feed that to gui state,
                    which will set the corresponding gui model's is_current_state flag to trigger highlight







TODO - ADD OPTIONS PANEL WITH
    -CONFIGURABLE STATE DELAY
    -CAMERA TRACKING 



*/


    public:
        RunController(HistoryManager* history_manager, Glib::RefPtr<Gtk::Builder> const& builder);

    private:
        HistoryManager* history_manager;
        LineClient line_client;
        Actionator actionator;

        Project deployed_project;
        bool has_deployed_project, project_open, is_connected, is_connecting;
        int32_t local_checksum, server_checksum;

        Gtk::Entry *server_entry;
        Gtk::Label *server_hash_label, *project_hash_label;
        Gtk::Button *connect_button, *deploy_button;

        void on_project_saved(uint16_t hash);
        void on_project_loaded(uint16_t hash);

        void on_project_open();
        void on_project_close();

        void on_connection_attempt_complete(ConnectionResult result);
        void on_disconnection(bool intentional);

        void update_enabled();

        void on_connect_click();
        void on_deploy_click();
        void on_echo_complete(Action* echo);  // for testing actionator temporary
};

