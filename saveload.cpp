#include <iostream>
#include <fstream>
#include <experimental/filesystem>
#include <boost/crc.hpp>

#include "saveload.h"

using std::string;
using namespace std::experimental;


string save_code(State* state, string save_path)
{
    if(state->code == "")
        return "";

    string filename = state->name + "_" + std::to_string(state->id) + ".py";
    string save_name = save_path + "/" + filename;
    std::ofstream cfile(save_name);
    cfile << state->code;
    cfile.close();
    return save_name;
}


bool get_code_py_id(string file, unsigned int& id)
{
    // return true if python code file with id
    // "mystate_44.py" -> true, 44

    int len = file.length();
    
    if(len < 5)
        return false;

    string end = file.substr(len-3, 3);
    if(end != ".py")
        return false;

    int pos = file.rfind('_');
    if(pos == string::npos)
        return false;

    string number = file.substr(pos+1, len-pos-4);
    if(!is_number(number, false))
        return false;

    int val;
    bool result = string_to_int(val, number);
    id = val;
    return result;
}


IOResult save_project(Project& project, string save_path, uint16_t& crc_out)
{
    
    boost::crc_ccitt_type crc_thingy;

    try 
    {
        // save project
        json jdata = project.to_json();
        string jstr = jdata.dump(4);
        crc_thingy.process_bytes(jstr.c_str(), jstr.length());
        string json_filename = save_path + "/project.json";
        std::ofstream jfile(json_filename);
        jfile << jstr;
        jfile.close();

        // save code state python files
        vector<string> saved_code_files;
        for(int i=0; i<project.machines.size(); i++)
            for(int j=0; j<project.machines[i]->states.size(); j++)
            {
                State* state = project.machines[i]->states[j];
                
                if(state->type != CODE)
                    continue;

                string save_file = save_code(project.machines[i]->states[j], save_path);
                crc_thingy.process_bytes(save_file.c_str(), save_file.length());
                saved_code_files.push_back(save_file);
            }

        // delete unneeded python files from renamed or deleted code states
        vector<filesystem::path> to_delete;
        unsigned int found_id;
        for (const auto& entry : filesystem::directory_iterator(save_path))
        {
            auto path = entry.path();
            string file = (string)path;
            if(!get_code_py_id(file, found_id))
                continue;

            bool keep = std::count(saved_code_files.begin(), saved_code_files.end(), file);
            
            if(!keep)
                to_delete.push_back(path);
        }

        for(int i=0; i<to_delete.size(); i++)
            filesystem::remove(to_delete[i]);

    }

    catch (const std::exception &exc)
    {
        string what(exc.what());
        return IOResult(false, "Damn, " + what);
    }

    crc_out = crc_thingy.checksum();
    return IOResult(true);
}


IOResult load_project(Project& project, string load_path, uint16_t& crc_out)
{
    boost::crc_ccitt_type crc_thingy;
    string json_filename = load_path + "/project.json";
    std::ifstream jfile(json_filename);
    
    if(!jfile.is_open())
        return IOResult(false, "could not open " + json_filename);
    
    string file_str;

    try
    {
        file_str = string(std::istreambuf_iterator<char>(jfile),
                        std::istreambuf_iterator<char>());
        crc_thingy.process_bytes(file_str.c_str(), file_str.length());
    }
    
    catch(...)
    {
        return IOResult(false, "could not read " + json_filename);
    }

    json jdata;
    
    try
    {
        jdata = json::parse(file_str);
    }
    
    catch(...)
    {
        return IOResult(false, "could not parse " + json_filename);        
    }
 
    project = Project(jdata);

    unsigned int state_id;
    for(const auto& entry : filesystem::directory_iterator(load_path))
    {
        auto path = entry.path();
        string file = (string) path;

        if(!get_code_py_id(file, state_id))
            continue;
        
        State* state = dynamic_cast<State*>(project.get_entity_by_id(state_id));

        if(state == nullptr)
            continue;

        std::ifstream codefile(file);
        
        if(!codefile.is_open())
            return IOResult(false, "could not read " + file);

        string code((std::istreambuf_iterator<char>(codefile)), std::istreambuf_iterator<char>());
        crc_thingy.process_bytes(code.c_str(), code.length());
        state->code = code;
    }   
    
    crc_out = crc_thingy.checksum();
    return IOResult(true);
}
