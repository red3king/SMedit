import sys
from os import system, name
from types import MethodType, FunctionType


def clear():
    if name == 'nt':
        system('cls')
    else:
        system('clear')


class Signal(object):

    def __init__(self):
        self.functions = []

    def __iadd__(self, other):
        typ = type(other)

        if typ is not MethodType and typ is not FunctionType:
            raise Exception("you can only add MethodType or FunctionType to an Event")

        self.functions.append(other)
        return self

    def __isub__(self, other):
        self.functions.remove(other)
        return self

    def fire(self, *args, **kwargs):
        res = []
        for f in self.functions:
            res.append(f(*args, **kwargs))

        if len(res) > 1:
            return res

        if len(res) == 1:
            return res[0]

        return None

    def __call__(self, *args, **kwargs):
        return self.fire(*args, **kwargs)


