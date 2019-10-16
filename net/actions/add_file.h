#pragma once

#include "net/action.h"



class AddFileAction : public Action
{
    public:
        AddFileAction(string filename, string filedata, ActionCB cb);  // only text files (ascii) for now
        AddFileAction(string filename, string filedata);

        string get_command_id();
        json create_request_impl();

    private:
        void _init(string filename, string filedata);

        string encoded_data;
        string filename;
};
