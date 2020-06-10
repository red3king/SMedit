import sys
import os.path

from importlib.abc import Loader, MetaPathFinder
from importlib.util import spec_from_file_location

from common import ProjectFileType

# based on the following - how to do custom importing:
# https://stackoverflow.com/a/43573798


class ProjectImportModules(object):
    def __init__(self, project):
        self.project = project
        self.loaded = []
    
    def get_file(self, module_name):
        filename = module_name + ".py"
        ftype = self.project.get_filetype(filename)
        
        if not ftype == ProjectFileType.INCLUDE:
            return None
        
        return self.project.get_file(filename)

    def set_loaded(self, module_name):
        self.loaded.append(module_name)
        
    def get_loaded(self):
        return self.loaded[:]


class NetMetaFinder(MetaPathFinder):
    
    def __init__(self):
        super().__init__()
        self.project_import_modules = None
        
    def set_project_modules(self, project_import_modules):
        self.project_import_modules = project_import_modules
    
    def clear_project_modules(self):
        if self.project_import_modules is None:
            return 
        
        for module_name in self.project_import_modules.get_loaded():
            del sys.modules[module_name]
            
        self.project_import_modules = None
    
    def find_spec(self, fullname, path, target=None):
        print("nmf::find_spec, fn=" + str(fullname) + ", path=" + str(path) + ", targ=" + str(target))
        
        if self.project_import_modules is None:
            return None
        
        filedata = self.project_import_modules.get_file(fullname)
        
        if filedata is None:
            return None
    
        # TODO should be not-none for loading packages
        # This works ok for single import files so far.
        submodule_locations = None  
        
        loader = NetLoader(self.project_import_modules, fullname)
        return spec_from_file_location(fullname, fullname, loader=loader,
                submodule_search_locations=submodule_locations)
    
    
net_finder = NetMetaFinder()    


class NetLoader(Loader):
    def __init__(self, project_import_modules, filename):
        self.project_import_modules = project_import_modules
        self.filename = filename
        print("nl::init - filename=" + str(filename))
        
    def create_module(self, spec):
        return None
    
    def exec_module(self, module):
        data = self.project_import_modules.get_file(self.filename)
        exec(data, vars(module))
        self.project_import_modules.set_loaded(self.filename)

    
def install_netloader():
    sys.meta_path.insert(0, net_finder)


def set_project_modules(project_import_modules):
    net_finder.set_project_modules(project_import_modules)
    

def clear_project_modules():
    net_finder.clear_project_modules()
