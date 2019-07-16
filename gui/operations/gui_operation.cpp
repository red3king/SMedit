#include "gui_operation.h"


GUIOpResult GUIOperation::should_continue(GUIState& gui_state, CurrentEvents& current_events) 
{ 
    return END; 
}


bool GUIOperation::on_continue(GUIState& gui_state, CurrentEvents& current_events, Operation*& op) 
{ 
    return false;
}


bool GUIOperation::on_cancel(GUIState& gui_state, CurrentEvents& current_events, Operation*& op) 
{
    return false;
}


bool GUIOperation::on_end(GUIState& gui_state, CurrentEvents& current_events, Operation*& op) 
{
    return false;
}

GUIOperation::~GUIOperation(){ }
