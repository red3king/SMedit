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
        void cancel_action(Action* action);
        void cancel_all();

    private:
        int action_id_ctr;
        vector<Action*> current_actions;
        LineClient* line_client;

        void on_line_received(string line);
        void on_disconnect(bool intentional);

        void _purge_timed_out();
};


