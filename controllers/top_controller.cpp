#include "top_controller.h"


TopController::TopController(HistoryManager* history_manager)
{
    this->history_manager = history_manager;
    project_open = false;

    signals.project_open.connect(sigc::mem_fun(this, &TopController::on_project_open));
    signals.project_close.connect(sigc::mem_fun(this, &TopController::on_project_close));
    signals.pre_gui_rebuild.connect(sigc::mem_fun(this, &TopController::on_pre_gui_rebuild));
    signals.gui_rebuild.connect(sigc::mem_fun(this, &TopController::on_gui_rebuild));
}


void TopController::on_pre_gui_rebuild()
{
    reset(true);
}


void TopController::on_gui_rebuild()
{
    load_from(history_manager->current_project, true);
}


void TopController::on_project_open()
{
    project_open = true;
    load_from(history_manager->current_project, false);
}


void TopController::on_project_close()
{
    project_open = false;
    reset(false);
}


