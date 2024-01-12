import os
import sys

File = os.path.abspath(__file__).replace("\\", "/")
ZetaDir = os.path.dirname(File).replace("\\", "/")

ZetaDevDir = f"{ZetaDir}/../"

import argparse

sys.path.append(ZetaDevDir)
from builder import *

INCLUDE_DIRS = [
    # "C:\Program Files\llvm-mingw-20231003-msvcrt-x86_64\include",
]

def builder_add(builder):
    debug = True

    cc = "clang"

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
        lambda : 0
    )

    builder.add(
        f"{ZetaDir}/Algorithm.h",
        {
            f"{File}",
            f"{ZetaDir}/define.h",
        },
        lambda : 0
    )

    builder.add(
        f"{ZetaDir}/Algorithm.c",
        {
            f"{File}",
            f"{ZetaDir}/Algorithm.h",
        },
        lambda : 0
    )

    builder.add(
        f"{ZetaDir}/Algorithm.o",
        {
            f"{File}",
            f"{ZetaDir}/Algorithm.c",
        },
        lambda : os.system(" ".join([
            cc,
            f"-o {ZetaDir}/Algorithm.o",
            *args,
            f"{ZetaDir}/Algorithm.c",
        ]))
    )

    builder.add(
        f"{ZetaDir}/Allocator.h",
        {
            f"{File}",
            f"{ZetaDir}/define.h",
        },
        lambda : 0
    )

    builder.add(
        f"{ZetaDir}/BinTree.h",
        {
            f"{File}",
            f"{ZetaDir}/define.h",
        },
        lambda : 0
    )

    builder.add(
        f"{ZetaDir}/BinTree.c",
        {
            f"{File}",
            f"{ZetaDir}/BinTree.h",
            f"{ZetaDir}/utils.h",
        },
        lambda : 0
    )

    builder.add(
        f"{ZetaDir}/BinTree.o",
        {
            f"{File}",
            f"{ZetaDir}/BinTree.h",
        },
        lambda : os.system(" ".join([
            cc,
            f"-o {ZetaDir}/BinTree.o",
            *args,
            f"{ZetaDir}/BinTree.c",
        ]))
    )

    builder.add(
        f"{ZetaDir}/CircularVector.h",
        {
            f"{File}",
            f"{ZetaDir}/Vector.h",
        },
        lambda : 0
    )

    builder.add(
        f"{ZetaDir}/CircularVector.c",
        {
            f"{File}",
            f"{ZetaDir}/CircularVector.h",
        },
        lambda : 0
    )

    builder.add(
        f"{ZetaDir}/CircularVector.o",
        {
            f"{File}",
            f"{ZetaDir}/CircularVector.c",
        },
        lambda : os.system(" ".join([
            cc,
            f"-o {ZetaDir}/CircularVector.o",
            *args,
            f"{ZetaDir}/CircularVector.c",
        ]))
    )

    builder.add(
        f"{ZetaDir}/CountingBinTree.h",
        {
            f"{File}",
            f"{ZetaDir}/RBTree.h",
        },
        lambda : 0
    )

    builder.add(
        f"{ZetaDir}/CountingBinTree.c",
        {
            f"{File}",
            f"{ZetaDir}/CountingBinTree.h",
            f"{ZetaDir}/utils.h",
        },
        lambda : 0
    )

    builder.add(
        f"{ZetaDir}/CountingBinTree.o",
        {
            f"{File}",
            f"{ZetaDir}/CountingBinTree.c",
        },
        lambda : os.system(" ".join([
            cc,
            f"-o {ZetaDir}/CountingBinTree.o",
            *args,
            f"{ZetaDir}/CountingBinTree.c",
        ]))
    )

    builder.add(
        f"{ZetaDir}/define.h",
        {
            f"{File}",
        },
        lambda : 0
    )

    builder.add(
        f"{ZetaDir}/DoublyLinkedNode.h",
        {
            f"{File}",
            f"{ZetaDir}/define.h",
        },
        lambda : 0
    )

    builder.add(
        f"{ZetaDir}/DoublyLinkedNode.c",
        {
            f"{File}",
            f"{ZetaDir}/DoublyLinkedNode.h",
        },
        lambda : 0
    )

    builder.add(
        f"{ZetaDir}/DoublyLinkedNode.o",
        {
            f"{File}",
            f"{ZetaDir}/DoublyLinkedNode.c",
        },
        lambda : os.system(" ".join([
            cc,
            f"-o {ZetaDir}/DoublyLinkedNode.o",
            *args,
            f"{ZetaDir}/DoublyLinkedNode.c",
        ]))
    )

    builder.add(
        f"{ZetaDir}/DynamicVector.h",
        {
            f"{File}",
            f"{ZetaDir}/Allocator.h",
            f"{ZetaDir}/CircularVector.h",
            f"{ZetaDir}/MultiLevelEntryTable.h",
            f"{ZetaDir}/Vector.h",
        },
        lambda : 0
    )

    builder.add(
        f"{ZetaDir}/DynamicVector.c",
        {
            f"{File}",
            f"{ZetaDir}/DynamicVector.h",
        },
        lambda : 0
    )

    builder.add(
        f"{ZetaDir}/DynamicVector.o",
        {
            f"{File}",
            f"{ZetaDir}/DynamicVector.h",
        },
        lambda : os.system(" ".join([
            cc,
            f"-o {ZetaDir}/DynamicVector.o",
            *args,
            f"{ZetaDir}/DynamicVector.c",
        ]))
    )

    builder.add(
        f"{ZetaDir}/MultiLevelEntryTable.h",
        {
            f"{File}",
            f"{ZetaDir}/define.h",
        },
        lambda : 0
    )

    builder.add(
        f"{ZetaDir}/MultiLevelEntryTable.c",
        {
            f"{File}",
            f"{ZetaDir}/MultiLevelEntryTable.h",
        },
        lambda : 0
    )

    builder.add(
        f"{ZetaDir}/MultiLevelEntryTable.o",
        {
            f"{File}",
            f"{ZetaDir}/MultiLevelEntryTable.c",
        },
        lambda : os.system(" ".join([
            cc,
            f"-o {ZetaDir}/MultiLevelEntryTable.o",
            *args,
            f"{ZetaDir}/MultiLevelEntryTable.c",
        ]))
    )

    builder.add(
        f"{ZetaDir}/MultiLevelHashTable.h",
        {
            f"{File}",
            f"{ZetaDir}/MultiLevelEntryTable.h",
        },
        lambda : 0
    )

    builder.add(
        f"{ZetaDir}/MultiLevelHashTable.c",
        {
            f"{File}",
            f"{ZetaDir}/MultiLevelHashTable.h",
        },
        lambda : 0
    )

    builder.add(
        f"{ZetaDir}/MultiLevelHashTable.o",
        {
            f"{File}",
            f"{ZetaDir}/MultiLevelHashTable.c",
        },
        lambda : os.system(" ".join([
            cc,
            f"-o {ZetaDir}/MultiLevelHashTable.o",
            *args,
            f"{ZetaDir}/MultiLevelHashTable.c",
        ]))
    )

    builder.add(
        f"{ZetaDir}/OrdinaryBinColoredTreeNode.h",
        {
            f"{File}",
            f"{ZetaDir}/define.h",
        },
        lambda : 0
    )

    builder.add(
        f"{ZetaDir}/OrdinaryBinColoredTreeNode.c",
        {
            f"{File}",
            f"{ZetaDir}/OrdinaryBinColoredTreeNode.h",
        },
        lambda : 0
    )

    builder.add(
        f"{ZetaDir}/OrdinaryBinColoredTreeNode.o",
        {
            f"{File}",
            f"{ZetaDir}/OrdinaryBinColoredTreeNode.c",
        },
        lambda : os.system(" ".join([
            cc,
            f"-o {ZetaDir}/OrdinaryBinColoredTreeNode.o",
            *args,
            f"{ZetaDir}/OrdinaryBinColoredTreeNode.c",
        ]))
    )

    builder.add(
        f"{ZetaDir}/OrdinaryCountingBinColoredTreeNode.h",
        {
            f"{File}",
            f"{ZetaDir}/define.h",
        },
        lambda : 0
    )

    builder.add(
        f"{ZetaDir}/OrdinaryCountingBinColoredTreeNode.c",
        {
            f"{File}",
            f"{ZetaDir}/OrdinaryCountingBinColoredTreeNode.h",
        },
        lambda : 0
    )

    builder.add(
        f"{ZetaDir}/OrdinaryCountingBinColoredTreeNode.o",
        {
            f"{File}",
            f"{ZetaDir}/OrdinaryCountingBinColoredTreeNode.c",
        },
        lambda : os.system(" ".join([
            cc,
            f"-o {ZetaDir}/OrdinaryCountingBinColoredTreeNode.o",
            *args,
            f"{ZetaDir}/OrdinaryCountingBinColoredTreeNode.c",
        ]))
    )

    builder.add(
        f"{ZetaDir}/PoolAllocator.h",
        {
            f"{File}",
            f"{ZetaDir}/Allocator.h",
        },
        lambda : 0
    )

    builder.add(
        f"{ZetaDir}/PoolAllocator.c",
        {
            f"{File}",
            f"{ZetaDir}/PoolAllocator.h",
        },
        lambda : 0
    )

    builder.add(
        f"{ZetaDir}/PoolAllocator.o",
        {
            f"{File}",
            f"{ZetaDir}/PoolAllocator.c",
        },
        lambda : os.system(" ".join([
            cc,
            f"-o {ZetaDir}/PoolAllocator.o",
            *args,
            f"{ZetaDir}/PoolAllocator.c",
        ]))
    )

    builder.add(
        f"{ZetaDir}/random.h",
        {
            f"{File}",
            f"{ZetaDir}/define.h",
        },
        lambda : 0
    )

    builder.add(
        f"{ZetaDir}/random.c",
        {
            f"{File}",
            f"{ZetaDir}/random.h",
        },
        lambda : 0
    )

    builder.add(
        f"{ZetaDir}/random.o",
        {
            f"{File}",
            f"{ZetaDir}/random.c",
        },
        lambda : os.system(" ".join([
            cc,
            f"-o {ZetaDir}/random.o",
            *args,
            f"{ZetaDir}/random.c",
        ]))
    )

    builder.add(
        f"{ZetaDir}/RawVector.h",
        {
            f"{File}",
            f"{ZetaDir}/Vector.h",
        },
        lambda : 0
    )

    builder.add(
        f"{ZetaDir}/RawVector.c",
        {
            f"{File}",
            f"{ZetaDir}/RawVector.h",
        },
        lambda : 0
    )

    builder.add(
        f"{ZetaDir}/RawVector.o",
        {
            f"{File}",
            f"{ZetaDir}/RawVector.c",
        },
        lambda : os.system(" ".join([
            cc,
            f"-o {ZetaDir}/RawVector.o",
            *args,
            f"{ZetaDir}/RawVector.c",
        ]))
    )

    builder.add(
        f"{ZetaDir}/RBTree.h",
        {
            f"{File}",
            f"{ZetaDir}/BinTree.h",
        },
        lambda : 0
    )

    builder.add(
        f"{ZetaDir}/RBTree.c",
        {
            f"{File}",
            f"{ZetaDir}/RBTree.h",
            f"{ZetaDir}/utils.h",
        },
        lambda : 0
    )

    builder.add(
        f"{ZetaDir}/RBTree.o",
        {
            f"{File}",
            f"{ZetaDir}/RBTree.c",
        },
        lambda : os.system(" ".join([
            cc,
            f"-o {ZetaDir}/RBTree.o",
            *args,
            f"{ZetaDir}/RBTree.c",
        ]))
    )

    builder.add(
        f"{ZetaDir}/SlabAllocator.h",
        {
            f"{File}",
            f"{ZetaDir}/Allocator.h",
            f"{ZetaDir}/DoublyLinkedNode.h",
        },
        lambda : 0
    )

    builder.add(
        f"{ZetaDir}/SlabAllocator.c",
        {
            f"{File}",
            f"{ZetaDir}/SlabAllocator.h",
        },
        lambda : 0
    )

    builder.add(
        f"{ZetaDir}/SlabAllocator.o",
        {
            f"{File}",
            f"{ZetaDir}/SlabAllocator.c",
        },
        lambda : os.system(" ".join([
            cc,
            f"-o {ZetaDir}/SlabAllocator.o",
            *args,
            f"{ZetaDir}/SlabAllocator.c",
        ]))
    )

    builder.add(
        f"{ZetaDir}/utils.h",
        {
            f"{File}",
            f"{ZetaDir}/define.h",
        },
        lambda : 0
    )

    builder.add(
        f"{ZetaDir}/utils.c",
        {
            f"{File}",
            f"{ZetaDir}/utils.h",
        },
        lambda : 0
    )

    builder.add(
        f"{ZetaDir}/utils.o",
        {
            f"{File}",
            f"{ZetaDir}/utils.c",
        },
        lambda : os.system(" ".join([
            cc,
            f"-o {ZetaDir}/utils.o",
            *args,
            f"{ZetaDir}/utils.c",
        ]))
    )

    builder.add(
        f"{ZetaDir}/Vector.h",
        {
            f"{File}",
            f"{ZetaDir}/define.h",
        },
        lambda : 0
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
