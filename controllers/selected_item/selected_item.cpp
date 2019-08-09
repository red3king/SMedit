#include "selected_item.h"


SelectedItemController::SelectedItemController(HistoryManager* history_manager)
{
    this->history_manager = history_manager;
    signals.model_changed.connect(sigc::mem_fun(this, &SelectedItemController::on_model_changed));
    owning_machine = nullptr;
}


SelectedItemController::~SelectedItemController()
{

}


void SelectedItemController::set_selected(Machine* owning_machine, Entity* entity)
{
    this->owning_machine = owning_machine;
    set_selected_impl(entity);
    update();
}


void SelectedItemController::on_model_changed(EntityType type, SignalType signal_type, unsigned int entity_id)
{
    Entity* my_selected = get_entity();

    if(my_selected == nullptr)
        return;

    if(my_selected->id != entity_id)
        return;

    if(signal_type == MODIFY)
    {
        update();
        return;
    }

    if(signal_type == PRE_DELETE)
    {
        Machine* owner = owning_machine;
        set_selected(nullptr, nullptr);
        // unselect and hide this controller if object is deleting
        signals.model_selected.emit(owner, NONE_ENTITY, nullptr); 
        return;
    }
}
