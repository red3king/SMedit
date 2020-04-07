#pragma once

#include <sigc++/sigc++.h>
#include <gtkmm.h>
#include <vector>

#include "models/custom_config.h"


using std::vector;


class CustomConfigEntry
{
    public:
        CustomConfigEntry(Glib::RefPtr<Gtk::Builder> const& builder);
        
        void set_config(vector<CustomStateConfig>& custom_config);
        
        CustomStateConfig get_config(string parameter_name);
        
        sigc::signal<void, string> config_changed;  // string is name of param
        
    private:
        Gtk::Box* widgets_box;
        vector<CustomStateConfig> custom_config;
        bool is_setting, is_emitting;
        
        vector<Gtk::Box*> boxes;
        vector<Gtk::Label*> labels;
        vector<Gtk::Entry*> entries;
        vector<sigc::connection> entry_changed_connections;
        
        void on_entry_changed(int index);
        
        void _delete_widgets();
        void _create_widgets(vector<CustomStateConfig>& custom_config);
};

