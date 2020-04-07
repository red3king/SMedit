#pragma once

#include <map>

#include "gui_box_model.h"

using std::map;


class GMState : public GMBox
{
    public:
        GMState(DrawContext* ctx, State* state);

        void draw_interior(float x, float y, float w, float h);

        Entity* get_entity();

        NVGcolor get_border_color();
        NVGcolor get_title_text_color();
        std::string get_title();
        bool get_icon(int& icon_image);

        State* state;
        
        void set_resourcelock_contained(ResourceLock* rlock, bool contained);

        bool is_custom();
        int get_state_def_id();
        void set_is_current(bool is_current);   // only used when the gui is in RUN mode

    private:
        bool is_current_state;
        map<int, int> rlock_to_note;  // maps resource lock ids to lock notification ids
    
        bool has_lock_notification(int rlock_id);
        
        void draw_interior_initial(float x, float y, float w, float h);
        void draw_interior_code(float x, float y, float w, float h);
        void draw_interior_return(float x, float y, float w, float h);
        void draw_interior_spawn(float x, float y, float w, float h);
        void draw_interior_join(float x, float y, float w, float h);
}; 
