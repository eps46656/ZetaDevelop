import dataclasses
import os
import pathlib
import subprocess
import typing

import termcolor
from typeguard import typechecked

from utils import ArchEnum, EnvEnum, SysEnum, Target, ToListCommand, VendorEnum


@typechecked
def GetClangTriple(target: Target):
    arch_table = {
        ArchEnum.INTEL64: "x86_64",
        ArchEnum.AMD64: "x86_64",
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
        ArchEnum.INTEL64: "x86-64",
        ArchEnum.AMD64: "x86-64",
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

    build_dir: object

    target: Target

    base_dir: typing.Optional[object]

    c_standard: str
    cpp_standard: str

    c_include_dirs: typing.Iterable[object]
    cpp_include_dirs: typing.Iterable[object]

    enable_debug: bool
    enable_asan: bool

    opt_type: str
    link_time_opt: bool


@typechecked
class LLVMCompiler:
    def __init__(self, config: LLVMCompilerConfig):
        self.verbose = config.verbose

        self.target = dataclasses.replace(config.target)

        self.base_dir = config.base_dir

        self.build_dir = pathlib.Path(config.build_dir)

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

        self.c_standard = config.c_standard
        self.cpp_standard = config.cpp_standard

        self.c_include_dirs = map(pathlib.Path, config.c_include_dirs)
        self.cpp_include_dirs = map(pathlib.Path, config.cpp_include_dirs)

        self.enable_debug = config.enable_debug
        self.enable_asan = config.enable_asan

        self.opt_type = config.opt_type
        self.link_time_opt = config.link_time_opt

        self.randomize_layout_seed = 13493037705

        self.clang_triple = GetClangTriple(self.target)

        self.error_limit = 8

        self.c_to_obj_args = [
            f"-v" if self.verbose else "",
            f"--target={self.clang_triple}",
            f"-m64",
            f"-std={self.c_standard}",
            *(f"--include-directory={include_dir}"
              for include_dir in self.c_include_dirs),
            f"-ferror-limit={self.error_limit}",
            # f"-frandomize-layout-seed={self.randomize_layout_seed}",

            f"" if self.base_dir is None else
            f"-ffile-prefix-map={self.base_dir}=.",

            "-Wall",
            "-Wextra",
            "-Werror",
        ]

        self.cpp_to_obj_args = [
            f"-v" if self.verbose else "",
            f"--target={self.clang_triple}",
            f"-m64",
            f"-std={self.cpp_standard}",
            *(f"--include-directory={include_dir}"
              for include_dir in self.cpp_include_dirs),
            f"-ferror-limit={self.error_limit}",
            # f"-frandomize-layout-seed={self.randomize_layout_seed}",

            f"" if self.base_dir is None else
            f"-ffile-prefix-map={self.base_dir}=.",

            "-Wall",
            "-Wextra",
            "-Werror",
        ]

        if self.enable_debug:
            args = [
                "-g",
                "-fno-omit-frame-pointer",
                "-fno-optimize-sibling-calls",

                "-DZETA_EnableDebug=1",
            ]

            self.c_to_obj_args.extend(args)
            self.cpp_to_obj_args.extend(args)

        if self.enable_asan:
            args = [
                # "-fprofile-instr-generate",
                # "-fcoverage-mapping",

                "-fsanitize=address",
                # "-fsanitize=undefined",
                # "-fsanitize=memory",
                # "-fsanitize-memory-track-origins",
            ]

            self.c_to_obj_args.extend(args)
            self.cpp_to_obj_args.extend(args)

        self.c_to_obj_args.append(f"-O{self.opt_type}")
        self.cpp_to_obj_args.append(f"-O{self.opt_type}")

        self.ll_to_asm_args = [
            f"-march={GetLLCArch(self.target)}",
        ]

        self.ll_to_obj_args = [
            f"-march={GetLLCArch(self.target)}",
        ]

        # ----------------------------------------------------------------------

        self.to_exe_args = [
            "--verbose" if self.verbose else "",
            f"--target={self.clang_triple}",
            "-m64",
            # "-lstdc++",
            # "-lm",

            # f"-frandomize-layout-seed={self.randomize_layout_seed}",
        ]

        self.to_exe_args.append(f"-O{self.opt_type}")

        if self.enable_debug:
            self.to_exe_args.extend([
                "-g",

                "-fno-omit-frame-pointer",
                "-fno-optimize-sibling-calls",

                "-DZETA_EnableDebug=1",
            ])

        if self.enable_asan:
            self.to_exe_args.extend([
                # "-fprofile-instr-generate",
                # "-fcoverage-mapping",

                "-fsanitize=address",
                # "-fsanitize=undefined",
                # "-fsanitize=memory",
                # "-fsanitize-memory-track-origins",
            ])

        if self.link_time_opt:
            self.to_exe_args.append("-flto")

    def RunCommand_(self, *cmd: typing.Iterable[typing.Optional[str | typing.Iterable]]):
        list_cmd = ToListCommand(cmd)
        PrintCommand(list_cmd)
        subprocess.run(list_cmd, check=True)

    def c_to_obj(self, dst: object, src: object):
        dst = pathlib.Path(dst)
        src = pathlib.Path(src)

        os.makedirs(dst.parents[0], exist_ok=True)

        self.RunCommand_(
            self.c_to_obj_command,
            "--compile",
            "-o", dst,
            self.c_to_obj_args,
            src,
        )

    def cpp_to_obj(self, dst: object, src: object):
        dst = pathlib.Path(dst)
        src = pathlib.Path(src)

        os.makedirs(dst.parents[0], exist_ok=True)

        self.RunCommand_(
            self.cpp_to_obj_command,
            "--compile",
            "-o", dst,
            self.cpp_to_obj_args,
            src,
        )

    def asm_to_obj(self, dst: object, src: object):
        dst = pathlib.Path(dst)
        src = pathlib.Path(src)

        os.makedirs(dst.parents[0], exist_ok=True)

        self.RunCommand_(
            self.c_to_obj_command,
            "--compile",
            "-o", dst,
            src,
        )

    def ll_to_asm(self, dst: object, src: object):
        dst = pathlib.Path(dst)
        src = pathlib.Path(src)

        self.RunCommand_(
            self.ll_to_obj_command,
            "-o", dst,
            "-filetype=asm",
            self.ll_to_asm_args,
            src,
        )

    def ll_to_obj(self, dst: object, src: object):
        dst = pathlib.Path(dst)
        src = pathlib.Path(src)

        self.RunCommand_(
            self.ll_to_obj_command,
            "-o", dst,
            "-filetype=obj",
            self.ll_to_obj_args,
            src,
        )

    def to_exe(self, dst: object, srcs: typing.Iterable[object]):
        dst = pathlib.Path(dst)
        srcs = map(pathlib.Path, srcs)

        self.RunCommand_(
            self.to_exe_command,
            "-o", dst,
            self.to_exe_args,
            *(src for src in srcs if src is not None),
        )
