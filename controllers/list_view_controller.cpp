#include "list_view_controller.h"


// ListViewColumns

ListViewColumns::ListViewColumns()
{
    add(name_column);
    add(id_column);
}



// ListViewController

ListViewController::ListViewController(Gtk::TreeView* tree_view)
{
    this->tree_view = tree_view;
    list_store = Gtk::ListStore::create(columns);

    tree_view->set_model(list_store);
    tree_view->append_column("Name:", columns.name_column);

    selection = tree_view->get_selection();
    selection->signal_changed().connect(sigc::mem_fun(this, &ListViewController::on_selection_changed));
}


void ListViewController::select_item(unsigned int id)
{
    auto children = list_store->children();

    for(auto iter = children.begin(); iter != children.end(); iter++)
    {
        auto row = *iter;
        if(row[columns.id_column] == id)
            selection->select(row);
    }
}


void ListViewController::on_selection_changed()
{
    auto iter = selection->get_selected();

    if(!iter)
    {
        selection_changed_signal.emit(0);
        return;
    }

    auto row = *iter;
    unsigned int id = row[columns.id_column];
    selection_changed_signal.emit(id);
}


void ListViewController::clear()
{
    list_store->clear();
}


void ListViewController::add_item(string item_name, unsigned int id)
{
    auto row = *(list_store->append());
    row[columns.name_column] = item_name;
    row[columns.id_column] = id;
}


void ListViewController::change_item(string item_name, unsigned int id)
{
    auto children = list_store->children();

    for(auto iter = children.begin(); iter != children.end(); iter++)
    {
        auto row = *iter;
        if(row[columns.id_column] == id)
        {
            row[columns.name_column] = item_name;
            break;
        }
    }
}
