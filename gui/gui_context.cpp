#include <epoxy/gl.h>
#include <gtkmm.h>
#include <gdkmm/device.h>

#define NANOVG_GL3_IMPLEMENTATION   
#include "lib/nanovg/nanovg.h"
#include "lib/nanovg/nanovg_gl.h"

#include "gui_context.h"
#include "gui/operations/gui_op_factory.h"
#include "log.h"


GUIContext::GUIContext(Gtk::GLArea* gl_area, HistoryManager* history_manager, GUIAreaMode execution_mode, RunningState* running_state) : gui_state(execution_mode, running_state, gl_area)
{
    this->gl_area = gl_area;
    this->history_manager = history_manager;
    mode = execution_mode;
    current_operation = nullptr;
    current_machine_id = 0;
    current_cursor_type = CT_DEFAULT;

    gui_state.signal_force_update.connect(sigc::mem_fun(*this, &GUIContext::update));
    register_sm_signal_handlers();
    register_gtk_signal_handlers();

    if(mode == GAM_RUN)
    {
        running_state->select_machine.connect(sigc::mem_fun(*this, &GUIContext::rs_hndl_select_machine));
        running_state->select_state.connect(sigc::mem_fun(*this, &GUIContext::rs_hndl_select_state));
    }
}


void GUIContext::rs_hndl_select_machine(Machine* machine_def)
{
    // RUN mode only
    if(machine_def == nullptr)
        unset_machine();
    else
        set_machine(machine_def);
}


void GUIContext::rs_hndl_select_state(int state_def_id)
{
    // RUN mode only
    // when state is selected, set desired x,y to center of state
    // set zoom factor such that screen includes longest of state's transitions.

    auto state_def = current_machine->get_state_by_id(state_def_id);

    float target_x, target_y, target_zoom;
    calc_to_state_zoom(state_def, target_x, target_y, target_zoom);

    float current_x, current_y, current_zoom;
    gui_state.draw_context.get_offsets(current_x, current_y);
    current_zoom = gui_state.draw_context.get_zoom_factor();
    
    current_events.enable_ap(current_x, current_y, current_zoom, target_x, target_y, target_zoom);
    gui_state.rs_state_select(state_def_id);
    update();
}


void GUIContext::calc_to_state_zoom(State* state, float& target_x, float& target_y, float& target_zoom)
{
    // find longest transition for zoom
    float scw = gl_area->get_width();
    float sch = gl_area->get_height();
    float longest_transition = 0;
    vector<Transition*> all_transitions = state->get_all_transitions();

    for(int i=0; i<all_transitions.size(); i++)
    {
        float length = all_transitions[i]->get_length();
        if(length > longest_transition)
            longest_transition = length;
    }

    // goal -  in screen coordinates, longest transition should be approrximately 
    // the size of the screen
    float longest_transition_screen = gui_state.draw_context.world_dist_to_screen(longest_transition);
    float half_screen_diagonal = sqrt(pow(scw,2) + pow(sch,2)) / 2.0;
    float ratio = longest_transition_screen / half_screen_diagonal;
    
    target_zoom = gui_state.draw_context.get_zoom_factor() / ratio;

    // target x,y is center of the state:
    float half_screen_w = .5 * scw / target_zoom;
    float half_screen_h = .5 * sch / target_zoom;

    target_x = state->x + state->w/2.0 - half_screen_w;
    target_y = state->y + state->h/2.0 - half_screen_h;
}


void GUIContext::set_machine(Machine* current_machine)
{
    this->current_machine = current_machine;
    current_machine_id = current_machine->id;
    gui_state.set_machine(current_machine);

    // pan to location of initial state
    State* state = nullptr;
    for(int i=0; i<current_machine->states.size(); i++)
    {
        if(current_machine->states[i]->type == INITIAL)
            state = current_machine->states[i];
    }
    
    if(state != nullptr)
    {
        float target_x, target_y, target_zoom;
        calc_to_state_zoom(state, target_x, target_y, target_zoom);
        gui_state.draw_context.move_raw(target_x, target_y);
        gui_state.draw_context.zoom_raw(target_zoom);
    }
    
    update();
}


void GUIContext::unset_machine()
{
    current_machine = nullptr;
    current_machine_id = 0;
    gui_state.unset_machine();
}


void GUIContext::register_sm_signal_handlers()
{
    if(mode == GAM_BUILD)
        signals.model_changed.connect(sigc::mem_fun(this, &GUIContext::handle_model_changed));
}


void GUIContext::unregister_sm_signal_handlers()
{

}


void GUIContext::register_gtk_signal_handlers()
{
    gl_area->set_events(Gdk::ALL_EVENTS_MASK);

    gl_area->signal_realize().connect(sigc::mem_fun(this, &GUIContext::on_gl_realize));
    gl_area->signal_unrealize().connect(sigc::mem_fun(this, &GUIContext::on_gl_unrealize), false);
    gl_area->signal_render().connect(sigc::mem_fun(this, &GUIContext::on_gl_render), false);

    gl_area->signal_button_press_event().connect(sigc::mem_fun(this, &GUIContext::handle_button_event));
    gl_area->signal_button_release_event().connect(sigc::mem_fun(this, &GUIContext::handle_button_event));
    gl_area->signal_scroll_event().connect(sigc::mem_fun(this, &GUIContext::handle_scroll_event));
    gl_area->signal_key_press_event().connect(sigc::mem_fun(this, &GUIContext::handle_key_event));
    gl_area->signal_key_release_event().connect(sigc::mem_fun(this, &GUIContext::handle_key_event));
    gl_area->signal_motion_notify_event().connect(sigc::mem_fun(this, &GUIContext::handle_motion_notify_event));
}


