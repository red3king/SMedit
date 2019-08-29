#include "historymanager.h"
#include "signals.h"


HistoryManager::HistoryManager(int max_operations, int op_ex_thresh, int min_undos)
{
    this->max_operations = max_operations;
    this->op_ex_thresh = op_ex_thresh;
    this->min_undos = min_undos;

    project_created = false;
    undo_position = 0;
    last_may_undo = false;
    last_may_redo = false;
}


unsigned int HistoryManager::submit_operation(Operation& operation)
{
    unsigned int result = operation.execute(current_project);

    if(undo_position > 0)
    {        
        operations.erase(operations.end() - undo_position, operations.end());
        undo_position = 0;
    }

    if(operations.size() > 0 && operations.back()->may_collapse(operation))
    {
        operations.back()->collapse(operation);
        return result;
    }

    operations.push_back(operation.clone());
    condense_if_needed();

    _update_last();
    return result;
}


void HistoryManager::undo()
{
    signals.fire_pre_gui_rebuild_signal();
    signals.disable_gui_signals();

    current_project = initial_project; 

    for(int i=0; i < operations.size() - undo_position - 1; i++)
        operations[i]->execute(current_project);

    undo_position++;
    
    signals.enable_gui_signals();
    signals.fire_gui_rebuild_signal();
    _update_last();
}


void HistoryManager::redo()
{
    int i = operations.size() - undo_position;
    operations[i]->execute(current_project);
    undo_position--;
    _update_last();
}


bool HistoryManager::may_undo()
{
    return operations.size() > 0 && undo_position < operations.size();// - 1;
}


bool HistoryManager::may_redo()
{
    return undo_position > 0;
}


void HistoryManager::condense_if_needed()
{
    // condense history if it's too long, and if doing so will leave user
    // with at least min_undos undos available
    if(operations.size() > max_operations + op_ex_thresh &&
            operations.size() - undo_position - 1 > op_ex_thresh + min_undos)
        condense_history();
}


void HistoryManager::condense_history()
{
    signals.disable_gui_signals();

    for(int i=0; i<op_ex_thresh; i++)
    {
        operations[i]->execute(initial_project);
        delete operations[i];
    }

    operations.erase(operations.begin(), operations.begin() + op_ex_thresh);
    signals.enable_gui_signals();
}


void HistoryManager::_update_last()
{
    bool can_undo = may_undo();
    bool can_redo = may_redo();

    if(last_may_undo != can_undo || last_may_redo != can_redo)
        signal_changed.emit(can_undo, can_redo);
    
    last_may_undo = can_undo;
    last_may_redo = can_redo;
}


void HistoryManager::reset()
{
    current_project = Project();
    initial_project = Project();
    operations.clear();
    project_created = false;
    last_may_undo = false;
    last_may_redo = false;
}


void HistoryManager::new_project()
{
    if(project_created)
        reset();

    initial_project = Project();
    current_project = Project();
    project_created = true;
    last_may_undo = false;
    last_may_redo = false;
}


IOResult HistoryManager::load_from(string load_path)
{
    if(project_created)
        reset();

    //if(something == bad)
    //    return IOResult(false, "File not found");

    //TODO
    project_created = true;


    return IOResult(true, "");
}


IOResult HistoryManager::save_to(string save_path)
{
    //TODO
    return IOResult(true, "");
}
