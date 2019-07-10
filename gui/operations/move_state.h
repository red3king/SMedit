#include "gui_operation.h"


class MoveState : public GUIOperation
{
    public:
        static bool maybe_create(Project& current_project, GUIState& gui_state, CurrentEvents& current_events, GUIOperation*& pref);

        GUIOpResult should_continue(Project& current_project, GUIState& gui_state, CurrentEvents& current_events);

        void on_continue(Project& current_project, GUIState& gui_state);

        void on_end(Project& current_project, GUIState& gui_state);
};
