from .action_generator import ActionGenerator

# defines actions which just send a command and get a success/fail response with no 
# arguments or parsing a complicated response


ACTION_SPECS = [
    #(class name, command id)
        ("NewProjectAction", "new project"),
        ("LoadProjectAction", "load project"),
        ("StartProjectAction", "start project"),
        ("StopProjectAction", "stop project"),
        ("PauseProjectAction", "pause project"),
        ("UnpauseProjectAction", "unpause project")
]


action_generator = ActionGenerator(ACTION_SPECS)

