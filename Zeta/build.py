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

    cppc = "clang++"

    cargs = [
        "--verbose",
        "-std=gnu17",
        *[f"-I \"{path}\"" for path in INCLUDE_DIRS],
        "-m64",
        "-Wall",
        "-Wextra",
        "-Werror",
        "-ferror-limit=3", # for clang
        # "-fmax-errors=3", # for gcc
    ]

    cppargs = [
        "--verbose",
        "-std=c++17",
        *[f"-I \"{path}\"" for path in INCLUDE_DIRS],
        "-m64",
        "-Wall",
        "-Wextra",
        "-Werror",
        "-ferror-limit=3", # for clang
        # "-fmax-errors=3", # for gcc
    ]

    if debug:
        cargs += ["-g", "-D DEBUG"]

        cppargs += ["-g", "-D DEBUG"]
    else:
        cargs += ["-O3"]

        cppargs += ["-O3"]

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
            f"--output {ZetaDir}/Algorithm.o",
            f"-c",
            *cargs,
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
            f"--output {ZetaDir}/BinTree.o",
            f"-c",
            *cargs,
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
            f"--output {ZetaDir}/CircularVector.o",
            f"-c",
            *cargs,
            f"{ZetaDir}/CircularVector.c",
        ]))
    )

    builder.add(
        f"{ZetaDir}/CntBinTree.h",
        {
            f"{File}",
            f"{ZetaDir}/define.h",
        },
        lambda : 0
    )

    builder.add(
        f"{ZetaDir}/CntBinTree.c",
        {
            f"{File}",
            f"{ZetaDir}/CntBinTree.h",
            f"{ZetaDir}/utils.h",
        },
        lambda : 0
    )

    builder.add(
        f"{ZetaDir}/CntBinTree.o",
        {
            f"{File}",
            f"{ZetaDir}/CntBinTree.c",
        },
        lambda : os.system(" ".join([
            cc,
            f"--output {ZetaDir}/CntBinTree.o",
            f"-c",
            *cargs,
            f"{ZetaDir}/CntBinTree.c",
        ]))
    )

    builder.add(
        f"{ZetaDir}/CRC.h",
        {
            f"{File}",
            f"{ZetaDir}/define.h",
        },
        lambda : 0
    )

    builder.add(
        f"{ZetaDir}/CRC.c",
        {
            f"{File}",
            f"{ZetaDir}/CRC.h",
        },
        lambda : 0
    )

    builder.add(
        f"{ZetaDir}/CRC.o",
        {
            f"{File}",
            f"{ZetaDir}/CRC.c",
        },
        lambda : os.system(" ".join([
            cc,
            f"--output {ZetaDir}/CRC.o",
            f"-c",
            *cargs,
            f"{ZetaDir}/CRC.c",
        ]))
    )

    builder.add(
        f"{ZetaDir}/DebugHashTable.h",
        {
            f"{File}",
            f"{ZetaDir}/define.h",
        },
        lambda : 0
    )

    builder.add(
        f"{ZetaDir}/DebugHashTable.cpp",
        {
            f"{File}",
            f"{ZetaDir}/DebugHashTable.h",
        },
        lambda : 0
    )

    builder.add(
        f"{ZetaDir}/DebugHashTable.o",
        {
            f"{File}",
            f"{ZetaDir}/DebugHashTable.cpp",
        },
        lambda : os.system(" ".join([
            cppc,
            f"-o {ZetaDir}/DebugHashTable.o",
            *cppargs,
            f"-c",
            f"-fPIC",
            f"{ZetaDir}/DebugHashTable.cpp",
        ]))
    )

    builder.add(
        f"{ZetaDir}/DebugHashTable.a",
        {
            f"{File}",
            f"{ZetaDir}/DebugHashTable.cpp",
        },
        lambda : os.system(" ".join([
            cppc,
            f"--output {ZetaDir}/DebugHashTable.a",
            f"-c",
            *cppargs,

            f"-static",
            f"-fPIC",

            f"{ZetaDir}/DebugHashTable.cpp",
        ]))
    )

    builder.add(
        f"{ZetaDir}/DebugTreeMap.h",
        {
            f"{File}",
            f"{ZetaDir}/define.h",
        },
        lambda : 0
    )

    builder.add(
        f"{ZetaDir}/DebugTreeMap.cpp",
        {
            f"{File}",
            f"{ZetaDir}/DebugTreeMap.h",
        },
        lambda : 0
    )

    builder.add(
        f"{ZetaDir}/DebugTreeMap.o",
        {
            f"{File}",
            f"{ZetaDir}/DebugTreeMap.cpp",
        },
        lambda : os.system(" ".join([
            cppc,
            f"-o {ZetaDir}/DebugTreeMap.o",
            *cppargs,
            f"-c",
            f"-fPIC",
            f"{ZetaDir}/DebugTreeMap.cpp",
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
        f"{ZetaDir}/DiffDoublyLinkedNode.h",
        {
            f"{File}",
            f"{ZetaDir}/define.h",
        },
        lambda : 0
    )

    builder.add(
        f"{ZetaDir}/DiffDoublyLinkedNode.c",
        {
            f"{File}",
            f"{ZetaDir}/DiffDoublyLinkedNode.h",
        },
        lambda : 0
    )

    builder.add(
        f"{ZetaDir}/DiffDoublyLinkedNode.o",
        {
            f"{File}",
            f"{ZetaDir}/DiffDoublyLinkedNode.c",
        },
        lambda : os.system(" ".join([
            cc,
            f"--output {ZetaDir}/DiffDoublyLinkedNode.o",
            f"-c",
            *cargs,
            f"{ZetaDir}/DiffDoublyLinkedNode.c",
        ]))
    )

    builder.add(
        f"{ZetaDir}/OrdDoublyLinkedNode.h",
        {
            f"{File}",
            f"{ZetaDir}/define.h",
        },
        lambda : 0
    )

    builder.add(
        f"{ZetaDir}/OrdDoublyLinkedNode.c",
        {
            f"{File}",
            f"{ZetaDir}/OrdDoublyLinkedNode.h",
        },
        lambda : 0
    )

    builder.add(
        f"{ZetaDir}/OrdDoublyLinkedNode.o",
        {
            f"{File}",
            f"{ZetaDir}/OrdDoublyLinkedNode.c",
        },
        lambda : os.system(" ".join([
            cc,
            f"--output {ZetaDir}/OrdDoublyLinkedNode.o",
            f"-c",
            *cargs,
            f"{ZetaDir}/OrdDoublyLinkedNode.c",
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
            f"--output {ZetaDir}/DynamicVector.o",
            f"-c",
            *cargs,
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
            f"--output {ZetaDir}/MultiLevelEntryTable.o",
            f"-c",
            *cargs,
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
            f"--output {ZetaDir}/MultiLevelHashTable.o",
            f"-c",
            *cargs,
            f"{ZetaDir}/MultiLevelHashTable.c",
        ]))
    )

    builder.add(
        f"{ZetaDir}/NearCntRBTreeNode.h",
        {
            f"{File}",
            f"{ZetaDir}/define.h",
        },
        lambda : 0
    )

    builder.add(
        f"{ZetaDir}/NearCntRBTreeNode.c",
        {
            f"{File}",
            f"{ZetaDir}/NearCntRBTreeNode.h",
        },
        lambda : 0
    )

    builder.add(
        f"{ZetaDir}/NearCntRBTreeNode.o",
        {
            f"{File}",
            f"{ZetaDir}/NearCntRBTreeNode.c",
        },
        lambda : os.system(" ".join([
            cc,
            f"--output {ZetaDir}/NearCntRBTreeNode.o",
            f"-c",
            *cargs,
            f"{ZetaDir}/NearCntRBTreeNode.c",
        ]))
    )

    builder.add(
        f"{ZetaDir}/NearRBTreeNode.h",
        {
            f"{File}",
            f"{ZetaDir}/define.h",
        },
        lambda : 0
    )

    builder.add(
        f"{ZetaDir}/NearRBTreeNode.c",
        {
            f"{File}",
            f"{ZetaDir}/NearRBTreeNode.h",
        },
        lambda : 0
    )

    builder.add(
        f"{ZetaDir}/NearRBTreeNode.o",
        {
            f"{File}",
            f"{ZetaDir}/NearRBTreeNode.c",
        },
        lambda : os.system(" ".join([
            cc,
            f"--output {ZetaDir}/NearRBTreeNode.o",
            f"-c",
            *cargs,
            f"{ZetaDir}/NearRBTreeNode.c",
        ]))
    )

    builder.add(
        f"{ZetaDir}/OrdAllocator.h",
        {
            f"{File}",
            f"{ZetaDir}/Allocator.h",
        },
        lambda : 0
    )

    builder.add(
        f"{ZetaDir}/OrdAllocator.c",
        {
            f"{File}",
            f"{ZetaDir}/OrdAllocator.h",
            f"{ZetaDir}/Algorithm.h",
            f"{ZetaDir}/DebugTreeMap.h",
            f"{ZetaDir}/RBTree.h",
            f"{ZetaDir}/RawVector.h",
            f"{ZetaDir}/utils.h",
        },
        lambda : 0
    )

    builder.add(
        f"{ZetaDir}/OrdAllocator.o",
        {
            f"{File}",
            f"{ZetaDir}/OrdAllocator.c",
        },
        lambda : os.system(" ".join([
            cc,
            f"--output {ZetaDir}/OrdAllocator.o",
            f"-c",
            *cargs,
            f"{ZetaDir}/OrdAllocator.c",
        ]))
    )

    builder.add(
        f"{ZetaDir}/OrdRBTreeNode.h",
        {
            f"{File}",
            f"{ZetaDir}/define.h",
        },
        lambda : 0
    )

    builder.add(
        f"{ZetaDir}/OrdRBTreeNode.c",
        {
            f"{File}",
            f"{ZetaDir}/OrdRBTreeNode.h",
        },
        lambda : 0
    )

    builder.add(
        f"{ZetaDir}/OrdRBTreeNode.o",
        {
            f"{File}",
            f"{ZetaDir}/OrdRBTreeNode.c",
        },
        lambda : os.system(" ".join([
            cc,
            f"--output {ZetaDir}/OrdRBTreeNode.o",
            f"-c",
            *cargs,
            f"{ZetaDir}/OrdRBTreeNode.c",
        ]))
    )

    builder.add(
        f"{ZetaDir}/OrdCntRBTreeNode.h",
        {
            f"{File}",
            f"{ZetaDir}/define.h",
        },
        lambda : 0
    )

    builder.add(
        f"{ZetaDir}/OrdCntRBTreeNode.c",
        {
            f"{File}",
            f"{ZetaDir}/OrdCntRBTreeNode.h",
        },
        lambda : 0
    )

    builder.add(
        f"{ZetaDir}/OrdCntRBTreeNode.o",
        {
            f"{File}",
            f"{ZetaDir}/OrdCntRBTreeNode.c",
        },
        lambda : os.system(" ".join([
            cc,
            f"--output {ZetaDir}/OrdCntRBTreeNode.o",
            f"-c",
            *cargs,
            f"{ZetaDir}/OrdCntRBTreeNode.c",
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
            f"--output {ZetaDir}/PoolAllocator.o",
            f"-c",
            *cargs,
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
            f"--output {ZetaDir}/random.o",
            f"-c",
            *cargs,
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
            f"--output {ZetaDir}/RawVector.o",
            f"-c",
            *cargs,
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
            f"--output {ZetaDir}/RBTree.o",
            f"-c",
            *cargs,
            f"{ZetaDir}/RBTree.c",
        ]))
    )

    builder.add(
        f"{ZetaDir}/SHA256.h",
        {
            f"{File}",
            f"{ZetaDir}/define.h",
        },
        lambda : 0
    )

    builder.add(
        f"{ZetaDir}/SHA256.c",
        {
            f"{File}",
            f"{ZetaDir}/SHA256.h",
            f"{ZetaDir}/utils.h",
        },
        lambda : 0
    )

    builder.add(
        f"{ZetaDir}/SHA256.o",
        {
            f"{File}",
            f"{ZetaDir}/SHA256.c",
        },
        lambda : os.system(" ".join([
            cc,
            f"--output {ZetaDir}/SHA256.o",
            f"-c",
            *cargs,
            f"{ZetaDir}/SHA256.c",
        ]))
    )

    builder.add(
        f"{ZetaDir}/SlabAllocator.h",
        {
            f"{File}",
            f"{ZetaDir}/Allocator.h",
            f"{ZetaDir}/OrdDoublyLinkedNode.h",
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
            f"--output {ZetaDir}/SlabAllocator.o",
            f"-c",
            *cargs,
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
            f"--output {ZetaDir}/utils.o",
            f"-c",
            *cargs,
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
