#include "state_type_ctrl.h"


StateTypeController::StateTypeController(HistoryManager* history_manager)
{
    selected_state = nullptr;
    owning_machine = nullptr;
    this->history_manager = history_manager;
}


StateTypeController::~StateTypeController(){ }


void StateTypeController::set_state(Machine* owning_machine, State* selected_state)
{
    this->owning_machine = owning_machine;
    this->selected_state = selected_state;
    set_state_impl(owning_machine, selected_state);
}


bool StateTypeController::has_state()
{
    return selected_state != nullptr;
}
