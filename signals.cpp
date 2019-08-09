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

    // TODO - this can probably be deleted and replaced with the sigc++ signals
    for(int i=0; i<gui_contexts.size(); i++)
    {
        gui_contexts[i]->handle_create(entity_type, signal_type, entity_id);
        gui_contexts[i]->handle_delete(entity_type, signal_type, entity_id);
        gui_contexts[i]->update();
    }
}


void Signals::enable_gui_signals()
{
    gui_signals_enabled = true;
}


void Signals::disable_gui_signals()
{
    gui_signals_enabled = false;
}


void Signals::register_gui_rebuild_handler(NoArgHandler handler)
{
    gui_rebuild_handlers.push_back(handler);
}


void Signals::fire_gui_rebuild_signal()
{
    for(int i=0; i<gui_rebuild_handlers.size(); i++)
        gui_rebuild_handlers[i]();

    for(int i=0; i<gui_contexts.size(); i++)
        gui_contexts[i]->handle_gui_rebuild();
}


void Signals::register_pre_gui_rebuild_handler(NoArgHandler handler)
{
    pre_gui_rebuild_handlers.push_back(handler);
}


void Signals::fire_pre_gui_rebuild_signal()
{
    for(int i=0; i<pre_gui_rebuild_handlers.size(); i++)
        pre_gui_rebuild_handlers[i]();

    for(int i=0; i<gui_contexts.size(); i++)
        gui_contexts[i]->handle_pre_gui_rebuild();
}


void Signals::register_gui_context(GUIContext* gui_context)
{
    gui_contexts.push_back(gui_context);
}


void Signals::unregister_gui_context(GUIContext* gui_context)
{
    int index = -1;
    for(int i=0; i<gui_contexts.size(); i++)
    {
        if(gui_contexts[i] == gui_context)
        {
            index = i;
            break;
        }
    }
    
    if(index != -1)
        gui_contexts.erase(gui_contexts.begin() + index);
}



Signals signals;
