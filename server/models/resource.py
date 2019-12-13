
class Resource(object):

    def __init__(self, resource_def):
        self.resource_def = resource

    def pause(self):
        # temporarily stop doing stuff
        pass

    def unpause(self):
        # stop temporarily stop doing stuff
        pass

    def start(self):
        # begin doing stuff
        pass

    def stop(self):
        # stop doing stuff, clear state back to initial
        # should be able to be started again as if new
        pass
