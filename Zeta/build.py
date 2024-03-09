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
        cargs += ["-O3", "-g", "-D DEBUG"]

        cppargs += ["-O3", "-g", "-D DEBUG"]
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
        f"{ZetaDir}/DebugDeque.h",
        {
            f"{File}",
            f"{ZetaDir}/define.h",
        },
        lambda : 0
    )

    builder.add(
        f"{ZetaDir}/DebugDeque.cpp",
        {
            f"{File}",
            f"{ZetaDir}/DebugDeque.h",
        },
        lambda : 0
    )

    builder.add(
        f"{ZetaDir}/DebugDeque.o",
        {
            f"{File}",
            f"{ZetaDir}/DebugDeque.cpp",
        },
        lambda : os.system(" ".join([
            cppc,
            f"--output {ZetaDir}/DebugDeque.o",
            *cppargs,
            f"-c",
            f"{ZetaDir}/DebugDeque.cpp",
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
            f"--output {ZetaDir}/DebugHashTable.o",
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
            f"--output {ZetaDir}/DebugTreeMap.o",
            *cppargs,
            f"-c",
            f"-fPIC",
            f"{ZetaDir}/DebugTreeMap.cpp",
        ]))
    )

    builder.add(
        f"{ZetaDir}/Deque.h",
        {
            f"{File}",
            f"{ZetaDir}/define.h",
        },
        lambda : 0
    )

    builder.add(
        f"{ZetaDir}/Disk.h",
        {
            f"{File}",
            f"{ZetaDir}/define.h",
        },
        lambda : 0
    )

    builder.add(
        f"{ZetaDir}/DiskInfo.h",
        {
            f"{File}",
            f"{ZetaDir}/define.h",
        },
        lambda : 0
    )

    builder.add(
        f"{ZetaDir}/DiskInfo.c",
        {
            f"{File}",
            f"{ZetaDir}/DiskPartGPT.h",
            f"{ZetaDir}/utils.h",
        },
        lambda : 0
    )

    builder.add(
        f"{ZetaDir}/DiskInfo.o",
        {
            f"{File}",
            f"{ZetaDir}/DiskInfo.c",
        },
        lambda : os.system(" ".join([
            cc,
            f"--output {ZetaDir}/DiskInfo.o",
            f"-c",
            *cargs,
            f"{ZetaDir}/DiskInfo.c",
        ]))
    )

    builder.add(
        f"{ZetaDir}/DiskPartGPT.h",
        {
            f"{File}",
            f"{ZetaDir}/define.h",
        },
        lambda : 0
    )

    builder.add(
        f"{ZetaDir}/DiskPartGPT.c",
        {
            f"{File}",
            f"{ZetaDir}/DiskPartGPT.h",
            f"{ZetaDir}/utils.h",
        },
        lambda : 0
    )

    builder.add(
        f"{ZetaDir}/DiskPartGPT.o",
        {
            f"{File}",
            f"{ZetaDir}/DiskPartGPT.c",
        },
        lambda : os.system(" ".join([
            cc,
            f"--output {ZetaDir}/DiskPartGPT.o",
            f"-c",
            *cargs,
            f"{ZetaDir}/DiskPartGPT.c",
        ]))
    )

    builder.add(
        f"{ZetaDir}/DiskPartMBR.h",
        {
            f"{File}",
            f"{ZetaDir}/define.h",
        },
        lambda : 0
    )

    builder.add(
        f"{ZetaDir}/DiskPartMBR.c",
        {
            f"{File}",
            f"{ZetaDir}/DiskPartMBR.h",
            f"{ZetaDir}/utils.h",
        },
        lambda : 0
    )

    builder.add(
        f"{ZetaDir}/DiskPartMBR.o",
        {
            f"{File}",
            f"{ZetaDir}/DiskPartMBR.c",
        },
        lambda : os.system(" ".join([
            cc,
            f"--output {ZetaDir}/DiskPartMBR.o",
            f"-c",
            *cargs,
            f"{ZetaDir}/DiskPartMBR.c",
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
        f"{ZetaDir}/DiffLinkedListNode.h",
        {
            f"{File}",
            f"{ZetaDir}/define.h",
        },
        lambda : 0
    )

    builder.add(
        f"{ZetaDir}/DiffLinkedListNode.c",
        {
            f"{File}",
            f"{ZetaDir}/DiffLinkedListNode.h",
        },
        lambda : 0
    )

    builder.add(
        f"{ZetaDir}/DiffLinkedListNode.o",
        {
            f"{File}",
            f"{ZetaDir}/DiffLinkedListNode.c",
        },
        lambda : os.system(" ".join([
            cc,
            f"--output {ZetaDir}/DiffLinkedListNode.o",
            f"-c",
            *cargs,
            f"{ZetaDir}/DiffLinkedListNode.c",
        ]))
    )

    builder.add(
        f"{ZetaDir}/ELF.h",
        {
            f"{File}",
            f"{ZetaDir}/define.h",
        },
        lambda : 0
    )

    builder.add(
        f"{ZetaDir}/ELF.c",
        {
            f"{File}",
            f"{ZetaDir}/ELF.h",
            f"{ZetaDir}/utils.h",
        },
        lambda : 0
    )

    builder.add(
        f"{ZetaDir}/ELF.o",
        {
            f"{File}",
            f"{ZetaDir}/ELF.c",
        },
        lambda : os.system(" ".join([
            cc,
            f"--output {ZetaDir}/ELF.o",
            f"-c",
            *cargs,
            f"{ZetaDir}/ELF.c",
        ]))
    )

    builder.add(
        f"{ZetaDir}/OrdLinkedListNode.h",
        {
            f"{File}",
            f"{ZetaDir}/define.h",
        },
        lambda : 0
    )

    builder.add(
        f"{ZetaDir}/OrdLinkedListNode.c",
        {
            f"{File}",
            f"{ZetaDir}/OrdLinkedListNode.h",
        },
        lambda : 0
    )

    builder.add(
        f"{ZetaDir}/OrdLinkedListNode.o",
        {
            f"{File}",
            f"{ZetaDir}/OrdLinkedListNode.c",
        },
        lambda : os.system(" ".join([
            cc,
            f"--output {ZetaDir}/OrdLinkedListNode.o",
            f"-c",
            *cargs,
            f"{ZetaDir}/OrdLinkedListNode.c",
        ]))
    )

    builder.add(
        f"{ZetaDir}/DynamicVector.h",
        {
            f"{File}",
            f"{ZetaDir}/Allocator.h",
            f"{ZetaDir}/CircularVector.h",
            f"{ZetaDir}/MultiLevelVector.h",
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
            f"{ZetaDir}/DynamicVector.c",
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
        f"{ZetaDir}/FileSysFAT32.h",
        {
            f"{File}",
            f"{ZetaDir}/Disk.h",
        },
        lambda : 0
    )

    builder.add(
        f"{ZetaDir}/FileSysFAT32.c",
        {
            f"{File}",
            f"{ZetaDir}/FileSysFAT32.h",
        },
        lambda : 0
    )

    builder.add(
        f"{ZetaDir}/FileSysFAT32.o",
        {
            f"{File}",
            f"{ZetaDir}/FileSysFAT32.c",
        },
        lambda : os.system(" ".join([
            cc,
            f"--output {ZetaDir}/FileSysFAT32.o",
            f"-c",
            *cargs,
            f"{ZetaDir}/FileSysFAT32.c",
        ]))
    )

    builder.add(
        f"{ZetaDir}/MultiLevelVector.h",
        {
            f"{File}",
            f"{ZetaDir}/define.h",
        },
        lambda : 0
    )

    builder.add(
        f"{ZetaDir}/MultiLevelVector.c",
        {
            f"{File}",
            f"{ZetaDir}/MultiLevelVector.h",
        },
        lambda : 0
    )

    builder.add(
        f"{ZetaDir}/MultiLevelVector.o",
        {
            f"{File}",
            f"{ZetaDir}/MultiLevelVector.c",
        },
        lambda : os.system(" ".join([
            cc,
            f"--output {ZetaDir}/MultiLevelVector.o",
            f"-c",
            *cargs,
            f"{ZetaDir}/MultiLevelVector.c",
        ]))
    )

    builder.add(
        f"{ZetaDir}/io.h",
        {
            f"{File}",
            f"{ZetaDir}/Stream.h",
        },
        lambda : 0
    )

    builder.add(
        f"{ZetaDir}/io.c",
        {
            f"{File}",
            f"{ZetaDir}/io.h",
        },
        lambda : 0
    )

    builder.add(
        f"{ZetaDir}/io.o",
        {
            f"{File}",
            f"{ZetaDir}/io.c",
        },
        lambda : os.system(" ".join([
            cc,
            f"--output {ZetaDir}/io.o",
            f"-c",
            *cargs,
            f"{ZetaDir}/io.c",
        ]))
    )

    builder.add(
        f"{ZetaDir}/LRUCacheManager.h",
        {
            f"{File}",
            f"{ZetaDir}/MultiLevelVector.h",
            f"{ZetaDir}/OrdLinkedListNode.h",
        },
        lambda : 0
    )

    builder.add(
        f"{ZetaDir}/LRUCacheManager.c",
        {
            f"{File}",
            f"{ZetaDir}/LRUCacheManager.h",
        },
        lambda : 0
    )

    builder.add(
        f"{ZetaDir}/LRUCacheManager.o",
        {
            f"{File}",
            f"{ZetaDir}/LRUCacheManager.c",
        },
        lambda : os.system(" ".join([
            cc,
            f"--output {ZetaDir}/LRUCacheManager.o",
            f"-c",
            *cargs,
            f"{ZetaDir}/LRUCacheManager.c",
        ]))
    )

    builder.add(
        f"{ZetaDir}/MultiLevelHashTable.h",
        {
            f"{File}",
            f"{ZetaDir}/MultiLevelVector.h",
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
        f"{ZetaDir}/RelCntRBTreeNode.h",
        {
            f"{File}",
            f"{ZetaDir}/define.h",
        },
        lambda : 0
    )

    builder.add(
        f"{ZetaDir}/RelCntRBTreeNode.c",
        {
            f"{File}",
            f"{ZetaDir}/RelCntRBTreeNode.h",
        },
        lambda : 0
    )

    builder.add(
        f"{ZetaDir}/RelCntRBTreeNode.o",
        {
            f"{File}",
            f"{ZetaDir}/RelCntRBTreeNode.c",
        },
        lambda : os.system(" ".join([
            cc,
            f"--output {ZetaDir}/RelCntRBTreeNode.o",
            f"-c",
            *cargs,
            f"{ZetaDir}/RelCntRBTreeNode.c",
        ]))
    )

    builder.add(
        f"{ZetaDir}/RelLinkedListNode.h",
        {
            f"{File}",
            f"{ZetaDir}/define.h",
        },
        lambda : 0
    )

    builder.add(
        f"{ZetaDir}/RelLinkedListNode.c",
        {
            f"{File}",
            f"{ZetaDir}/RelLinkedListNode.h",
        },
        lambda : 0
    )

    builder.add(
        f"{ZetaDir}/RelLinkedListNode.o",
        {
            f"{File}",
            f"{ZetaDir}/RelLinkedListNode.c",
        },
        lambda : os.system(" ".join([
            cc,
            f"--output {ZetaDir}/RelLinkedListNode.o",
            f"-c",
            *cargs,
            f"{ZetaDir}/RelLinkedListNode.c",
        ]))
    )

    builder.add(
        f"{ZetaDir}/RelRBLinkedListNode.h",
        {
            f"{File}",
            f"{ZetaDir}/define.h",
        },
        lambda : 0
    )

    builder.add(
        f"{ZetaDir}/RelRBLinkedListNode.c",
        {
            f"{File}",
            f"{ZetaDir}/RelRBLinkedListNode.h",
        },
        lambda : 0
    )

    builder.add(
        f"{ZetaDir}/RelRBLinkedListNode.o",
        {
            f"{File}",
            f"{ZetaDir}/RelRBLinkedListNode.c",
        },
        lambda : os.system(" ".join([
            cc,
            f"--output {ZetaDir}/RelRBLinkedListNode.o",
            f"-c",
            *cargs,
            f"{ZetaDir}/RelRBLinkedListNode.c",
        ]))
    )

    builder.add(
        f"{ZetaDir}/RelRBTreeNode.h",
        {
            f"{File}",
            f"{ZetaDir}/define.h",
        },
        lambda : 0
    )

    builder.add(
        f"{ZetaDir}/RelRBTreeNode.c",
        {
            f"{File}",
            f"{ZetaDir}/RelRBTreeNode.h",
        },
        lambda : 0
    )

    builder.add(
        f"{ZetaDir}/RelRBTreeNode.o",
        {
            f"{File}",
            f"{ZetaDir}/RelRBTreeNode.c",
        },
        lambda : os.system(" ".join([
            cc,
            f"--output {ZetaDir}/RelRBTreeNode.o",
            f"-c",
            *cargs,
            f"{ZetaDir}/RelRBTreeNode.c",
        ]))
    )

    builder.add(
        f"{ZetaDir}/SegVector.h",
        {
            f"{File}",
            f"{ZetaDir}/Allocator.h",
            f"{ZetaDir}/RelCntRBTreeNode.h",
            f"{ZetaDir}/Vector.h",
        },
        lambda : 0
    )

    builder.add(
        f"{ZetaDir}/SegVector.c",
        {
            f"{File}",
            f"{ZetaDir}/SegVector.h",

            f"{ZetaDir}/BinTree.h",
            f"{ZetaDir}/CircularVector.h",
            f"{ZetaDir}/CntBinTree.h",
            f"{ZetaDir}/RBTree.h",
            f"{ZetaDir}/RawVector.h",
            f"{ZetaDir}/utils.h",
        },
        lambda : 0
    )

    builder.add(
        f"{ZetaDir}/SegVector.o",
        {
            f"{File}",
            f"{ZetaDir}/SegVector.c",
        },
        lambda : os.system(" ".join([
            cc,
            f"--output {ZetaDir}/SegVector.o",
            f"-c",
            *cargs,
            f"{ZetaDir}/SegVector.c",
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
            f"{ZetaDir}/OrdLinkedListNode.h",
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
        f"{ZetaDir}/Stream.h",
        {
            f"{File}",
            f"{ZetaDir}/define.h",
        },
        lambda : 0
    )

    builder.add(
        f"{ZetaDir}/Stream.c",
        {
            f"{File}",
            f"{ZetaDir}/Stream.h",
        },
        lambda : 0
    )

    builder.add(
        f"{ZetaDir}/Stream.o",
        {
            f"{File}",
            f"{ZetaDir}/Stream.c",
        },
        lambda : os.system(" ".join([
            cc,
            f"--output {ZetaDir}/Stream.o",
            f"-c",
            *cargs,
            f"{ZetaDir}/Stream.c",
        ]))
    )

    builder.add(
        f"{ZetaDir}/TreeAllocator.h",
        {
            f"{File}",
            f"{ZetaDir}/Allocator.h",
            f"{ZetaDir}/RelRBTreeNode.h",
        },
        lambda : 0
    )

    builder.add(
        f"{ZetaDir}/TreeAllocator.c",
        {
            f"{File}",
            f"{ZetaDir}/TreeAllocator.h",
            f"{ZetaDir}/DebugTreeMap.h",
            f"{ZetaDir}/RBTree.h",
            f"{ZetaDir}/RelLinkedListNode.h",
            f"{ZetaDir}/utils.h",
        },
        lambda : 0
    )

    builder.add(
        f"{ZetaDir}/TreeAllocator.o",
        {
            f"{File}",
            f"{ZetaDir}/TreeAllocator.c",
        },
        lambda : os.system(" ".join([
            cc,
            f"--output {ZetaDir}/TreeAllocator.o",
            f"-c",
            *cargs,
            f"{ZetaDir}/TreeAllocator.c",
        ]))
    )

    builder.add(
        f"{ZetaDir}/UTF8.h",
        {
            f"{File}",
            f"{ZetaDir}/define.h",
        },
        lambda : 0
    )

    builder.add(
        f"{ZetaDir}/UTF8.c",
        {
            f"{File}",
            f"{ZetaDir}/UTF8.h",
            f"{ZetaDir}/utils.h",
        },
        lambda : 0
    )

    builder.add(
        f"{ZetaDir}/UTF8.o",
        {
            f"{File}",
            f"{ZetaDir}/UTF8.c",
        },
        lambda : os.system(" ".join([
            cc,
            f"--output {ZetaDir}/UTF8.o",
            f"-c",
            *cargs,
            f"{ZetaDir}/UTF8.c",
        ]))
    )

    builder.add(
        f"{ZetaDir}/UTF16.h",
        {
            f"{File}",
            f"{ZetaDir}/define.h",
        },
        lambda : 0
    )

    builder.add(
        f"{ZetaDir}/UTF16.c",
        {
            f"{File}",
            f"{ZetaDir}/UTF16.h",
        },
        lambda : 0
    )

    builder.add(
        f"{ZetaDir}/UTF16.o",
        {
            f"{File}",
            f"{ZetaDir}/UTF16.c",
        },
        lambda : os.system(" ".join([
            cc,
            f"--output {ZetaDir}/UTF16.o",
            f"-c",
            *cargs,
            f"{ZetaDir}/UTF16.c",
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

    ybeg = "\033[93m"
    yend = "\033[0m"

    print("success")
    print(f"{ybeg}not_built:{yend} {non_built}")
    print(f"{ybeg}    built:{yend} {built}")
