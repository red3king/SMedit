#pragma once

#include <string>
#include <vector>
#include <sigc++/sigc++.h>

#include "utils.h"
#include "operation.h"
#include "models/project.h"


class HistoryManager
{
    public:
        HistoryManager(int max_operations, int op_ex_thresh, int min_undos);
        
        sigc::signal<void, bool, bool> signal_changed;  // may undo, may redo

        unsigned int submit_operation(Operation& operation);
        void undo();
        void redo();
        
        bool may_undo();
        bool may_redo();

        void new_project();
        IOResult load_from(string load_path);
        IOResult save_to(string save_path);
        Project current_project;

    private:
        int max_operations, op_ex_thresh, min_undos;

        bool last_may_undo, last_may_redo;
        bool project_created;
        Project initial_project;        
        vector<Operation*> operations;    
        int undo_position;

        void reset();
        void condense_if_needed();
        void condense_history();

        void _update_last();
};


