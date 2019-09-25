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
    for(int i=0; i<set_cursor_handlers.size(); i++)
        set_cursor_handlers[i](cursor_type);
}


void Signals::fire_model_changed(EntityType entity_type, SignalType signal_type, unsigned int entity_id)
{
    if(!gui_signals_enabled)
        return;

    model_changed.emit(entity_type, signal_type, entity_id);
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


void Signals::fire_project_saved(uint16_t hash)
{
    project_save.emit(hash);
}


void Signals::fire_project_loaded(uint16_t hash)
{
    project_load.emit(hash);
}


Signals signals;
