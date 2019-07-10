#include "gui_state.h"


GUIModel* GUIState::get_model_by_id(unsigned int id)
{
    for(int i=0; i<gui_models.size(); i++)
    {
        GUIModel* model = gui_models[i];
        if((*model).model_id == id)
            return model;
    }

    return nullptr;
}



