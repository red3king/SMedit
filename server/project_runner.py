

class ProjectRunner(object):

    def __init__(self):
        self.command_handler = None
        self.connected_clients = []

    def set_command_handler(self, command_handler):
        self.command_handler = command_handler

    def add_client(self, client):
        self.connected_clients.append(client)

    def remove_client(self, client):
        self.connected_clients.remove(client)


