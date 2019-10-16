#include "add_file.h"
#include "lib/base64/base64.h"


AddFileAction::AddFileAction(string filename, string filedata, ActionCB cb) : Action(cb)
{
    _init(filename, filedata);
}


AddFileAction::AddFileAction(string filename, string filedata) : Action() 
{
    _init(filename, filedata);
}


void AddFileAction::_init(string filename, string filedata)
{
    this->filename = filename;
    encoded_data = base64_encode((unsigned char*)filedata.c_str(), filedata.length());
}


string AddFileAction::get_command_id()
{
    return "add file";
}


json AddFileAction::create_request_impl()
{
    return { 
        { "name", filename },
        { "data", encoded_data }
    };
}




