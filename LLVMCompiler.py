from config import *
from utils import *

import os
import typing
import subprocess
import termcolor
from typeguard import typechecked

@typechecked
def GetClangTriple(target: Target):
    arch_table = {
        ArchEnum.X86_64: "x86_64",
        ArchEnum.ARM32: "arm32",
        ArchEnum.ARM64: "arm64",
        ArchEnum.RISCV32: "riscv32",
        ArchEnum.RISCV64: "riscv64",
    }

    vendor_table = {
        VendorEnum.PC: "pc",
    }

    sys_table = {
        SysEnum.LINUX: "linux",
        SysEnum.WINDOWS: "windows",
    }

    env_table = {
        EnvEnum.GNU: "gun",
        EnvEnum.ELF: "elf",
        EnvEnum.MSVC: "msvc",
    }

    arch = arch_table[target.arch]
    vendor = vendor_table[target.vendor]
    sys = sys_table[target.sys]
    env = env_table[target.env]

    return f"{arch}-{vendor}-{sys}-{env}"

@typechecked
def GetLLCArch(target: Target):
    arch_table = {
        ArchEnum.X86_64: "x86-64",
        ArchEnum.ARM32: "arm32",
        ArchEnum.ARM64: "arm64",
        ArchEnum.RISCV32: "riscv32",
        ArchEnum.RISCV64: "riscv64",
    }

    arch = arch_table[target.arch]

    return arch

@typechecked
def PrintCommand(cmd: typing.Iterable[str]):
    cmd = " ".join(cmd)
    print(termcolor.colored(f"{cmd=}", "cyan"))

@dataclasses.dataclass
class LLVMCompilerConfig:
    verbose: bool
    target: Target
    mode: ModeEnum
    build_dir: str
    working_dirs: typing.Iterable[str]
    c_include_dirs: typing.Iterable[str]
    cpp_include_dirs: typing.Iterable[str]

