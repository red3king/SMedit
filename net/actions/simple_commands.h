#include "net/action.h"

// MACHINE GENERATED CODE BELOW, DO NOT EDIT
// USE COG TO REGENERATE


/*[[[cog

import os
import sys
sys.path.append(os.getcwd())
from codegen.action_specs import *

for line in action_generator.make_header():
    cog.outl(line)

]]]*/

            
class NewProjectAction : public Action
{
    public:
        NewProjectAction(ActionCB callback);
        NewProjectAction();
        string get_command_id();
};

            
class StartProjectAction : public Action
{
    public:
        StartProjectAction(ActionCB callback);
        StartProjectAction();
        string get_command_id();
};

            
class StopProjectAction : public Action
{
    public:
        StopProjectAction(ActionCB callback);
        StopProjectAction();
        string get_command_id();
};

            
class PauseProjectAction : public Action
{
    public:
        PauseProjectAction(ActionCB callback);
        PauseProjectAction();
        string get_command_id();
};

            
class UnpauseProjectAction : public Action
{
    public:
        UnpauseProjectAction(ActionCB callback);
        UnpauseProjectAction();
        string get_command_id();
};



//[[[end]]]

