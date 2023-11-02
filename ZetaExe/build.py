import os
import sys

File = os.path.abspath(__file__).replace("\\", "/")
ZetaExeDir = os.path.dirname(File).replace("\\", "/")

ZetaDevDir = f"{ZetaExeDir}/../"

sys.path.append(ZetaDevDir)
from builder import *

ZetaDir = os.path.abspath(f"{ZetaDevDir}/Zeta").replace("\\", "/")

sys.path.append(os.path.dirname(ZetaDir))
import Zeta.build

INCLUDE_DIRS = [
]

def builder_add(builder):
    debug = False

    cc = "gcc"

    args = [
            "--verbose",
            "-std=gnu17",
            *[f"-I \"{path}\"" for path in INCLUDE_DIRS],
            "-m64",
            "-Wall",
            "-Wextra",
            "-ferror-limit=3", # for clang
            # "-fmax-errors=3", # for gcc
            ]

    if debug:
        args += ["-g", "-D DEBUG"]
    else:
        args += ["-O3"]

    Zeta.build.builder_add(builder)

    builder.add(
        f"{File}",
        set(),
        NullFunc
    )

    builder.add(
        f"{ZetaExeDir}/test_head.h",
        {
            f"{ZetaDir}/define.h",
        },
        NullFunc
    )

    builder.add(
        f"{ZetaExeDir}/SimpleVector.h",
        {
            f"{ZetaExeDir}/test_head.h",
        },
        NullFunc
    )

    builder.add(
        f"{ZetaExeDir}/test_1.c",
        {
            f"{File}",
            f"{ZetaDir}/CountingBinTree.h",
            f"{ZetaDir}/OrdinaryCountingBinColoredTreeNode.h",
            f"{ZetaDir}/RBTree.h",
            f"{ZetaDir}/random.h",
            f"{ZetaDir}/utils.h",
            f"{ZetaExeDir}/SimpleVector.h",
            f"{ZetaExeDir}/test_head.h",
        },
        NullFunc
    )

    builder.add(
        f"{ZetaExeDir}/test_1.exe",
        {
            f"{File}",
            f"{ZetaDir}/CountingBinTree.o",
            f"{ZetaDir}/RBTree.o",
            f"{ZetaDir}/OrdinaryCountingBinColoredTreeNode.o",
            f"{ZetaDir}/random.o",
            f"{ZetaDir}/utils.o",
            f"{ZetaExeDir}/test_1.c",
        },
        CmdExecutor([
            cc,
            f"-o {ZetaExeDir}/test_1.exe",
            *args,
            f"{ZetaDir}/CountingBinTree.o",
            f"{ZetaDir}/RBTree.o",
            f"{ZetaDir}/OrdinaryCountingBinColoredTreeNode.o",
            f"{ZetaDir}/random.o",
            f"{ZetaDir}/utils.o",
            f"{ZetaExeDir}/test_1.c",
        ])
    )

    builder.add(
        f"{ZetaExeDir}/test_gplist.c",
        {
            f"{File}",
            f"{ZetaDir}/CountingBinTree.h",
            f"{ZetaDir}/OrdinaryCountingBinColoredTreeNode.h",
            f"{ZetaDir}/RBTree.h",
            f"{ZetaDir}/utils.h",
            f"{ZetaExeDir}/SimpleVector.h",
            f"{ZetaExeDir}/test_head.h",
        },
        NullFunc
    )

    builder.add(
        f"{ZetaExeDir}/test_gplist.exe",
        {
            f"{File}",
            f"{ZetaDir}/CountingBinTree.o",
            f"{ZetaDir}/OrdinaryCountingBinColoredTreeNode.o",
            f"{ZetaDir}/RBTree.o",
            f"{ZetaDir}/utils.o",
            f"{ZetaExeDir}/test_gplist.c",
        },
        CmdExecutor([
            cc,
            f"-o {ZetaExeDir}/test_gplist.exe",
            *args,
            f"{ZetaDir}/CountingBinTree.o",
            f"{ZetaDir}/OrdinaryCountingBinColoredTreeNode.o",
            f"{ZetaDir}/RBTree.o",
            f"{ZetaDir}/utils.o",
            f"{ZetaExeDir}/test_gplist.c",
        ])
    )

    builder.add(
        f"{ZetaExeDir}/test_gplist.so",
        {
            f"{File}",
            f"{ZetaDir}/CountingBinTree.o",
            f"{ZetaDir}/OrdinaryCountingBinColoredTreeNode.o",
            f"{ZetaDir}/RBTree.o",
            f"{ZetaDir}/utils.o",
            f"{ZetaExeDir}/test_gplist.c",
        },
        CmdExecutor([
            cc,
            f"-o {ZetaExeDir}/test_gplist.so",
            *args,
            f"-shared",
            f"{ZetaDir}/CountingBinTree.o",
            f"{ZetaDir}/OrdinaryCountingBinColoredTreeNode.o",
            f"{ZetaDir}/RBTree.o",
            f"{ZetaDir}/utils.o",
            f"{ZetaExeDir}/test_gplist.c",
        ])
    )

    builder.add(
        f"{ZetaExeDir}/gplist.c",
        {
            f"{File}",
            f"{ZetaDir}/CountingBinTree.h",
            f"{ZetaDir}/OrdinaryCountingBinColoredTreeNode.h",
            f"{ZetaDir}/RBTree.h",
            f"{ZetaDir}/utils.h",
        },
        NullFunc
    )

    builder.add(
        f"{ZetaExeDir}/gplist.so",
        {
            f"{File}",
            f"{ZetaDir}/BinTree.o",
            f"{ZetaDir}/CountingBinTree.o",
            f"{ZetaDir}/OrdinaryCountingBinColoredTreeNode.o",
            f"{ZetaDir}/RBTree.o",
            f"{ZetaDir}/utils.o",
            f"{ZetaExeDir}/gplist.c",
        },
        CmdExecutor([
            cc,
            f"-o {ZetaExeDir}/gplist.so",
            *args,
            f"-shared",
            f"{ZetaDir}/BinTree.o",
            f"{ZetaDir}/CountingBinTree.o",
            f"{ZetaDir}/OrdinaryCountingBinColoredTreeNode.o",
            f"{ZetaDir}/RBTree.o",
            f"{ZetaDir}/utils.o",
            f"{ZetaExeDir}/gplist.c",
        ])
    )

    builder.add(
        f"{ZetaExeDir}/mlmap.c",
        {
            f"{File}",
            f"{ZetaDir}/MultiLevelHashTable.h",
            f"{ZetaDir}/utils.h",
        },
        NullFunc
    )

    builder.add(
        f"{ZetaExeDir}/mlmap.so",
        {
            f"{File}",
            f"{ZetaDir}/MultiLevelEntryTable.o",
            f"{ZetaDir}/MultiLevelHashTable.o",
            f"{ZetaDir}/utils.o",
            f"{ZetaExeDir}/mlmap.c",
        },
        CmdExecutor([
            cc,
            f"-o {ZetaExeDir}/mlmap.so",
            *args,
            f"-shared",
            f"{ZetaDir}/MultiLevelEntryTable.o",
            f"{ZetaDir}/MultiLevelHashTable.o",
            f"{ZetaDir}/utils.o",
            f"{ZetaExeDir}/mlmap.c",
        ])
    )

    builder.add(
        f"{ZetaExeDir}/test_hashtable.c",
        {
            f"{File}",
            f"{ZetaDir}/MultiLevelHashTable.h",
            f"{ZetaDir}/utils.h",
            f"{ZetaExeDir}/SimpleVector.h",
            f"{ZetaExeDir}/test_head.h",
        },
        NullFunc
    )

    builder.add(
        f"{ZetaExeDir}/test_hashtable.exe",
        {
            f"{File}",
            f"{ZetaDir}/MultiLevelEntryTable.o",
            f"{ZetaDir}/MultiLevelHashTable.o",
            f"{ZetaDir}/utils.o",
            f"{ZetaExeDir}/test_hashtable.c",
        },
        CmdExecutor([
            cc,
            f"-o {ZetaExeDir}/test_hashtable.exe",
            *args,
            f"{ZetaDir}/MultiLevelEntryTable.o",
            f"{ZetaDir}/MultiLevelHashTable.o",
            f"{ZetaDir}/utils.o",
            f"{ZetaExeDir}/test_hashtable.c",
        ])
    )

    builder.add(
        f"{ZetaExeDir}/test_2.c",
        {
            f"{File}",
        },
        NullFunc
    )

    builder.add(
        f"{ZetaExeDir}/test_2.exe",
        {
            f"{File}",
            f"{ZetaExeDir}/test_2.c",
        },
        CmdExecutor([
            cc,
            f"-o {ZetaExeDir}/test_2.exe",
            *args,
            "-stdlib=libc",
            f"{ZetaExeDir}/test_2.c",
        ])
    )

if __name__ == "__main__":
    builder = Builder()
    builder_add(builder)
    builder.add("all", builder.units(), NullFunc)

    non_built, built = builder.build(GetABSPath(sys.argv[1]))

    print("success")
    print("non_built: ", non_built)
    print("built: ", built)
