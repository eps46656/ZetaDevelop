import dataclasses
import enum
import typing


from typeguard import typechecked


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
def ToListCommand(*cmd: typing.Iterable[typing.Optional[str | typing.Iterable]]):
    ret: list[str] = list()

    q = list(cmd)

    while 0 < len(q):
        x = q.pop()

        if not x:
            continue

        if not hasattr(x, "__iter__") or isinstance(x, str):
            ret.append(str(x))
        else:
            q.extend(x)

    return list(reversed(ret))
