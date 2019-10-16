#pragma once

#include <map>

#include "utils.h"
#include "models/project.h"


using std::map;


class ProjectInfo
{
    public:
        ProjectInfo();

        Project saved_project;

        bool has_project();
        void reset();
        void add_file(string filename, string filedata);
        uint16_t get_hash();

        int get_num_files();
        string get_filename(int i);
        string get_filedata(int i);

    private:
        bool hash_calculated;
        uint16_t hash;
        map<string, string> filename_to_data;
};


bool get_code_py_id(string file, unsigned int& id);

IOResult save_project(Project& project, ProjectInfo& project_info, string save_path);

IOResult load_project(Project& project, ProjectInfo& project_info, string load_path);




