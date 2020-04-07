#include "custom_config_entry.h"


CustomConfigEntry::CustomConfigEntry(Glib::RefPtr<Gtk::Builder> const& builder)
{
    is_setting = false;
    is_emitting = false;
    
    builder->get_widget("custom_state_options_box", widgets_box);
}


void CustomConfigEntry::set_config(vector<CustomStateConfig>& custom_config)
{
    if(is_emitting)
        return;
    
    is_setting = true;
    widgets_box->hide();
    _delete_widgets();
    _create_widgets(custom_config);
    this->custom_config = custom_config;
    widgets_box->show();
    is_setting = false;
}


void CustomConfigEntry::_create_widgets(vector<CustomStateConfig>& custom_config)
{
    for(int i = 0; i < custom_config.size(); i++)
    {
        string name_string = custom_config[i].name;
        LVOV value = custom_config[i].value;
        ValueType value_type = value.type;
        
        // create and add widgets
        Gtk::Label* label = new Gtk::Label();
        label->set_markup("<b>" + name_string + "</b>");
        
        Gtk::Entry* entry = new Gtk::Entry();
        entry->set_text(value.to_string());
        
        Gtk::Box* box = new Gtk::Box();  // default = horizontal orientation
        box->pack_start(*entry, true, true);
        box->pack_start(*label, true, true);
        
        auto bound = sigc::bind<int>(sigc::mem_fun(*this, &CustomConfigEntry::on_entry_changed), i);
        auto connection = entry->signal_changed().connect(bound);
        
        boxes.push_back(box);
        entries.push_back(entry);
        labels.push_back(label);
        entry_changed_connections.push_back(connection);
        
        widgets_box->pack_end(*box, true, true);
        
        label->show();
        entry->show();
        box->show();
    }
}


void CustomConfigEntry::_delete_widgets()
{
    for(int i = 0; i < boxes.size(); i++)
    {
        Gtk::Box* box = boxes[i];
        Gtk::Label* label = labels[i];
        Gtk::Entry* entry = entries[i];
        
        // remove boxes from main box:
        widgets_box->remove(*box);
        
        // remove labels and entries from boxes:
        box->remove(*label);
        box->remove(*entry);
        
        // disconnect entry changed signals
        entry_changed_connections[i].disconnect();
    }
    
    boxes.clear();
    labels.clear();
    entries.clear();
    entry_changed_connections.clear();
}


void CustomConfigEntry::on_entry_changed(int index)
{
    if(is_setting)
        return;
    
    Gtk::Entry* entry = entries[index];
    string input = entry->get_text();
    
    auto lvov = custom_config[index].value;
    string param_name = custom_config[index].name;
    
    if(lvov.from_string(input))
    {
        custom_config[index] = CustomStateConfig(param_name, lvov);
        is_emitting = true;
        config_changed.emit(param_name);
        is_emitting = false;
    }
    
    else
    {
        is_setting = true;
        entry->set_text(lvov.to_string());
        is_setting = false;
    }
}


CustomStateConfig CustomConfigEntry::get_config(string parameter_name)
{
    int i;
    
    for(i = 0; i < custom_config.size(); i++)
        if(custom_config[i].name == parameter_name)
            break;
        
    return custom_config[i];
}

