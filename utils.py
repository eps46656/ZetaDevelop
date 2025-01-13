import dataclasses
import enum
import typing


from typeguard import typechecked


class ArchEnum(enum.Enum):
    INTEL64 = enum.auto(),
    AMD64 = enum.auto(),
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


def ThreeWayCompare(x, y):
    if x < y:
        return -1

    if y < x:
        return 1

    return 0


@typechecked
class CompareWrapper:
    def __init__(self,
                 obj: object,
                 cmp: typing.Callable[[object, object], int]):
        self.obj = obj
        self.cmp = cmp

    def __eq__(self, d: typing.Self):
        return self.obj == d.obj

    def __ne__(self, d: typing.Self):
        return self.obj != d.obj

    def __lt__(self, d: typing.Self):
        assert self.cmp is d.cmp
        return self.cmp(self.obj, d.obj) < 0

    def __gt__(self, d: typing.Self):
        assert self.cmp is d.cmp
        return self.cmp(self.obj, d.obj) > 0

    def __le__(self, d: typing.Self):
        assert self.cmp is d.cmp
        return self.cmp(self.obj, d.obj) <= 0

    def __ge__(self, d: typing.Self):
        assert self.cmp is d.cmp
        return self.cmp(self.obj, d.obj) >= 0


@typechecked
def CompareToKey(cmp: typing.Callable[[object, object], int]):
    return lambda obj: CompareWrapper(obj, cmp)
