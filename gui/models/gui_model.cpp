#include "gui_model.h"


GUIModel::GUIModel(DrawContext* ctx, GUIModelType type, unsigned int machine_id, unsigned int model_id)
{
    this->machine_id = machine_id; 
    this->model_id = model_id;
    this->type = type;
    this->ctx = ctx;
}


