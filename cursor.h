#pragma once

#include <map>
#include <gtkmm.h>
#include "signals.h"

extern Glib::RefPtr<Gdk::Window> main_gdk_win;
extern std::map<CursorType, Glib::RefPtr<Gdk::Cursor>> cursors;


void set_cursor_handler(CursorType cursor_type);
void register_set_cursor_handler();
