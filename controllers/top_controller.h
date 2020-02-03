#pragma once
#include "signals.h"
#include "historymanager/historymanager.h"


class TopController
{
    public:
        TopController(HistoryManager *history_manager);

        virtual void reset(bool reload)=0;  // zero out pointers, release resources
        virtual void load_from(Project& current_project, bool reload)=0; // load a project

    protected:
        HistoryManager *history_manager;
        bool project_open;

    private:
        void on_pre_gui_rebuild();
        void on_gui_rebuild();
        void on_project_open();
        void on_project_close();
};
