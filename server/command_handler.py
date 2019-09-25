import json
import base64

RESP_SUCCESS = 1
RESP_FAILURE = 0


class CommandHandler(object):
    # Translates between received strings and command dicts

    def __init__(self, project_runner):
        project_runner.set_command_handler(self)
        self.project_runner = project_runner
        self.handlers = {
                "echo": self.echo_handler
                }

    def add_client(self, client):
        self.project_runner.add_client(client)

    def remove_client(self, client):
        self.project_runner.remove_client(client)


    def handle_command(self, client, command_str):
        if len(command_str) < 1:
            print("handle_command line is too short (0)!")
            return

        try:
            data = base64.b64decode(command_str)
        except Exception as exp:
            print("handle_command b64 decode fail: " + str(command_str) + ":\n")
            print('\t' + str(exp) + '\n')
            return

        print("cmd = " + str(command_str) + "\n")

        try:
            jdata = json.loads(data)
        except Exception as exp:
            print("handle_command json load fail: " + str(data) + ":\n")
            print(str(exp) + "\n")
            return

        try:
            command_id = jdata["cid"]
            message_id  = jdata["mid"]
            command_json = jdata["cdata"]
        except KeyError:
            print("handle_command json missing field")
            return

        resp = None

        if command_id not in self.handlers:
            resp = self.unknown_handler(client, message_id)

        else:
            resp = self.handlers[command_id](client, message_id, command_json)

        if resp is not None:
            print("resp = " + str(resp) + "\n")
            line = self.resp_to_line(resp)
            client.send_response(line)


    def resp_to_line(self, response_dict):
        str_data = json.dumps(response_dict)
        return base64.b64encode(str_data.encode('ascii'))

    def success_response(self, message_id, response_data=None):
        jdict = {
            "mid": message_id,
            "resp": {} if response_data is None else response_data,
            "status": RESP_SUCCESS
        }

        return jdict #return self.resp_to_line(jdict)

    def fail_response(self, message_id, reason=""):
        jdict = {
            "mid": message_id,
            "status": RESP_FAIL,
            "err": reason
        }
        return jdict # self.resp_to_line(jdict)

    def unknown_handler(self, client, message_id):
        return self.fail_response(message_id, "unknown command")
    
    def echo_handler(self, client, message_id, command_json):
        return self.success_response(message_id)




