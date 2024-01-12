import os
import sys

File = os.path.abspath(__file__).replace("\\", "/")
ZetaExeDir = os.path.dirname(File).replace("\\", "/")

ZetaDevDir = f"{ZetaExeDir}/../"

import argparse

sys.path.append(ZetaDevDir)
from builder import *

ZetaDir = os.path.abspath(f"{ZetaDevDir}/Zeta").replace("\\", "/")

sys.path.append(os.path.dirname(ZetaDir))
import Zeta.build

INCLUDE_DIRS = [
]

def builder_add(builder):
    debug = True

    cc = "clang"

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
        lambda : 0
    )

    builder.add(
        f"{ZetaExeDir}/test_head.h",
        {
            f"{ZetaDir}/define.h",
        },
        lambda : 0
    )

    builder.add(
        f"{ZetaExeDir}/SimpleVector.h",
        {
            f"{ZetaExeDir}/test_head.h",
        },
        lambda : 0
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
        lambda : 0
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
        lambda : os.system(" ".join([
            cc,
            f"-o {ZetaExeDir}/test_1.exe",
            *args,
            f"{ZetaDir}/CountingBinTree.o",
            f"{ZetaDir}/RBTree.o",
            f"{ZetaDir}/OrdinaryCountingBinColoredTreeNode.o",
            f"{ZetaDir}/random.o",
            f"{ZetaDir}/utils.o",
            f"{ZetaExeDir}/test_1.c",
        ]))
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
        lambda : 0
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
        lambda : os.system(" ".join([
            cc,
            f"-o {ZetaExeDir}/test_gplist.exe",
            *args,
            f"{ZetaDir}/CountingBinTree.o",
            f"{ZetaDir}/OrdinaryCountingBinColoredTreeNode.o",
            f"{ZetaDir}/RBTree.o",
            f"{ZetaDir}/utils.o",
            f"{ZetaExeDir}/test_gplist.c",
        ]))
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
        lambda : os.system(" ".join([
            cc,
            f"-o {ZetaExeDir}/test_gplist.so",
            *args,
            f"-shared",
            f"-fPIC",
            f"{ZetaDir}/CountingBinTree.o",
            f"{ZetaDir}/OrdinaryCountingBinColoredTreeNode.o",
            f"{ZetaDir}/RBTree.o",
            f"{ZetaDir}/utils.o",
            f"{ZetaExeDir}/test_gplist.c",
        ]))
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
        lambda : 0
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
        lambda : os.system(" ".join([
            cc,
            f"-o {ZetaExeDir}/gplist.so",
            *args,
            f"-shared",
            f"-fPIC",
            f"{ZetaDir}/BinTree.o",
            f"{ZetaDir}/CountingBinTree.o",
            f"{ZetaDir}/OrdinaryCountingBinColoredTreeNode.o",
            f"{ZetaDir}/RBTree.o",
            f"{ZetaDir}/utils.o",
            f"{ZetaExeDir}/gplist.c",
        ]))
    )

    builder.add(
        f"{ZetaExeDir}/mlmap.c",
        {
            f"{File}",
            f"{ZetaDir}/MultiLevelHashTable.h",
            f"{ZetaDir}/utils.h",
        },
        lambda : 0
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
        lambda : os.system(" ".join([
            cc,
            f"-o {ZetaExeDir}/mlmap.so",
            *args,
            f"-shared",
            f"-fPIC",
            f"{ZetaDir}/MultiLevelEntryTable.o",
            f"{ZetaDir}/MultiLevelHashTable.o",
            f"{ZetaDir}/utils.o",
            f"{ZetaExeDir}/mlmap.c",
        ]))
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
        lambda : 0
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
        lambda : os.system(" ".join([
            cc,
            f"-o {ZetaExeDir}/test_hashtable.exe",
            *args,
            f"{ZetaDir}/MultiLevelEntryTable.o",
            f"{ZetaDir}/MultiLevelHashTable.o",
            f"{ZetaDir}/utils.o",
            f"{ZetaExeDir}/test_hashtable.c",
        ]))
    )

    builder.add(
        f"{ZetaExeDir}/test_2.c",
        {
            f"{File}",
        },
        lambda : 0
    )

    builder.add(
        f"{ZetaExeDir}/test_2.exe",
        {
            f"{File}",
            f"{ZetaExeDir}/test_2.c",
        },
        lambda : os.system(" ".join([
            cc,
            f"-o {ZetaExeDir}/test_2.exe",
            *args,
            "-stdlib=libc",
            f"{ZetaExeDir}/test_2.c",
        ]))
    )

    builder.add(
        f"{ZetaExeDir}/FileIO.s",
        {
            f"{File}",
        },
        lambda : 0
    )

    builder.add(
        f"{ZetaExeDir}/test_io.c",
        {
            f"{File}",
        },
        lambda : 0
    )

    builder.add(
        f"{ZetaExeDir}/test_io.exe",
        {
            f"{File}",
            f"{ZetaExeDir}/FileIO.s",
            f"{ZetaExeDir}/test_io.c",
        },
        lambda : os.system(" ".join([
            cc,
            f"-o {ZetaExeDir}/test_io.exe",
            *args,
            f"{ZetaExeDir}/FileIO.s",
            f"{ZetaExeDir}/test_io.c",
        ]))
    )

    builder.add(
        f"{ZetaExeDir}/test_sort.c",
        {
            f"{File}",
            f"{ZetaDir}/Algorithm.h",
        },
        lambda : 0
    )

    builder.add(
        f"{ZetaExeDir}/test_sort.exe",
        {
            f"{File}",
            f"{ZetaDir}/utils.o",
            f"{ZetaDir}/Algorithm.o",
            f"{ZetaDir}/RawVector.o",
            f"{ZetaDir}/random.o",
            f"{ZetaExeDir}/test_sort.c",
        },
        lambda : os.system(" ".join([
            cc,
            f"-o {ZetaExeDir}/test_sort.exe",
            *args,
            f"{ZetaDir}/utils.o",
            f"{ZetaDir}/Algorithm.o",
            f"{ZetaDir}/RawVector.o",
            f"{ZetaDir}/random.o",
            f"{ZetaExeDir}/test_sort.c",
        ]))
    )

    builder.add(
        f"{ZetaExeDir}/test_sort.so",
        {
            f"{File}",
            f"{ZetaDir}/utils.o",
            f"{ZetaDir}/Algorithm.o",
            f"{ZetaDir}/RawVector.o",
            f"{ZetaDir}/random.o",
            f"{ZetaExeDir}/test_sort.c",
        },
        lambda : os.system(" ".join([
            cc,
            f"-o {ZetaExeDir}/test_sort.so",
            *args,
            f"-shared",
            f"-fPIC",
            f"{ZetaDir}/utils.o",
            f"{ZetaDir}/Algorithm.o",
            f"{ZetaDir}/RawVector.o",
            f"{ZetaDir}/random.o",
            f"{ZetaExeDir}/test_sort.c",
        ]))
    )

    builder.add(
        f"{ZetaExeDir}/test_slab.c",
        {
            f"{File}",
            f"{ZetaDir}/Algorithm.h",
            f"{ZetaDir}/PoolAllocator.h",
            f"{ZetaDir}/RawVector.h",
            f"{ZetaDir}/SlabAllocator.h",
            f"{ZetaDir}/random.h",
        },
        lambda : 0
    )

    builder.add(
        f"{ZetaExeDir}/test_slab.exe",
        {
            f"{File}",
            f"{ZetaDir}/utils.o",
            f"{ZetaDir}/Algorithm.o",
            f"{ZetaDir}/DoublyLinkedNode.o",
            f"{ZetaDir}/PoolAllocator.o",
            f"{ZetaDir}/RawVector.o",
            f"{ZetaDir}/random.o",
            f"{ZetaDir}/SlabAllocator.o",
            f"{ZetaExeDir}/test_slab.c",
        },
        lambda : os.system(" ".join([
            cc,
            f"-o {ZetaExeDir}/test_slab.exe",
            *args,
            f"{ZetaDir}/utils.o",
            f"{ZetaDir}/Algorithm.o",
            f"{ZetaDir}/DoublyLinkedNode.o",
            f"{ZetaDir}/PoolAllocator.o",
            f"{ZetaDir}/RawVector.o",
            f"{ZetaDir}/random.o",
            f"{ZetaDir}/SlabAllocator.o",
            f"{ZetaExeDir}/test_slab.c",
        ]))
    )

    builder.add(
        f"{ZetaExeDir}/switch.s",
        {
            f"{File}",
        },
        lambda : 0
    )

    builder.add(
        f"{ZetaExeDir}/test_context.c",
        {
            f"{File}",
            f"{ZetaDir}/define.h",
        },
        lambda : 0
    )

    builder.add(
        f"{ZetaExeDir}/test_context.exe",
        {
            f"{File}",
            f"{ZetaExeDir}/switch.s",
            f"{ZetaExeDir}/test_context.c",
        },
        lambda : os.system(" ".join([
            cc,
            f"-o {ZetaExeDir}/test_context.exe",
            *args,
            f"{ZetaExeDir}/switch.s",
            f"{ZetaExeDir}/test_context.c",
        ]))
    )

    builder.add(
        f"{ZetaExeDir}/test_context.s",
        {
            f"{File}",
            f"{ZetaExeDir}/switch.s",
            f"{ZetaExeDir}/test_context.c",
        },
        lambda : os.system(" ".join([
            cc,
            f"-o {ZetaExeDir}/test_context.s",
            f"-S",
            *args,
            f"{ZetaExeDir}/switch.s",
            f"{ZetaExeDir}/test_context.c",
        ]))
    )

if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("target", action="store")
    parser.add_argument("--rebuild", dest="rebuild", action="store_true")
    args = parser.parse_args(sys.argv[1:])

    print(f"args = {args}")

    builder = Builder()
    builder_add(builder)
    builder.add("all", builder.units(), lambda : 0)

    non_built, built = builder.build(GetABSPath(args.target), args.rebuild)

    print("success")
    print("non_built: ", non_built)
    print("built: ", built)
