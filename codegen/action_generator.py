
class ActionGenerator(object):
    def __init__(self, action_spec_list):
        self.action_spec_list = action_spec_list


    def make_header(self):
        result = ""

        for spec_tuple in self.action_spec_list:
            class_name = spec_tuple[0]
            command_id = spec_tuple[1]

            result += f'''
            
class {class_name} : public Action
{{
    public:
        {class_name}(ActionCB callback);
        {class_name}();
        string get_command_id();
}};
'''
        result += "\n\n"
        return result.split('\n')


    def make_cpp(self):
        result =  ""

        for spec_tuple in self.action_spec_list:
            class_name = spec_tuple[0]
            command_id = spec_tuple[1]

            result += f'''

{class_name}::{class_name}(ActionCB callback) : Action(callback) {{ }}

{class_name}::{class_name}() : Action() {{ }}

string {class_name}::get_command_id() {{ return "{command_id}"; }}

'''
        result += "\n\n"
        return result.split('\n')


