#pragma once

#include <sigc++/sigc++.h>
#include <gtkmm.h>
#include <string>

using std::string;


class ListViewColumns : public Gtk::TreeModel::ColumnRecord
{
    public:
        ListViewColumns();

        Gtk::TreeModelColumn<Glib::ustring> name_column;
        Gtk::TreeModelColumn<unsigned int> id_column;
};


class ListViewController
{
    // Thin wrapper around TreeView for displaying list of names associated with ids
    public:
        ListViewController(Gtk::TreeView* tree_view);   // column header hidden
        ListViewController(Gtk::TreeView* tree_view, string title);

        sigc::signal<void, unsigned int> selection_changed_signal;  // value is object id

        void _init(Gtk::TreeView* tree_view, string title, bool has_title);

        void clear();
        void add_item(string item_name, unsigned int id);
        void change_item(string item_name, unsigned int id);
        void select_item(unsigned int id);

    private:
        Gtk::TreeView* tree_view;
        Glib::RefPtr<Gtk::ListStore> list_store;
        Glib::RefPtr<Gtk::TreeSelection> selection;
        ListViewColumns columns;

        void on_selection_changed();
};
