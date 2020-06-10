#include "add_file.h"
#include "lib/base64/base64.h"


AddFileAction::AddFileAction(string filename, ProjectFileType filetype, string filedata, ActionCB cb) : Action(cb)
{
    _init(filename, filetype, filedata);
}


AddFileAction::AddFileAction(string filename, ProjectFileType filetype, string filedata) : Action() 
{
    _init(filename, filetype, filedata);
}


void AddFileAction::_init(string filename, ProjectFileType filetype, string filedata)
{
    this->filename = filename;
    this->filetype = filetype;
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
        { "type", (int) filetype },
        { "data", encoded_data }
    };
}
