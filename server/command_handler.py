import json
import base64
import traceback

from common import InadequateUserException

RESP_SUCCESS = 1
RESP_FAILURE = 0


class CommandHandler(object):
    # Translates between received strings and command dicts

    def __init__(self, project_runner):
        project_runner.set_command_handler(self)
        self.project_runner = project_runner
        self.handlers = {
                "echo": self.echo_handler,
                "new project": self.new_project_handler,
                "add file": self.add_file_handler,
                "start project": self.start_project_handler,
                "stop project": self.stop_project_handler,
                "pause project": self.pause_project_handler,
                "unpause project": self.unpause_project_handler,
                "project hash": self.project_hash_handler
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

        print("cmd = " + str(data) + "\n")

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
            try:
                resp = self.handlers[command_id](client, message_id, command_json)
            except InadequateUserException as ex:
                resp = self.fail_response(message_id, "you messed up - " + repr(ex))

            except Exception as ex:
                resp = self.fail_response(message_id, "UNHANDLED EXCPETION: " + traceback.format_exc())

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

        return jdict

    def fail_response(self, message_id, reason=""):
        jdict = {
            "mid": message_id,
            "status": RESP_FAILURE,
            "err": reason
        }
        return jdict

    def unknown_handler(self, client, message_id):
        return self.fail_response(message_id, "unknown command")
    
    def echo_handler(self, client, message_id, command_json):
        return self.success_response(message_id)

    def new_project_handler(self, client, message_id, command_json):
        self.project_runner.new_project()
        return self.success_response(message_id)

    def add_file_handler(self, client, message_id, command_json):
        """
        expects command_json = {
            "name": "filename.py",
            "data": "023920b009f039"   #base64
        }
        """
        
        try:
            filename = command_json["name"]
            data = command_json["data"]
        
        except KeyError:
            raise InadequateUserException("missing field!")

        self.project_runner.project_add_file(filename, data)
        return self.success_response(message_id)

    def start_project_handler(self, client, message_id, command_json):
        self.project_runner.start_project()
        return self.success_response(message_id)

    def stop_project_handler(self, client, message_id, command_json):
        self.project_runner.stop_project()
        return self.success_response(message_id)

    def pause_project_handler(self, client, message_id, command_json):
        self.project_runner.pause_project()
        return self.success_response(message_id)

    def unpause_project_handler(self, client, message_id, command_json):
        self.project_runner.unpause_project()
        return self.success_response(message_id)
    
    def project_hash_handler(self, client, message_id, command_json):
        hash_val = self.project_runner.hash_project()
        return self.success_response(message_id, { "hash": hash_val } )

    

