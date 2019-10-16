import base64
from common import InadequateUserException
from models.project import Project


class ProjectRunner(object):

    def __init__(self):
        self.command_handler = None
        self.connected_clients = []
        self.project = Project()

    @property
    def project_started(self):
        return False  # TODO

    @property
    def project_running(self):
        return self.project_started and not self.project_paused

    @property
    def project_paused(self):
        return False  # TODO

    @property
    def project_loaded(self):
        return self.project.loaded

    def set_command_handler(self, command_handler):
        self.command_handler = command_handler

    def add_client(self, client):
        self.connected_clients.append(client)

    def remove_client(self, client):
        self.connected_clients.remove(client)

    
    # commands

    def new_project(self):
        if self.project_running:
            raise InadequateUserException("Cannot create a project when one is running.")

        self.project = Project()


    def project_add_file(self, file_name, filedata_b64):
        try:
            file_data = base64.b64decode(filedata_b64)
            file_data = file_data.decode('ascii')
        except:
            raise InadequateUserException("ProjectRunner.project_add_file: file data not base64!")

        self.project.add_file(file_name, file_data)


    def start_project(self):
        if not self.project_loaded:
            raise InadequateUserException("cannot start project: project not loaded")
        
        if self.project_running:
            raise InadequateUserException("cannot start project: project is running")

        # TODO implement start project


    def stop_project(self):
        if not self.project_loaded:
            raise InadequateUserException("cannot stop project: project not loaded")

        if not self.project_running:
            raise InadequateUserException("cannot stop project: project not started")

        # todo implemtn stop project


    def pause_project(self):
        if not self.project_loaded:
            raise InadequateUserException("cannot pause project: project not loaded")

        if self.project_paused:
            raise InadequateUserException("cannot pause project: project already paused")

        if not self.project_running:
            raise InadequateUserException("cannot pause project: project not running")

        # TODO implement pause


    def unpause_project(self):
        if not self.project_loaded:
            raise InadequateUserException("cannot unpause project: project not loaded")

        if not self.project_started:
            raise InadequateUserException("cannot unpause projeect: project was never started")

        if not self.project_paused:
            raise InadequateUserException("cannot unpause project: project not paused")

        # TODO implement unpause

    def hash_project(self):
        return self.project.calculate_hash()


