#include <string>
#include <vector>
#include <gdkmm/rgba.h>

#include "code_state.h"
#include "log.h"


using namespace std;


CodeStateController::CodeStateController(Gtk::ScrolledWindow* parent_box)
{
    is_emitting = false;
    is_setting = false;

    // Init gtksourceview
    Gsv::init();    
    Glib::RefPtr<Gsv::LanguageManager> language_manager =  Gsv::LanguageManager::create();
    
    vector<string> langs = language_manager->get_language_ids();

    Glib::RefPtr<Gsv::Language> py_lang;
    bool python3_found = false;

    for(vector<string>::const_iterator iter = langs.begin(); iter != langs.end(); ++iter) 
    {
        py_lang = language_manager->get_language (*iter);

        if(py_lang->get_name() == "Python 3")
        {
            python3_found = true;
            break;
        }
    }

    if(python3_found)
        buffer = Gsv::Buffer::create(py_lang);

    else
    {
        log(LL_WARN, "could not find python3 language definition, syntax highlighting unavailable");
        buffer = Gsv::Buffer::create();
    }

    buffer->signal_changed().connect(sigc::mem_fun(this, &CodeStateController::on_buffer_changed));
    source_view = Gsv::View(buffer);
    parent_box->add(source_view);

    source_view.set_hexpand(true);
    source_view.set_vexpand(true);

    Gdk::RGBA color;
    color.set_grey(.1);
    source_view.override_background_color(color);

    source_view.show();
}


void CodeStateController::on_buffer_changed()
{
    if(is_setting)
        return;

    string text = buffer->get_text();
    is_emitting = true;
    signal_code_changed.emit(text); 
    is_emitting = false;
}


void CodeStateController::set_text(string code)
{
    if(is_emitting)
        return;

    is_setting = true;
    buffer->set_text(code);
    is_setting = false;
}

