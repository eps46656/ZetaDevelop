from config import *

import os

def NotNoneFilter(iterable):
    return filter(lambda x: x is not None, iterable)

def GetNormPath(path):
    return os.path.abspath(path).replace("\\", "/")

def ToPath(path):
    path = path.strip("\'\"")
    return f"\'{path}\'"

def HighLightPrint(x):
    print(f"\033[93m{x}\033[0m")

def FilterNotNone(iter):
    return filter(lambda x: x is not None, iter)

def ToListCommand_(dst, cmd):
    if cmd is None:
        return None

    if isinstance(cmd, str):
        dst.append(cmd)
        return

    for k in cmd:
        ToListCommand_(dst, k)

def ToListCommand(*cmd):
    ret = []

    ToListCommand_(ret, cmd)

    return ret


