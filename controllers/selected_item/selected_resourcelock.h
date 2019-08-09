#pragma once

#include <gtkmm.h>

#include "selected_item.h"
#include "models/resourcelock.h"


class SelectedResourceLock : public SelectedItemController
{
    public:
        SelectedResourceLock(HistoryManager* history_manager, Glib::RefPtr<Gtk::Builder> const& builder);

        void set_selected_impl(Entity* entity);
        Entity* get_entity();
        void update();
        void on_delete_clicked();

    private:
        ResourceLock* selected_resourcelock;
        Gtk::Label* selected_resourcelock_name;
        Gtk::Button* selected_resourcelock_delete;
};
