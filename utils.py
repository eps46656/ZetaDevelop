import os
import typing

from typeguard import typechecked


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
def SanitizeJsonStruct(x: typing.Optional[bool | int | float | str | list | dict]):
    if x is None:
        return None

    if isinstance(x, (bool, int, float)):
        return x

    if isinstance(x, str):
        return None if len(x) == 0 else x

    if isinstance(x, dict):
        ret = {k: v for k, v in ((k, FilterNotNone(v))
                                 for k, v in x.items()) if v is not None}
        return None if len(ret) == 0 else x

    if isinstance(x, list):
        ret = [e for e in (FilterNotNone(e) for e in x) if x is not None]
        return None if len(ret) == 0 else x

    return None


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
