import os
import sys

File = os.path.abspath(__file__).replace("\\", "/")
ZetaDir = os.path.dirname(File).replace("\\", "/")

ZetaDevDir = f"{ZetaDir}/../"

sys.path.append(ZetaDevDir)
from builder import *

INCLUDE_DIRS = [
    # "C:\Program Files\llvm-mingw-20231003-msvcrt-x86_64\include",
]

def builder_add(builder):
    debug = False

    cc = "gcc"

    args = [
        "--verbose",
        "-c",
        "-std=gnu17",
        *[f"-I \"{path}\"" for path in INCLUDE_DIRS],
        "-m64",
        "-Wall",
        "-Wextra",
        "-Werror",
        "-ferror-limit=3", # for clang
        # "-fmax-errors=3", # for gcc
    ]

    if debug:
        args += ["-g", "-D DEBUG"]
    else:
        args += ["-O3"]

    builder.add(
        f"{File}",
        set(),
        NullFunc
    )

    builder.add(
        f"{ZetaDir}/define.h",
        {
            f"{File}",
        },
        NullFunc
    )

    builder.add(
        f"{ZetaDir}/utils.h",
        {
            f"{File}",
            f"{ZetaDir}/define.h",
        },
        NullFunc
    )

    builder.add(
        f"{ZetaDir}/utils.c",
        {
            f"{File}",
            f"{ZetaDir}/utils.h",
        },
        NullFunc
    )

    builder.add(
        f"{ZetaDir}/utils.o",
        {
            f"{File}",
            f"{ZetaDir}/utils.c",
        },
        CmdExecutor([
            cc,
            f"-o {ZetaDir}/utils.o",
            *args,
            f"{ZetaDir}/utils.c",
        ])
    )

    builder.add(
        f"{ZetaDir}/MultiLevelEntryTable.h",
        {
            f"{File}",
            f"{ZetaDir}/define.h",
        },
        NullFunc
    )

    builder.add(
        f"{ZetaDir}/MultiLevelEntryTable.c",
        {
            f"{File}",
            f"{ZetaDir}/MultiLevelEntryTable.h",
        },
        NullFunc
    )

    builder.add(
        f"{ZetaDir}/MultiLevelEntryTable.o",
        {
            f"{File}",
            f"{ZetaDir}/MultiLevelEntryTable.c",
        },
        CmdExecutor([
            cc,
            f"-o {ZetaDir}/MultiLevelEntryTable.o",
            *args,
            f"{ZetaDir}/MultiLevelEntryTable.c",
        ])
    )

    builder.add(
        f"{ZetaDir}/MultiLevelHashTable.h",
        {
            f"{File}",
            f"{ZetaDir}/MultiLevelEntryTable.h",
        },
        NullFunc
    )

    builder.add(
        f"{ZetaDir}/MultiLevelHashTable.c",
        {
            f"{File}",
            f"{ZetaDir}/MultiLevelHashTable.h",
        },
        NullFunc
    )

    builder.add(
        f"{ZetaDir}/MultiLevelHashTable.o",
        {
            f"{File}",
            f"{ZetaDir}/MultiLevelHashTable.c",
        },
        CmdExecutor([
            cc,
            f"-o {ZetaDir}/MultiLevelHashTable.o",
            *args,
            f"{ZetaDir}/MultiLevelHashTable.c",
        ])
    )

    builder.add(
        f"{ZetaDir}/BinTree.h",
        {
            f"{File}",
            f"{ZetaDir}/define.h",
        },
        NullFunc
    )

    builder.add(
        f"{ZetaDir}/BinTree.c",
        {
            f"{File}",
            f"{ZetaDir}/BinTree.h",
            f"{ZetaDir}/utils.h",
        },
        NullFunc
    )

    builder.add(
        f"{ZetaDir}/BinTree.o",
        {
            f"{File}",
            f"{ZetaDir}/BinTree.h",
        },
        CmdExecutor([
            cc,
            f"-o {ZetaDir}/BinTree.o",
            *args,
            f"{ZetaDir}/BinTree.c",
        ])
    )

    builder.add(
        f"{ZetaDir}/CountingBinTree.h",
        {
            f"{File}",
            f"{ZetaDir}/RBTree.h",
        },
        NullFunc
    )

    builder.add(
        f"{ZetaDir}/CountingBinTree.c",
        {
            f"{File}",
            f"{ZetaDir}/CountingBinTree.h",
            f"{ZetaDir}/utils.h",
        },
        NullFunc
    )

    builder.add(
        f"{ZetaDir}/CountingBinTree.o",
        {
            f"{File}",
            f"{ZetaDir}/CountingBinTree.c",
        },
        CmdExecutor([
            cc,
            f"-o {ZetaDir}/CountingBinTree.o",
            *args,
            f"{ZetaDir}/CountingBinTree.c",
        ])
    )

    builder.add(
        f"{ZetaDir}/RBTree.h",
        {
            f"{File}",
            f"{ZetaDir}/BinTree.h",
        },
        NullFunc
    )

    builder.add(
        f"{ZetaDir}/RBTree.c",
        {
            f"{File}",
            f"{ZetaDir}/RBTree.h",
            f"{ZetaDir}/utils.h",
        },
        NullFunc
    )

    builder.add(
        f"{ZetaDir}/RBTree.o",
        {
            f"{File}",
            f"{ZetaDir}/RBTree.c",
        },
        CmdExecutor([
            cc,
            f"-o {ZetaDir}/RBTree.o",
            *args,
            f"{ZetaDir}/RBTree.c",
        ])
    )

    builder.add(
        f"{ZetaDir}/OrdinaryCountingBinColoredTreeNode.h",
        {
            f"{File}",
            f"{ZetaDir}/define.h",
        },
        NullFunc
    )

    builder.add(
        f"{ZetaDir}/OrdinaryCountingBinColoredTreeNode.c",
        {
            f"{File}",
            f"{ZetaDir}/OrdinaryCountingBinColoredTreeNode.h",
        },
        NullFunc
    )

    builder.add(
        f"{ZetaDir}/OrdinaryCountingBinColoredTreeNode.o",
        {
            f"{File}",
            f"{ZetaDir}/OrdinaryCountingBinColoredTreeNode.c",
        },
        CmdExecutor([
            cc,
            f"-o {ZetaDir}/OrdinaryCountingBinColoredTreeNode.o",
            *args,
            f"{ZetaDir}/OrdinaryCountingBinColoredTreeNode.c",
        ])
    )

    builder.add(
        f"{ZetaDir}/random.h",
        {
            f"{File}",
            f"{ZetaDir}/define.h",
        },
        NullFunc
    )

    builder.add(
        f"{ZetaDir}/random.c",
        {
            f"{File}",
            f"{ZetaDir}/random.h",
        },
        NullFunc
    )

    builder.add(
        f"{ZetaDir}/random.o",
        {
            f"{File}",
            f"{ZetaDir}/random.c",
        },
        CmdExecutor([
            cc,
            f"-o {ZetaDir}/random.o",
            *args,
            f"{ZetaDir}/random.c",
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
