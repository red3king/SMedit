#include <epoxy/gl.h>
#include <gtkmm.h>
#include <gdkmm/device.h>

#define NANOVG_GL3_IMPLEMENTATION   // TODO - figure out if i need this in both places (in main.cpp as well now)
                                    // is there even any nanovg stuff in main.cpp anymore?
#include "lib/nanovg/nanovg.h"
#include "lib/nanovg/nanovg_gl.h"

#include "gui_context.h"
#include "gui/operations/gui_op_factory.h"
#include "log.h"


GUIContext::GUIContext(Gtk::GLArea* gl_area, HistoryManager* history_manager)
{
    this->gl_area = gl_area;
    this->history_manager = history_manager;
    current_operation = nullptr;
    
    register_sm_signal_handlers();
    register_gtk_signal_handlers();
}


void GUIContext::set_machine(Machine* current_machine)
{
    gui_state.set_machine(current_machine);
}


void GUIContext::unset_machine()
{
    gui_state.unset_machine();
}


void GUIContext::register_sm_signal_handlers()
{
    signals.register_gui_context(this);
}


void GUIContext::unregister_sm_signal_handlers()
{
    signals.unregister_gui_context(this);
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


void GUIContext::handle_create(EntityType entity_type, SignalType signal_type, unsigned int entity_id)
{
    if(signal_type != CREATE)
        return;

    gui_state.add_gui_model(entity_type, entity_id);
}


void GUIContext::handle_delete(EntityType entity_type, SignalType signal_type, unsigned int entity_id)
{
    if(signal_type != PRE_DELETE)
        return;

    gui_state.remove_gui_model(entity_id);
}


void GUIContext::handle_gui_rebuild()
{
    gui_state.rebuild_models();
}


void GUIContext::handle_pre_gui_rebuild()
{
    gui_state.pre_rebuild_models();
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


bool GUIContext::handle_button_event(GdkEventButton* button_event)
{
    current_events.on_button_event(button_event);
    _handle_event();
    return true;
}


bool GUIContext::handle_scroll_event(GdkEventScroll* scroll_event)
{
    current_events.on_scroll_event(scroll_event);
    _handle_event();
    return true;
}


bool GUIContext::handle_key_event(GdkEventKey* key_event)
{
    current_events.on_key_event(key_event);
    _handle_event();
    return true;
}


bool GUIContext::handle_motion_notify_event(GdkEventMotion* motion_event)
{
    current_events.on_motion_notify_event(motion_event);
    _handle_event();
    return true;
}


bool GUIContext::has_current_operation()
{
    return !(current_operation == nullptr);
}


void GUIContext::_handle_event()
{
    // TODO - code for highlighting nearest object and other gui stuff
    // which is not based on operations goes here

    if(!has_current_operation())
    {
        if(!GUIOpFactory::maybe_create_gui_op(gui_state, current_events, current_operation))
        {
            gl_area->queue_draw();
            return;
        }
    }

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
