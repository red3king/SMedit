#pragma once

#include <vector>
#include <sigc++/sigc++.h>

#include "models/entity.h"
#include "historymanager/operation.h"

using std::vector;


class GUIContext;

enum SignalType { CREATE, PRE_DELETE, MODIFY };
enum CursorType { CT_DEFAULT, CT_MOVE, CT_RS_EW, CT_RS_NS, CT_RS_NW_SE, CT_RS_NE_SW };
enum ChangeType { CG_OTHER, CG_LOCATION, CG_CODE };

typedef void(*SetCursorHandler)(CursorType);

class Machine;


class Signals
{
    public:
        Signals();

        sigc::signal<void> project_open, project_close;
        sigc::signal<void> project_save, project_load;
        
        sigc::signal<void, EntityType, SignalType, unsigned int, ChangeType> model_changed;
        sigc::signal<void, Machine*, EntityType, Entity*> model_selected;
        
        sigc::signal<void> pre_gui_rebuild, gui_rebuild, post_gui_rebuild;

        sigc::signal<void, Operation*, unsigned int> focus_operation;  // UI listens to this to focus to the element corresponding to the undone / redone operation

        void register_set_cursor_handler(SetCursorHandler handler);
        void fire_set_cursor(CursorType cursor_type);

        void fire_focus_operation(Operation* operation, unsigned int result);
        void fire_model_changed(EntityType entity_type, SignalType signal_type, unsigned int entity_id, ChangeType change_type=CG_OTHER);

        void enable_gui_signals();
        void disable_gui_signals();

        void fire_gui_rebuild_signal();
        void fire_pre_gui_rebuild_signal();
        void fire_post_gui_rebuild_signal();
        
        void fire_project_saved();
        void fire_project_loaded();

    private:
        bool gui_signals_enabled;

        // TODO - nuke this & replace w/ sigc::signals
        vector<SetCursorHandler> set_cursor_handlers;
};


extern Signals signals;

