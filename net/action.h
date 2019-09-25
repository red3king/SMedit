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
        virtual ~Action() = 0;
        
        ActionStatus status;
        string error_message;
        int id;
        
        virtual string get_command_id() = 0;
        virtual json create_request_impl() = 0;
        virtual ActionStatus handle_success_response_impl(json response) = 0;
        virtual float get_timeout_seconds();

        json create_request();
        void handle_response(int status, json response, string error);
        void assign_id(int id);
        bool has_timed_out();

        void do_timeout();

    private:
        std::time_t send_time;
        ActionCB callback;

};
