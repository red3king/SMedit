#include "gui_state.h"


GMState::GMState(DrawContext* ctx, unsigned int machine_id, unsigned int model_id) : GUIBoxModel(ctx, STATE, machine_id, model_id) { }


void GMState::get_coords(Project p, float&x, float&y, float&w, float&h)
{
    // TODO - THIS IS JUST DUMB
    for(int i=0; i<p.machines.size(); i++)
    {
        if(p.machines[i].id == machine_id)
        {
            for(int j=0; j<p.machines[i].states.size(); j++)
            {   
                if(p.machines[i].states[j].id == model_id)
                {
                    x = p.machines[i].states[j].x;
                    y = p.machines[i].states[j].y;
                    w = p.machines[i].states[j].w;
                    h = p.machines[i].states[j].h;
                }
            }
        }
    }
}
