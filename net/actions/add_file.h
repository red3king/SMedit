#pragma once

#include "net/action.h"
#include "saveload.h"



class AddFileAction : public Action
{
    public:
        AddFileAction(string filename, ProjectFileType filetype, string filedata, ActionCB cb);  // only text files (ascii) for now
        AddFileAction(string filename, ProjectFileType filetype, string filedata);

        string get_command_id();
        json create_request_impl();

    private:
        void _init(string filename, ProjectFileType filetype, string filedata);

        string encoded_data;
        string filename;
        ProjectFileType filetype;
};
