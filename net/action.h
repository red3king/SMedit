#pragma once

#include <ctime>
#include <string>
#include "boost/function.hpp"

#include "lib/json.hpp"

enum ActionStatus { AS_PENDING, AS_SUCCESS, AS_FAIL, AS_CANCELLED, AS_TIMEOUT };

class Action;
typedef boost::function<void(Action*)> ActionCB;

using std::string;
using nlohmann::json;


class Action
{
    public:
        Action(ActionCB callback);
        Action();
        virtual ~Action() = 0;
        
        ActionStatus status;
        string error_message;
        int id;
        
        virtual string get_command_id() = 0;
        virtual json create_request_impl();
        virtual ActionStatus handle_success_response_impl(json response);
        virtual float get_timeout_seconds();

        json create_request();
        ActionStatus handle_response(int status, json response, string error);
        void assign_id(int id);

        bool has_timed_out();
        void do_timeout();


        // action chaining stuff
        void add_next_action(Action* next_action);  // add action to be run if next if this one succeeds
        bool has_next_action();
        Action* get_next_action();
        void fail_chain();

    private:
        void _init();
        std::time_t send_time;
        ActionCB callback;
        Action *next_action;
};
