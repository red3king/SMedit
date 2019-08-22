#pragma once

#include "historymanager/historymanager.h"
#include "models/entity.h"
#include "signals.h"


class SelectedItemController
{
    public:
        SelectedItemController(HistoryManager* history_manager);

        void set_selected(Machine* machine, Entity* entity);

        virtual void set_selected_impl(Machine* owning_machine, Entity* entity)=0;
        virtual Entity* get_entity()=0;
        virtual void update()=0;

        virtual ~SelectedItemController()=0;

    protected:
        HistoryManager* history_manager;
        Machine* owning_machine;

    private:
        void on_model_changed(EntityType type, SignalType signal_type, unsigned int entity_id);
};
