from common import InadequateUserException
import crc16
import binascii


class Project(object):

    def __init__(self):
        self.files = {}  # maps filename to string file data
        self.built = False

    @property
    def loaded(self):
        return self.built

    def add_file(self, name, data):
        if self.built:
            raise InadequateUserException("project was already loaded, cannot add file.")

        self.files[name] = data

    def calculate_hash(self):
        hash_val = 0
    
        for data in self.files.values():
            data_bytes = data.encode("ascii")
            hash_val ^= binascii.crc32(data_bytes)
        
        return hash_val

    
        



