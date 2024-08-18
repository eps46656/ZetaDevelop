from config import *
from utils import *

import os
import subprocess

def GetClangTriple(target):
    arch_table = {
        arch_enum.x86_64: "x86_64",
        arch_enum.arm: "arm",
        arch_enum.arm32: "arm32",
        arch_enum.arm64: "arm64",
        arch_enum.riscv32: "riscv32",
        arch_enum.riscv64: "riscv64",
    }

    vendor_table = {
        vendor_enum.pc: "pc",
    }

    sys_table = {
        sys_enum.linux: "linux",
        sys_enum.windows: "windows",
    }

    env_table = {
        env_enum.gnu: "gun",
        env_enum.elf: "elf",
        env_enum.msvc: "msvc",
    }

    arch = arch_table[target.arch]
    vendor = vendor_table[target.vendor]
    sys = sys_table[target.sys]
    env = env_table[target.env]

    return f"{arch}-{vendor}-{sys}-{env}"

def GetLLCArch(target):
    arch_table = {
        arch_enum.x86_64: "x86-64",
        arch_enum.arm: "arm",
        arch_enum.arm32: "arm32",
        arch_enum.arm64: "arm64",
        arch_enum.riscv32: "riscv32",
        arch_enum.riscv64: "riscv64",
    }

    arch = arch_table[target.arch]

    return arch

def ShowCommand(cmd):
    cmd = f" ".join(cmd)
    HighLightPrint(f"cmd = {cmd}")

class Compiler:
    def __init__(self, config):
        self.mode = config.__dict__.get("mode", mode_enum.debug)
        assert self.mode in mode_enum.__dict__.values()

        self.verbose = config.verbose

        self.build_dir = config.build_dir

        self.working_dirs = set(NotNoneFilter(
            config.__dict__.get("working_dirs", [])))

        self.target = EmptyClass
        self.target.arch = config.target.arch
        self.target.vendor = config.target.vendor
        self.target.sys = config.target.sys
        self.target.env = config.target.env

        self.c_to_obj_command = None
        self.cpp_to_obj_command = None
        self.link_lls_command = None
        self.opt_ll_command = None
        self.to_exe_command = None

        if os.name == "nt":
            self.c_to_obj_command = "clang"
            self.cpp_to_obj_command = "clang"
            self.link_lls_command = "llvm-link"
            self.opt_ll_command = "opt"
            self.to_exe_command = "clang"
        else:
            self.c_to_obj_command = "clang-18"
            self.cpp_to_obj_command = "clang-18"
            self.link_lls_command = "llvm-link-18"
            self.opt_ll_command = "opt-18"
            self.to_exe_command = "clang-18"

        self.c_include_dirs = config.__dict__.get("c_include_dirs", [])
        self.cpp_include_dirs = config.__dict__.get("cpp_include_dirs", [])

        print(f"{self.c_include_dirs=}")

        self.include_dirs = {}

        clang_triple = GetClangTriple(self.target)

        self.c_to_obj_args = [
            f"-v" if self.verbose else "",
            # f"-target {clang_triple}",
            f"-m64",
            # f"-std=c2x",
            *(f"--include-directory={include_dir}"
              for include_dir in self.c_include_dirs),
            "-ferror-limit=2",
        ]

        self.cpp_to_obj_args = [
            f"-v" if self.verbose else "",
            # f"-target {clang_triple}",
            f"-m64",
            # f"-std=c++17",
            *(f"--include-directory={include_dir}"
              for include_dir in self.cpp_include_dirs),
            "-ferror-limit=2",
        ]

        if self.mode == mode_enum.debug:
            self.c_to_obj_args += [
                "-O1",
                "-g",
                "-D DEBUG",

                "-Wall",
                "-Wextra",
                "-Werror",

                "-fno-omit-frame-pointer",
                "-fno-optimize-sibling-calls",
                "-fsanitize=address",
                # "-fsanitize=memory",
                # "-fsanitize-memory-track-origins",
                # "-fsanitize=undefined",
            ]

            self.cpp_to_obj_args += [
                "-O1",
                "-g",
                "-D DEBUG",

                "-Wall",
                "-Wextra",
                "-Werror",

                "-fno-omit-frame-pointer",
                "-fno-optimize-sibling-calls",
                "-fsanitize=address",
                # "-fsanitize=memory",
                # "-fsanitize-memory-track-origins",
                # "-fsanitize=undefined",
            ]

        if self.mode == mode_enum.release:
            self.c_to_obj_args += [
                "-O3",
            ]

            self.cpp_to_obj_args += [
                "-O3",
            ]

        self.ll_to_asm_args = [
            f"-march={GetLLCArch(self.target)}",
        ]

        self.ll_to_obj_args = [
            f"-march={GetLLCArch(self.target)}",
        ]

        self.to_exe_args = [
            "--verbose" if self.verbose else "",

            "-m64",

            "-lstdc++",
            # "-lm",
        ]

        if self.mode == mode_enum.debug:
            self.to_exe_args += [
                "-O1",
                "-g",
                "-D DEBUG",

                "-fno-omit-frame-pointer",
                "-fno-optimize-sibling-calls",
                "-fsanitize=address",
                # "-fsanitize=memory",
                # "-fsanitize-memory-track-origins",
                # "-fsanitize=undefined",
            ]

        if self.mode == mode_enum.release:
            self.to_exe_args += [
                "-O3",
                "-flto",
            ]

    def GetIncludes(self, src):
        includes =  subprocess.run(
            ToListCommand(
                "clang++",
                "--trace-includes",
                "-fshow-skipped-includes",
                "-fsyntax-only",
                *(["--include-directory", include_dir]
                  for include_dir in self.include_dirs),
                src,
            ),
            check=True,
            stdin=None,
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
        ).stderr.decode("utf-8").replace("\r", "").split("\n")

        ret = []

        for include in includes:
            if not include or not include.startswith(". "):
                continue

            include = GetNormPath(include[2:])

            if any(include.startswith(working_dir)
                   for working_dir in self.working_dirs):
                ret.append(include)

        return ret

    def c_to_obj(self, dst, src):
        os.makedirs(os.path.dirname(dst), exist_ok=True)

        cmd = ToListCommand(
            self.c_to_obj_command,
            "--compile",
            "-o", dst,
            self.c_to_obj_args,
            src,
        )

        ShowCommand(cmd)

        subprocess.run(cmd, check=True)

    def cpp_to_obj(self, dst, src):
        os.makedirs(os.path.dirname(dst), exist_ok=True)

        cmd = ToListCommand(
            self.cpp_to_obj_command,
            "--compile",
            "-o", dst,
            self.cpp_to_obj_args,
            src,
        )

        ShowCommand(cmd)

        subprocess.run(cmd, check=True)

    def ll_to_asm(self, dst, src):
        cmd = ToListCommand([
            self.ll_to_obj_command,
            "-o", dst,
            "-filetype=asm",
            self.ll_to_asm_args,
            src,
        ])

        ShowCommand(cmd)

        subprocess.run(cmd, check=True)

    def ll_to_obj(self, dst, src):
        cmd = ToListCommand(
            self.ll_to_obj_command,
            "-o", dst,
            "-filetype=obj",
            self.ll_to_obj_args,
            src,
        )

        ShowCommand(cmd)

        subprocess.run(cmd, check=True)

    def to_exe(self, dst, srcs):
        cmd = ToListCommand(
            self.to_exe_command,
            "-o", dst,
            self.to_exe_args,
            *(src for src in FilterNotNone(srcs)),
        )

        ShowCommand(cmd)

        subprocess.run(cmd, check=True)
