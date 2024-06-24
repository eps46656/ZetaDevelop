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

target = EmptyClass

target.arch_enum = EmptyClass
target.arch_enum.x86_64 = GetEnumInt()
target.arch_enum.arm = GetEnumInt()
target.arch_enum.arm32 = GetEnumInt()
target.arch_enum.arm64 = GetEnumInt()
target.arch_enum.riscv32 = GetEnumInt()
target.arch_enum.riscv64 = GetEnumInt()

target.vendor_enum = EmptyClass
target.vendor_enum.pc = GetEnumInt()

target.sys_enum = EmptyClass
target.sys_enum.linux = GetEnumInt()
target.sys_enum.windows = GetEnumInt()

target.env_enum = EmptyClass
target.env_enum.gnu = GetEnumInt()
target.env_enum.elf = GetEnumInt()
target.env_enum.msvc = GetEnumInt()

target.arch = target.arch_enum.x86_64
# target.arch = target.arch_enum.riscv64
target.vendor = target.vendor_enum.pc
target.sys = target.sys_enum.windows
target.env = target.env_enum.msvc
