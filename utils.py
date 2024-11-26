import dataclasses
import enum
import typing

from typeguard import typechecked


class ModeEnum(enum.Enum):
    DEBUG = enum.auto(),
    RELEASE = enum.auto(),


class ArchEnum(enum.Enum):
    X86_64 = enum.auto(),
    ARM32 = enum.auto(),
    ARM64 = enum.auto(),
    RISCV32 = enum.auto(),
    RISCV64 = enum.auto(),


class VendorEnum(enum.Enum):
    PC = enum.auto(),


class SysEnum(enum.Enum):
    LINUX = enum.auto(),
    WINDOWS = enum.auto(),


class EnvEnum(enum.Enum):
    GNU = enum.auto(),
    ELF = enum.auto(),
    MSVC = enum.auto(),


class ColorEnum(enum.Enum):
    red = enum.auto(),
    orange = enum.auto(),
    yellow = enum.auto(),


@dataclasses.dataclass
class Target:
    arch: ArchEnum
    vendor: VendorEnum
    sys: SysEnum
    env: EnvEnum


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
def ToListCommand(*cmd: typing.Iterable[typing.Optional[str | typing.Iterable]]):
    ret: list[str] = list()

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
