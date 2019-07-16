#include "gui_model.h"


GUIModel::GUIModel(DrawContext* ctx, EntityType type)
{
    this->type = type;
    this->ctx = ctx;
}


unsigned int GUIModel::get_id()
{
    return get_entity()->id;
}

GUIModel::~GUIModel(){ }

