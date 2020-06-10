

class InadequateUserException(Exception):
    """thrown by ProjectRunner when user does something incorrect"""
    pass


class ValueType(object):
    VT_ANY = 0
    VT_STR = 1
    VT_INT = 2
    VT_FLOAT = 3
    VT_REF = 4

    ALL = [VT_ANY, VT_STR, VT_INT, VT_FLOAT, VT_REF]


class ProjectFileType(object):
    # Values correspond to ProjectFileType enum
    CODE_STATE = 0
    RESOURCE = 1
    CUSTOM_STATE_CLASS = 2
    INCLUDE = 3
    NONE = 4
    PROJFILE = 5
