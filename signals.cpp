#include "signals.h"
#include "gui/gui_context.h"


Signals::Signals()
{
    gui_signals_enabled = true;
}


void Signals::register_gui_signal_handler(GuiModelSignalHandler handler)
{
    gui_signal_handlers.push_back(handler);
}


void Signals::fire_gui_signal(EntityType entity_type, SignalType signal_type, unsigned int entity_id)
{
    if(!gui_signals_enabled)
        return;

    for(int i=0; i<gui_signal_handlers.size(); i++)
        gui_signal_handlers[i](entity_type, signal_type, entity_id);

    for(int i=0; i<gui_contexts.size(); i++)
    {
        gui_contexts[i]->handle_create(entity_type, signal_type, entity_id);
        gui_contexts[i]->handle_delete(entity_type, signal_type, entity_id);
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


void Signals::register_gui_rebuild_handler(GuiRebuildHandler handler)
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


void Signals::register_pre_gui_rebuild_handler(GuiRebuildHandler handler)
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