void GUIContext::handle_model_changed(EntityType entity_type, SignalType signal_type, unsigned int entity_id)
{
    if(signal_type == PRE_DELETE && entity_type == MACHINE && entity_id == current_machine_id)
        current_machine_id = 0;
    
    if(entity_type != STATE && entity_type != TRANSITION && entity_type != RESOURCELOCK)
        return;

    if(signal_type == CREATE)
        gui_state.add_gui_model(entity_type, entity_id);


    if(signal_type == PRE_DELETE)
        gui_state.remove_gui_model(entity_id);

    gl_area->queue_draw();
}


void GUIContext::reset(bool reload)
{
    if(!reload)
        current_machine_id = 0;

    gui_state.unset_machine();
}


void GUIContext::load_from(Project& current_project, bool reload)
{
    if(reload)
    {
        Machine* current_machine = current_project.get_machine_by_id(current_machine_id);

        if(current_machine != nullptr)
            gui_state.set_machine(current_machine);
    }
}


void GUIContext::on_gl_realize()
{
    gl_area->make_current();
    gl_area->set_has_stencil_buffer(true);
    vg = nvgCreateGL3(NVG_ANTIALIAS | NVG_STENCIL_STROKES | NVG_DEBUG);
    gl_area->throw_if_error();
    gui_state.set_nvg_context(vg);
}


void GUIContext::on_gl_unrealize()
{
    gl_area->make_current();
    gl_area->throw_if_error();
    // TODO should probably tear down the nvgcontext here and unset it from gui_state
}


bool GUIContext::on_gl_render(const Glib::RefPtr<Gdk::GLContext>& context)
{
    _render();
    return true;
}


void GUIContext::_render()
{
    gl_area->throw_if_error();
    int width = gl_area->get_allocated_width();
    int height = gl_area->get_allocated_height();
    float px_ratio = (float) width / (float) height;
    glViewport(0, 0, width, height);

    glClearColor(0.5, 0.5, 0.5, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    nvgBeginFrame(vg, width, height, px_ratio);
    
    gui_state.draw();

    nvgEndFrame(vg);
    glFlush();
}


void GUIContext::restore_selected_entity(Entity* entity)
{
    gui_state.restore_selected_entity(entity);
}


bool GUIContext::handle_button_event(GdkEventButton* button_event)
{
    current_events.on_button_event(button_event);
    update();
    return true;
}


bool GUIContext::handle_scroll_event(GdkEventScroll* scroll_event)
{
    current_events.on_scroll_event(scroll_event);
    update();
    return true;
}


bool GUIContext::handle_key_event(GdkEventKey* key_event)
{
    current_events.on_key_event(key_event);
    update();
    return true;
}


bool GUIContext::handle_motion_notify_event(GdkEventMotion* motion_event)
{
    current_events.on_motion_notify_event(motion_event);
    update();
    return true;
}


bool GUIContext::has_current_operation()
{
    return !(current_operation == nullptr);
}


void GUIContext::update()
{
    // Let gui models react to mouse being nearby & possibly get new cursor 
    bool clear_selected;
    GUIModel* just_selected;
    CursorType new_cursor = gui_state.update_models(current_events, just_selected, clear_selected);   

    if(mode == GAM_BUILD)
    {
        // this signal is for the selected item controller, BUILD mode only    
        if(just_selected != nullptr)
            signals.model_selected.emit(current_machine, just_selected->type, just_selected->get_entity());

        if(clear_selected)
            signals.model_selected.emit(current_machine, NONE_ENTITY, nullptr);
    }

    if(new_cursor != current_cursor_type)
    {
        current_cursor_type = new_cursor;
        signals.fire_set_cursor(current_cursor_type);
    }

    // Maybe start a new gui operation

    if(!has_current_operation())
    {
        if(!GUIOpFactory::maybe_create_gui_op(gui_state, current_events, current_operation, mode))
        {
            gl_area->queue_draw();
            return;
        }
    }

    // Continue current gui operation

    Operation* new_op = nullptr;
    bool created = false;

    GUIOpResult result = current_operation->should_continue(gui_state, current_events);

    if(result == END)
    {
        created = current_operation->on_end(gui_state, current_events, new_op);
        delete current_operation;
        current_operation = nullptr;
    }
    
    else if(result == CONTINUE)
    {
        created = current_operation->on_continue(gui_state, current_events, new_op);
    }

    else if(result == CANCEL)
    {
        created = current_operation->on_cancel(gui_state, current_events, new_op);
        delete current_operation;
        current_operation = nullptr;
    }

    if(created)
    {
        history_manager->submit_operation(*new_op);
        delete new_op;
    }

    gl_area->queue_draw();
}
