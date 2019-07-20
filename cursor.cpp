#include "cursor.h"
#include <string>


Glib::RefPtr<Gdk::Window> main_gdk_win;
std::map<CursorType, Glib::RefPtr<Gdk::Cursor>> cursors;


void set_cursor_handler(CursorType cursor_type)
{
    Glib::RefPtr<Gdk::Cursor> cursor;
    if(cursors.find(cursor_type) == cursors.end())
    {
        Glib::RefPtr<Gdk::Display> display = main_gdk_win->get_display();
        std::string cname;

        switch(cursor_type)
        {
            case CT_DEFAULT:
                cname = "default";
                break;
            case CT_MOVE:
                cname = "move";
                break;
            case CT_RS_EW:
                cname = "ew-resize";
                break;
            case CT_RS_NS:
                cname = "ns-resize";
                break;
            case CT_RS_NW_SE:
                cname = "nwse-resize";
                break;
            case CT_RS_NE_SW:
                cname = "nesw-resize";
                break;
        }

        cursors[cursor_type] = Gdk::Cursor::create(display, Glib::ustring(cname));
    }

    cursor = cursors[cursor_type];
    main_gdk_win->set_cursor(cursor);
}


void register_set_cursor_handler()
{
    signals.register_set_cursor_handler(set_cursor_handler);
}

