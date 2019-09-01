#pragma once
#include "utils.h"
#include "models/project.h"


bool get_code_py_id(string file, unsigned int& id);

IOResult save_project(Project& project, string save_path);

IOResult load_project(Project& project, string load_path);
