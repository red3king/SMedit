#pragma once

#include <string>
#include <gtkmm.h>
#include <gtksourceviewmm.h>


using std::string;


class CodeStateController
{
    public:
        CodeStateController(Gtk::ScrolledWindow* parent_box);

        sigc::signal<void, string> signal_code_changed;

        void set_text(string code);

    private:
        bool is_emitting, is_setting;
        Gsv::View source_view;
        Glib::RefPtr<Gsv::Buffer> buffer;

        void on_buffer_changed();
};
