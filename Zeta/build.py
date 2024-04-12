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

def AddDeps(builder):
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
        cargs += ["-O3", "-flto"]

        cppargs += ["-O3", "-flto"]

    builder.Add(
        f"{File}",
        set(),
        None
    )

    builder.Add(
        f"{ZetaDir}/Algorithm.h",
        {
            f"{File}",
            f"{ZetaDir}/define.h",
        },
        None
    )

    builder.Add(
        f"{ZetaDir}/Algorithm.c",
        {
            f"{File}",
            f"{ZetaDir}/Algorithm.h",
        },
        None
    )

    builder.Add(
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

    builder.Add(
        f"{ZetaDir}/Allocator.h",
        {
            f"{File}",
            f"{ZetaDir}/define.h",
        },
        None
    )

    builder.Add(
        f"{ZetaDir}/Allocator.c",
        {
            f"{File}",
            f"{ZetaDir}/Allocator.h",
        },
        None
    )

    builder.Add(
        f"{ZetaDir}/Allocator.o",
        {
            f"{File}",
            f"{ZetaDir}/Allocator.c",
        },
        lambda : os.system(" ".join([
            cc,

            f"--output {ZetaDir}/Allocator.o",

            f"-c",

            *cargs,

            f"{ZetaDir}/Allocator.c",
        ]))
    )

    builder.Add(
        f"{ZetaDir}/BinHeap.h",
        {
            f"{File}",
            f"{ZetaDir}/BinTree.h",
        },
        None
    )

    builder.Add(
        f"{ZetaDir}/BinHeap.c",
        {
            f"{File}",
            f"{ZetaDir}/BinHeap.h",
            f"{ZetaDir}/utils.h",
        },
        None
    )

    builder.Add(
        f"{ZetaDir}/BinHeap.o",
        {
            f"{File}",
            f"{ZetaDir}/BinHeap.c",
        },
        lambda : os.system(" ".join([
            cc,
            f"--output {ZetaDir}/BinHeap.o",
            f"-c",
            *cargs,
            f"{ZetaDir}/BinHeap.c",
        ]))
    )

    builder.Add(
        f"{ZetaDir}/BinTree.h",
        {
            f"{File}",
            f"{ZetaDir}/define.h",
        },
        None
    )

    builder.Add(
        f"{ZetaDir}/BinTree.c",
        {
            f"{File}",
            f"{ZetaDir}/BinTree.h",
            f"{ZetaDir}/utils.h",
        },
        None
    )

    builder.Add(
        f"{ZetaDir}/BinTree.o",
        {
            f"{File}",
            f"{ZetaDir}/BinTree.c",
        },
        lambda : os.system(" ".join([
            cc,
            f"--output {ZetaDir}/BinTree.o",
            f"-c",
            *cargs,
            f"{ZetaDir}/BinTree.c",
        ]))
    )

    builder.Add(
        f"{ZetaDir}/BlockVector.h",
        {
            f"{File}",

            f"{ZetaDir}/define.h",
        },
        None
    )

    builder.Add(
        f"{ZetaDir}/BlockVector.c",
        {
            f"{File}",
            f"{ZetaDir}/BlockVector.h",
        },
        None
    )

    builder.Add(
        f"{ZetaDir}/BlockVector.o",
        {
            f"{File}",
            f"{ZetaDir}/BlockVector.c",
        },
        lambda : os.system(" ".join([
            cc,
            f"--output {ZetaDir}/BlockVector.o",

            f"-c",

            *cargs,

            f"{ZetaDir}/BlockVector.c",
        ]))
    )

    builder.Add(
        f"{ZetaDir}/CircularVector.h",
        {
            f"{File}",
            f"{ZetaDir}/Cursor.h",
            f"{ZetaDir}/SeqContainer.h",
        },
        None
    )

    builder.Add(
        f"{ZetaDir}/CircularVector.c",
        {
            f"{File}",
            f"{ZetaDir}/CircularVector.h",
        },
        None
    )

    builder.Add(
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

    builder.Add(
        f"{ZetaDir}/CRC.h",
        {
            f"{File}",
            f"{ZetaDir}/define.h",
        },
        None
    )

    builder.Add(
        f"{ZetaDir}/CRC.c",
        {
            f"{File}",
            f"{ZetaDir}/CRC.h",
        },
        None
    )

    builder.Add(
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

    builder.Add(
        f"{ZetaDir}/Cursor.h",
        {
            f"{File}",
            f"{ZetaDir}/define.h",
        },
        None
    )

    builder.Add(
        f"{ZetaDir}/DebugDeque.h",
        {
            f"{File}",
            f"{ZetaDir}/define.h",
        },
        None
    )

    builder.Add(
        f"{ZetaDir}/DebugDeque.cpp",
        {
            f"{File}",
            f"{ZetaDir}/DebugDeque.h",
        },
        None
    )

    builder.Add(
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

    builder.Add(
        f"{ZetaDir}/DebugHashTable.h",
        {
            f"{File}",
            f"{ZetaDir}/define.h",
        },
        None
    )

    builder.Add(
        f"{ZetaDir}/DebugHashTable.cpp",
        {
            f"{File}",
            f"{ZetaDir}/DebugHashTable.h",
        },
        None
    )

    builder.Add(
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

    builder.Add(
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

    builder.Add(
        f"{ZetaDir}/DebugPipe.h",
        {
            f"{File}",
            f"{ZetaDir}/define.h",
        },
        None
    )

    builder.Add(
        f"{ZetaDir}/DebugPipe.cpp",
        {
            f"{File}",
            f"{ZetaDir}/DebugPipe.h",
        },
        None
    )

    builder.Add(
        f"{ZetaDir}/DebugPipe.o",
        {
            f"{File}",
            f"{ZetaDir}/DebugPipe.cpp",
        },
        lambda : os.system(" ".join([
            cppc,
            f"--output {ZetaDir}/DebugPipe.o",
            *cppargs,
            f"-c",
            f"-fPIC",
            f"{ZetaDir}/DebugPipe.cpp",
        ]))
    )

    builder.Add(
        f"{ZetaDir}/DebugTreeMap.h",
        {
            f"{File}",
            f"{ZetaDir}/define.h",
        },
        None
    )

    builder.Add(
        f"{ZetaDir}/DebugTreeMap.cpp",
        {
            f"{File}",
            f"{ZetaDir}/DebugTreeMap.h",
        },
        None
    )

    builder.Add(
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

    builder.Add(
        f"{ZetaDir}/Deque.h",
        {
            f"{File}",
            f"{ZetaDir}/define.h",
        },
        None
    )

    builder.Add(
        f"{ZetaDir}/Disk.h",
        {
            f"{File}",
            f"{ZetaDir}/define.h",
        },
        None
    )

    builder.Add(
        f"{ZetaDir}/DiskInfo.h",
        {
            f"{File}",
            f"{ZetaDir}/define.h",
        },
        None
    )

    builder.Add(
        f"{ZetaDir}/DiskInfo.c",
        {
            f"{File}",
            f"{ZetaDir}/DiskPartGPT.h",
            f"{ZetaDir}/utils.h",
        },
        None
    )

    builder.Add(
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

    builder.Add(
        f"{ZetaDir}/DiskPartGPT.h",
        {
            f"{File}",
            f"{ZetaDir}/define.h",
        },
        None
    )

    builder.Add(
        f"{ZetaDir}/DiskPartGPT.c",
        {
            f"{File}",
            f"{ZetaDir}/DiskPartGPT.h",
            f"{ZetaDir}/utils.h",
        },
        None
    )

    builder.Add(
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

    builder.Add(
        f"{ZetaDir}/DiskPartMBR.h",
        {
            f"{File}",
            f"{ZetaDir}/define.h",
        },
        None
    )

    builder.Add(
        f"{ZetaDir}/DiskPartMBR.c",
        {
            f"{File}",
            f"{ZetaDir}/DiskPartMBR.h",
            f"{ZetaDir}/utils.h",
        },
        None
    )

    builder.Add(
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

    builder.Add(
        f"{ZetaDir}/DummyBlockVector.h",
        {
            f"{File}",
            f"{ZetaDir}/BlockVector.h",
        },
        None
    )

    builder.Add(
        f"{ZetaDir}/DummyBlockVector.c",
        {
            f"{File}",
            f"{ZetaDir}/DummyBlockVector.h",
            f"{ZetaDir}/utils.h",
        },
        None
    )

    builder.Add(
        f"{ZetaDir}/DummyBlockVector.o",
        {
            f"{File}",
            f"{ZetaDir}/DummyBlockVector.c",
        },
        lambda : os.system(" ".join([
            cc,
            f"--output {ZetaDir}/DummyBlockVector.o",
            f"-c",
            *cargs,
            f"{ZetaDir}/DummyBlockVector.c",
        ]))
    )

    builder.Add(
        f"{ZetaDir}/DummyCacheManager.h",
        {
            f"{File}",
            f"{ZetaDir}/BlockVector.h",
        },
        None
    )

    builder.Add(
        f"{ZetaDir}/DummyCacheManager.c",
        {
            f"{File}",
            f"{ZetaDir}/DummyCacheManager.h",
        },
        None
    )

    builder.Add(
        f"{ZetaDir}/DummyCacheManager.o",
        {
            f"{File}",
            f"{ZetaDir}/DummyCacheManager.c",
        },
        lambda : os.system(" ".join([
            cc,
            f"--output {ZetaDir}/DummyCacheManager.o",
            f"-c",
            *cargs,
            f"{ZetaDir}/DummyCacheManager.c",
        ]))
    )

    builder.Add(
        f"{ZetaDir}/define.h",
        {
            f"{File}",
        },
        None
    )

    builder.Add(
        f"{ZetaDir}/DiffLinkedListNode.h",
        {
            f"{File}",
            f"{ZetaDir}/define.h",
        },
        None
    )

    builder.Add(
        f"{ZetaDir}/DiffLinkedListNode.c",
        {
            f"{File}",
            f"{ZetaDir}/DiffLinkedListNode.h",
        },
        None
    )

    builder.Add(
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

    builder.Add(
        f"{ZetaDir}/ELF.h",
        {
            f"{File}",
            f"{ZetaDir}/define.h",
        },
        None
    )

    builder.Add(
        f"{ZetaDir}/ELF.c",
        {
            f"{File}",
            f"{ZetaDir}/ELF.h",
            f"{ZetaDir}/utils.h",
        },
        None
    )

    builder.Add(
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

    builder.Add(
        f"{ZetaDir}/OrdBinTreeNode.h",
        {
            f"{File}",

            f"{ZetaDir}/BinTree.h",
        },
        None
    )

    builder.Add(
        f"{ZetaDir}/OrdBinTreeNode.c",
        {
            f"{File}",

            f"{ZetaDir}/OrdBinTreeNode.h",
        },
        None
    )

    builder.Add(
        f"{ZetaDir}/OrdBinTreeNode.o",
        {
            f"{File}",
            f"{ZetaDir}/OrdBinTreeNode.c",
        },
        lambda : os.system(" ".join([
            cc,
            f"--output {ZetaDir}/OrdBinTreeNode.o",
            f"-c",
            *cargs,
            f"{ZetaDir}/OrdBinTreeNode.c",
        ]))
    )

    builder.Add(
        f"{ZetaDir}/OrdLinkedListNode.h",
        {
            f"{File}",
            f"{ZetaDir}/define.h",
        },
        None
    )

    builder.Add(
        f"{ZetaDir}/OrdLinkedListNode.c",
        {
            f"{File}",
            f"{ZetaDir}/OrdLinkedListNode.h",
        },
        None
    )

    builder.Add(
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

    builder.Add(
        f"{ZetaDir}/OrdRBLinkedListNode.h",
        {
            f"{File}",
            f"{ZetaDir}/define.h",
        },
        None
    )

    builder.Add(
        f"{ZetaDir}/OrdRBLinkedListNode.c",
        {
            f"{File}",
            f"{ZetaDir}/OrdRBLinkedListNode.h",
        },
        None
    )

    builder.Add(
        f"{ZetaDir}/OrdRBLinkedListNode.o",
        {
            f"{File}",
            f"{ZetaDir}/OrdRBLinkedListNode.c",
        },
        lambda : os.system(" ".join([
            cc,
            f"--output {ZetaDir}/OrdRBLinkedListNode.o",
            f"-c",
            *cargs,
            f"{ZetaDir}/OrdRBLinkedListNode.c",
        ]))
    )

    builder.Add(
        f"{ZetaDir}/DynamicVector.h",
        {
            f"{File}",
            f"{ZetaDir}/Allocator.h",
            f"{ZetaDir}/CircularVector.h",
            f"{ZetaDir}/MultiLevelVector.h",
        },
        None
    )

    builder.Add(
        f"{ZetaDir}/DynamicVector.c",
        {
            f"{File}",
            f"{ZetaDir}/DynamicVector.h",
        },
        None
    )

    builder.Add(
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

    builder.Add(
        f"{ZetaDir}/FileSysFAT32.h",
        {
            f"{File}",
            f"{ZetaDir}/Disk.h",
        },
        None
    )

    builder.Add(
        f"{ZetaDir}/FileSysFAT32.c",
        {
            f"{File}",
            f"{ZetaDir}/FileSysFAT32.h",
        },
        None
    )

    builder.Add(
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

    builder.Add(
        f"{ZetaDir}/MultiLevelVector.h",
        {
            f"{File}",
            f"{ZetaDir}/define.h",
        },
        None
    )

    builder.Add(
        f"{ZetaDir}/MultiLevelVector.c",
        {
            f"{File}",
            f"{ZetaDir}/MultiLevelVector.h",
        },
        None
    )

    builder.Add(
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

    builder.Add(
        f"{ZetaDir}/io.h",
        {
            f"{File}",
            f"{ZetaDir}/Stream.h",
        },
        None
    )

    builder.Add(
        f"{ZetaDir}/io.c",
        {
            f"{File}",
            f"{ZetaDir}/io.h",
        },
        None
    )

    builder.Add(
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

    builder.Add(
        f"{ZetaDir}/LRUCacheManager.h",
        {
            f"{File}",

            f"{ZetaDir}/Allocator.h",
            f"{ZetaDir}/BinHeap.h",
            f"{ZetaDir}/BlockVector.h",
            f"{ZetaDir}/OrdBinTreeNode.h",
            f"{ZetaDir}/OrdLinkedListNode.h",
            f"{ZetaDir}/OrdRBTreeNode.h",
        },
        None
    )

    builder.Add(
        f"{ZetaDir}/LRUCacheManager.c",
        {
            f"{File}",

            f"{ZetaDir}/LRUCacheManager.h",

            f"{ZetaDir}/RBTree.h",
            f"{ZetaDir}/utils.h",
        },
        None
    )

    builder.Add(
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

    builder.Add(
        f"{ZetaDir}/MultiLevelHashTable.h",
        {
            f"{File}",
            f"{ZetaDir}/MultiLevelVector.h",
        },
        None
    )

    builder.Add(
        f"{ZetaDir}/MultiLevelHashTable.c",
        {
            f"{File}",
            f"{ZetaDir}/MultiLevelHashTable.h",
        },
        None
    )

    builder.Add(
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

    builder.Add(
        f"{ZetaDir}/NearCntRBTreeNode.h",
        {
            f"{File}",
            f"{ZetaDir}/define.h",
        },
        None
    )

    builder.Add(
        f"{ZetaDir}/NearCntRBTreeNode.c",
        {
            f"{File}",
            f"{ZetaDir}/NearCntRBTreeNode.h",
        },
        None
    )

    builder.Add(
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

    builder.Add(
        f"{ZetaDir}/NearRBTreeNode.h",
        {
            f"{File}",
            f"{ZetaDir}/define.h",
        },
        None
    )

    builder.Add(
        f"{ZetaDir}/NearRBTreeNode.c",
        {
            f"{File}",
            f"{ZetaDir}/NearRBTreeNode.h",
        },
        None
    )

    builder.Add(
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

    builder.Add(
        f"{ZetaDir}/OrdAllocator.h",
        {
            f"{File}",
            f"{ZetaDir}/Allocator.h",
        },
        None
    )

    builder.Add(
        f"{ZetaDir}/OrdAllocator.c",
        {
            f"{File}",
            f"{ZetaDir}/OrdAllocator.h",
            f"{ZetaDir}/Algorithm.h",
            f"{ZetaDir}/DebugTreeMap.h",
            f"{ZetaDir}/RBTree.h",
            f"{ZetaDir}/utils.h",
        },
        None
    )

    builder.Add(
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

    builder.Add(
        f"{ZetaDir}/OrdRBTreeNode.h",
        {
            f"{File}",
            f"{ZetaDir}/BinTree.h",
        },
        None
    )

    builder.Add(
        f"{ZetaDir}/OrdRBTreeNode.c",
        {
            f"{File}",
            f"{ZetaDir}/OrdRBTreeNode.h",
        },
        None
    )

    builder.Add(
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

    builder.Add(
        f"{ZetaDir}/OrdCntRBTreeNode.h",
        {
            f"{File}",
            f"{ZetaDir}/BinTree.h",
        },
        None
    )

    builder.Add(
        f"{ZetaDir}/OrdCntRBTreeNode.c",
        {
            f"{File}",
            f"{ZetaDir}/OrdCntRBTreeNode.h",
        },
        None
    )

    builder.Add(
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

    builder.Add(
        f"{ZetaDir}/PoolAllocator.h",
        {
            f"{File}",
            f"{ZetaDir}/Allocator.h",
        },
        None
    )

    builder.Add(
        f"{ZetaDir}/PoolAllocator.c",
        {
            f"{File}",
            f"{ZetaDir}/PoolAllocator.h",
        },
        None
    )

    builder.Add(
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

    builder.Add(
        f"{ZetaDir}/random.h",
        {
            f"{File}",
            f"{ZetaDir}/define.h",
        },
        None
    )

    builder.Add(
        f"{ZetaDir}/random.c",
        {
            f"{File}",
            f"{ZetaDir}/random.h",
        },
        None
    )

    builder.Add(
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

    builder.Add(
        f"{ZetaDir}/RBTree.h",
        {
            f"{File}",
            f"{ZetaDir}/BinTree.h",
        },
        None
    )

    builder.Add(
        f"{ZetaDir}/RBTree.c",
        {
            f"{File}",
            f"{ZetaDir}/RBTree.h",
            f"{ZetaDir}/utils.h",
        },
        None
    )

    builder.Add(
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

    builder.Add(
        f"{ZetaDir}/SegList.h",
        {
            f"{File}",
            f"{ZetaDir}/Allocator.h",
            f"{ZetaDir}/DebugTreeMap.h",
            f"{ZetaDir}/OrdLinkedListNode.h",
        },
        None
    )

    builder.Add(
        f"{ZetaDir}/SegList.c",
        {
            f"{File}",
            f"{ZetaDir}/SegList.h",
        },
        None
    )

    builder.Add(
        f"{ZetaDir}/SegList.o",
        {
            f"{File}",
            f"{ZetaDir}/SegList.c",
        },
        lambda : os.system(" ".join([
            cc,
            f"--output {ZetaDir}/SegList.o",
            f"-c",
            *cargs,
            f"{ZetaDir}/SegList.c",
        ]))
    )

    builder.Add(
        f"{ZetaDir}/SegVector.h",
        {
            f"{File}",
            f"{ZetaDir}/Allocator.h",
            f"{ZetaDir}/Cursor.h",
            f"{ZetaDir}/DebugTreeMap.h",
            f"{ZetaDir}/OrdCntRBTreeNode.h",
            f"{ZetaDir}/SeqContainer.h",
        },
        None
    )

    builder.Add(
        f"{ZetaDir}/SegVector.c",
        {
            f"{File}",
            f"{ZetaDir}/SegVector.h",

            f"{ZetaDir}/BinTree.h",
            f"{ZetaDir}/CircularVector.h",
            f"{ZetaDir}/RBTree.h",
            f"{ZetaDir}/utils.h",
        },
        None
    )

    builder.Add(
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

    builder.Add(
        f"{ZetaDir}/SeqContainer.h",
        {
            f"{File}",
            f"{ZetaDir}/define.h",
        },
        None
    )

    builder.Add(
        f"{ZetaDir}/SeqContainer.c",
        {
            f"{File}",
            f"{ZetaDir}/SeqContainer.h",
        },
        None
    )

    builder.Add(
        f"{ZetaDir}/SeqContainer.o",
        {
            f"{File}",
            f"{ZetaDir}/SeqContainer.c",
        },
        lambda : os.system(" ".join([
            cc,
            f"--output {ZetaDir}/SeqContainer.o",
            f"-c",
            *cargs,
            f"{ZetaDir}/SeqContainer.c",
        ]))
    )

    builder.Add(
        f"{ZetaDir}/SHA256.h",
        {
            f"{File}",
            f"{ZetaDir}/define.h",
        },
        None
    )

    builder.Add(
        f"{ZetaDir}/SHA256.c",
        {
            f"{File}",
            f"{ZetaDir}/SHA256.h",
            f"{ZetaDir}/utils.h",
        },
        None
    )

    builder.Add(
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

    builder.Add(
        f"{ZetaDir}/SlabAllocator.h",
        {
            f"{File}",
            f"{ZetaDir}/Allocator.h",
            f"{ZetaDir}/OrdLinkedListNode.h",
        },
        None
    )

    builder.Add(
        f"{ZetaDir}/SlabAllocator.c",
        {
            f"{File}",
            f"{ZetaDir}/SlabAllocator.h",
        },
        None
    )

    builder.Add(
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

    builder.Add(
        f"{ZetaDir}/Stream.h",
        {
            f"{File}",
            f"{ZetaDir}/define.h",
        },
        None
    )

    builder.Add(
        f"{ZetaDir}/Stream.c",
        {
            f"{File}",
            f"{ZetaDir}/Stream.h",
        },
        None
    )

    builder.Add(
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

    builder.Add(
        f"{ZetaDir}/TreeAllocator.h",
        {
            f"{File}",
            f"{ZetaDir}/Allocator.h",
            f"{ZetaDir}/OrdRBTreeNode.h",
        },
        None
    )

    builder.Add(
        f"{ZetaDir}/TreeAllocator.c",
        {
            f"{File}",
            f"{ZetaDir}/TreeAllocator.h",
            f"{ZetaDir}/DebugTreeMap.h",
            f"{ZetaDir}/RBTree.h",
            f"{ZetaDir}/OrdLinkedListNode.h",
            f"{ZetaDir}/utils.h",
        },
        None
    )

    builder.Add(
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

    builder.Add(
        f"{ZetaDir}/UTF8.h",
        {
            f"{File}",
            f"{ZetaDir}/define.h",
        },
        None
    )

    builder.Add(
        f"{ZetaDir}/UTF8.c",
        {
            f"{File}",
            f"{ZetaDir}/UTF8.h",
            f"{ZetaDir}/utils.h",
        },
        None
    )

    builder.Add(
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

    builder.Add(
        f"{ZetaDir}/UTF16.h",
        {
            f"{File}",
            f"{ZetaDir}/define.h",
        },
        None
    )

    builder.Add(
        f"{ZetaDir}/UTF16.c",
        {
            f"{File}",
            f"{ZetaDir}/UTF16.h",
        },
        None
    )

    builder.Add(
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

    builder.Add(
        f"{ZetaDir}/utils.h",
        {
            f"{File}",
            f"{ZetaDir}/define.h",
        },
        None
    )

    builder.Add(
        f"{ZetaDir}/utils.c",
        {
            f"{File}",
            f"{ZetaDir}/utils.h",
        },
        None
    )

    builder.Add(
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

if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("target", action="store")
    parser.add_argument("--rebuild", dest="rebuild", action="store_true")
    args = parser.parse_args(sys.argv[1:])

    print(f"args = {args}")

    builder = Builder()
    AddDeps(builder)
    builder.Add("all", builder.units(), None)

    non_built, built = builder.build(GetABSPath(args.target), args.rebuild)

    ybeg = "\033[93m"
    yend = "\033[0m"

    print("success")

    non_built.sort()
    built.sort()

    print(f"{ybeg}not_built:{yend}")

    for i in non_built:
        print(f"\t{i}")

    print(f"{ybeg}    built:{yend}")

    for i in built:
        print(f"\t{i}")
