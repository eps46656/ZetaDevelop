from config import *
from utils import *

import os

def GetClangTriple(target):
    arch_table = {
        target.arch_enum.x86_64: "x86_64",
        target.arch_enum.arm: "arm",
        target.arch_enum.arm32: "arm32",
        target.arch_enum.arm64: "arm64",
        target.arch_enum.riscv32: "riscv32",
        target.arch_enum.riscv64: "riscv64",
    }

    vendor_table = {
        target.vendor_enum.pc: "pc",
    }

    sys_table = {
        target.sys_enum.linux: "linux",
        target.sys_enum.windows: "windows",
    }

    env_table = {
        target.env_enum.gnu: "gun",
        target.env_enum.elf: "elf",
        target.env_enum.msvc: "msvc",
    }

    arch = arch_table[target.arch]
    vendor = vendor_table[target.vendor]
    sys = sys_table[target.sys]
    env = env_table[target.env]

    return f"{arch}-{vendor}-{sys}-{env}"

def GetLLCArch(target):
    arch_table = {
        target.arch_enum.x86_64: "x86-64",
        target.arch_enum.arm: "arm",
        target.arch_enum.arm32: "arm32",
        target.arch_enum.arm64: "arm64",
        target.arch_enum.riscv32: "riscv32",
        target.arch_enum.riscv64: "riscv64",
    }

    arch = arch_table[target.arch]

    return arch

class Compiler:
    def __init__(self, config):
        self.mode = config.__dict__.get("mode", mode_enum.debug)
        assert self.mode in mode_enum.__dict__.values()

        self.verbose = config.verbose

        self.build_dir = config.build_dir

        self.target = EmptyClass
        self.target.arch = config.target.arch
        self.target.vendor = config.target.vendor
        self.target.sys = config.target.sys
        self.target.env = config.target.env

        self.c_to_ll_command = None
        self.cpp_to_ll_command = None
        self.link_lls_command = None
        self.opt_ll_command = None
        self.ll_to_obj_command = None

        if os.name == "nt":
            self.c_to_ll_command = "clang"
            self.cpp_to_ll_command = "clang++"
            self.link_lls_command = "llvm-link"
            self.opt_ll_command = "opt"
            self.ll_to_obj_command = "llc"
        else:
            self.c_to_ll_command = "clang-18"
            self.cpp_to_ll_command = "clang++-18"
            self.link_lls_command = "llvm-link-18"
            self.opt_ll_command = "opt-18"
            self.ll_to_obj_command = "llc-18"

        self.c_include_dirs = config.__dict__.get("c_include_dirs", [])
        self.cpp_include_dirs = config.__dict__.get("cpp_include_dirs", [])

        clang_triple = GetClangTriple(self.target)

        include_dirs = [
            # ToPath("C:\Program Files\llvm-mingw-20231003-msvcrt-x86_64\include")
        ]

        self.c_to_ll_args = [
            f"-v" if self.verbose else "",
            # f"-target {clang_triple}",
            f"-m64",
            f"-std=c2x",
            *(f"-I {include_dir}" for include_dir in include_dirs),
        ]

        self.cpp_to_ll_args = [
            f"-v" if self.verbose else "",
            # f"-target {clang_triple}",
            f"-m64",
            f"-std=c++17",
            *(f"-I {include_dir}" for include_dir in include_dirs),
        ]

        if self.mode == mode_enum.debug:
            self.c_to_ll_args += [
                "-g",

                "-D DEBUG",

                "-Wall",
                "-Wextra",
                "-Werror",


            ]

            '''
            "-fno-omit-frame-pointer",
            "-fsanitize=address",
            # "-fsanitize=memory",
            # "-fsanitize-memory-track-origins",
            "-fsanitize=undefined",
            '''

            self.cpp_to_ll_args += [
                "-g",

                "-D DEBUG",

                "-Wall",
                "-Wextra",
                "-Werror",
            ]

        if self.mode == mode_enum.release:
            self.c_to_ll_args += [
                "-O3",
            ]

            self.cpp_to_ll_args += [
                "-O3",
            ]

        self.ll_to_asm_args = [
            f"-march={GetLLCArch(self.target)}",
        ]

        self.ll_to_obj_args = [
            f"-march={GetLLCArch(self.target)}",
        ]

    def c_to_ll(self, dst, src):
        os.makedirs(os.path.dirname(dst), exist_ok=True)

        cmd = " ".join([
            self.c_to_ll_command,
            f"-o {ToPath(dst)}",
            f"-emit-llvm -S",
            *self.c_to_ll_args,
            ToPath(src),
        ])

        HighLightPrint(f"cmd = {cmd}")

        rc = os.system(cmd)

        return rc

    def cpp_to_ll(self, dst, src):
        os.makedirs(os.path.dirname(dst), exist_ok=True)

        cmd = " ".join([
            self.cpp_to_ll_command,
            f"-o {ToPath(dst)}",
            f"-emit-llvm -S",
            *self.cpp_to_ll_args,
            ToPath(src),
        ])

        HighLightPrint(f"cmd = {cmd}")

        rc = os.system(cmd)

        return rc

    def ll_to_asm(self, dst, src):
        cmd = " ".join([
            self.ll_to_obj_command,
            f"-o {ToPath(dst)}",
            "-filetype=asm",
            *self.ll_to_asm_args,
            ToPath(src),
        ])

        HighLightPrint(f"cmd = {cmd}")

        rc = os.system(cmd)

        return rc

    def ll_to_obj(self, dst, src):
        cmd = " ".join([
            self.ll_to_obj_command,
            f"-o {ToPath(dst)}",
            "-filetype=obj",
            *self.ll_to_obj_args,
            ToPath(src),
        ])

        HighLightPrint(f"cmd = {cmd}")

        rc = os.system(cmd)

        return rc

    def lls_to_exe(self, dst, srcs):
        linked_tmp_file = f"{self.build_dir}/tmp.ll"
        opted_tmp_file = f"{self.build_dir}/tmp.opt.ll"
        rc = self.lls_to_exe_(dst, srcs, linked_tmp_file, opted_tmp_file)
        # os.remove(linked_tmp_file)
        # os.remove(opted_tmp_file)
        return rc

    def lls_to_exe_(self, dst, srcs, linked_tmp_file, opted_tmp_file):
        os.makedirs(os.path.dirname(dst), exist_ok=True)
        os.makedirs(os.path.dirname(linked_tmp_file), exist_ok=True)
        os.makedirs(os.path.dirname(opted_tmp_file), exist_ok=True)

        cmd = " ".join([
            self.link_lls_command,
            f"-o {ToPath(linked_tmp_file)}",
            f"-v" if self.verbose else "",
            f"-S",
            *[ToPath(src) for src in FilterNotNone(srcs)],
        ])

        HighLightPrint(f"cmd = {cmd}")

        rc = os.system(cmd)

        if rc != 0:
            return rc

        cmd = " ".join([
            self.opt_ll_command,
            f"-o {ToPath(opted_tmp_file)}",
            f"-S",
            ToPath(linked_tmp_file),
        ])

        HighLightPrint(f"cmd = {cmd}")

        rc = os.system(cmd)

        if rc != 0:
            return rc

        cmd = " ".join([
            self.c_to_ll_command,
            f"-o {ToPath(dst)}",
            "--verbose" if self.verbose else "",
            "-m64",
            "-O1",
            "-g",
            # *[ToPath(lib) for lib in self.runtime_libs],

            "-lstdc++",
            ToPath(opted_tmp_file),
        ])

        HighLightPrint(f"cmd = {cmd}")

        rc = os.system(cmd)

        return rc
