#pragma once

#include "historymanager/historymanager.h"


class StateTypeController
{
    public:
        StateTypeController(HistoryManager* history_manager);
        void set_state(Machine* owning_machine, State* selected_state);

        virtual ~StateTypeController()=0;

    protected:
        virtual void set_state_impl(Machine* owning_machine, State* selected_state)=0;
        bool has_state();
        
        State* selected_state;
        Machine* owning_machine;
        HistoryManager* history_manager;
        

    private:

};
