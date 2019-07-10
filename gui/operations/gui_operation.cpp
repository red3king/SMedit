#include "gui_operation.h"


GUIOpResult GUIOperation::should_continue(Project& current_project, GUIState& gui_state, CurrentEvents& current_events) { return END; }


void GUIOperation::on_continue(Project& current_project, GUIState& gui_state) { }


void GUIOperation::on_cancel(Project& current_project, GUIState& gui_state) { }


void GUIOperation::on_end(Project& current_project, GUIState& gui_state) { }


