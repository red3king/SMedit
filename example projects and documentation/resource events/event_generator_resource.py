import random


class EventGenerator(PeriodicScanResource):
    # Randomly fires either event A or event B every 1.7 seconds
    
    def __init__(self, resource_def):
        super().__init__(resource_def)
        
    @property
    def duration_milliseconds(self):
        return 1700
    
    def on_tick_impl(self):       
        if random.randint(0, 1):
            self.fire_event('A')
        
        else:
            self.fire_event('B')
