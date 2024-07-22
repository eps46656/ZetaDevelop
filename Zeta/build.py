import os
import sys

File = os.path.abspath(__file__).replace("\\", "/")
Dir = os.path.abspath(os.path.dirname(__file__)).replace("\\", "/")

ZetaDir = os.path.dirname(File).replace("\\", "/")

ZetaDevDir = os.path.abspath(f"{ZetaDir}/..").replace("\\", "/")

import argparse

sys.path.append(ZetaDevDir)

from Builder import *
from Compiler import *

DEBUG_MODE = 0
RELEASE_MODE = 1

def AddDeps(builder, ZetaBuildDir, verbose, mode):
    compiler_config = EmptyClass

    compiler_config.verbose = verbose
    compiler_config.mode = mode
    compiler_config.build_dir = ZetaBuildDir

    compiler_config.target = EmptyClass
    compiler_config.target.arch = target.arch
    compiler_config.target.vendor = target.vendor
    compiler_config.target.sys = target.sys
    compiler_config.target.env = target.env

    compiler = Compiler(compiler_config)

    # --------------------------------------------------------------------------

    builder.Add(
        f"{ZetaDevDir}/Compiler.py",
        set(),
        None
    )

    builder.Add(
        f"{File}",
        {
            f"{ZetaDevDir}/Compiler.py",
        },
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
        lambda : compiler.c_to_ll(
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
        lambda : compiler.c_to_ll(
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
        lambda : compiler.c_to_ll(
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
            f"{ZetaDir}/Debugger.h",
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
        lambda : compiler.c_to_ll(
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
        lambda : compiler.c_to_ll(
            f"{ZetaBuildDir}/BlockVector.ll",
            f"{ZetaDir}/BlockVector.c",
        )
    )

    builder.Add(
        f"{ZetaDir}/BTree.h",
        {
            f"{File}",
            f"{ZetaDir}/define.h",
        },
        None
    )

    builder.Add(
        f"{ZetaDir}/BTree.c",
        {
            f"{File}",
            f"{ZetaDir}/BTree.h",
            f"{ZetaDir}/CircularVector.h",
            f"{ZetaDir}/utils.h",
        },
        None
    )

    builder.Add(
        f"{ZetaBuildDir}/BTree.ll",
        {
            f"{File}",
            f"{ZetaDir}/BTree.c",
        },
        lambda : compiler.c_to_ll(
            f"{ZetaBuildDir}/BTree.ll",
            f"{ZetaDir}/BTree.c",
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
        lambda : compiler.c_to_ll(
            f"{ZetaBuildDir}/CacheManager.ll",
            f"{ZetaDir}/CacheManager.c",
        )
    )

    builder.Add(
        f"{ZetaDir}/Logger.h",
        {
            f"{File}",
            f"{ZetaDir}/io.h",
        },
        None
    )

    builder.Add(
        f"{ZetaDir}/Logger.c",
        {
            f"{File}",
            f"{ZetaDir}/Logger.h",
            f"{ZetaDir}/Pipe.h",
        },
        None
    )

    builder.Add(
        f"{ZetaBuildDir}/Logger.ll",
        {
            f"{File}",
            f"{ZetaDir}/Logger.c",
        },
        lambda : compiler.c_to_ll(
            f"{ZetaBuildDir}/Logger.ll",
            f"{ZetaDir}/Logger.c",
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
            f"{ZetaDir}/Debugger.h",
            f"{ZetaDir}/utils.h",
        },
        None
    )

    builder.Add(
        f"{ZetaBuildDir}/CircularVector.ll",
        {
            f"{File}",
            f"{ZetaDir}/CircularVector.c",
        },
        lambda : compiler.c_to_ll(
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
        lambda : compiler.c_to_ll(
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
        f"{ZetaDir}/DateTime.h",
        {
            f"{File}",
            f"{ZetaDir}/define.h",
        },
        None
    )

    builder.Add(
        f"{ZetaDir}/DateTime.c",
        {
            f"{File}",
            f"{ZetaDir}/DateTime.h",
        },
        None
    )

    builder.Add(
        f"{ZetaBuildDir}/DateTime.ll",
        {
            f"{File}",
            f"{ZetaDir}/DateTime.c",
        },
        lambda : compiler.c_to_ll(
            f"{ZetaBuildDir}/DateTime.ll",
            f"{ZetaDir}/DateTime.c",
        )
    )

    builder.Add(
        f"{ZetaDir}/DebugDeque.h",
        {
            f"{File}",
            f"{ZetaDir}/SeqContainer.h",
        },
        None
    )

    builder.Add(
        f"{ZetaDir}/DebugDeque.cpp",
        {
            f"{File}",
            f"{ZetaDir}/DebugDeque.h",

            f"{ZetaDir}/Debugger.h",
            f"{ZetaDir}/utils.h",
        },
        None
    )

    builder.Add(
        f"{ZetaBuildDir}/DebugDeque.ll",
        {
            f"{File}",
            f"{ZetaDir}/DebugDeque.cpp",
        },
        lambda : compiler.cpp_to_ll(
            f"{ZetaBuildDir}/DebugDeque.ll",
            f"{ZetaDir}/DebugDeque.cpp",
        )
    )

    builder.Add(
        f"{ZetaDir}/Debugger.h",
        {
            f"{File}",
            f"{ZetaDir}/Logger.h",
        },
        None
    )

    builder.Add(
        f"{ZetaDir}/Debugger.c",
        {
            f"{File}",
            f"{ZetaDir}/Debugger.h",
            f"{ZetaDir}/DebugStrPipe.h",
            f"{ZetaDir}/Pipe.h",
        },
        None
    )

    builder.Add(
        f"{ZetaBuildDir}/Debugger.ll",
        {
            f"{File}",
            f"{ZetaDir}/Debugger.c",
        },
        lambda : compiler.c_to_ll(
            f"{ZetaBuildDir}/Debugger.ll",
            f"{ZetaDir}/Debugger.c",
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
            f"{ZetaDir}/Debugger.h",
        },
        None
    )

    builder.Add(
        f"{ZetaBuildDir}/DebugHashMap.ll",
        {
            f"{File}",
            f"{ZetaDir}/DebugHashMap.cpp",
        },
        lambda : compiler.cpp_to_ll(
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
        lambda : compiler.cpp_to_ll(
            f"{ZetaBuildDir}/DebugPipe.ll",
            f"{ZetaDir}/DebugPipe.cpp",
        )
    )

    builder.Add(
        f"{ZetaDir}/DebugStrPipe.h",
        {
            f"{File}",
            f"{ZetaDir}/define.h",
        },
        None
    )

    builder.Add(
        f"{ZetaDir}/DebugStrPipe.cpp",
        {
            f"{File}",
            f"{ZetaDir}/DebugStrPipe.h",
        },
        None
    )

    builder.Add(
        f"{ZetaBuildDir}/DebugStrPipe.ll",
        {
            f"{File}",
            f"{ZetaDir}/DebugStrPipe.cpp",
        },
        lambda : compiler.cpp_to_ll(
            f"{ZetaBuildDir}/DebugStrPipe.ll",
            f"{ZetaDir}/DebugStrPipe.cpp",
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
        lambda : compiler.c_to_ll(
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
        lambda : compiler.c_to_ll(
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
        lambda : compiler.c_to_ll(
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
        lambda : compiler.c_to_ll(
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
        lambda : compiler.c_to_ll(
            f"{ZetaBuildDir}/DummyCacheManager.ll",
            f"{ZetaDir}/DummyCacheManager.c",
        )
    )

    builder.Add(
        f"{ZetaDir}/DummyVector.h",
        {
            f"{File}",
            f"{ZetaDir}/SeqContainer.h",
        },
        None
    )

    builder.Add(
        f"{ZetaDir}/DummyVector.c",
        {
            f"{File}",
            f"{ZetaDir}/DummyVector.h",

            f"{ZetaDir}/CircularVector.h",
            f"{ZetaDir}/RBTree.h",
            f"{ZetaDir}/utils.h",
        },
        None
    )

    builder.Add(
        f"{ZetaBuildDir}/DummyVector.ll",
        {
            f"{File}",
            f"{ZetaDir}/DummyVector.c",
        },
        lambda : compiler.c_to_ll(
            f"{ZetaBuildDir}/DummyVector.ll",
            f"{ZetaDir}/DummyVector.c",
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
        lambda : compiler.c_to_ll(
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
        lambda : compiler.c_to_ll(
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
        lambda : compiler.c_to_ll(
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
        lambda : compiler.c_to_ll(
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
        lambda : compiler.c_to_ll(
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
        lambda : compiler.c_to_ll(
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
        lambda : compiler.c_to_ll(
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
        lambda : compiler.c_to_ll(
            f"{ZetaBuildDir}/MultiLevelTable.ll",
            f"{ZetaDir}/MultiLevelTable.c",
        )
    )

    builder.Add(
        f"{ZetaDir}/io.h",
        {
            f"{File}",
            f"{ZetaDir}/Pipe.h",
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
        lambda : compiler.c_to_ll(
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
        lambda : compiler.c_to_ll(
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
        lambda : compiler.c_to_ll(
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
        lambda : compiler.c_to_ll(
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
        lambda : compiler.c_to_ll(
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
            f"{ZetaDir}/Debugger.h",
        },
        None
    )

    builder.Add(
        f"{ZetaBuildDir}/OrdRBTreeNode.ll",
        {
            f"{File}",
            f"{ZetaDir}/OrdRBTreeNode.c",
        },
        lambda : compiler.c_to_ll(
            f"{ZetaBuildDir}/OrdRBTreeNode.ll",
            f"{ZetaDir}/OrdRBTreeNode.c",
        )
    )

    builder.Add(
        f"{ZetaDir}/OrdCnt3RBTreeNode.h",
        {
            f"{File}",
            f"{ZetaDir}/BinTree.h",
        },
        None
    )

    builder.Add(
        f"{ZetaDir}/OrdCnt3RBTreeNode.c",
        {
            f"{File}",
            f"{ZetaDir}/OrdCntRBTreeNode.h",
            f"{ZetaDir}/Debugger.h",
        },
        None
    )

    builder.Add(
        f"{ZetaBuildDir}/OrdCnt3RBTreeNode.ll",
        {
            f"{File}",
            f"{ZetaDir}/OrdCnt3RBTreeNode.c",
        },
        lambda : compiler.c_to_ll(
            f"{ZetaBuildDir}/OrdCnt3RBTreeNode.ll",
            f"{ZetaDir}/OrdCnt3RBTreeNode.c",
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
        lambda : compiler.c_to_ll(
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
        lambda : compiler.c_to_ll(
            f"{ZetaBuildDir}/PoolAllocator.ll",
            f"{ZetaDir}/PoolAllocator.c",
        )
    )

    builder.Add(
        f"{ZetaDir}/QTree.h",
        {
            f"{File}",
            f"{ZetaDir}/define.h",
        },
        None
    )

    builder.Add(
        f"{ZetaDir}/QTree.c",
        {
            f"{File}",
            f"{ZetaDir}/QTree.h",
            f"{ZetaDir}/CircularVector.h",
            f"{ZetaDir}/SegUtils.h",
            f"{ZetaDir}/utils.h",
        },
        None
    )

    builder.Add(
        f"{ZetaBuildDir}/QTree.ll",
        {
            f"{File}",
            f"{ZetaDir}/QTree.c",
        },
        lambda : compiler.c_to_ll(
            f"{ZetaBuildDir}/QTree.ll",
            f"{ZetaDir}/QTree.c",
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
        lambda : compiler.c_to_ll(
            f"{ZetaBuildDir}/random.ll",
            f"{ZetaDir}/random.c",
        )
    )

    builder.Add(
        f"{ZetaDir}/RadixDeque.h",
        {
            f"{File}",
            f"{ZetaDir}/Allocator.h",
            f"{ZetaDir}/SeqContainer.h",
        },
        None
    )

    builder.Add(
        f"{ZetaDir}/RadixDeque.c",
        {
            f"{File}",
            f"{ZetaDir}/RadixDeque.h",

            f"{ZetaDir}/Debugger.h",
            f"{ZetaDir}/CircularVector.h",
            f"{ZetaDir}/utils.h",
        },
        None
    )

    builder.Add(
        f"{ZetaBuildDir}/RadixDeque.ll",
        {
            f"{File}",
            f"{ZetaDir}/RadixDeque.c",
        },
        lambda : compiler.c_to_ll(
            f"{ZetaBuildDir}/RadixDeque.ll",
            f"{ZetaDir}/RadixDeque.c",
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
            f"{ZetaDir}/Debugger.h",
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
        lambda : compiler.c_to_ll(
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
        lambda : compiler.c_to_ll(
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
        lambda : compiler.c_to_ll(
            f"{ZetaBuildDir}/SegVector.ll",
            f"{ZetaDir}/SegVector.c",
        )
    )

    builder.Add(
        f"{ZetaDir}/SegUtils.h",
        {
            f"{File}",
            f"{ZetaDir}/CircularVector.h",
        },
        None
    )

    builder.Add(
        f"{ZetaDir}/SegUtils.c",
        {
            f"{File}",
            f"{ZetaDir}/SegUtils.h",
            f"{ZetaDir}/Debugger.h",
            f"{ZetaDir}/utils.h",
        },
        None
    )

    builder.Add(
        f"{ZetaBuildDir}/SegUtils.ll",
        {
            f"{File}",
            f"{ZetaDir}/SegUtils.c",
        },
        lambda : compiler.c_to_ll(
            f"{ZetaBuildDir}/SegUtils.ll",
            f"{ZetaDir}/SegUtils.c",
        )
    )

    builder.Add(
        f"{ZetaDir}/StageVector.h",
        {
            f"{File}",
            f"{ZetaDir}/Allocator.h",
            f"{ZetaDir}/Cursor.h",
            f"{ZetaDir}/DebugHashMap.h",
            f"{ZetaDir}/OrdCnt3RBTreeNode.h",
            f"{ZetaDir}/SeqContainer.h",
        },
        None
    )

    builder.Add(
        f"{ZetaDir}/StageVector.c",
        {
            f"{File}",
            f"{ZetaDir}/StageVector.h",

            f"{ZetaDir}/CircularVector.h",
            f"{ZetaDir}/Debugger.h",
            f"{ZetaDir}/RBTree.h",
            f"{ZetaDir}/SegUtils.h",
            f"{ZetaDir}/utils.h",
        },
        None
    )

    builder.Add(
        f"{ZetaBuildDir}/StageVector.ll",
        {
            f"{File}",
            f"{ZetaDir}/StageVector.c",
        },
        lambda : compiler.c_to_ll(
            f"{ZetaBuildDir}/StageVector.ll",
            f"{ZetaDir}/StageVector.c",
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
        lambda : compiler.c_to_ll(
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
        lambda : compiler.c_to_ll(
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
        lambda : compiler.c_to_ll(
            f"{ZetaBuildDir}/SlabAllocator.ll",
            f"{ZetaDir}/SlabAllocator.c",
        )
    )

    builder.Add(
        f"{ZetaDir}/Pipe.h",
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
            f"{ZetaDir}/Pipe.h",
        },
        None
    )

    builder.Add(
        f"{ZetaBuildDir}/Stream.ll",
        {
            f"{File}",
            f"{ZetaDir}/Stream.c",
        },
        lambda : compiler.c_to_ll(
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
            f"{ZetaDir}/Debugger.h",
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
        lambda : compiler.c_to_ll(
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
        lambda : compiler.c_to_ll(
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
        lambda : compiler.c_to_ll(
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
            f"{ZetaDir}/Debugger.h",
        },
        None
    )

    builder.Add(
        f"{ZetaBuildDir}/utils.ll",
        {
            f"{File}",
            f"{ZetaDir}/utils.c",
        },
        lambda : compiler.c_to_ll(
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

    parser.add_argument("--verbose",
                        dest="verbose",
                        action="store_true")

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

    AddDeps(builder, ZetaBuildDir, args.verbose, mode)
    builder.Add("all", builder.GetUnits(), None)

    target = f"{ZetaBuildDir}/{args.target}"

    non_built, built = builder.Build(target, args.rebuild)

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
