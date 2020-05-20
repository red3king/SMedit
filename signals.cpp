#include "signals.h"
#include "gui/gui_context.h"


Signals::Signals()
{
    gui_signals_enabled = true;
}


void Signals::register_set_cursor_handler(SetCursorHandler handler)
{
    set_cursor_handlers.push_back(handler);
}


void Signals::fire_set_cursor(CursorType cursor_type)
{
    for(int i = 0; i < set_cursor_handlers.size(); i++)
        set_cursor_handlers[i](cursor_type);
}


void Signals::fire_focus_operation(Operation* operation, unsigned int result)
{
    focus_operation.emit(operation, result);
}


void Signals::fire_model_changed(EntityType entity_type, SignalType signal_type, unsigned int entity_id, ChangeType change_type)
{
    if(!gui_signals_enabled)
        return;

    model_changed.emit(entity_type, signal_type, entity_id, change_type);
}


void Signals::enable_gui_signals()
{
    gui_signals_enabled = true;
}


void Signals::disable_gui_signals()
{
    gui_signals_enabled = false;
}


void Signals::fire_gui_rebuild_signal()
{
    gui_rebuild.emit();
}


void Signals::fire_pre_gui_rebuild_signal()
{
    pre_gui_rebuild.emit();
}


void Signals::fire_post_gui_rebuild_signal()
{
    post_gui_rebuild.emit();
}


void Signals::fire_project_saved()
{
    project_save.emit();
}


void Signals::fire_project_loaded()
{
    project_load.emit();
}


Signals signals;
