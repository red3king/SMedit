#pragma once

#include "state_type_ctrl.h"
#include "controllers/custom_config_entry.h"


class CustomStateController : public StateTypeController
{
    public:
        CustomStateController(HistoryManager* history_manager, Glib::RefPtr<Gtk::Builder> const& builder);

    protected:
        void set_state_impl(Machine* owning_machine, State* selected_state);

    private:
        Gtk::Label* name_label;
	Gtk::Label* detail_label;

        CustomConfigEntry config_entry_ctrl;

        void on_config_changed(string parameter_name);
        void _update();
};
