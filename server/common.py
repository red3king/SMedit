
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




