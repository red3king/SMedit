#pragma once

#include <vector>
#include <sigc++/sigc++.h>

#include "models/entity.h"

using std::vector;


class GUIContext;

enum SignalType { CREATE, PRE_DELETE, MODIFY };
enum CursorType { CT_DEFAULT, CT_MOVE, CT_RS_EW, CT_RS_NS, CT_RS_NW_SE, CT_RS_NE_SW };

typedef void(*GuiModelSignalHandler)(EntityType, SignalType, unsigned int);
typedef void(*NoArgHandler)();
typedef void(*SetCursorHandler)(CursorType);

class Machine;


class Signals
{
    public:
        Signals();

        sigc::signal<void> project_open, project_close;
        sigc::signal<void, EntityType, SignalType, unsigned int> model_changed;

        void register_set_cursor_handler(SetCursorHandler handler);
        void fire_set_cursor(CursorType cursor_type);

        void fire_model_changed(EntityType entity_type, SignalType signal_type, unsigned int entity_id);

        void enable_gui_signals();
        void disable_gui_signals();

        void register_gui_rebuild_handler(NoArgHandler handler);
        void fire_gui_rebuild_signal();
        
        void register_pre_gui_rebuild_handler(NoArgHandler handler);
        void fire_pre_gui_rebuild_signal();

        void register_gui_context(GUIContext* gui_context);
        void unregister_gui_context(GUIContext* gui_context);

    private:
        bool gui_signals_enabled;

        vector<GuiModelSignalHandler> gui_signal_handlers;            
        vector<NoArgHandler> gui_rebuild_handlers;
        vector<NoArgHandler> pre_gui_rebuild_handlers;
        vector<SetCursorHandler> set_cursor_handlers;
        
        vector<GUIContext*> gui_contexts;
};


extern Signals signals;

