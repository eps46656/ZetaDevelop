import enum
import dataclasses

class EmptyClass:
    pass

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

# ------------------------------------------------------------------------------

@dataclasses.dataclass
class Target:
    arch: ArchEnum
    vendor: VendorEnum
    sys: SysEnum
    env: EnvEnum

target = Target(
    arch=ArchEnum.X86_64,
    # arch=ArchEnum.RISCV64,
    vendor=VendorEnum.PC,
    sys=SysEnum.LINUX,
    # sys=SysEnum.WINDOWS,
    env=EnvEnum.GNU,
    # env=EnvEnum.MSVC,
)
