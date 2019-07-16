#pragma once

#include <vector>

#include "models/entity.h"

using std::vector;


class GUIContext;
enum SignalType { CREATE, PRE_DELETE };
typedef void(*GuiModelSignalHandler)(EntityType, SignalType, unsigned int);
typedef void(*GuiRebuildHandler)();


class Signals
{
    // note: "gui signals" here means signals which are used to create / delete the 
    // GUIModel objects for the glgtkarea 

    public:
        void register_gui_signal_handler(GuiModelSignalHandler handler);
        void fire_gui_signal(EntityType entity_type, SignalType signal_type, unsigned int entity_id);

        void enable_gui_signals();
        void disable_gui_signals();

        void register_gui_rebuild_handler(GuiRebuildHandler handler);
        void fire_gui_rebuild_signal();
        
        void register_pre_gui_rebuild_handler(GuiRebuildHandler handler);
        void fire_pre_gui_rebuild_signal();

        void register_gui_context(GUIContext* gui_context);
        void unregister_gui_context(GUIContext* gui_context);

    private:
        bool gui_signals_enabled;
        vector<GuiModelSignalHandler> gui_signal_handlers;            
        vector<GuiRebuildHandler> gui_rebuild_handlers;
        vector<GuiRebuildHandler> pre_gui_rebuild_handlers;
        vector<GUIContext*> gui_contexts;
};


extern Signals signals;

