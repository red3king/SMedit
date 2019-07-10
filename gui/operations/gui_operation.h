#pragma once

#include "models/project.h"
#include "gui/gui_state.h"
#include "gui/current_events.h"


enum GUIOpResult { END, CANCEL, CONTINUE };


class GUIOperation
{
    public:
        // the update function (s), that modifies the GUIState / GUIModels,
        // should be given a ref to the project and the guistate,
        // and possibly return an Opertion instance so it may be submitted 
        // into the historymanager.

        virtual ~GUIOperation()=0;
        
        // each subclass should define:  static bool maybe_create(Project& current_project, GUIState& gui_state, CurrentEvents& current_events, GUIOperation*& pref);
        // and throw that into the factory
        

        virtual GUIOpResult should_continue(Project& current_project, GUIState& gui_state, CurrentEvents& current_events);


        virtual void on_continue(Project& current_project, GUIState& gui_state);

        virtual void on_cancel(Project& current_project, GUIState& gui_state);

        virtual void on_end(Project& current_project, GUIState& gui_state);
};


