
class NothingResource(Resource):

    def __init__(self, resource_def):
        super().__init__(resource_def)

    def pause_impl(self):
        pass

    def unpause_impl(self):
        pass

    def start_impl(self):
        pass

    def stop_impl(self):
        pass

