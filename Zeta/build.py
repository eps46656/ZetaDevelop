import os
import sys

File = os.path.abspath(__file__).replace("\\", "/")
ZetaDir = os.path.dirname(File).replace("\\", "/")

ZetaDevDir = os.path.abspath(f"{ZetaDir}/..").replace("\\", "/")

import argparse

sys.path.append(ZetaDevDir)
from builder import *

DEBUG_MODE = 0
RELEASE_MODE = 1

def AddDeps(builder, ZetaBuildDir, mode):
    c_include_dirs = []
    cpp_include_dirs = []

    c_to_ll_args = [
        "--verbose",
        "-std=c2x",
        *[f"-I {ToPath(dir)}" for dir in c_include_dirs],
        "-m64",
        "-O3",
    ]

    cpp_to_ll_args = [
        "--verbose",
        "-std=c++17",
        *[f"-I {ToPath(dir)}" for dir in cpp_include_dirs],
        "-m64",
        "-O3",
    ]

    if mode == DEBUG_MODE:
        c_to_ll_args += [
            "-g",
            "-D DEBUG",
            "-Wall",
            "-Wextra",
            "-Werror",
        ]

        cpp_to_ll_args += [
            "-g",
            "-D DEBUG",
            "-Wall",
            "-Wextra",
            "-Werror",
        ]

    def c_to_ll_func(dst, src):
        os.makedirs(os.path.dirname(dst), exist_ok=True)

        cmd = " ".join([
            f"clang",
            f"-o {ToPath(dst)}",
            f"-emit-llvm -S",
            *c_to_ll_args,
            ToPath(src),
        ])

        HighLightPrint(f"cmd = {cmd}")

        rc = os.system(cmd)

        return rc

    def cpp_to_ll_func(dst, src):
        os.makedirs(os.path.dirname(dst), exist_ok=True)

        cmd = " ".join([
            f"clang++",
            f"-o {ToPath(dst)}",
            f"-emit-llvm -S",
            *cpp_to_ll_args,
            ToPath(src),
        ])

        HighLightPrint(f"cmd = {cmd}")

        rc = os.system(cmd)

        return rc

    # --------------------------------------------------------------------------

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
        f"{ZetaBuildDir}/Algorithm.ll",
        {
            f"{File}",
            f"{ZetaDir}/Algorithm.c",
        },
        lambda : c_to_ll_func(
            f"{ZetaBuildDir}/Algorithm.ll",
            f"{ZetaDir}/Algorithm.c",
        )
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
        f"{ZetaBuildDir}/Allocator.ll",
        {
            f"{File}",
            f"{ZetaDir}/Allocator.c",
        },
        lambda : c_to_ll_func(
            f"{ZetaBuildDir}/Allocator.ll",
            f"{ZetaDir}/Allocator.c",
        )
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
        f"{ZetaBuildDir}/BinHeap.ll",
        {
            f"{File}",
            f"{ZetaDir}/BinHeap.c",
        },
        lambda : c_to_ll_func(
            f"{ZetaBuildDir}/BinHeap.ll",
            f"{ZetaDir}/BinHeap.c",
        )
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
        f"{ZetaBuildDir}/BinTree.ll",
        {
            f"{File}",
            f"{ZetaDir}/BinTree.c",
        },
        lambda : c_to_ll_func(
            f"{ZetaBuildDir}/BinTree.ll",
            f"{ZetaDir}/BinTree.c",
        )
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
        f"{ZetaBuildDir}/BlockVector.ll",
        {
            f"{File}",
            f"{ZetaDir}/BlockVector.c",
        },
        lambda : c_to_ll_func(
            f"{ZetaBuildDir}/BlockVector.ll",
            f"{ZetaDir}/BlockVector.c",
        )
    )

    builder.Add(
        f"{ZetaDir}/CacheManager.h",
        {
            f"{File}",
            f"{ZetaDir}/define.h",
        },
        None
    )

    builder.Add(
        f"{ZetaDir}/CacheManager.c",
        {
            f"{File}",
            f"{ZetaDir}/CacheManager.h",
        },
        None
    )

    builder.Add(
        f"{ZetaBuildDir}/CacheManager.ll",
        {
            f"{File}",
            f"{ZetaDir}/CacheManager.c",
        },
        lambda : c_to_ll_func(
            f"{ZetaBuildDir}/CacheManager.ll",
            f"{ZetaDir}/CacheManager.c",
        )
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
        f"{ZetaBuildDir}/CircularVector.ll",
        {
            f"{File}",
            f"{ZetaDir}/CircularVector.c",
        },
        lambda : c_to_ll_func(
            f"{ZetaBuildDir}/CircularVector.ll",
            f"{ZetaDir}/CircularVector.c",
        )
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
        f"{ZetaBuildDir}/CRC.ll",
        {
            f"{File}",
            f"{ZetaDir}/CRC.c",
        },
        lambda : c_to_ll_func(
            f"{ZetaBuildDir}/CRC.ll",
            f"{ZetaDir}/CRC.c",
        )
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
        f"{ZetaBuildDir}/DebugDeque.ll",
        {
            f"{File}",
            f"{ZetaDir}/DebugDeque.cpp",
        },
        lambda : cpp_to_ll_func(
            f"{ZetaBuildDir}/DebugDeque.ll",
            f"{ZetaDir}/DebugDeque.cpp",
        )
    )

    builder.Add(
        f"{ZetaDir}/DebugHashMap.h",
        {
            f"{File}",
            f"{ZetaDir}/define.h",
        },
        None
    )

    builder.Add(
        f"{ZetaDir}/DebugHashMap.cpp",
        {
            f"{File}",
            f"{ZetaDir}/DebugHashMap.h",
        },
        None
    )

    builder.Add(
        f"{ZetaBuildDir}/DebugHashMap.ll",
        {
            f"{File}",
            f"{ZetaDir}/DebugHashMap.cpp",
        },
        lambda : cpp_to_ll_func(
            f"{ZetaBuildDir}/DebugHashMap.ll",
            f"{ZetaDir}/DebugHashMap.cpp",
        )
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
        f"{ZetaBuildDir}/DebugPipe.ll",
        {
            f"{File}",
            f"{ZetaDir}/DebugPipe.cpp",
        },
        lambda : cpp_to_ll_func(
            f"{ZetaBuildDir}/DebugPipe.ll",
            f"{ZetaDir}/DebugPipe.cpp",
        )
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
        f"{ZetaBuildDir}/DiskInfo.ll",
        {
            f"{File}",
            f"{ZetaDir}/DiskInfo.c",
        },
        lambda : c_to_ll_func(
            f"{ZetaBuildDir}/DiskInfo.ll",
            f"{ZetaDir}/DiskInfo.c",
        )
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
        f"{ZetaBuildDir}/DiskPartGPT.ll",
        {
            f"{File}",
            f"{ZetaDir}/DiskPartGPT.c",
        },
        lambda : c_to_ll_func(
            f"{ZetaBuildDir}/DiskPartGPT.ll",
            f"{ZetaDir}/DiskPartGPT.c",
        )
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
        f"{ZetaBuildDir}/DiskPartMBR.ll",
        {
            f"{File}",
            f"{ZetaDir}/DiskPartMBR.c",
        },
        lambda : c_to_ll_func(
            f"{ZetaBuildDir}/DiskPartMBR.ll",
            f"{ZetaDir}/DiskPartMBR.c",
        )
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
        f"{ZetaBuildDir}/DummyBlockVector.ll",
        {
            f"{File}",
            f"{ZetaDir}/DummyBlockVector.c",
        },
        lambda : c_to_ll_func(
            f"{ZetaBuildDir}/DummyBlockVector.ll",
            f"{ZetaDir}/DummyBlockVector.c",
        )
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
        f"{ZetaBuildDir}/DummyCacheManager.ll",
        {
            f"{File}",
            f"{ZetaDir}/DummyCacheManager.c",
        },
        lambda : c_to_ll_func(
            f"{ZetaBuildDir}/DummyCacheManager.ll",
            f"{ZetaDir}/DummyCacheManager.c",
        )
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
        f"{ZetaBuildDir}/DiffLinkedListNode.ll",
        {
            f"{File}",
            f"{ZetaDir}/DiffLinkedListNode.c",
        },
        lambda : c_to_ll_func(
            f"{ZetaBuildDir}/DiffLinkedListNode.ll",
            f"{ZetaDir}/DiffLinkedListNode.c",
        )
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
        f"{ZetaBuildDir}/ELF.ll",
        {
            f"{File}",
            f"{ZetaDir}/ELF.c",
        },
        lambda : c_to_ll_func(
            f"{ZetaBuildDir}/ELF.ll",
            f"{ZetaDir}/ELF.c",
        )
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
        f"{ZetaBuildDir}/OrdBinTreeNode.ll",
        {
            f"{File}",
            f"{ZetaDir}/OrdBinTreeNode.c",
        },
        lambda : c_to_ll_func(
            f"{ZetaBuildDir}/OrdBinTreeNode.ll",
            f"{ZetaDir}/OrdBinTreeNode.c",
        )
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
        f"{ZetaBuildDir}/OrdLinkedListNode.ll",
        {
            f"{File}",
            f"{ZetaDir}/OrdLinkedListNode.c",
        },
        lambda : c_to_ll_func(
            f"{ZetaBuildDir}/OrdLinkedListNode.ll",
            f"{ZetaDir}/OrdLinkedListNode.c",
        )
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
        f"{ZetaBuildDir}/OrdRBLinkedListNode.ll",
        {
            f"{File}",
            f"{ZetaDir}/OrdRBLinkedListNode.c",
        },
        lambda : c_to_ll_func(
            f"{ZetaBuildDir}/OrdRBLinkedListNode.ll",
            f"{ZetaDir}/OrdRBLinkedListNode.c",
        )
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
        f"{ZetaBuildDir}/DynamicVector.ll",
        {
            f"{File}",
            f"{ZetaDir}/DynamicVector.c",
        },
        lambda : c_to_ll_func(
            f"{ZetaBuildDir}/DynamicVector.ll",
            f"{ZetaDir}/DynamicVector.c",
        )
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
        f"{ZetaBuildDir}/FileSysFAT32.ll",
        {
            f"{File}",
            f"{ZetaDir}/FileSysFAT32.c",
        },
        lambda : c_to_ll_func(
            f"{ZetaBuildDir}/FileSysFAT32.ll",
            f"{ZetaDir}/FileSysFAT32.c",
        )
    )

    builder.Add(
        f"{ZetaDir}/MultiLevelTable.h",
        {
            f"{File}",
            f"{ZetaDir}/define.h",
        },
        None
    )

    builder.Add(
        f"{ZetaDir}/MultiLevelTable.c",
        {
            f"{File}",
            f"{ZetaDir}/MultiLevelTable.h",
        },
        None
    )

    builder.Add(
        f"{ZetaBuildDir}/MultiLevelTable.ll",
        {
            f"{File}",
            f"{ZetaDir}/MultiLevelTable.c",
        },
        lambda : c_to_ll_func(
            f"{ZetaBuildDir}/MultiLevelTable.ll",
            f"{ZetaDir}/MultiLevelTable.c",
        )
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
        f"{ZetaBuildDir}/io.ll",
        {
            f"{File}",
            f"{ZetaDir}/io.c",
        },
        lambda : c_to_ll_func(
            f"{ZetaBuildDir}/io.ll",
            f"{ZetaDir}/io.c",
        )
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
        f"{ZetaBuildDir}/LRUCacheManager.ll",
        {
            f"{File}",
            f"{ZetaDir}/LRUCacheManager.c",
        },
        lambda : c_to_ll_func(
            f"{ZetaBuildDir}/LRUCacheManager.ll",
            f"{ZetaDir}/LRUCacheManager.c",
        )
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
        f"{ZetaBuildDir}/MultiLevelHashTable.ll",
        {
            f"{File}",
            f"{ZetaDir}/MultiLevelHashTable.c",
        },
        lambda : c_to_ll_func(
            f"{ZetaBuildDir}/MultiLevelHashTable.ll",
            f"{ZetaDir}/MultiLevelHashTable.c",
        )
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
        f"{ZetaBuildDir}/NearCntRBTreeNode.ll",
        {
            f"{File}",
            f"{ZetaDir}/NearCntRBTreeNode.c",
        },
        lambda : c_to_ll_func(
            f"{ZetaBuildDir}/NearCntRBTreeNode.ll",
            f"{ZetaDir}/NearCntRBTreeNode.c",
        )
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
        f"{ZetaBuildDir}/NearRBTreeNode.ll",
        {
            f"{File}",
            f"{ZetaDir}/NearRBTreeNode.c",
        },
        lambda : c_to_ll_func(
            f"{ZetaBuildDir}/NearRBTreeNode.ll",
            f"{ZetaDir}/NearRBTreeNode.c",
        )
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
        f"{ZetaBuildDir}/OrdRBTreeNode.ll",
        {
            f"{File}",
            f"{ZetaDir}/OrdRBTreeNode.c",
        },
        lambda : c_to_ll_func(
            f"{ZetaBuildDir}/OrdRBTreeNode.ll",
            f"{ZetaDir}/OrdRBTreeNode.c",
        )
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
        f"{ZetaBuildDir}/OrdCntRBTreeNode.ll",
        {
            f"{File}",
            f"{ZetaDir}/OrdCntRBTreeNode.c",
        },
        lambda : c_to_ll_func(
            f"{ZetaBuildDir}/OrdCntRBTreeNode.ll",
            f"{ZetaDir}/OrdCntRBTreeNode.c",
        )
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
        f"{ZetaBuildDir}/PoolAllocator.ll",
        {
            f"{File}",
            f"{ZetaDir}/PoolAllocator.c",
        },
        lambda : c_to_ll_func(
            f"{ZetaBuildDir}/PoolAllocator.ll",
            f"{ZetaDir}/PoolAllocator.c",
        )
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
        f"{ZetaBuildDir}/random.ll",
        {
            f"{File}",
            f"{ZetaDir}/random.c",
        },
        lambda : c_to_ll_func(
            f"{ZetaBuildDir}/random.ll",
            f"{ZetaDir}/random.c",
        )
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
        f"{ZetaBuildDir}/RBTree.ll",
        {
            f"{File}",
            f"{ZetaDir}/RBTree.c",
        },
        lambda : c_to_ll_func(
            f"{ZetaBuildDir}/RBTree.ll",
            f"{ZetaDir}/RBTree.c",
        )
    )

    builder.Add(
        f"{ZetaDir}/SegList.h",
        {
            f"{File}",
            f"{ZetaDir}/Allocator.h",
            f"{ZetaDir}/DebugHashMap.h",
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
        f"{ZetaBuildDir}/SegList.ll",
        {
            f"{File}",
            f"{ZetaDir}/SegList.c",
        },
        lambda : c_to_ll_func(
            f"{ZetaBuildDir}/SegList.ll",
            f"{ZetaDir}/SegList.c",
        )
    )

    builder.Add(
        f"{ZetaDir}/SegVector.h",
        {
            f"{File}",
            f"{ZetaDir}/Allocator.h",
            f"{ZetaDir}/Cursor.h",
            f"{ZetaDir}/DebugHashMap.h",
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
        f"{ZetaBuildDir}/SegVector.ll",
        {
            f"{File}",
            f"{ZetaDir}/SegVector.c",
        },
        lambda : c_to_ll_func(
            f"{ZetaBuildDir}/SegVector.ll",
            f"{ZetaDir}/SegVector.c",
        )
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
        f"{ZetaBuildDir}/SeqContainer.ll",
        {
            f"{File}",
            f"{ZetaDir}/SeqContainer.c",
        },
        lambda : c_to_ll_func(
            f"{ZetaBuildDir}/SeqContainer.ll",
            f"{ZetaDir}/SeqContainer.c",
        )
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
        f"{ZetaBuildDir}/SHA256.ll",
        {
            f"{File}",
            f"{ZetaDir}/SHA256.c",
        },
        lambda : c_to_ll_func(
            f"{ZetaBuildDir}/SHA256.ll",
            f"{ZetaDir}/SHA256.c",
        )
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
        f"{ZetaBuildDir}/SlabAllocator.ll",
        {
            f"{File}",
            f"{ZetaDir}/SlabAllocator.c",
        },
        lambda : c_to_ll_func(
            f"{ZetaBuildDir}/SlabAllocator.ll",
            f"{ZetaDir}/SlabAllocator.c",
        )
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
        f"{ZetaBuildDir}/Stream.ll",
        {
            f"{File}",
            f"{ZetaDir}/Stream.c",
        },
        lambda : c_to_ll_func(
            f"{ZetaBuildDir}/Stream.ll",
            f"{ZetaDir}/Stream.c",
        )
    )

    builder.Add(
        f"{ZetaDir}/TreeAllocator.h",
        {
            f"{File}",
            f"{ZetaDir}/Allocator.h",
            f"{ZetaDir}/DebugHashMap.h",
            f"{ZetaDir}/OrdRBLinkedListNode.h",
            f"{ZetaDir}/OrdRBTreeNode.h",
        },
        None
    )

    builder.Add(
        f"{ZetaDir}/TreeAllocator.c",
        {
            f"{File}",
            f"{ZetaDir}/TreeAllocator.h",
            f"{ZetaDir}/DebugHashMap.h",
            f"{ZetaDir}/RBTree.h",
            f"{ZetaDir}/OrdLinkedListNode.h",
            f"{ZetaDir}/utils.h",
        },
        None
    )

    builder.Add(
        f"{ZetaBuildDir}/TreeAllocator.ll",
        {
            f"{File}",
            f"{ZetaDir}/TreeAllocator.c",
        },
        lambda : c_to_ll_func(
            f"{ZetaBuildDir}/TreeAllocator.ll",
            f"{ZetaDir}/TreeAllocator.c",
        )
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
        f"{ZetaBuildDir}/UTF8.ll",
        {
            f"{File}",
            f"{ZetaDir}/UTF8.c",
        },
        lambda : c_to_ll_func(
            f"{ZetaBuildDir}/UTF8.ll",
            f"{ZetaDir}/UTF8.c",
        )
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
        f"{ZetaBuildDir}/UTF16.ll",
        {
            f"{File}",
            f"{ZetaDir}/UTF16.c",
        },
        lambda : c_to_ll_func(
            f"{ZetaBuildDir}/UTF16.ll",
            f"{ZetaDir}/UTF16.c",
        )
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
        f"{ZetaBuildDir}/utils.ll",
        {
            f"{File}",
            f"{ZetaDir}/utils.c",
        },
        lambda : c_to_ll_func(
            f"{ZetaBuildDir}/utils.ll",
            f"{ZetaDir}/utils.c",
        )
    )

def main():
    parser = argparse.ArgumentParser()

    parser.add_argument("--target",
                        dest="target",
                        action="store",
                        required=True,)

    parser.add_argument("--mode",
                        dest="mode",
                        action="store",
                        choices=["debug", "release"],
                        required=True,)

    parser.add_argument("--run_target",
                        dest="run_target",
                        action="store_true")

    parser.add_argument("--rebuild",
                        dest="rebuild",
                        action="store_true")

    args = parser.parse_args(sys.argv[1:])

    print(f"args = {args}")

    mode = {
        "debug": DEBUG_MODE,
        "release": RELEASE_MODE,
    }[args.mode]

    ZetaBuildDir = None

    if mode == DEBUG_MODE:
        ZetaBuildDir = f"{ZetaDevDir}/ZetaDebugBuild"

    if mode == RELEASE_MODE:
        ZetaBuildDir = f"{ZetaDevDir}/ZetaReleaseBuild"

    builder = Builder()

    AddDeps(builder, ZetaBuildDir, mode)
    builder.Add("all", builder.units(), None)

    target = f"{ZetaBuildDir}/{args.target}"

    non_built, built = builder.build(target, args.rebuild)

    ybeg = "\033[93m"
    yend = "\033[0m"

    print("success")

    non_built.sort()
    built.sort()

    print(f"{ybeg}not_built:{yend}")

    for i in non_built:
        print(f"\t{i}")

    print(f"{ybeg}built:{yend}")

    for i in built:
        print(f"\t{i}")

    print(f"{ybeg}target:{yend} {target}")

    if args.run_target:
        print(f"{ybeg}running{yend} {target}")
        os.system(target)

if __name__ == "__main__":
    main()
