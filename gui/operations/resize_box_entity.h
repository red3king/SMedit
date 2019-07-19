#pragma once

#include "gui_operation.h"
#include "gui/models/gui_box_model.h"
#include "gui/gui_state.h"


class ResizeBoxEntity : public GUIOperation
{
    public:
        ResizeBoxEntity(GMBox* gm_box, BorderType resize_border);

        static bool maybe_create(GMBox* gm_box, GUIState& gui_state, CurrentEvents& current_events, GUIOperation*& prpef);

        GUIOpResult should_continue(GUIState& gui_state, CurrentEvents& current_events);

        bool on_continue(GUIState& gui_state, CurrentEvents& current_events, Operation*& op);

    private:
        BorderType resize_border;
        GMBox* gm_box;
};
