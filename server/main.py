#!./venv/bin/python
from twisted.protocols.basic import LineReceiver
from twisted.internet.protocol import Factory
from twisted.internet import reactor, protocol
from twisted.internet.endpoints import TCP4ServerEndpoint


from command_handler import CommandHandler
from project_runner import ProjectRunner
from utils import clear



# https://medium.com/@jtpaasch/the-right-way-to-use-virtual-environments-1bc255a0cba7


class ConnectedClient(object):
    def __init__(self, twisted_proto):
        self.twisted_proto = twisted_proto

    def send_response(self, response_line):
        print("raw send: " + str(response_line))
        self.twisted_proto.sendLine(response_line)


class LJProtocol(LineReceiver):
    # "line json" protocol

    def __init__(self, command_handler):
        self.command_handler = command_handler
        self.client = None

    def connectionMade(self):
        self.client = ConnectedClient(self)
        self.command_handler.add_client(self.client)

    def connectionLost(self, reason):
        self.command_handler.remove_client(self.client)

    def lineReceived(self, line):
        self.command_handler.handle_command(self.client, line)       


class LJFactory(Factory):
    def __init__(self, command_handler):
        self.command_handler = command_handler

    def buildProtocol(self, addr):
        return LJProtocol(self.command_handler)


def main():
    clear()
    print("\n    ====== smsrv v0.0.1 ======\n")
    print("loading...")

    project_runner = ProjectRunner()
    command_handler = CommandHandler(project_runner)
    factory = LJFactory(command_handler)

    print("running reactor\n")
    
    endpoint = TCP4ServerEndpoint(reactor, 24203)
    endpoint.listen(factory)
    reactor.run()

if __name__ == '__main__':
    main()
