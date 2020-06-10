#include <iostream>
#include <fstream>
#include <experimental/filesystem>
#include <boost/crc.hpp>

#include "saveload.h"
#include "log.h"


using std::string;
using namespace std::experimental;


string save_code(ProjectInfo& project_info, State* state, string save_path)
{
    if(state->code == "")
        return "";

    string filename = state->name + "_" + std::to_string(state->id) + ".py";
    string save_name = save_path + "/" + filename;
    project_info.add_file(filename, PFT_CODE_STATE, state->code);
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


IOResult save_project(Project& project, ProjectInfo& project_info, string save_path)
{
    project_info.reset();
    try 
    {
        // save project
        json jdata = project.to_json();
        string jstr = jdata.dump(4);
        project_info.add_file("project.json", PFT_PROJFILE, jstr);
        string json_filename = save_path + "/project.json";
        std::ofstream jfile(json_filename);
        jfile << jstr;
        jfile.close();

        // save code state python files
        vector<string> saved_code_files;
        
        for(int i = 0; i < project.machines.size(); i++)
            for(int j = 0; j < project.machines[i]->states.size(); j++)
            {
                State* state = project.machines[i]->states[j];
                
                if(state->type != CODE)
                    continue;

                string save_file = save_code(project_info, project.machines[i]->states[j], save_path);
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

        for(int i = 0; i < to_delete.size(); i++)
            filesystem::remove(to_delete[i]);

    }

    catch (const std::exception &exc)
    {
        string what(exc.what());
        return IOResult(false, "Damn, " + what);
    }

    project_info.saved_project = project;
    project_info.project_directory = save_path;
    return IOResult(true);
}


IOResult load_project(Project& project, ProjectInfo& project_info, string load_path)
{
    project_info.reset();


    // Load Project.json file
    
    string json_filename = load_path + "/project.json";
    std::ifstream jfile(json_filename);
    
    if(!jfile.is_open())
        return IOResult(false, "could not open " + json_filename);
    
    string file_str;

    try
    {
        file_str = string(std::istreambuf_iterator<char>(jfile),
                        std::istreambuf_iterator<char>());
        project_info.add_file("project.json", PFT_PROJFILE, file_str);
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
 
    project = Project(jdata);   // TODO maybe reconsider the frequent copying (assignment, copy ctor, etc)
                                // of Project objects, happens 2 unnecessary times on project load now
                                // This could get slow when projects get big.
    project_info.saved_project = project;

    
    // Load code state py files, and include py files:
    
    unsigned int state_id;
    for(const auto& entry : filesystem::directory_iterator(load_path))
    {
        auto path = entry.path();
        string filepath = (string) path;
        string filename = path.filename();

        if(filepath.find(".py") == string::npos)
            continue;

        std::ifstream codefile(filepath);
        
        if(!codefile.is_open())
            return IOResult(false, "could not read " + filepath);

        string code((std::istreambuf_iterator<char>(codefile)), std::istreambuf_iterator<char>());
        
        // Code state file
        if(get_code_py_id(filepath, state_id))
        {
            State* state = dynamic_cast<State*>(project.get_entity_by_id(state_id));

            if(state == nullptr)
                continue;

            project_info.add_file(filename, PFT_CODE_STATE, code);
            state->code = code;
        }
        
        // Must be include file
        else 
            project_info.add_file(filename, PFT_INCLUDE, code);        
    }   
  
    // Code states with nothing saved should get the default function stub 
    for(int i = 0; i < project.machines.size(); i++)
    {
        auto mach = project.machines[i];
        for(int j = 0; j < mach->states.size(); j++)
        {
            auto state = mach->states[j];
            if(state->code == "")
                state->code = DEFAULT_STATE_CODE;
        }
    }

    project_info.project_directory = load_path;
    return IOResult(true);
}


// ProjectInfo

ProjectInfo::ProjectInfo()
{
    reset();
}


bool ProjectInfo::has_project()
{
    return hash != 0;
}


void ProjectInfo::reset()
{
    hash_calculated = false;
    hash = 0;
    filename_to_data.clear();
    filename_to_type.clear();
    saved_project = Project();
    project_directory = "";
}


void ProjectInfo::add_file(string filename, ProjectFileType filetype, string filedata)
{
    filename_to_data[filename] = filedata;
    filename_to_type[filename] = filetype;
    hash_calculated = false;
}


uint16_t ProjectInfo::get_hash()
{
    if(hash_calculated)
        return hash;
    
    hash = 0;

    for(int i = 0; i < get_num_files(); i++)
    {
        boost::crc_32_type crc_comp;
        string data = get_filedata(i);
        crc_comp.process_bytes(data.c_str(), data.length());
        hash ^= crc_comp.checksum();
    }

    hash_calculated = true;
    return hash;
}


int ProjectInfo::get_num_files()
{
    int num_resources = saved_project.resources.size();
    int num_csc = saved_project.custom_state_classes.size();
    return filename_to_data.size() + num_resources + num_csc;
}


ProjectFileType ProjectInfo::get_filetype(int i)
{
    ProjectFileType file_type;
    get_file_index(file_type, i);
    return file_type;
}


int ProjectInfo::get_file_index(ProjectFileType& file_type, int i)
{
    int l = filename_to_data.size();
    
    if(i < l)
    {
        auto it = filename_to_type.begin(); 
        int ii = i;
        
        while(ii)
        { 
            it++;
            ii--;
        }
        
        file_type = it->second;
        return i;
    }
    
    int num_resources = saved_project.resources.size();
        
    if(i - l < num_resources)
    {
        file_type = PFT_RESOURCE;
        return i - l;
    }
    
    int num_custom_stateclasses = saved_project.custom_state_classes.size();
    
    if(i - l - num_resources < num_custom_stateclasses)
    {
        file_type = PFT_CUSTOM_STATE_CLASS;
        return i - l - num_resources;
    }
}


string ProjectInfo::get_filename(int i)
{
    ProjectFileType file_type;
    i = get_file_index(file_type, i);
    
    if(file_type == PFT_CODE_STATE || file_type == PFT_INCLUDE || file_type == PFT_PROJFILE)
    {
        auto it = filename_to_data.begin(); 
        
        while(i)
        { 
            it++;
            i--;
        }
        
        return it->first;
    }
    
    else if(file_type == PFT_RESOURCE)
        return saved_project.resources[i]->name;            
    
    else if(file_type == PFT_CUSTOM_STATE_CLASS)
        return saved_project.custom_state_classes[i]->get_py_filename();
}


string ProjectInfo::get_filedata(int i)
{
    ProjectFileType file_type;
    i = get_file_index(file_type, i);
    
    if(file_type == PFT_CODE_STATE || file_type == PFT_INCLUDE || file_type == PFT_PROJFILE)
    {
        auto it = filename_to_data.begin();
        while(i--) it++;
        return it->second;
    }
    
    else if(file_type == PFT_RESOURCE)
    {
        Resource* resource = saved_project.resources[i];
        string result;
        string absolute_path = make_absolute_path(project_directory, resource->path);
        file_to_string(absolute_path, result);
        return result;        
    }
    
    // PFT_CUSTOM_STATE_CLASS
    string result;
    auto csc = saved_project.custom_state_classes[i];
    string absolute_path = make_absolute_path(project_directory, csc->path);
    file_to_string(absolute_path, result);
    return result;   
}


bool ProjectInfo::any_missing_files(string& missing_filenames)
{
    bool any_missing = false;
    missing_filenames = "";

    for(int i = 0; i < saved_project.resources.size(); i++)
    {
        Resource* resource = saved_project.resources[i];
        string absolute_path = make_absolute_path(project_directory, resource->path);
        
        if(!std::experimental::filesystem::exists(absolute_path))
        {
            any_missing = true;
            missing_filenames += "\n" + resource->name + " : " + absolute_path;
        }
    }

    for(int i = 0; i < saved_project.custom_state_classes.size(); i++)
    {
        CustomStateClass* csc = saved_project.custom_state_classes[i];
        string absolute_path = make_absolute_path(project_directory, csc->path);
        
        if(!std::experimental::filesystem::exists(absolute_path))
        {
            any_missing = true;
            missing_filenames += "\n" + csc->name + " : " + absolute_path;
        }
    }    
    
    return any_missing;
}

