from .op_generator import *
os = OperationSpec


RESOURCE_OP_SPECS = [
    os("path", "string"),
    os("name", "string")
]


CUSTOM_STATE_CLASS_OP_SPECS = [
    os("path", "string"),
    os("name", "string"),
    os("transition_defs", "vector<CustomTransitionDef>")
]


MACHINE_OP_SPECS = [
    os("name", "string"),
    os("run_on_start", "bool")
]


STATE_OP_SPECS = [
    os("name", "string"),
    os("code", "string"),
    os("join_pid_variable", "string"),
    os("join_result_variable", "string"),
    os("return_value", "LVOV"),
    os("initial_args", "vector<ArgDef>"),
    os("launch_task_name", "LVOV"),
    os("launch_args", "vector<Arg>"),
    os("launch_result_variable", "string"),
    os("launch_synchronous", "bool"),
    os("has_return_value", "bool")
]


TRANSITION_OP_SPECS = [
    os("type", "TransitionType"),
    os("timeout", "float"),
    os("event_name", "string")
]


resource_opgen = OperationGenerator("Resource", False, RESOURCE_OP_SPECS)
custom_state_class_opgen = OperationGenerator("CustomStateClass", False, CUSTOM_STATE_CLASS_OP_SPECS)
machine_opgen = OperationGenerator("Machine", False, MACHINE_OP_SPECS)
state_opgen = OperationGenerator("State", True, STATE_OP_SPECS)
transition_opgen = OperationGenerator("Transition", True, TRANSITION_OP_SPECS)


