#include "gui_state.h"
#include "gui/models/gui_transition.h"
#include "gui/models/gui_resourcelock.h"
#include "gui/models/gui_state.h"


GUIModel* GUIState::get_model_by_id(unsigned int id)
{
    for(int i=0; i<gui_models.size(); i++)
    {
        GUIModel* model = gui_models[i];
        if(model->get_id() == id)
            return model;
    }

    return nullptr;
}


void GUIState::set_nvg_context(NVGcontext* vg)
{
    draw_context.set_nvg_context(vg);
}


void GUIState::set_machine(Machine* current_machine)
{
    this->current_machine = current_machine;
    draw_context.reset();
    _create_models();
}


void GUIState::_create_models()
{
    for(int i=0; i<current_machine->states.size(); i++)
    {
        State* state = current_machine->states[i];
        gui_models.push_back(new GMState(&draw_context, state));
    }

    for(int i=0; i<current_machine->transitions.size(); i++)
    {
        Transition* transition = current_machine->transitions[i];
        gui_models.push_back(new GMTransition(&draw_context, transition));
    }

    for(int i=0; i<current_machine->resourcelocks.size(); i++)
    {
        ResourceLock* lock = current_machine->resourcelocks[i];
        gui_models.push_back(new GMResourceLock(&draw_context, lock));
    }
}


void GUIState::unset_machine()
{
    this->current_machine = nullptr;
    _delete_models();
}


void GUIState::_delete_models()
{
    for(int i=0; i<gui_models.size(); i++)
        delete gui_models[i];

    gui_models.clear();
}


void GUIState::pre_rebuild_models()
{
    _delete_models();
}


void GUIState::rebuild_models()
{
    _create_models();
}


void GUIState::draw()
{
    for(int i=0; i<gui_models.size(); i++)
        gui_models[i]->draw();
}


void GUIState::add_gui_model(EntityType entity_type, unsigned int entity_id)
{
    bool created = true;
    GUIModel* new_model;

    switch(entity_type)
    {
        case STATE:
            {
                State* state = current_machine->get_state_by_id(entity_id);
                
                if(state == nullptr)
                    return;

                new_model = new GMState(&draw_context, state);
            }
            break;

        case RESOURCELOCK:
            {
                ResourceLock* lock = current_machine->get_resourcelock_by_id(entity_id);
                
                if(lock == nullptr)
                    return;

                new_model = new GMResourceLock(&draw_context, lock);
            }
            break;

        case TRANSITION:
            {
                Transition* transition = current_machine->get_transition_by_id(entity_id);
                if(transition == nullptr)
                    return;

                new_model = new GMTransition(&draw_context, transition);
            }
            break;

        default:
            return;
    }

    gui_models.push_back(new_model);
}


void GUIState::remove_gui_model(unsigned int entity_id)
{
    int index = -1;

    for(int i=0; i<gui_models.size(); i++)
    {
        if(gui_models[i]->get_id() == entity_id)
        {
            index = i;
            break;
        }
    }

    if(index == -1)
        return;

    delete gui_models[index];
    gui_models.erase(gui_models.begin() + index);
}






