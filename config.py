class EmptyClass:
    pass

counter = 0

def GetEnumInt():
    global counter
    counter += 1
    return counter

mode_enum = EmptyClass
mode_enum.debug = GetEnumInt()
mode_enum.release = GetEnumInt()

arch_enum = EmptyClass
arch_enum.x86_64 = GetEnumInt()
arch_enum.arm = GetEnumInt()
arch_enum.arm32 = GetEnumInt()
arch_enum.arm64 = GetEnumInt()
arch_enum.riscv32 = GetEnumInt()
arch_enum.riscv64 = GetEnumInt()

vendor_enum = EmptyClass
vendor_enum.pc = GetEnumInt()

sys_enum = EmptyClass
sys_enum.linux = GetEnumInt()
sys_enum.windows = GetEnumInt()

env_enum = EmptyClass
env_enum.gnu = GetEnumInt()
env_enum.elf = GetEnumInt()
env_enum.msvc = GetEnumInt()

# ------------------------------------------------------------------------------

target = EmptyClass

target.arch = arch_enum.x86_64
# target.arch = arch_enum.riscv64

target.vendor = vendor_enum.pc

target.sys = sys_enum.linux
# target.sys = sys_enum.windows

target.env = env_enum.gnu
# target.env = env_enum.msvc
