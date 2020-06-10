#pragma once

#include <map>

#include "utils.h"
#include "models/project.h"


using std::map;


enum ProjectFileType { PFT_CODE_STATE, PFT_RESOURCE, PFT_CUSTOM_STATE_CLASS, PFT_INCLUDE, PFT_NONE, PFT_PROJFILE };


class ProjectInfo
{
    // TODO - refactor? this is a mess
    public:
        ProjectInfo();

        Project saved_project;
        string project_directory;

        bool has_project();
        void reset();
        void add_file(string filename, ProjectFileType filetype, string filedata);
        uint16_t get_hash();

        // ProjectInfo contains file data from loaded code files, include files,
        // data from external resource python files, and custom state class defs

        int get_num_files();
        string get_filename(int i);
        string get_filedata(int i);
        ProjectFileType get_filetype(int i);
        bool any_missing_files(string& missing_filenames);  // returns true if file referred to by Resource is missing

    private:
        bool hash_calculated;
        uint16_t hash;
        
        map<string, string> filename_to_data;
        map<string, ProjectFileType> filename_to_type;
        
        int get_file_index(ProjectFileType& file_type, int i);
};


bool get_code_py_id(string file, unsigned int& id);

IOResult save_project(Project& project, ProjectInfo& project_info, string save_path);

IOResult load_project(Project& project, ProjectInfo& project_info, string load_path);




