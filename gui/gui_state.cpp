#include "log.h"
#include "gui_state.h"
#include "colors.h"
#include "gui/models/gui_transition.h"
#include "gui/models/gui_resourcelock.h"
#include "gui/models/gui_state.h"


GUIState::GUIState(GUIAreaMode execution_mode, RunningState* running_state, Gtk::GLArea* gl_area) : draw_context(gl_area)
{
    mode = execution_mode;
    this->running_state = running_state; // only  for GAM_RUN

    //if(mode == GAM_RUN)
    //    running_state->select_state.connect(sigc::mem_fun(*this, &GUIState::on_rs_state_select));
}


void GUIState::rs_state_select(int state_def_id)
{
    for(int i=0; i<gui_models.size(); i++)
    {
        auto model = gui_models[i];

        if(model->type != STATE)
            continue;

        GMState* state = (GMState*) model;
        bool current = state->get_state_def_id() == state_def_id;
        state->set_is_current(current);
    }
}


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
    create_models();
}


void GUIState::create_models()
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
    delete_models();
}


void GUIState::delete_models()
{
    for(int i=0; i<gui_models.size(); i++)
        delete gui_models[i];

    gui_models.clear();
}


void GUIState::draw()
{
    for(int i=0; i<gui_models.size(); i++)
        gui_models[i]->draw();

    int font = draw_context.font_hack;
    float current_x, current_y, current_zoom;
    current_zoom = draw_context.get_zoom_factor();
    auto gl_area = draw_context.get_gl_area();
    float sch = gl_area->get_height();
    float scw = gl_area->get_width();
    draw_context.screen_to_world(current_x, current_y, scw/2.0, sch/2.0);
    string x_line = "x = " + std::to_string(current_x);
    string y_line = "y = " + std::to_string(current_y);
    string z_line = "zoom = " + std::to_string(current_zoom);
    string wxline = "woffset_x = " + std::to_string(draw_context.woffset_x);
    string wyline = "woffset_y = " + std::to_string(draw_context.woffset_y);
    draw_context.draw_text_one_line("[screen center]", font, 15, WHITE, 10, 0, 300);
    draw_context.draw_text_one_line(x_line, font, 15, WHITE, 10, 20, 300);
    draw_context.draw_text_one_line(y_line, font, 15, WHITE, 10, 40, 300);
    draw_context.draw_text_one_line(z_line, font, 15, WHITE, 10, 60, 300);
    draw_context.draw_text_one_line(wxline, font, 15, WHITE, 10, 80, 300);
    draw_context.draw_text_one_line(wyline, font, 15, WHITE, 10, 100, 300);
}


CursorType _filterupdate(vector<GUIModel*>& models, CurrentEvents& current_events, EntityType type, GUIModel*& just_selected)
{
    CursorType result = CT_DEFAULT;
    CursorType res;

    for(int i=0; i<models.size(); i++)
    {
        if(models[i]->type == type)
        {
            res = models[i]->update(current_events, just_selected);
    
            if(result == CT_DEFAULT)
                result = res;
        }
    }

    return result;
}


CursorType GUIState::update_models(CurrentEvents& current_events, GUIModel*& just_selected, bool& clear_selected)
{
    // asks each of the models if the cursor is within them, did they get selected, etc..
    CursorType result = CT_DEFAULT;
    CursorType current;
    just_selected = nullptr;
    clear_selected = false;

    for(int i=0; i<gui_update_order.size(); i++)
    {
        EntityType to_update = gui_update_order[i];
        current = _filterupdate(gui_models, current_events, to_update, just_selected);

        if(result == CT_DEFAULT)
            result = current;
    }

    if(just_selected == nullptr && current_events.event_type == ET_MB_PRESS && 
            current_events.button_pressed(MB_LEFT))
        clear_selected = true;

    // GUI models can't tell when they have been deselected
    // they must be told when something else gets clicked
    for(int i=0; i<gui_models.size(); i++)
    {
        GUIModel* model = gui_models[i];
        if(clear_selected || (just_selected != nullptr && just_selected != model))
            model->set_selected(false);
    }

   return result; 
}


void GUIState::restore_selected_entity(Entity* entity)
{
    for(int i=0; i<gui_models.size(); i++)
    {
        if(gui_models[i]->get_entity() == entity)
            gui_models[i]->set_selected(true);
    }
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


void GUIState::force_gui_context_update()
{
    signal_force_update.emit();
}   
