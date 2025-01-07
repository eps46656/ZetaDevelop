import argparse
import dataclasses
import os
import pathlib
import sys

import termcolor

import Zeta
import ZetaExe
from Builder import Builder
from utils import ArchEnum, EnvEnum, SysEnum, Target, VendorEnum

FILE = pathlib.Path(__file__).absolute()
DIR = FILE.parents[0]

zeta_develop_dir = DIR
zeta_dir = zeta_develop_dir / "Zeta"
zeta_exe_dir = zeta_develop_dir / "ZetaExe"

target = None

if os.name == "nt":
    target = Target(
        arch=ArchEnum.INTEL64,
        vendor=VendorEnum.PC,
        sys=SysEnum.WINDOWS,
        env=EnvEnum.MSVC,
    )
elif os.name == "posix":
    target = Target(
        arch=ArchEnum.INTEL64,
        vendor=VendorEnum.PC,
        sys=SysEnum.LINUX,
        env=EnvEnum.ELF,
    )
else:
    assert False, "Unsupported architecture."

zeta_debug_config = Zeta.Config(
    name="debug",

    verbose=True,

    build_dir=zeta_develop_dir / "ZetaDebugBuild",

    target=target,

    c_standard="c2x",
    cpp_standard="c++17",

    c_include_dirs=[],
    cpp_include_dirs=[],

    enable_debug=True,
    enable_asan=True,

    opt_type=0,
    link_time_opt=False,
)

zeta_exe_debug_config = ZetaExe.Config(
    name="debug",

    verbose=True,

    build_dir=zeta_develop_dir / "ZetaExeDebugBuild",

    zeta_dir=zeta_develop_dir / "Zeta",
    zeta_build_dir=zeta_debug_config.build_dir,

    target=target,

    c_standard="c2x",
    cpp_standard="c++17",

    c_include_dirs=[zeta_dir],
    cpp_include_dirs=[zeta_dir],

    enable_debug=True,
    enable_asan=True,

    opt_type=0,
    link_time_opt=False,
)

zeta_release_config = Zeta.Config(
    name="release",

    verbose=True,

    build_dir=zeta_develop_dir / "ZetaReleaseBuild",

    target=target,

    c_standard="c2x",
    cpp_standard="c++17",

    c_include_dirs=[],
    cpp_include_dirs=[],

    enable_debug=False,
    enable_asan=False,

    opt_type=3,
    link_time_opt=True,
)

zeta_exe_release_config = ZetaExe.Config(
    name="release",

    verbose=True,

    build_dir=zeta_develop_dir / "ZetaExeReleaseBuild",

    zeta_dir=zeta_develop_dir / "Zeta",
    zeta_build_dir=zeta_release_config.build_dir,

    target=target,

    c_standard="c2x",
    cpp_standard="c++17",

    c_include_dirs=[zeta_dir],
    cpp_include_dirs=[zeta_dir],

    enable_debug=False,
    enable_asan=False,

    opt_type=3,
    link_time_opt=True,
)

zeta_raw_config = Zeta.Config(
    name="raw",

    verbose=True,

    build_dir=zeta_develop_dir / "ZetaRawBuild",

    target=target,

    c_standard="c2x",
    cpp_standard="c++17",

    c_include_dirs=[],
    cpp_include_dirs=[],

    enable_debug=True,
    enable_asan=False,

    opt_type=0,
    link_time_opt=False,
)

zeta_exe_raw_config = ZetaExe.Config(
    name="raw",

    verbose=True,

    build_dir=zeta_develop_dir / "ZetaExeRawBuild",

    zeta_dir=zeta_develop_dir / "Zeta",
    zeta_build_dir=zeta_raw_config.build_dir,

    target=target,

    c_standard="c2x",
    cpp_standard="c++17",

    c_include_dirs=[zeta_dir],
    cpp_include_dirs=[zeta_dir],

    enable_debug=True,
    enable_asan=False,

    opt_type=0,
    link_time_opt=False,
)


@dataclasses.dataclass
class ConfigSet:
    zeta_config: Zeta.Config
    zeta_exe_config: ZetaExe.Config


configs = {
    "debug": ConfigSet(
        zeta_config=zeta_debug_config,
        zeta_exe_config=zeta_exe_debug_config,
    ),

    "release": ConfigSet(
        zeta_config=zeta_release_config,
        zeta_exe_config=zeta_exe_release_config,
    ),

    "raw": ConfigSet(
        zeta_config=zeta_raw_config,
        zeta_exe_config=zeta_exe_raw_config,
    ),
}


def main():
    parser = argparse.ArgumentParser()

    parser.add_argument("--target",
                        dest="target",
                        action="store",
                        required=True)

    parser.add_argument("--config",
                        dest="config",
                        action="store",
                        choices=list(configs.keys()),
                        required=True)

    parser.add_argument("--run_target",
                        dest="run_target",
                        action="store_true")

    parser.add_argument("--rebuild",
                        dest="rebuild",
                        action="store_true")

    args = parser.parse_args(sys.argv[1:])

    print(f"args = {args}")

    config = configs[args.config]

    builder = Builder()

    Zeta.AddDeps(builder, config.zeta_config)
    ZetaExe.AddDeps(builder, config.zeta_exe_config)

    for unit in builder.GetUnits():
        builder.Add(unit, [FILE], None)

    builder.Add(FILE, None, None)

    builder.Check()

    target_replace_table = {
        "?zeta_build_dir": str(config.zeta_config.build_dir),
        "?zeta_exe_build_dir": str(config.zeta_exe_config.build_dir),
    }

    target = args.target

    for k, l in target_replace_table.items():
        target = target.replace(k, l)

    target = pathlib.Path(target)

    is_success, units = builder.Build(target, args.rebuild)

    status = "success" if is_success else "failed"

    print(termcolor.colored(status, "yellow"))

    for unit_state, units in units.items():
        print(termcolor.colored(f"{unit_state} build units:", "yellow"))

        for i in sorted(units):
            print(f"\t{i}")

    print(termcolor.colored(f"target: ", "yellow") + f"{target}")

    print(termcolor.colored(status, "yellow"))

    if not is_success:
        return

    if args.run_target:
        print(termcolor.colored(f"Running: ", "yellow") + f"{target}")
        os.system(str(target))


if __name__ == "__main__":
    main()
