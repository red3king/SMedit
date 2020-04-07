#include "gui_state.h"
#include "gui/colors.h"


GMState::GMState(DrawContext* ctx, State* state) : GMBox(ctx, STATE)
{
    this->state = state;
    is_current_state = false;
}


int GMState::get_state_def_id()
{
    return state->id;
}


void GMState::set_is_current(bool is_current)
{
    is_current_state = is_current;
}


Entity* GMState::get_entity()
{
    return state;
}


bool GMState::is_custom()
{
    return state->is_custom();
}


NVGcolor GMState::get_border_color()
{
    return nvgRGBA(255, 255, 255, 255);
}


NVGcolor GMState::get_title_text_color()
{
    return nvgRGBA(0, 0, 0, 255);
}


std::string GMState::get_title()
{
    return state->name;
}


void GMState::draw_interior(float x, float y, float w, float h)
{
    // background fill
    NVGcontext* vg = ctx->vg;
    nvgBeginPath(vg);     
    nvgRect(vg, x, y, w, h);

    // TODO make current selection better ? at the very least, make sure pink has good contrast
    // to all interior colors
    auto bg_col = is_current_state ? ORANGEREDSEETHROUGH : nvgRGBA(0x00, 0x2b, 0x36, 155); 
    nvgFillColor(vg, bg_col);
    nvgFill(vg);

    // adjust for the icon size, may change this later
    y += 10;
    h -= 10;

    switch(state->type)
    {
        case INITIAL:
            draw_interior_initial(x, y, w, h);
            break;
        case CODE:
            draw_interior_code(x, y, w, h);
            break;
        case RETURN:
            draw_interior_return(x, y, w, h);
            break;
        case SPAWN:
            draw_interior_spawn(x, y, w, h);
            break;
        case JOIN:
            draw_interior_join(x, y, w, h);
            break;
    }
}


void GMState::draw_interior_initial(float x, float y, float w, float h)
{
    text("Arguments:", ORANGE);

    int len = state->initial_args.size();

    if(len == 0)
    {
        text("  none", WHITE);
        return;
    }

    for(int i = 0; i < len; i++)
    {
        ArgDef def = state->initial_args[i];
        auto vtstr = value_type_to_string(def.value_type);
        text("  " + def.variable_name + " - " + vtstr, WHITE);
    }
}


void GMState::draw_interior_code(float x, float y, float w, float h)
{
    // render the first few code lines in the gui box
    int len = state->code.length();
    if(len == 0)
    {
        text("[empty]", GRAYWHITE);
        return;
    }

    int j = 0;
    for(int i = 0; i < len; i++)
    {
        if(state->code[i] == '\n' || i == len - 1)
        {
            string line(state->code, j, 1+i-j);

            if(!text(line, WHITE))
                return;

            j = i + 1;
        }
    }
}


void GMState::draw_interior_return(float x, float y, float w, float h)
{
    text("Return Value:", ORANGE);
    
    if(!state->has_return_value)
    {
       text("  None", WHITE);
       return;
    }

    text("  " + state->return_value.describe(), WHITE);
}


void GMState::draw_interior_spawn(float x, float y, float w, float h)
{
    string res_str = state->launch_synchronous ? "Result Variable:" : "Pid Variable:";

    text("Machine: ", ORANGE);
    text("  " + state->launch_task_name.describe(), WHITE);
    
    text(res_str, ORANGE);
    text("  " + state->launch_result_variable, WHITE);
    
    text("Args:", ORANGE); 
    int len = state->launch_args.size();

    if(len == 0)
        text("  none", WHITE);

    else
    {
        for(int i = 0; i < len; i++)
            text("  " + state->launch_args[i].describe(), WHITE);
    }    
}


void GMState::draw_interior_join(float x, float y, float w, float h)
{
    text("Pid variable:", ORANGE);
    text("  " + state->join_pid_variable, WHITE);
    text("Result variable:", ORANGE);
    text("  " + state->join_result_variable, WHITE);
}


bool GMState::get_icon(int& icon_image)
{
    switch(state->type)
    {
        case INITIAL:
            icon_image = ctx->icon_flow;
            break;
        case CODE:
            icon_image = ctx->icon_coding;
            break;
        case RETURN:
            icon_image = ctx->icon_uturn;
            break;
        case SPAWN:
            icon_image = ctx->icon_fork;
            break;
        case JOIN:
            icon_image = ctx->icon_join;
            break;
    }

    return true;
}


bool GMState::has_lock_notification(int rlock_id)
{
    return rlock_to_note.find(rlock_id) != rlock_to_note.end();
}


void GMState::set_resourcelock_contained(ResourceLock* rlock, bool contained)
{
    unsigned int rlock_id = rlock->id;
    bool old = has_lock_notification(rlock_id);
    
    if(old == contained)
        return;
    
    if(contained)
    {
        string name = rlock->resource->name;
        GMNotification note = GMNotification(ctx, ctx->notif_icon_lock, name);
        add_notification(note);
        rlock_to_note[rlock_id] = note.id;
    }
    
    else
    {
        int note_id = rlock_to_note[rlock_id];
        remove_notification(note_id);
        auto it = rlock_to_note.find(rlock_id);
        rlock_to_note.erase(it);
    }
}

