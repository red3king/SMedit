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
    _init(tree_view, "", false);
}


ListViewController::ListViewController(Gtk::TreeView* tree_view, string title)
{
    _init(tree_view, title, true);
}


void ListViewController::_init(Gtk::TreeView* tree_view, string title, bool has_title)
{
    this->tree_view = tree_view;
    list_store = Gtk::ListStore::create(columns);
    is_setting = false;

    tree_view->set_model(list_store);
    tree_view->append_column(title, columns.name_column);

    if(!has_title)
        tree_view->set_headers_visible(false);

    selection = tree_view->get_selection();
    selection->signal_changed().connect(sigc::mem_fun(this, &ListViewController::on_selection_changed));
}


void ListViewController::select_item(unsigned int id)
{
    auto children = list_store->children();
    is_setting = true;

    for(auto iter = children.begin(); iter != children.end(); iter++)
    {
        auto row = *iter;
        if(row[columns.id_column] == id)
            selection->select(row);
    }

    is_setting = false;
}


void ListViewController::on_selection_changed()
{
    if(is_setting)
        return;
    
    auto iter = selection->get_selected();

    if(!iter)
    {
        selection_changed_signal.emit(0);
        selection_changed_details.emit(0, !is_setting);
        return;
    }

    auto row = *iter;
    unsigned int id = row[columns.id_column];
    selection_changed_signal.emit(id);
    selection_changed_details.emit(id, !is_setting);
}


void ListViewController::clear()
{
    is_setting = true;
    list_store->clear();
    is_setting = false;
}


void ListViewController::add_item(string item_name, unsigned int id)
{
    is_setting = true;
    auto row = *(list_store->append());
    row[columns.name_column] = item_name;
    row[columns.id_column] = id;
    is_setting = false;
}


void ListViewController::change_item(string item_name, unsigned int id)
{
    is_setting = true;
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

    is_setting = false;
}
