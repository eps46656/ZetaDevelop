import os
import typing
from typeguard import typechecked
import enum

@typechecked
def GetRealPath(path: str):
    return os.path.realpath(path).replace("\\", "/")

@typechecked
def GetDirPath(path: str):
    return GetRealPath(os.path.dirname(path))

@typechecked
def ToPath(path):
    path = path.strip("\'\"")
    return f"\'{path}\'"

@typechecked
def FilterNotNone(iter: typing.Iterable):
    return (x for x in iter if x)

@typechecked
def ToListCommand(*cmd):
    ret = list()

    q = list(cmd)

    while 0 < len(q):
        x = q.pop()

        if not x:
            continue

        if isinstance(x, str):
            ret.append(x)
        else:
            q.extend(x)

    return list(reversed(ret))
