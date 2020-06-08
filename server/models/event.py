
class Event(object):
    
    def __init__(self, name, data=None):
        self.name = name  # string
        self.data = data  # dict

    def __str__(self):
        if self.data is None:
            return self.name
        
        return self.name + " - " + str(self.data)[:30]
