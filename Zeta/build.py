import os
import sys
import argparse

sys.path.append(f"{os.path.dirname(__file__)}/..")

from utils import *
from Builder import *
from Compiler import *

File = GetNormPath(__file__)
Dir = GetNormPath(os.path.dirname(__file__))

ZetaDir = GetNormPath(os.path.dirname(File))
ZetaDevDir = GetNormPath(f"{ZetaDir}/..")

def AddDeps(builder, ZetaBuildDir, verbose, mode):
    compiler_config = EmptyClass

    compiler_config.verbose = verbose
    compiler_config.mode = mode
    compiler_config.build_dir = ZetaBuildDir

    compiler_config.working_dirs = [
        ZetaDevDir,
        ZetaDir,
    ]

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
            f"{ZetaDir}/utils.h",
        },
        None
    )

    builder.Add(
        f"{ZetaDir}/Algorithm.c",
        {
            f"{File}",
            f"{ZetaDir}/Algorithm.h",
            f"{ZetaDir}/Debugger.h",
        },
        None
    )

    builder.Add(
        f"{ZetaBuildDir}/Algorithm.o",
        {
            f"{File}",
            f"{ZetaDir}/Algorithm.c",
        },
        lambda : compiler.c_to_obj(
            f"{ZetaBuildDir}/Algorithm.o",
            f"{ZetaDir}/Algorithm.c",
        )
    )

    builder.Add(
        f"{ZetaDir}/Allocator.h",
        {
            f"{File}",
            f"{ZetaDir}/MemCheck.h",
        },
        None
    )

    builder.Add(
        f"{ZetaDir}/Allocator.c",
        {
            f"{File}",
            f"{ZetaDir}/Allocator.h",
            f"{ZetaDir}/Debugger.h",
        },
        None
    )

    builder.Add(
        f"{ZetaBuildDir}/Allocator.o",
        {
            f"{File}",
            f"{ZetaDir}/Allocator.c",
        },
        lambda : compiler.c_to_obj(
            f"{ZetaBuildDir}/Allocator.o",
            f"{ZetaDir}/Allocator.c",
        )
    )

    builder.Add(
        f"{ZetaDir}/AssocContainer.h",
        {
            f"{File}",
            f"{ZetaDir}/define.h",
        },
        None
    )

    builder.Add(
        f"{ZetaDir}/AssocContainer.c",
        {
            f"{File}",
            f"{ZetaDir}/AssocContainer.h",
            f"{ZetaDir}/Debugger.h",
        },
        None
    )

    builder.Add(
        f"{ZetaBuildDir}/AssocContainer.o",
        {
            f"{File}",
            f"{ZetaDir}/AssocContainer.c",
        },
        lambda : compiler.c_to_obj(
            f"{ZetaBuildDir}/AssocContainer.o",
            f"{ZetaDir}/AssocContainer.c",
        )
    )

    builder.Add(
        f"{ZetaDir}/BinHeap.h",
        {
            f"{File}",
            f"{ZetaDir}/define.h",
        },
        None
    )

    builder.Add(
        f"{ZetaDir}/BinHeap.c",
        {
            f"{File}",
            f"{ZetaDir}/BinHeap.h",

            f"{ZetaDir}/Debugger.h",
            f"{ZetaDir}/utils.h",
        },
        None
    )

    builder.Add(
        f"{ZetaBuildDir}/BinHeap.o",
        {
            f"{File}",
            f"{ZetaDir}/BinHeap.c",
        },
        lambda : compiler.c_to_obj(
            f"{ZetaBuildDir}/BinHeap.o",
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
        f"{ZetaBuildDir}/BinTree.o",
        {
            f"{File}",
            f"{ZetaDir}/BinTree.c",
        },
        lambda : compiler.c_to_obj(
            f"{ZetaBuildDir}/BinTree.o",
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
        f"{ZetaBuildDir}/BlockVector.o",
        {
            f"{File}",
            f"{ZetaDir}/BlockVector.c",
        },
        lambda : compiler.c_to_obj(
            f"{ZetaBuildDir}/BlockVector.o",
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
            f"{ZetaDir}/CircularArray.h",
            f"{ZetaDir}/utils.h",
        },
        None
    )

    builder.Add(
        f"{ZetaBuildDir}/BTree.o",
        {
            f"{File}",
            f"{ZetaDir}/BTree.c",
        },
        lambda : compiler.c_to_obj(
            f"{ZetaBuildDir}/BTree.o",
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
        f"{ZetaBuildDir}/CacheManager.o",
        {
            f"{File}",
            f"{ZetaDir}/CacheManager.c",
        },
        lambda : compiler.c_to_obj(
            f"{ZetaBuildDir}/CacheManager.o",
            f"{ZetaDir}/CacheManager.c",
        )
    )

    builder.Add(
        f"{ZetaDir}/CircularArray.h",
        {
            f"{File}",
            f"{ZetaDir}/SeqContainer.h",
        },
        None
    )

    builder.Add(
        f"{ZetaDir}/CircularArray.c",
        {
            f"{File}",
            f"{ZetaDir}/CircularArray.h",
            f"{ZetaDir}/Debugger.h",
            f"{ZetaDir}/utils.h",
        },
        None
    )

    builder.Add(
        f"{ZetaBuildDir}/CircularArray.o",
        {
            f"{File}",
            f"{ZetaDir}/CircularArray.c",
        },
        lambda : compiler.c_to_obj(
            f"{ZetaBuildDir}/CircularArray.o",
            f"{ZetaDir}/CircularArray.c",
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
            f"{ZetaDir}/Debugger.h",
        },
        None
    )

    builder.Add(
        f"{ZetaBuildDir}/CRC.o",
        {
            f"{File}",
            f"{ZetaDir}/CRC.c",
        },
        lambda : compiler.c_to_obj(
            f"{ZetaBuildDir}/CRC.o",
            f"{ZetaDir}/CRC.c",
        )
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
            f"{ZetaDir}/Debugger.h",
        },
        None
    )

    builder.Add(
        f"{ZetaBuildDir}/DateTime.o",
        {
            f"{File}",
            f"{ZetaDir}/DateTime.c",
        },
        lambda : compiler.c_to_obj(
            f"{ZetaBuildDir}/DateTime.o",
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
        f"{ZetaBuildDir}/DebugDeque.o",
        {
            f"{File}",
            f"{ZetaDir}/DebugDeque.cpp",
        },
        lambda : compiler.cpp_to_obj(
            f"{ZetaBuildDir}/DebugDeque.o",
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
        f"{ZetaBuildDir}/Debugger.o",
        {
            f"{File}",
            f"{ZetaDir}/Debugger.c",
        },
        lambda : compiler.c_to_obj(
            f"{ZetaBuildDir}/Debugger.o",
            f"{ZetaDir}/Debugger.c",
        )
    )

    builder.Add(
        f"{ZetaDir}/DebugHashTable.h",
        {
            f"{File}",
            f"{ZetaDir}/AssocContainer.h",
        },
        None
    )

    builder.Add(
        f"{ZetaDir}/DebugHashTable.cpp",
        {
            f"{File}",
            f"{ZetaDir}/DebugHashTable.h",
            f"{ZetaDir}/Debugger.h",
        },
        None
    )

    builder.Add(
        f"{ZetaBuildDir}/DebugHashTable.o",
        {
            f"{File}",
            f"{ZetaDir}/DebugHashTable.cpp",
        },
        lambda : compiler.cpp_to_obj(
            f"{ZetaBuildDir}/DebugHashTable.o",
            f"{ZetaDir}/DebugHashTable.cpp",
        )
    )

    builder.Add(
        f"{ZetaDir}/DebugPipe.h",
        {
            f"{File}",
            f"{ZetaDir}/SeqContainer.h",
        },
        None
    )

    builder.Add(
        f"{ZetaDir}/DebugPipe.cpp",
        {
            f"{File}",
            f"{ZetaDir}/DebugPipe.h",
            f"{ZetaDir}/Debugger.h",
        },
        None
    )

    builder.Add(
        f"{ZetaBuildDir}/DebugPipe.o",
        {
            f"{File}",
            f"{ZetaDir}/DebugPipe.cpp",
        },
        lambda : compiler.cpp_to_obj(
            f"{ZetaBuildDir}/DebugPipe.o",
            f"{ZetaDir}/DebugPipe.cpp",
        )
    )

    builder.Add(
        f"{ZetaDir}/DebugStrPipe.h",
        {
            f"{File}",
            f"{ZetaDir}/Pipe.h",
        },
        None
    )

    builder.Add(
        f"{ZetaDir}/DebugStrPipe.cpp",
        {
            f"{File}",
            f"{ZetaDir}/DebugStrPipe.h",
            f"{ZetaDir}/Debugger.h",
        },
        None
    )

    builder.Add(
        f"{ZetaBuildDir}/DebugStrPipe.o",
        {
            f"{File}",
            f"{ZetaDir}/DebugStrPipe.cpp",
        },
        lambda : compiler.cpp_to_obj(
            f"{ZetaBuildDir}/DebugStrPipe.o",
            f"{ZetaDir}/DebugStrPipe.cpp",
        )
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
        f"{ZetaBuildDir}/DiskInfo.o",
        {
            f"{File}",
            f"{ZetaDir}/DiskInfo.c",
        },
        lambda : compiler.c_to_obj(
            f"{ZetaBuildDir}/DiskInfo.o",
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
        f"{ZetaBuildDir}/DiskPartGPT.o",
        {
            f"{File}",
            f"{ZetaDir}/DiskPartGPT.c",
        },
        lambda : compiler.c_to_obj(
            f"{ZetaBuildDir}/DiskPartGPT.o",
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
        f"{ZetaBuildDir}/DiskPartMBR.o",
        {
            f"{File}",
            f"{ZetaDir}/DiskPartMBR.c",
        },
        lambda : compiler.c_to_obj(
            f"{ZetaBuildDir}/DiskPartMBR.o",
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
            f"{ZetaDir}/Debugger.h",
            f"{ZetaDir}/utils.h",
        },
        None
    )

    builder.Add(
        f"{ZetaBuildDir}/DummyBlockVector.o",
        {
            f"{File}",
            f"{ZetaDir}/DummyBlockVector.c",
        },
        lambda : compiler.c_to_obj(
            f"{ZetaBuildDir}/DummyBlockVector.o",
            f"{ZetaDir}/DummyBlockVector.c",
        )
    )

    builder.Add(
        f"{ZetaDir}/DummyCacheManager.h",
        {
            f"{File}",
            f"{ZetaDir}/BlockVector.h",
            f"{ZetaDir}/CacheManager.h",
        },
        None
    )

    builder.Add(
        f"{ZetaDir}/DummyCacheManager.c",
        {
            f"{File}",
            f"{ZetaDir}/DummyCacheManager.h",
            f"{ZetaDir}/Debugger.h",
        },
        None
    )

    builder.Add(
        f"{ZetaBuildDir}/DummyCacheManager.o",
        {
            f"{File}",
            f"{ZetaDir}/DummyCacheManager.c",
        },
        lambda : compiler.c_to_obj(
            f"{ZetaBuildDir}/DummyCacheManager.o",
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
            f"{ZetaDir}/Debugger.h",
        },
        None
    )

    builder.Add(
        f"{ZetaBuildDir}/DummyVector.o",
        {
            f"{File}",
            f"{ZetaDir}/DummyVector.c",
        },
        lambda : compiler.c_to_obj(
            f"{ZetaBuildDir}/DummyVector.o",
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
        f"{ZetaBuildDir}/DiffLinkedListNode.o",
        {
            f"{File}",
            f"{ZetaDir}/DiffLinkedListNode.c",
        },
        lambda : compiler.c_to_obj(
            f"{ZetaBuildDir}/DiffLinkedListNode.o",
            f"{ZetaDir}/DiffLinkedListNode.c",
        )
    )

    builder.Add(
        f"{ZetaDir}/DynamicHashTable.h",
        {
            f"{File}",
            f"{ZetaDir}/Allocator.h",
            f"{ZetaDir}/AssocContainer.h",
            f"{ZetaDir}/GenericHashTable.h",
            f"{ZetaDir}/MemCheck.h",
            f"{ZetaDir}/OrdLinkedListNode.h",
        },
        None
    )

    builder.Add(
        f"{ZetaDir}/DynamicHashTable.c",
        {
            f"{File}",

            f"{ZetaDir}/DynamicHashTable.h",
            f"{ZetaDir}/Debugger.h",
            f"{ZetaDir}/utils.h",
        },
        None
    )

    builder.Add(
        f"{ZetaBuildDir}/DynamicHashTable.o",
        {
            f"{File}",
            f"{ZetaDir}/DynamicHashTable.c",
        },
        lambda : compiler.c_to_obj(
            f"{ZetaBuildDir}/DynamicHashTable.o",
            f"{ZetaDir}/DynamicHashTable.c",
        )
    )

    builder.Add(
        f"{ZetaDir}/DynamicVector.h",
        {
            f"{File}",
            f"{ZetaDir}/Allocator.h",
            f"{ZetaDir}/MemCheck.h",
            f"{ZetaDir}/SeqContainer.h",
        },
        None
    )

    builder.Add(
        f"{ZetaDir}/DynamicVector.c",
        {
            f"{File}",
            f"{ZetaDir}/DynamicVector.h",
            f"{ZetaDir}/CircularArray.h",
            f"{ZetaDir}/Debugger.h",
            f"{ZetaDir}/utils.h",
        },
        None
    )

    builder.Add(
        f"{ZetaBuildDir}/DynamicVector.o",
        {
            f"{File}",
            f"{ZetaDir}/DynamicVector.c",
        },
        lambda : compiler.c_to_obj(
            f"{ZetaBuildDir}/DynamicVector.o",
            f"{ZetaDir}/DynamicVector.c",
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
            f"{ZetaDir}/Debugger.h",
            f"{ZetaDir}/utils.h",
        },
        None
    )

    builder.Add(
        f"{ZetaBuildDir}/ELF.o",
        {
            f"{File}",
            f"{ZetaDir}/ELF.c",
        },
        lambda : compiler.c_to_obj(
            f"{ZetaBuildDir}/ELF.o",
            f"{ZetaDir}/ELF.c",
        )
    )

    builder.Add(
        f"{ZetaDir}/GenericHashTable.h",
        {
            f"{File}",
            f"{ZetaDir}/Allocator.h",
            f"{ZetaDir}/MemCheck.h",
            f"{ZetaDir}/OrdRBTreeNode.h",
        },
        None
    )

    builder.Add(
        f"{ZetaDir}/GenericHashTable.c",
        {
            f"{File}",
            f"{ZetaDir}/GenericHashTable.h",
            f"{ZetaDir}/Debugger.h",
            f"{ZetaDir}/MemCheck.h",
            f"{ZetaDir}/RBTree.h",
            f"{ZetaDir}/utils.h",
        },
        None
    )

    builder.Add(
        f"{ZetaBuildDir}/GenericHashTable.o",
        {
            f"{File}",
            f"{ZetaDir}/GenericHashTable.c",
        },
        lambda : compiler.c_to_obj(
            f"{ZetaBuildDir}/GenericHashTable.o",
            f"{ZetaDir}/GenericHashTable.c",
        )
    )

    builder.Add(
        f"{ZetaDir}/GroupBinHeap.h",
        {
            f"{File}",
            f"{ZetaDir}/define.h",
        },
        None
    )

    builder.Add(
        f"{ZetaDir}/GroupBinHeap.c",
        {
            f"{File}",
            f"{ZetaDir}/GroupBinHeap.h",

            f"{ZetaDir}/Debugger.h",
            f"{ZetaDir}/utils.h",
        },
        None
    )

    builder.Add(
        f"{ZetaBuildDir}/GroupBinHeap.o",
        {
            f"{File}",
            f"{ZetaDir}/GroupBinHeap.c",
        },
        lambda : compiler.c_to_obj(
            f"{ZetaBuildDir}/GroupBinHeap.o",
            f"{ZetaDir}/GroupBinHeap.c",
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
        f"{ZetaBuildDir}/FileSysFAT32.o",
        {
            f"{File}",
            f"{ZetaDir}/FileSysFAT32.c",
        },
        lambda : compiler.c_to_obj(
            f"{ZetaBuildDir}/FileSysFAT32.o",
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
        f"{ZetaBuildDir}/MultiLevelTable.o",
        {
            f"{File}",
            f"{ZetaDir}/MultiLevelTable.c",
        },
        lambda : compiler.c_to_obj(
            f"{ZetaBuildDir}/MultiLevelTable.o",
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
            f"{ZetaDir}/Debugger.h",
            f"{ZetaDir}/utils.h",
        },
        None
    )

    builder.Add(
        f"{ZetaBuildDir}/io.o",
        {
            f"{File}",
            f"{ZetaDir}/io.c",
        },
        lambda : compiler.c_to_obj(
            f"{ZetaBuildDir}/io.o",
            f"{ZetaDir}/io.c",
        )
    )

    builder.Add(
        f"{ZetaDir}/MemCheck.h",
        {
            f"{File}",
            f"{ZetaDir}/define.h",
        },
        None
    )

    builder.Add(
        f"{ZetaDir}/MemCheck.cpp",
        {
            f"{File}",
            f"{ZetaDir}/MemCheck.h",
            f"{ZetaDir}/Debugger.h",
            f"{ZetaDir}/utils.h",
        },
        None
    )

    builder.Add(
        f"{ZetaBuildDir}/MemCheck.o",
        {
            f"{File}",
            f"{ZetaDir}/MemCheck.cpp",
        },
        lambda : compiler.cpp_to_obj(
            f"{ZetaBuildDir}/MemCheck.o",
            f"{ZetaDir}/MemCheck.cpp",
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
        f"{ZetaBuildDir}/Logger.o",
        {
            f"{File}",
            f"{ZetaDir}/Logger.c",
        },
        lambda : compiler.c_to_obj(
            f"{ZetaBuildDir}/Logger.o",
            f"{ZetaDir}/Logger.c",
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
        f"{ZetaBuildDir}/LRUCacheManager.o",
        {
            f"{File}",
            f"{ZetaDir}/LRUCacheManager.c",
        },
        lambda : compiler.c_to_obj(
            f"{ZetaBuildDir}/LRUCacheManager.o",
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
        f"{ZetaBuildDir}/MultiLevelHashTable.o",
        {
            f"{File}",
            f"{ZetaDir}/MultiLevelHashTable.c",
        },
        lambda : compiler.c_to_obj(
            f"{ZetaBuildDir}/MultiLevelHashTable.o",
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
        f"{ZetaBuildDir}/NearCntRBTreeNode.o",
        {
            f"{File}",
            f"{ZetaDir}/NearCntRBTreeNode.c",
        },
        lambda : compiler.c_to_obj(
            f"{ZetaBuildDir}/NearCntRBTreeNode.o",
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
        f"{ZetaBuildDir}/NearRBTreeNode.o",
        {
            f"{File}",
            f"{ZetaDir}/NearRBTreeNode.c",
        },
        lambda : compiler.c_to_obj(
            f"{ZetaBuildDir}/NearRBTreeNode.o",
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
        f"{ZetaBuildDir}/OrdBinTreeNode.o",
        {
            f"{File}",
            f"{ZetaDir}/OrdBinTreeNode.c",
        },
        lambda : compiler.c_to_obj(
            f"{ZetaBuildDir}/OrdBinTreeNode.o",
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
        f"{ZetaBuildDir}/OrdLinkedListNode.o",
        {
            f"{File}",
            f"{ZetaDir}/OrdLinkedListNode.c",
        },
        lambda : compiler.c_to_obj(
            f"{ZetaBuildDir}/OrdLinkedListNode.o",
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
        f"{ZetaBuildDir}/OrdRBLinkedListNode.o",
        {
            f"{File}",
            f"{ZetaDir}/OrdRBLinkedListNode.c",
        },
        lambda : compiler.c_to_obj(
            f"{ZetaBuildDir}/OrdRBLinkedListNode.o",
            f"{ZetaDir}/OrdRBLinkedListNode.c",
        )
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
        f"{ZetaBuildDir}/OrdRBTreeNode.o",
        {
            f"{File}",
            f"{ZetaDir}/OrdRBTreeNode.c",
        },
        lambda : compiler.c_to_obj(
            f"{ZetaBuildDir}/OrdRBTreeNode.o",
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
        f"{ZetaBuildDir}/OrdCnt3RBTreeNode.o",
        {
            f"{File}",
            f"{ZetaDir}/OrdCnt3RBTreeNode.c",
        },
        lambda : compiler.c_to_obj(
            f"{ZetaBuildDir}/OrdCnt3RBTreeNode.o",
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
        f"{ZetaBuildDir}/OrdCntRBTreeNode.o",
        {
            f"{File}",
            f"{ZetaDir}/OrdCntRBTreeNode.c",
        },
        lambda : compiler.c_to_obj(
            f"{ZetaBuildDir}/OrdCntRBTreeNode.o",
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
        f"{ZetaBuildDir}/PoolAllocator.o",
        {
            f"{File}",
            f"{ZetaDir}/PoolAllocator.c",
        },
        lambda : compiler.c_to_obj(
            f"{ZetaBuildDir}/PoolAllocator.o",
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
            f"{ZetaDir}/CircularArray.h",
            f"{ZetaDir}/SegUtils.h",
            f"{ZetaDir}/utils.h",
        },
        None
    )

    builder.Add(
        f"{ZetaBuildDir}/QTree.o",
        {
            f"{File}",
            f"{ZetaDir}/QTree.c",
        },
        lambda : compiler.c_to_obj(
            f"{ZetaBuildDir}/QTree.o",
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
        f"{ZetaBuildDir}/random.o",
        {
            f"{File}",
            f"{ZetaDir}/random.c",
        },
        lambda : compiler.c_to_obj(
            f"{ZetaBuildDir}/random.o",
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
            f"{ZetaDir}/CircularArray.h",
            f"{ZetaDir}/utils.h",
        },
        None
    )

    builder.Add(
        f"{ZetaBuildDir}/RadixDeque.o",
        {
            f"{File}",
            f"{ZetaDir}/RadixDeque.c",
        },
        lambda : compiler.c_to_obj(
            f"{ZetaBuildDir}/RadixDeque.o",
            f"{ZetaDir}/RadixDeque.c",
        )
    )

    builder.Add(
        f"{ZetaDir}/RadixVector.h",
        {
            f"{File}",
            f"{ZetaDir}/Allocator.h",
            f"{ZetaDir}/SeqContainer.h",
        },
        None
    )

    builder.Add(
        f"{ZetaDir}/RadixVector.c",
        {
            f"{File}",
            f"{ZetaDir}/RadixVector.h",

            f"{ZetaDir}/Debugger.h",
            f"{ZetaDir}/CircularArray.h",
            f"{ZetaDir}/utils.h",
        },
        None
    )

    builder.Add(
        f"{ZetaBuildDir}/RadixVector.o",
        {
            f"{File}",
            f"{ZetaDir}/RadixVector.c",
        },
        lambda : compiler.c_to_obj(
            f"{ZetaBuildDir}/RadixVector.o",
            f"{ZetaDir}/RadixVector.c",
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
        f"{ZetaBuildDir}/RBTree.o",
        {
            f"{File}",
            f"{ZetaDir}/RBTree.c",
        },
        lambda : compiler.c_to_obj(
            f"{ZetaBuildDir}/RBTree.o",
            f"{ZetaDir}/RBTree.c",
        )
    )

    builder.Add(
        f"{ZetaDir}/SegList.h",
        {
            f"{File}",
            f"{ZetaDir}/Allocator.h",
            f"{ZetaDir}/DebugHashTable.h",
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
        f"{ZetaBuildDir}/SegList.o",
        {
            f"{File}",
            f"{ZetaDir}/SegList.c",
        },
        lambda : compiler.c_to_obj(
            f"{ZetaBuildDir}/SegList.o",
            f"{ZetaDir}/SegList.c",
        )
    )

    builder.Add(
        f"{ZetaDir}/SegVector.h",
        {
            f"{File}",
            f"{ZetaDir}/Allocator.h",
            f"{ZetaDir}/DebugHashTable.h",
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
            f"{ZetaDir}/CircularArray.h",
            f"{ZetaDir}/RBTree.h",
            f"{ZetaDir}/utils.h",
        },
        None
    )

    builder.Add(
        f"{ZetaBuildDir}/SegVector.o",
        {
            f"{File}",
            f"{ZetaDir}/SegVector.c",
        },
        lambda : compiler.c_to_obj(
            f"{ZetaBuildDir}/SegVector.o",
            f"{ZetaDir}/SegVector.c",
        )
    )

    builder.Add(
        f"{ZetaDir}/SegUtils.h",
        {
            f"{File}",
            f"{ZetaDir}/CircularArray.h",
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
        f"{ZetaBuildDir}/SegUtils.o",
        {
            f"{File}",
            f"{ZetaDir}/SegUtils.c",
        },
        lambda : compiler.c_to_obj(
            f"{ZetaBuildDir}/SegUtils.o",
            f"{ZetaDir}/SegUtils.c",
        )
    )

    builder.Add(
        f"{ZetaDir}/StageVector.h",
        {
            f"{File}",
            f"{ZetaDir}/Allocator.h",
            f"{ZetaDir}/DebugHashTable.h",
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

            f"{ZetaDir}/CircularArray.h",
            f"{ZetaDir}/Debugger.h",
            f"{ZetaDir}/RBTree.h",
            f"{ZetaDir}/SegUtils.h",
            f"{ZetaDir}/utils.h",
        },
        None
    )

    builder.Add(
        f"{ZetaBuildDir}/StageVector.o",
        {
            f"{File}",
            f"{ZetaDir}/StageVector.c",
        },
        lambda : compiler.c_to_obj(
            f"{ZetaBuildDir}/StageVector.o",
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
            f"{ZetaDir}/Debugger.h",
        },
        None
    )

    builder.Add(
        f"{ZetaBuildDir}/SeqContainer.o",
        {
            f"{File}",
            f"{ZetaDir}/SeqContainer.c",
        },
        lambda : compiler.c_to_obj(
            f"{ZetaBuildDir}/SeqContainer.o",
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
        f"{ZetaBuildDir}/SHA256.o",
        {
            f"{File}",
            f"{ZetaDir}/SHA256.c",
        },
        lambda : compiler.c_to_obj(
            f"{ZetaBuildDir}/SHA256.o",
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
        f"{ZetaBuildDir}/SlabAllocator.o",
        {
            f"{File}",
            f"{ZetaDir}/SlabAllocator.c",
        },
        lambda : compiler.c_to_obj(
            f"{ZetaBuildDir}/SlabAllocator.o",
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
        f"{ZetaBuildDir}/Stream.o",
        {
            f"{File}",
            f"{ZetaDir}/Stream.c",
        },
        lambda : compiler.c_to_obj(
            f"{ZetaBuildDir}/Stream.o",
            f"{ZetaDir}/Stream.c",
        )
    )

    builder.Add(
        f"{ZetaDir}/TreeAllocator.h",
        {
            f"{File}",
            f"{ZetaDir}/Allocator.h",
            f"{ZetaDir}/DebugHashTable.h",
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
            f"{ZetaDir}/DebugHashTable.h",
            f"{ZetaDir}/Debugger.h",
            f"{ZetaDir}/RBTree.h",
            f"{ZetaDir}/OrdLinkedListNode.h",
            f"{ZetaDir}/utils.h",
        },
        None
    )

    builder.Add(
        f"{ZetaBuildDir}/TreeAllocator.o",
        {
            f"{File}",
            f"{ZetaDir}/TreeAllocator.c",
        },
        lambda : compiler.c_to_obj(
            f"{ZetaBuildDir}/TreeAllocator.o",
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
        f"{ZetaBuildDir}/UTF8.o",
        {
            f"{File}",
            f"{ZetaDir}/UTF8.c",
        },
        lambda : compiler.c_to_obj(
            f"{ZetaBuildDir}/UTF8.o",
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
            f"{ZetaDir}/utils.h",
        },
        None
    )

    builder.Add(
        f"{ZetaBuildDir}/UTF16.o",
        {
            f"{File}",
            f"{ZetaDir}/UTF16.c",
        },
        lambda : compiler.c_to_obj(
            f"{ZetaBuildDir}/UTF16.o",
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
        f"{ZetaBuildDir}/utils.o",
        {
            f"{File}",
            f"{ZetaDir}/utils.c",
        },
        lambda : compiler.c_to_obj(
            f"{ZetaBuildDir}/utils.o",
            f"{ZetaDir}/utils.c",
        )
    )

def main():
    parser = argparse.ArgumentParser()

    parser.add_argument("--target",
                        dest="target",
                        action="store",
                        required=True)

    parser.add_argument("--verbose",
                        dest="verbose",
                        action="store_true")

    parser.add_argument("--mode",
                        dest="mode",
                        action="store",
                        choices=["debug", "release"],
                        required=True)

    parser.add_argument("--run_target",
                        dest="run_target",
                        action="store_true")

    parser.add_argument("--rebuild",
                        dest="rebuild",
                        action="store_true")

    args = parser.parse_args(sys.argv[1:])

    print(f"args = {args}")

    mode = {
        "debug": mode_enum.debug,
        "release": mode_enum.release,
    }[args.mode]

    ZetaBuildDir = None

    if mode == mode_enum.debug:
        ZetaBuildDir = f"{ZetaDevDir}/ZetaDebugBuild"

    if mode == mode_enum.release:
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
