import base64

from common import InadequateUserException
from custom_import import set_project_modules, clear_project_modules
from models.project import Project


class ProjectRunner(object):

    def __init__(self):
        self.command_handler = None
        self._started = False
        self._paused = False
        self.new_project()

    def on_broadcast(self, broadcast):
        broadcast_dict = broadcast.to_dict()
        self.command_handler.send_broadcast(broadcast_dict)

    @property
    def project_started(self):
        return self._started

    @property
    def project_running(self):
        return self.project_started and not self.project_paused

    @property
    def project_paused(self):
        return self._paused

    @property
    def project_loaded(self):
        return self.project.loaded

    def set_command_handler(self, command_handler):
        self.command_handler = command_handler
   
    # commands

    def new_project(self):
        if self.project_running:
            raise InadequateUserException("Cannot create a project when one is running.")

        if hasattr(self, "project"):
            self.project.broadcast_signal -= self.on_broadcast

        clear_project_modules()
        self.project = Project()
        self.project.broadcast_signal += self.on_broadcast
        set_project_modules(self.project.import_modules)
        
    def project_add_file(self, file_name, file_type, filedata_b64):
        try:
            file_data = base64.b64decode(filedata_b64)
            file_data = file_data.decode('ascii')
        except:
            raise InadequateUserException("ProjectRunner.project_add_file: file data not base64!")

        self.project.add_file(file_name, file_type, file_data)

    def start_project(self):
        if not self.project_loaded:
            raise InadequateUserException("cannot start project: project not loaded")
        
        if self.project_running:
            raise InadequateUserException("cannot start project: project is running")
        
        self.project.start()
        self._started = True

    def stop_project(self):
        if not self.project_loaded:
            raise InadequateUserException("cannot stop project: project not loaded")

        if not self.project_running:
            raise InadequateUserException("cannot stop project: project not started")

        self.project.stop()
        self._started = False

    def pause_project(self):
        if not self.project_loaded:
            raise InadequateUserException("cannot pause project: project not loaded")

        if self.project_paused:
            raise InadequateUserException("cannot pause project: project already paused")

        if not self.project_running:
            raise InadequateUserException("cannot pause project: project not running")

        self.project.pause()
        self._paused = True

    def unpause_project(self):
        if not self.project_loaded:
            raise InadequateUserException("cannot unpause project: project not loaded")

        if not self.project_started:
            raise InadequateUserException("cannot unpause projeect: project was never started")

        if not self.project_paused:
            raise InadequateUserException("cannot unpause project: project not paused")

        self.project.unpause()
        self._paused = False

    def hash_project(self):
        return self.project.calculate_hash()

    def load_project(self):
        self.project.load()

    def dump_state(self):
        return {
            "project_started": self._started,
            "project_paused": self._paused,
            "machines_list": self.project.dump_state()
        }
