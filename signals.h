#pragma once

#include <vector>
#include <sigc++/sigc++.h>

#include "models/entity.h"

using std::vector;


class GUIContext;

enum SignalType { CREATE, PRE_DELETE, MODIFY };
enum CursorType { CT_DEFAULT, CT_MOVE, CT_RS_EW, CT_RS_NS, CT_RS_NW_SE, CT_RS_NE_SW };

typedef void(*SetCursorHandler)(CursorType);

class Machine;


class Signals
{
    public:
        Signals();

        sigc::signal<void> project_open, project_close;
        sigc::signal<void, EntityType, SignalType, unsigned int> model_changed;
        sigc::signal<void, Machine*, EntityType, Entity*> model_selected;
        sigc::signal<void> pre_gui_rebuild, gui_rebuild;

        void register_set_cursor_handler(SetCursorHandler handler);
        void fire_set_cursor(CursorType cursor_type);

        void fire_model_changed(EntityType entity_type, SignalType signal_type, unsigned int entity_id);

        void enable_gui_signals();
        void disable_gui_signals();

        void fire_gui_rebuild_signal();
        void fire_pre_gui_rebuild_signal();

    private:
        bool gui_signals_enabled;

        // TODO - nuke this & replace w/ sigc::signals
        vector<SetCursorHandler> set_cursor_handlers;
};


extern Signals signals;

