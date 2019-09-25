#pragma once

#include <vector>

#include "lineclient.h"
#include "action.h"

using std::vector;


class Actionator
{
    public:
        Actionator(LineClient* line_client);
        
        void submit_action(Action* action);

    private:
        int action_id_ctr;
        vector<Action*> current_actions;
        LineClient* line_client;

        void on_line_received(string line);
        void _purge_timed_out();
};


