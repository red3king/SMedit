#include "simple_commands.h"

// MACHINE GENERATED CODE BELOW, DO NOT EDIT
// USE COG TO REGENERATE


/*[[[cog

import os
import sys
sys.path.append(os.getcwd())
from codegen.action_specs import *

for line in action_generator.make_cpp():
    cog.outl(line)

]]]*/


NewProjectAction::NewProjectAction(ActionCB callback) : Action(callback) { }

NewProjectAction::NewProjectAction() : Action() { }

string NewProjectAction::get_command_id() { return "new project"; }



StartProjectAction::StartProjectAction(ActionCB callback) : Action(callback) { }

StartProjectAction::StartProjectAction() : Action() { }

string StartProjectAction::get_command_id() { return "start project"; }



StopProjectAction::StopProjectAction(ActionCB callback) : Action(callback) { }

StopProjectAction::StopProjectAction() : Action() { }

string StopProjectAction::get_command_id() { return "stop project"; }



PauseProjectAction::PauseProjectAction(ActionCB callback) : Action(callback) { }

PauseProjectAction::PauseProjectAction() : Action() { }

string PauseProjectAction::get_command_id() { return "pause project"; }



UnpauseProjectAction::UnpauseProjectAction(ActionCB callback) : Action(callback) { }

UnpauseProjectAction::UnpauseProjectAction() : Action() { }

string UnpauseProjectAction::get_command_id() { return "unpause project"; }




//[[[end]]]