@typechecked
class LLVMCompiler:
    def __init__(self, config: LLVMCompilerConfig):
        self.verbose = config.verbose

        self.target = dataclasses.replace(config.target)

        self.mode = config.mode
        assert self.mode in ModeEnum

        self.build_dir = config.build_dir

        self.working_dirs = set(FilterNotNone(config.working_dirs))

        self.c_to_obj_command = None
        self.cpp_to_obj_command = None
        self.link_lls_command = None
        self.opt_ll_command = None
        self.to_exe_command = None

        if os.name == "nt":
            self.c_to_obj_command = "clang"
            self.cpp_to_obj_command = "clang"
            self.asm_to_obj_command = "llvm-as"
            self.link_lls_command = "llvm-link"
            self.opt_ll_command = "opt"
            self.to_exe_command = "clang"
        else:
            self.c_to_obj_command = "clang-18"
            self.cpp_to_obj_command = "clang-18"
            self.asm_to_obj_command = "llvm-as-18"
            self.link_lls_command = "llvm-link-18"
            self.opt_ll_command = "opt-18"
            self.to_exe_command = "clang-18"

        self.c_include_dirs = config.c_include_dirs
        self.cpp_include_dirs = config.cpp_include_dirs

        self.randomize_layout_seed = 13493037705

        self.clang_triple = GetClangTriple(self.target)

        self.error_limit = 10

        self.address_sanitizer = True

        self.c_to_obj_args = [
            f"-v" if self.verbose else "",
            f"--target={self.clang_triple}",
            f"-m64",
            f"-std=c2x",
            *(f"--include-directory={include_dir}"
              for include_dir in self.c_include_dirs),
            f"-ferror-limit={self.error_limit}",
            # f"-frandomize-layout-seed={self.randomize_layout_seed}",
        ]

        self.cpp_to_obj_args = [
            f"-v" if self.verbose else "",
            f"--target={self.clang_triple}",
            f"-m64",
            f"-std=c++17",
            *(f"--include-directory={include_dir}"
              for include_dir in self.cpp_include_dirs),
            f"-ferror-limit={self.error_limit}",
            # f"-frandomize-layout-seed={self.randomize_layout_seed}",
        ]

        if self.mode == ModeEnum.DEBUG:
            self.c_to_obj_args += [
                "-O1",
                "-g",
                "-D DEBUG",

                "-Wall",
                "-Wextra",
                "-Werror",

                "-fno-omit-frame-pointer",
                "-fno-optimize-sibling-calls",

                # "-fprofile-instr-generate",
                # "-fcoverage-mapping",

                "-fsanitize=address" if self.address_sanitizer else "",
                # "-fsanitize=undefined",
                # "-fsanitize=memory",
                # "-fsanitize-memory-track-origins",
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

                # "-fprofile-instr-generate",
                # "-fcoverage-mapping",

                "-fsanitize=address" if self.address_sanitizer else "",
                # "-fsanitize=undefined",
                # "-fsanitize=memory",
                # "-fsanitize-memory-track-origins",
            ]

        if self.mode == ModeEnum.RELEASE:
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
            f"--target={self.clang_triple}",
            "-m64",
            "-lstdc++",
            # "-lm",

            # f"-frandomize-layout-seed={self.randomize_layout_seed}",
        ]

        if self.mode == ModeEnum.DEBUG:
            self.to_exe_args += [
                "-O1",
                "-g",
                "-D DEBUG",

                "-fno-omit-frame-pointer",
                "-fno-optimize-sibling-calls",

                "-fsanitize=address" if self.address_sanitizer else "",
                # "-fsanitize=undefined",
                # "-fsanitize=memory",
                # "-fsanitize-memory-track-origins",
            ]

        if self.mode == ModeEnum.RELEASE:
            self.to_exe_args += [
                "-O3",
                "-flto",
            ]

    def RunCommand_(self, *cmd):
        cmd = ToListCommand(cmd)
        PrintCommand(cmd)
        subprocess.run(cmd, check=True)

    def GetIncludes(self, src):
        includes =  subprocess.run(
            ToListCommand(
                "clang++",
                "--trace-includes",
                "-fshow-skipped-includes",
                "-fsyntax-only",
                (("--include-directory", include_dir)
                 for include_dir in self.cpp_include_dirs),
                src,
            ),
            check=True,
            stdin=None,
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
        ).stderr.decode("utf-8").replace("\r", "").split("\n")

        ret = list()

        for include in includes:
            if not include or not include.startswith(". "):
                continue

            include = GetRealPath(include[2:])

            if any(include.startswith(working_dir)
                   for working_dir in self.working_dirs):
                ret.append(include)

        return ret

    def c_to_obj(self, dst: str, src: str):
        os.makedirs(os.path.dirname(dst), exist_ok=True)

        self.RunCommand_(
            self.c_to_obj_command,
            "--compile",
            "-o", dst,
            self.c_to_obj_args,
            src,
        )

    def cpp_to_obj(self, dst: str, src: str):
        os.makedirs(os.path.dirname(dst), exist_ok=True)

        self.RunCommand_(
            self.cpp_to_obj_command,
            "--compile",
            "-o", dst,
            self.cpp_to_obj_args,
            src,
        )

    def asm_to_obj(self, dst: str, src: str):
        os.makedirs(os.path.dirname(dst), exist_ok=True)

        self.RunCommand_(
            self.c_to_obj_command,
            "--compile",
            "-o", dst,
            src,
        )

    def ll_to_asm(self, dst: str, src: str):
        self.RunCommand_(
            self.ll_to_obj_command,
            "-o", dst,
            "-filetype=asm",
            self.ll_to_asm_args,
            src,
        )

    def ll_to_obj(self, dst, src):
        self.RunCommand_(
            self.ll_to_obj_command,
            "-o", dst,
            "-filetype=obj",
            self.ll_to_obj_args,
            src,
        )

    def to_exe(self, dst, srcs):
        self.RunCommand_(
            self.to_exe_command,
            "-o", dst,
            self.to_exe_args,
            FilterNotNone(srcs),
        )
