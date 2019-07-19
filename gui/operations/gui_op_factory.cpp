#include "gui_op_factory.h"
#include "gui_operation.h" 

#include "zoom.h"
#include "move.h"
#include "move_state.h"
#include "move_resourcelock.h"
#include "resize_box_entity.h"

#include "gui/models/gui_resourcelock.h"
#include "gui/models/gui_state.h"
#include "gui/models/gui_transition.h"


typedef bool(*maybe_func)(GUIModel*, GUIState&, CurrentEvents&, GUIOperation*&);


bool _apply_on(GUIState& gui_state, CurrentEvents& current_events, EntityType type, maybe_func fn, GUIOperation*& pref)
{
    GUIModel* model;
    for(int i=0; i<gui_state.gui_models.size(); i++)
    {
        model = gui_state.gui_models[i];
        if(model->type != type)
            continue;

        if(fn(model, gui_state, current_events, pref))
            return true;
    }

    return false;
}


bool _mcgo_state(GUIModel* model, GUIState& gui_state, CurrentEvents& current_events, GUIOperation*& pref)
{
    GMState* state_model = dynamic_cast<GMState*>(model);

    if(ResizeBoxEntity::maybe_create(state_model, gui_state, current_events, pref))
        return true;

    if(MoveState::maybe_create(state_model, gui_state, current_events, pref))
        return true;
    
    return false;
}


bool _mcgo_transition(GUIModel* model, GUIState& gui_state, CurrentEvents& current_events, GUIOperation*& pref)
{
    GMTransition* transition_model = dynamic_cast<GMTransition*>(model);

    return false;
}


bool _mcgo_resourcelock(GUIModel* model, GUIState& gui_state, CurrentEvents& current_events, GUIOperation*& pref)
{
    GMResourceLock* resourcelock_model = dynamic_cast<GMResourceLock*>(model);

    if(ResizeBoxEntity::maybe_create(resourcelock_model, gui_state, current_events, pref))
        return true;

    if(MoveResourceLock::maybe_create(resourcelock_model, gui_state, current_events, pref))
        return true;

    return false;
}


bool GUIOpFactory::maybe_create_gui_op(GUIState& gui_state, CurrentEvents& current_events, GUIOperation*& pref)
{
    if(_apply_on(gui_state, current_events, TRANSITION, _mcgo_resourcelock, pref))
        return true;

    if(_apply_on(gui_state, current_events, STATE, _mcgo_state, pref))
        return true;

    if(_apply_on(gui_state, current_events, RESOURCELOCK, _mcgo_resourcelock, pref))
        return true;

    if(Move::maybe_create(gui_state, current_events, pref))
        return true;

    if(Zoom::maybe_create(gui_state, current_events, pref))
        return true;

    return false;
}
