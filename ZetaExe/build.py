import os
import sys
import argparse

sys.path.append(f"{os.path.dirname(__file__)}/..")

from utils import *
from Builder import *
from Compiler import *

File = GetNormPath(__file__)
Dir = GetNormPath(os.path.dirname(__file__))

ZetaExeDir = GetNormPath(os.path.dirname(File))
ZetaDevDir = GetNormPath(f"{ZetaExeDir}/..")
ZetaDir = GetNormPath(f"{ZetaDevDir}/Zeta")

sys.path.append(os.path.dirname(ZetaDir))
import Zeta.build

def AddDeps(builder, ZetaBuildDir, ZetaExeBuildDir, verbose, mode):
    compiler_config = EmptyClass

    compiler_config.verbose = verbose
    compiler_config.mode = mode
    compiler_config.build_dir = ZetaExeBuildDir

    compiler_config.working_dirs = {
        ZetaDevDir,
        ZetaDir,
        ZetaExeDir,
    }

    compiler_config.target = EmptyClass
    compiler_config.target.arch = target.arch
    compiler_config.target.vendor = target.vendor
    compiler_config.target.sys = target.sys
    compiler_config.target.env = target.env

    compiler_config.c_include_dirs = {
        f"{ZetaDir}",
    }

    compiler_config.cpp_include_dirs = {
        f"{ZetaDir}",
    }

    compiler = Compiler(compiler_config)

    '''
    if os.name == "nt":
        runtime_libs.append(
            "C:/Program Files/clang+llvm-18.1.4-x86_64-pc-windows-msvc/lib/clang/18/lib/windows/clang_rt.builtins-x86_64.lib")
    '''

    # --------------------------------------------------------------------------

    Zeta.build.AddDeps(builder, ZetaBuildDir, verbose, mode)

    '''
    builder.Add(
        f"{ZetaDevDir}/Compiler.py",
        set(),
        None
    )'''

    builder.Add(
        f"{File}",
        {
            f"{ZetaDevDir}/Compiler.py",
        },
        None
    )

    builder.Add(
        f"{ZetaExeDir}/CppStdAllocator.h",
        {
            f"{ZetaDir}/define.h",
        },
        None
    )

    builder.Add(
        f"{ZetaExeDir}/ChainingMLTScheduler.h",
        {
            f"{File}",
            f"{ZetaDir}/MultiLevelTable.h",
            f"{ZetaDir}/OrdLinkedListNode.h",
            f"{ZetaExeDir}/ZetaPoolAllocator.h",
        },
        None
    )

    builder.Add(
        f"{ZetaExeDir}/ChainingStdRBTScheduler.h",
        {
            f"{File}",
            f"{ZetaDir}/OrdLinkedListNode.h",
            f"{ZetaExeDir}/CppStdAllocator.h",
        },
        None
    )

    builder.Add(
        f"{ZetaExeDir}/CircularArrayUtils.h",
        {
            f"{File}",
            f"{ZetaDir}/CircularArray.h",
            f"{ZetaExeDir}/MemAllocatorCheck.h",
            f"{ZetaExeDir}/StdAllocator.h",
        },
        None
    )

    builder.Add(
        f"{ZetaExeDir}/DebugDequeUtils.h",
        {
            f"{File}",
            f"{ZetaDir}/DebugDeque.h",
            f"{ZetaExeDir}/MemAllocatorCheck.h",
            f"{ZetaExeDir}/StdAllocator.h",
        },
        None
    )

    builder.Add(
        f"{ZetaExeDir}/DynamicVectorUtils.h",
        {
            f"{File}",
            f"{ZetaDir}/DynamicVector.h",
            f"{ZetaExeDir}/MemAllocatorCheck.h",
            f"{ZetaExeDir}/StdAllocator.h",
        },
        None
    )

    builder.Add(
        f"{ZetaExeDir}/FileBlockVector.h",
        {
            f"{ZetaDir}/BlockVector.h",
        },
        None
    )

    builder.Add(
        f"{ZetaExeDir}/PriorityQueue.h",
        {
            f"{File}",
            f"{ZetaDir}/Debugger.h",
            f"{ZetaDir}/GroupBinHeap.h",
        },
        None
    )

    builder.Add(
        f"{ZetaExeDir}/RadixDequeUtils.h",
        {
            f"{File}",
            f"{ZetaDir}/RadixDeque.h",
            f"{ZetaExeDir}/MemAllocatorCheck.h",
            f"{ZetaExeDir}/StdAllocator.h",
        },
        None
    )

    builder.Add(
        f"{ZetaExeDir}/RadixVectorUtils.h",
        {
            f"{File}",
            f"{ZetaDir}/RadixVector.h",
            f"{ZetaExeDir}/MemAllocatorCheck.h",
            f"{ZetaExeDir}/StdAllocator.h",
        },
        None
    )

    builder.Add(
        f"{ZetaExeDir}/StageVectorUtils.h",
        {
            f"{File}",
            f"{ZetaDir}/StageVector.h",
            f"{ZetaExeDir}/StdAllocator.h",
        },
        None
    )

    builder.Add(
        f"{ZetaExeDir}/StdAllocator.h",
        {
            f"{ZetaDir}/Allocator.h",
            f"{ZetaDir}/DebugHashTable.h",
            f"{ZetaExeDir}/test_head.h",
        },
        None
    )

    builder.Add(
        f"{ZetaExeDir}/test_head.h",
        {
            f"{ZetaDir}/define.h",
        },
        None
    )

    builder.Add(
        f"{ZetaExeDir}/test_1.c",
        {
            f"{File}",
            f"{ZetaDir}/BinTree.h",
            f"{ZetaDir}/OrdCntRBTreeNode.h",
            f"{ZetaDir}/RBTree.h",
            f"{ZetaDir}/random.h",
            f"{ZetaDir}/utils.h",
            f"{ZetaExeDir}/test_head.h",
        },
        None
    )

    builder.Add(
        f"{ZetaExeBuildDir}/test_1.o",
        {
            f"{File}",
            f"{ZetaExeDir}/test_1.c",
        },
        lambda : compiler.c_to_obj(
            f"{ZetaExeBuildDir}/test_1.o",
            f"{ZetaExeDir}/test_1.c",
        )
    )

    builder.Add(
        f"{ZetaExeDir}/test_binheap.cpp",
        {
            f"{File}",
            f"{ZetaDir}/BinHeap.h",
            f"{ZetaDir}/OrdCntRBTreeNode.h"
        },
        None
    )

    builder.Add(
        f"{ZetaExeBuildDir}/test_binheap.o",
        {
            f"{File}",
            f"{ZetaExeDir}/test_binheap.c",
        },
        lambda : compiler.cpp_to_obj(
            f"{ZetaExeBuildDir}/test_binheap.o",
            f"{ZetaExeDir}/test_binheap.cpp",
        )
    )

    builder.Add(
        f"{ZetaExeDir}/test_gplist.c",
        {
            f"{File}",
            f"{ZetaDir}/BinTree.h",
            f"{ZetaDir}/OrdCntRBTreeNode.h",
            f"{ZetaDir}/RBTree.h",
            f"{ZetaDir}/utils.h",
            f"{ZetaExeDir}/test_head.h",
        },
        None
    )

    builder.Add(
        f"{ZetaExeDir}/OkMap.h",
        {
            f"{File}",
        },
        None
    )

    builder.Add(
        f"{ZetaExeDir}/OkMap.cpp",
        {
            f"{File}",
            f"{ZetaExeDir}/OkMap.h",
        },
        None
    )

    builder.Add(
        f"{ZetaExeDir}/gplist.c",
        {
            f"{File}",
            f"{ZetaDir}/BinTree.h",
            f"{ZetaDir}/OrdCntRBTreeNode.h",
            f"{ZetaDir}/RBTree.h",
            f"{ZetaDir}/utils.h",
        },
        None
    )

    builder.Add(
        f"{ZetaExeDir}/LeftistHeap.h",
        {
            f"{File}",
        },
        None
    )

    builder.Add(
        f"{ZetaExeDir}/MemAllocatorCheck.h",
        {
            f"{File}",
        },
        None
    )

    builder.Add(
        f"{ZetaExeDir}/NaiveStdRBTScheduler.h",
        {
            f"{File}",
            f"{ZetaExeDir}/CppStdAllocator.h",
        },
        None
    )

    builder.Add(
        f"{ZetaExeDir}/mlmap.c",
        {
            f"{File}",
            f"{ZetaDir}/MultiLevelHashTable.h",
            f"{ZetaDir}/utils.h",
        },
        None
    )

    builder.Add(
        f"{ZetaExeDir}/test_hashtable.c",
        {
            f"{File}",
            f"{ZetaDir}/MultiLevelHashTable.h",
            f"{ZetaDir}/utils.h",
            f"{ZetaExeDir}/test_head.h",
        },
        None
    )

    builder.Add(
        f"{ZetaExeDir}/test_fs.cpp",
        {
            f"{File}",
        },
        None
    )

    builder.Add(
        f"{ZetaExeDir}/test_2.c",
        {
            f"{File}",
        },
        None
    )

    builder.Add(
        f"{ZetaExeBuildDir}/test_2.o",
        {
            f"{File}",
            f"{ZetaExeDir}/test_2.c"
        },
        lambda : compiler.c_to_obj(
            f"{ZetaExeBuildDir}/test_2.o",
            f"{ZetaExeDir}/test_2.c",
        )
    )

    builder.Add(
        f"{ZetaExeDir}/test_3.c",
        {
            f"{File}",

            f"{ZetaDir}/utils.h"
        },
        None
    )

    builder.Add(
        f"{ZetaExeBuildDir}/test_3.o",
        {
            f"{File}",
            f"{ZetaExeDir}/test_3.c"
        },
        lambda : compiler.c_to_obj(
            f"{ZetaExeBuildDir}/test_3.o",
            f"{ZetaExeDir}/test_3.c",
        )
    )

    builder.Add(
        f"{ZetaExeBuildDir}/test_3.exe",
        {
            f"{File}",
            f"{ZetaExeBuildDir}/test_3.o"
        },
        lambda : compiler.to_exe(
            f"{ZetaExeBuildDir}/test_3.exe",
            {
                f"{ZetaExeBuildDir}/test_3.o",
            }
        )
    )

    builder.Add(
        f"{ZetaExeDir}/test_btree.cpp",
        {
            f"{File}",

            f"{ZetaDir}/BTree.h",
            f"{ZetaDir}/utils.h",
        },
        None
    )

    builder.Add(
        f"{ZetaExeBuildDir}/test_btree.o",
        {
            f"{File}",
            f"{ZetaExeDir}/test_btree.cpp"
        },
        lambda : compiler.cpp_to_obj(
            f"{ZetaExeBuildDir}/test_btree.o",
            f"{ZetaExeDir}/test_btree.cpp",
        )
    )

    builder.Add(
        f"{ZetaExeBuildDir}/test_btree.exe",
        {
            f"{File}",
            f"{ZetaExeBuildDir}/test_btree.o",
            f"{ZetaBuildDir}/BTree.o",
            f"{ZetaBuildDir}/utils.o",
        },
        lambda : compiler.to_exe(
            f"{ZetaExeBuildDir}/test_btree.exe",
            {
                f"{ZetaExeBuildDir}/test_btree.o",
                f"{ZetaBuildDir}/BTree.o",
                f"{ZetaBuildDir}/utils.o",
            }
        )
    )

    builder.Add(
        f"{ZetaExeDir}/test_datetime.cpp",
        {
            f"{File}",
            f"{ZetaDir}/DateTime.h",
        },
        None
    )

    builder.Add(
        f"{ZetaExeBuildDir}/test_datetime.o",
        {
            f"{File}",
            f"{ZetaExeDir}/test_datetime.cpp",
        },
        lambda : compiler.cpp_to_obj(
            f"{ZetaExeBuildDir}/test_datetime.o",
            f"{ZetaExeDir}/test_datetime.cpp",
        )
    )

    builder.Add(
        f"{ZetaExeBuildDir}/test_datetime.exe",
        {
            f"{File}",

            f"{ZetaBuildDir}/DateTime.o",

            f"{ZetaExeBuildDir}/test_datetime.o",
        },
        lambda : compiler.to_exe(
            f"{ZetaExeBuildDir}/test_datetime.exe",
            {
                f"{ZetaBuildDir}/DateTime.o",

                f"{ZetaExeBuildDir}/test_datetime.o",
            }
        )
    )

    builder.Add(
        f"{ZetaExeDir}/test_dht.cpp",
        {
            f"{File}",

            f"{ZetaDir}/DebugDeque.h",
            f"{ZetaDir}/Debugger.h",
            f"{ZetaDir}/DynamicHashTable.h",
            f"{ZetaDir}/Logger.h",
            f"{ZetaExeDir}/StdAllocator.h",
            f"{ZetaExeDir}/Timer.h",
            f"{ZetaExeDir}/Value.h",
        },
        None
    )

    builder.Add(
        f"{ZetaExeBuildDir}/test_dht.o",
        {
            f"{File}",
            f"{ZetaExeDir}/test_dht.cpp",
        },
        lambda : compiler.cpp_to_obj(
            f"{ZetaExeBuildDir}/test_dht.o",
            f"{ZetaExeDir}/test_dht.cpp",
        )
    )

    builder.Add(
        f"{ZetaExeBuildDir}/test_dht.exe",
        {
            f"{File}",
            f"{ZetaBuildDir}/BinTree.o",
            f"{ZetaBuildDir}/Debugger.o",
            f"{ZetaBuildDir}/DebugPipe.o",
            f"{ZetaBuildDir}/DebugStrPipe.o",
            f"{ZetaBuildDir}/DynamicHashTable.o",
            f"{ZetaBuildDir}/GenericHashTable.o",
            f"{ZetaBuildDir}/io.o",
            f"{ZetaBuildDir}/Logger.o",
            f"{ZetaBuildDir}/MemCheck.o",
            f"{ZetaBuildDir}/OrdLinkedListNode.o",
            f"{ZetaBuildDir}/OrdRBTreeNode.o",
            f"{ZetaBuildDir}/RBTree.o",
            f"{ZetaBuildDir}/utils.o",
            f"{ZetaExeBuildDir}/Timer.o",
            f"{ZetaExeBuildDir}/Value.o",
            f"{ZetaExeBuildDir}/test_dht.o",
        },
        lambda : compiler.to_exe(
            f"{ZetaExeBuildDir}/test_dht.exe",

            {
                f"{ZetaBuildDir}/BinTree.o",
                f"{ZetaBuildDir}/Debugger.o",
                f"{ZetaBuildDir}/DebugPipe.o",
                f"{ZetaBuildDir}/DebugStrPipe.o",
                f"{ZetaBuildDir}/DynamicHashTable.o",
                f"{ZetaBuildDir}/GenericHashTable.o",
                f"{ZetaBuildDir}/io.o",
                f"{ZetaBuildDir}/Logger.o",
                f"{ZetaBuildDir}/MemCheck.o",
                f"{ZetaBuildDir}/OrdLinkedListNode.o",
                f"{ZetaBuildDir}/OrdRBTreeNode.o",
                f"{ZetaBuildDir}/RBTree.o",
                f"{ZetaBuildDir}/utils.o",
                f"{ZetaExeBuildDir}/Timer.o",
                f"{ZetaExeBuildDir}/Value.o",
                f"{ZetaExeBuildDir}/test_dht.o",
            }
        )
    )

    builder.Add(
        f"{ZetaExeDir}/test_dv.cpp",
        {
            f"{File}",
            f"{ZetaDir}/DynamicVector.h",
            f"{ZetaDir}/utils.h",
        },
        None
    )

    builder.Add(
        f"{ZetaExeDir}/test_elf.cpp",
        {
            f"{File}",
        },
        None
    )

    builder.Add(
        f"{ZetaExeDir}/test_fat32.cpp",
        {
            f"{File}",
            f"{ZetaDir}/Disk.h",
            f"{ZetaDir}/DiskInfo.h",
            f"{ZetaDir}/DiskPartGPT.h",
            f"{ZetaDir}/DiskPartMBR.h",
            f"{ZetaDir}/define.h"
        },
        None
    )

    builder.Add(
        f"{ZetaExeDir}/test_treealloc.cpp",
        {
            f"{File}",
            f"{ZetaDir}/DebugHashTable.h",
            f"{ZetaDir}/TreeAllocator.h",
            f"{ZetaExeDir}/MemAllocatorCheck.h",
        },
        None
    )

    builder.Add(
        f"{ZetaExeBuildDir}/test_treealloc.o",
        {
            f"{File}",
            f"{ZetaExeDir}/test_treealloc.cpp",
        },
        lambda : compiler.cpp_to_obj(
            f"{ZetaExeBuildDir}/test_treealloc.o",
            f"{ZetaExeDir}/test_treealloc.cpp",
        )
    )

    builder.Add(
        f"{ZetaExeBuildDir}/test_treealloc.exe",
        {
            f"{File}",

            f"{ZetaBuildDir}/BinTree.o",
            f"{ZetaBuildDir}/DebugHashTable.o",
            f"{ZetaBuildDir}/DebugStrPipe.o",
            f"{ZetaBuildDir}/DebugPipe.o",
            f"{ZetaBuildDir}/Debugger.o",
            f"{ZetaBuildDir}/io.o",
            f"{ZetaBuildDir}/Logger.o",
            f"{ZetaBuildDir}/OrdRBLinkedListNode.o",
            f"{ZetaBuildDir}/OrdRBTreeNode.o",
            f"{ZetaBuildDir}/RBTree.o",
            f"{ZetaBuildDir}/TreeAllocator.o",
            f"{ZetaBuildDir}/utils.o",

            f"{ZetaExeBuildDir}/test_treealloc.o",
        },
        lambda : compiler.to_exe(
            f"{ZetaExeBuildDir}/test_treealloc.exe",
            {
                f"{ZetaBuildDir}/BinTree.o",
                f"{ZetaBuildDir}/DebugHashTable.o",
                f"{ZetaBuildDir}/DebugStrPipe.o",
                f"{ZetaBuildDir}/DebugPipe.o",
                f"{ZetaBuildDir}/Debugger.o",
                f"{ZetaBuildDir}/io.o",
                f"{ZetaBuildDir}/Logger.o",
                f"{ZetaBuildDir}/OrdRBLinkedListNode.o",
                f"{ZetaBuildDir}/OrdRBTreeNode.o",
                f"{ZetaBuildDir}/RBTree.o",
                f"{ZetaBuildDir}/TreeAllocator.o",
                f"{ZetaBuildDir}/utils.o",

                f"{ZetaExeBuildDir}/test_treealloc.o",
            }
        )
    )

    builder.Add(
        f"{ZetaExeDir}/test_usb.cpp",
        {
            f"{File}",
            f"{ZetaDir}/DiskInfo.h",
            f"{ZetaDir}/DiskPartGPT.h",
            f"{ZetaDir}/DiskPartMBR.h",
            f"{ZetaDir}/define.h"
        },
        None
    )

    builder.Add(
        f"{ZetaExeDir}/test_utf8.c",
        {
            f"{File}",

            f"{ZetaDir}/Debugger.h",
            f"{ZetaDir}/UTF8.h",
        },
        None
    )

    builder.Add(
        f"{ZetaExeBuildDir}/test_utf8.o",
        {
            f"{File}",

            f"{ZetaExeDir}/test_utf8.c",

            f"{ZetaDir}/UTF8.h",
        },

        lambda : compiler.cpp_to_obj(
            f"{ZetaExeBuildDir}/test_utf8.o",
            f"{ZetaExeDir}/test_utf8.c",
        )
    )

    builder.Add(
        f"{ZetaExeBuildDir}/test_utf8.exe",
        {
            f"{File}",

            f"{ZetaBuildDir}/Debugger.o",
            f"{ZetaBuildDir}/DebugPipe.o",
            f"{ZetaBuildDir}/DebugStrPipe.o",
            f"{ZetaBuildDir}/Logger.o",
            f"{ZetaBuildDir}/io.o",
            f"{ZetaBuildDir}/UTF8.o",
            f"{ZetaBuildDir}/utils.o",

            f"{ZetaExeBuildDir}/test_utf8.o",
        },
        lambda : compiler.to_exe(
            f"{ZetaExeBuildDir}/test_utf8.exe",
            {
                f"{ZetaBuildDir}/Debugger.o",
                f"{ZetaBuildDir}/DebugPipe.o",
                f"{ZetaBuildDir}/DebugStrPipe.o",
                f"{ZetaBuildDir}/Logger.o",
                f"{ZetaBuildDir}/io.o",
                f"{ZetaBuildDir}/UTF8.o",
                f"{ZetaBuildDir}/utils.o",

                f"{ZetaExeBuildDir}/test_utf8.o",
            }
        )
    )

    builder.Add(
        f"{ZetaExeDir}/test_utf16.c",
        {
            f"{File}",

            f"{ZetaDir}/Debugger.h",
            f"{ZetaDir}/UTF16.h",
        },
        None
    )

    builder.Add(
        f"{ZetaExeBuildDir}/test_utf16.o",
        {
            f"{File}",

            f"{ZetaExeDir}/test_utf16.c",

            f"{ZetaDir}/UTF16.h",
        },

        lambda : compiler.cpp_to_obj(
            f"{ZetaExeBuildDir}/test_utf16.o",
            f"{ZetaExeDir}/test_utf16.c",
        )
    )

    builder.Add(
        f"{ZetaExeBuildDir}/test_utf16.exe",
        {
            f"{File}",

            f"{ZetaBuildDir}/Debugger.o",
            f"{ZetaBuildDir}/DebugPipe.o",
            f"{ZetaBuildDir}/DebugStrPipe.o",
            f"{ZetaBuildDir}/Logger.o",
            f"{ZetaBuildDir}/io.o",
            f"{ZetaBuildDir}/UTF16.o",
            f"{ZetaBuildDir}/utils.o",

            f"{ZetaExeBuildDir}/test_utf16.o",
        },
        lambda : compiler.to_exe(
            f"{ZetaExeBuildDir}/test_utf16.exe",
            {
                f"{ZetaBuildDir}/Debugger.o",
                f"{ZetaBuildDir}/DebugPipe.o",
                f"{ZetaBuildDir}/DebugStrPipe.o",
                f"{ZetaBuildDir}/Logger.o",
                f"{ZetaBuildDir}/io.o",
                f"{ZetaBuildDir}/UTF16.o",
                f"{ZetaBuildDir}/utils.o",

                f"{ZetaExeBuildDir}/test_utf16.o",
            }
        )
    )

    builder.Add(
        f"{ZetaExeDir}/test_segvec.cpp",
        {
            f"{File}",
            f"{ZetaDir}/SegVector.h",
            f"{ZetaExeDir}/StdAllocator.h"
        },
        None
    )

    builder.Add(
        f"{ZetaExeBuildDir}/test_segvec.o",
        {
            f"{File}",
            f"{ZetaExeDir}/test_segvec.cpp",
        },
        lambda : compiler.cpp_to_obj(
            f"{ZetaExeBuildDir}/test_segvec.o",
            f"{ZetaExeDir}/test_segvec.cpp",
        )
    )

    builder.Add(
        f"{ZetaExeBuildDir}/test_segvec.exe",
        {
            f"{File}",

            f"{ZetaBuildDir}/SegVector.o",
            f"{ZetaBuildDir}/BinTree.o",
            f"{ZetaBuildDir}/CircularArray.o",
            f"{ZetaBuildDir}/DebugDeque.o",
            f"{ZetaBuildDir}/DebugHashTable.o",
            f"{ZetaBuildDir}/DebugPipe.o",
            f"{ZetaBuildDir}/RBTree.o",
            f"{ZetaBuildDir}/OrdCntRBTreeNode.o",
            f"{ZetaBuildDir}/utils.o",

            f"{ZetaExeBuildDir}/test_segvec.o",
        },
        lambda : compiler.to_exe(
            f"{ZetaExeBuildDir}/test_segvec.exe",
            {
                f"{ZetaBuildDir}/SegVector.o",
                f"{ZetaBuildDir}/BinTree.o",
                f"{ZetaBuildDir}/CircularArray.o",
                f"{ZetaBuildDir}/DebugDeque.o",
                f"{ZetaBuildDir}/DebugHashTable.o",
                f"{ZetaBuildDir}/DebugPipe.o",
                f"{ZetaBuildDir}/RBTree.o",
                f"{ZetaBuildDir}/OrdCntRBTreeNode.o",
                f"{ZetaBuildDir}/utils.o",

                f"{ZetaExeBuildDir}/test_segvec.o",
            }
        )
    )

    builder.Add(
        f"{ZetaExeDir}/test_segvec2.cpp",
        {
            f"{File}",
            f"{ZetaDir}/SegVector.h",
            f"{ZetaExeDir}/StdAllocator.h"
        },
        None
    )

    builder.Add(
        f"{ZetaExeBuildDir}/test_segvec2.o",
        {
            f"{File}",
            f"{ZetaExeDir}/test_segvec2.cpp",
        },
        lambda : compiler.cpp_to_obj(
            f"{ZetaExeBuildDir}/test_segvec2.o",
            f"{ZetaExeDir}/test_segvec2.cpp",
        )
    )

    builder.Add(
        f"{ZetaExeBuildDir}/test_segvec2.exe",
        {
            f"{File}",

            f"{ZetaBuildDir}/BinTree.o",
            f"{ZetaBuildDir}/CircularArray.o",
            f"{ZetaBuildDir}/DebugDeque.o",
            f"{ZetaBuildDir}/DebugHashTable.o",
            f"{ZetaBuildDir}/DebugPipe.o",
            f"{ZetaBuildDir}/OrdCntRBTreeNode.o",
            f"{ZetaBuildDir}/RBTree.o",
            f"{ZetaBuildDir}/SegVector.o",
            f"{ZetaBuildDir}/utils.o",

            f"{ZetaExeBuildDir}/test_segvec2.o",
        },
        lambda : compiler.to_exe(
            f"{ZetaExeBuildDir}/test_segvec2.exe",
            {
                f"{ZetaBuildDir}/BinTree.o",
                f"{ZetaBuildDir}/CircularArray.o",
                f"{ZetaBuildDir}/DebugDeque.o",
                f"{ZetaBuildDir}/DebugHashTable.o",
                f"{ZetaBuildDir}/DebugPipe.o",
                f"{ZetaBuildDir}/OrdCntRBTreeNode.o",
                f"{ZetaBuildDir}/RBTree.o",
                f"{ZetaBuildDir}/SegVector.o",
                f"{ZetaBuildDir}/utils.o",

                f"{ZetaExeBuildDir}/test_segvec2.o",
            }
        )
    )

    builder.Add(
        f"{ZetaExeDir}/test_segvec_speed.cpp",
        {
            f"{File}",
            f"{ZetaDir}/SegVector.h",
            f"{ZetaExeDir}/StdAllocator.h"
        },
        None
    )

    builder.Add(
        f"{ZetaExeBuildDir}/test_segvec_speed.o",
        {
            f"{File}",
            f"{ZetaExeDir}/test_segvec_speed.cpp",
        },
        lambda : compiler.cpp_to_obj(
            f"{ZetaExeBuildDir}/test_segvec_speed.o",
            f"{ZetaExeDir}/test_segvec_speed.cpp",
        )
    )

    builder.Add(
        f"{ZetaExeBuildDir}/test_segvec_speed.exe",
        {
            f"{File}",

            f"{ZetaBuildDir}/SegVector.o",
            f"{ZetaBuildDir}/BinTree.o",
            f"{ZetaBuildDir}/CircularArray.o",
            f"{ZetaBuildDir}/DebugDeque.o",
            f"{ZetaBuildDir}/DebugHashTable.o",
            f"{ZetaBuildDir}/DebugPipe.o",
            f"{ZetaBuildDir}/RBTree.o",
            f"{ZetaBuildDir}/OrdCntRBTreeNode.o",
            f"{ZetaBuildDir}/utils.o",

            f"{ZetaExeBuildDir}/test_segvec_speed.o",
        },
        lambda : compiler.to_exe(
            f"{ZetaExeBuildDir}/test_segvec_speed.exe",
            {
                f"{ZetaBuildDir}/SegVector.o",
                f"{ZetaBuildDir}/BinTree.o",
                f"{ZetaBuildDir}/CircularArray.o",
                f"{ZetaBuildDir}/DebugDeque.o",
                f"{ZetaBuildDir}/DebugHashTable.o",
                f"{ZetaBuildDir}/DebugPipe.o",
                f"{ZetaBuildDir}/RBTree.o",
                f"{ZetaBuildDir}/OrdCntRBTreeNode.o",
                f"{ZetaBuildDir}/utils.o",

                f"{ZetaExeBuildDir}/test_segvec_speed.o",
            }
        )
    )

    builder.Add(
        f"{ZetaExeDir}/test_segvec2_speed.cpp",
        {
            f"{File}",
            f"{ZetaDir}/SegVector.h",
            f"{ZetaExeDir}/StdAllocator.h"
        },
        None
    )

    builder.Add(
        f"{ZetaExeBuildDir}/test_segvec2_speed.o",
        {
            f"{File}",
            f"{ZetaExeDir}/test_segvec2_speed.cpp",
        },
        lambda : compiler.cpp_to_obj(
            f"{ZetaExeBuildDir}/test_segvec2_speed.o",
            f"{ZetaExeDir}/test_segvec2_speed.cpp",
        )
    )

    builder.Add(
        f"{ZetaExeBuildDir}/test_segvec2_speed.exe",
        {
            f"{File}",

            f"{ZetaBuildDir}/SegVector.o",
            f"{ZetaBuildDir}/BinTree.o",
            f"{ZetaBuildDir}/CircularArray.o",
            f"{ZetaBuildDir}/DebugHashTable.o",
            f"{ZetaBuildDir}/DebugPipe.o",
            f"{ZetaBuildDir}/RBTree.o",
            f"{ZetaBuildDir}/OrdCntRBTreeNode.o",
            f"{ZetaBuildDir}/utils.o",

            f"{ZetaExeBuildDir}/test_segvec2_speed.o",
        },
        lambda : compiler.to_exe(
            f"{ZetaExeBuildDir}/test_segvec2_speed.exe",
            {
                f"{ZetaBuildDir}/SegVector.o",
                f"{ZetaBuildDir}/BinTree.o",
                f"{ZetaBuildDir}/CircularArray.o",
                f"{ZetaBuildDir}/DebugHashTable.o",
                f"{ZetaBuildDir}/DebugPipe.o",
                f"{ZetaBuildDir}/RBTree.o",
                f"{ZetaBuildDir}/OrdCntRBTreeNode.o",
                f"{ZetaBuildDir}/utils.o",

                f"{ZetaExeBuildDir}/test_segvec2_speed.o",
            }
        )
    )

    builder.Add(
        f"{ZetaExeDir}/test_seqcntr.cpp",
        {
            f"{File}",
            f"{ZetaDir}/DebugDeque.h",
            f"{ZetaDir}/CircularArray.h",
            f"{ZetaDir}/SegVector.h",
            f"{ZetaExeDir}/StdAllocator.h"
        },
        None
    )

    builder.Add(
        f"{ZetaExeBuildDir}/test_seqcntr.o",
        {
            f"{File}",
            f"{ZetaExeDir}/test_seqcntr.cpp",
        },
        lambda : compiler.cpp_to_obj(
            f"{ZetaExeBuildDir}/test_seqcntr.o",
            f"{ZetaExeDir}/test_seqcntr.cpp",
        )
    )

    builder.Add(
        f"{ZetaExeBuildDir}/test_seqcntr.exe",
        {
            f"{File}",

            f"{ZetaBuildDir}/BinTree.o",
            f"{ZetaBuildDir}/CircularArray.o",
            f"{ZetaBuildDir}/DebugDeque.o",
            f"{ZetaBuildDir}/DebugHashTable.o",
            f"{ZetaBuildDir}/DebugPipe.o",
            f"{ZetaBuildDir}/RBTree.o",
            f"{ZetaBuildDir}/OrdCntRBTreeNode.o",
            f"{ZetaBuildDir}/SegVector.o",
            f"{ZetaBuildDir}/utils.o",

            f"{ZetaExeBuildDir}/test_seqcntr.o",
        },
        lambda : compiler.to_exe(
            f"{ZetaExeBuildDir}/test_seqcntr.exe",
            {
                f"{ZetaBuildDir}/BinTree.o",
                f"{ZetaBuildDir}/CircularArray.o",
                f"{ZetaBuildDir}/DebugDeque.o",
                f"{ZetaBuildDir}/DebugHashTable.o",
                f"{ZetaBuildDir}/DebugPipe.o",
                f"{ZetaBuildDir}/RBTree.o",
                f"{ZetaBuildDir}/OrdCntRBTreeNode.o",
                f"{ZetaBuildDir}/SegVector.o",
                f"{ZetaBuildDir}/utils.o",

                f"{ZetaExeBuildDir}/test_seqcntr.o",
            }
        )
    )

    builder.Add(
        f"{ZetaExeDir}/test_stagevec.cpp",
        {
            f"{File}",

            f"{ZetaDir}/DebugDeque.h",
            f"{ZetaDir}/Debugger.h",
            f"{ZetaDir}/Logger.h",
            f"{ZetaExeDir}/CircularArrayUtils.h",
            f"{ZetaExeDir}/DebugDequeUtils.h",
            f"{ZetaExeDir}/DynamicVectorUtils.h",
            f"{ZetaExeDir}/MemAllocatorCheck.h",
            f"{ZetaExeDir}/RadixDequeUtils.h",
            f"{ZetaExeDir}/RadixVectorUtils.h",
            f"{ZetaExeDir}/StageVectorUtils.h",
            f"{ZetaExeDir}/Timer.h",
        },
        None
    )

    builder.Add(
        f"{ZetaExeBuildDir}/test_stagevec.o",
        {
            f"{File}",
            f"{ZetaExeDir}/test_stagevec.cpp",
        },
        lambda : compiler.cpp_to_obj(
            f"{ZetaExeBuildDir}/test_stagevec.o",
            f"{ZetaExeDir}/test_stagevec.cpp",
        )
    )

    builder.Add(
        f"{ZetaExeBuildDir}/test_stagevec.s",
        {
            f"{File}",
            f"{ZetaExeBuildDir}/test_stagevec.o",
        },
        lambda : compiler.ll_to_asm(
            f"{ZetaExeBuildDir}/test_stagevec.s",
            f"{ZetaExeBuildDir}/test_stagevec.o",
        )
    )

    builder.Add(
        f"{ZetaExeBuildDir}/test_stagevec.exe",
        {
            f"{File}",

            f"{ZetaBuildDir}/Algorithm.o",
            f"{ZetaBuildDir}/AssocContainer.o",
            f"{ZetaBuildDir}/BinTree.o",
            f"{ZetaBuildDir}/CircularArray.o",
            f"{ZetaBuildDir}/DebugDeque.o",
            f"{ZetaBuildDir}/DebugHashTable.o",
            f"{ZetaBuildDir}/Debugger.o",
            f"{ZetaBuildDir}/DebugPipe.o",
            f"{ZetaBuildDir}/DebugStrPipe.o",
            f"{ZetaBuildDir}/DynamicVector.o",
            f"{ZetaBuildDir}/io.o",
            f"{ZetaBuildDir}/MemCheck.o",
            f"{ZetaBuildDir}/Logger.o",
            f"{ZetaBuildDir}/RBTree.o",
            f"{ZetaBuildDir}/RadixVector.o",
            f"{ZetaBuildDir}/RadixDeque.o",
            f"{ZetaBuildDir}/OrdCnt3RBTreeNode.o",
            f"{ZetaBuildDir}/OrdCntRBTreeNode.o",
            f"{ZetaBuildDir}/SegUtils.o",
            f"{ZetaBuildDir}/SeqContainer.o",
            f"{ZetaBuildDir}/StageVector.o",
            f"{ZetaBuildDir}/utils.o",

            f"{ZetaExeBuildDir}/Timer.o",

            f"{ZetaExeBuildDir}/test_stagevec.o",
        },
        lambda : compiler.to_exe(
            f"{ZetaExeBuildDir}/test_stagevec.exe",

            {
                f"{ZetaBuildDir}/Algorithm.o",
                f"{ZetaBuildDir}/AssocContainer.o",
                f"{ZetaBuildDir}/BinTree.o",
                f"{ZetaBuildDir}/CircularArray.o",
                f"{ZetaBuildDir}/DebugDeque.o",
                f"{ZetaBuildDir}/DebugHashTable.o",
                f"{ZetaBuildDir}/Debugger.o",
                f"{ZetaBuildDir}/DebugPipe.o",
                f"{ZetaBuildDir}/DebugStrPipe.o",
                f"{ZetaBuildDir}/DynamicVector.o",
                f"{ZetaBuildDir}/io.o",
                f"{ZetaBuildDir}/MemCheck.o",
                f"{ZetaBuildDir}/Logger.o",
                f"{ZetaBuildDir}/RBTree.o",
                f"{ZetaBuildDir}/RadixVector.o",
                f"{ZetaBuildDir}/RadixDeque.o",
                f"{ZetaBuildDir}/OrdCnt3RBTreeNode.o",
                f"{ZetaBuildDir}/OrdCntRBTreeNode.o",
                f"{ZetaBuildDir}/SegUtils.o",
                f"{ZetaBuildDir}/SeqContainer.o",
                f"{ZetaBuildDir}/StageVector.o",
                f"{ZetaBuildDir}/utils.o",

                f"{ZetaExeBuildDir}/Timer.o",

                f"{ZetaExeBuildDir}/test_stagevec.o",
            }
        )
    )

    builder.Add(
        f"{ZetaExeDir}/test_stagevec_speed.cpp",
        {
            f"{File}",
            f"{ZetaDir}/CircularArray.h",
            f"{ZetaDir}/DebugDeque.h",
            f"{ZetaDir}/Debugger.h",
            f"{ZetaDir}/Logger.h",
            f"{ZetaDir}/SegVector.h",
            f"{ZetaDir}/StageVector.h",
            f"{ZetaExeDir}/MemAllocatorCheck.h",
            f"{ZetaExeDir}/StdAllocator.h",
        },
        None
    )

    builder.Add(
        f"{ZetaExeBuildDir}/test_stagevec_speed.o",
        {
            f"{File}",
            f"{ZetaExeDir}/test_stagevec_speed.cpp",
        },
        lambda : compiler.cpp_to_obj(
            f"{ZetaExeBuildDir}/test_stagevec_speed.o",
            f"{ZetaExeDir}/test_stagevec_speed.cpp",
        )
    )

    builder.Add(
        f"{ZetaExeBuildDir}/test_stagevec_speed.exe",
        {
            f"{File}",

            f"{ZetaBuildDir}/BinTree.o",
            f"{ZetaBuildDir}/CircularArray.o",
            f"{ZetaBuildDir}/DebugDeque.o",
            f"{ZetaBuildDir}/DebugHashTable.o",
            f"{ZetaBuildDir}/Debugger.o",
            f"{ZetaBuildDir}/DebugPipe.o",
            f"{ZetaBuildDir}/DebugStrPipe.o",
            f"{ZetaBuildDir}/DummyVector.o",
            f"{ZetaBuildDir}/io.o",
            f"{ZetaBuildDir}/Logger.o",
            f"{ZetaBuildDir}/RBTree.o",
            f"{ZetaBuildDir}/OrdCnt3RBTreeNode.o",
            f"{ZetaBuildDir}/OrdCntRBTreeNode.o",
            f"{ZetaBuildDir}/SegUtils.o",
            f"{ZetaBuildDir}/SeqContainer.o",
            f"{ZetaBuildDir}/StageVector.o",
            f"{ZetaBuildDir}/utils.o",

            f"{ZetaExeBuildDir}/test_stagevec_speed.o",
        },
        lambda : compiler.to_exe(
            f"{ZetaExeBuildDir}/test_stagevec_speed.exe",

            {
                f"{ZetaBuildDir}/BinTree.o",
                f"{ZetaBuildDir}/CircularArray.o",
                f"{ZetaBuildDir}/DebugDeque.o",
                f"{ZetaBuildDir}/DebugHashTable.o",
                f"{ZetaBuildDir}/Debugger.o",
                f"{ZetaBuildDir}/DebugPipe.o",
                f"{ZetaBuildDir}/DebugStrPipe.o",
                f"{ZetaBuildDir}/DummyVector.o",
                f"{ZetaBuildDir}/io.o",
                f"{ZetaBuildDir}/Logger.o",
                f"{ZetaBuildDir}/RBTree.o",
                f"{ZetaBuildDir}/OrdCnt3RBTreeNode.o",
                f"{ZetaBuildDir}/OrdCntRBTreeNode.o",
                f"{ZetaBuildDir}/SegUtils.o",
                f"{ZetaBuildDir}/SeqContainer.o",
                f"{ZetaBuildDir}/StageVector.o",
                f"{ZetaBuildDir}/utils.o",

                f"{ZetaExeBuildDir}/test_stagevec_speed.o",
            }
        )
    )

    builder.Add(
        f"{ZetaExeDir}/test_stdio.c",
        {
            f"{File}",
            f"{ZetaDir}/define.h"
            f"{ZetaDir}/Pipe.h"
        },
        None
    )

    builder.Add(
        f"{ZetaExeDir}/FileIO.s",
        {
            f"{File}",
        },
        None
    )

    builder.Add(
        f"{ZetaExeDir}/test_io.c",
        {
            f"{File}",
        },
        None
    )

    builder.Add(
        f"{ZetaExeDir}/test_kmp.cpp",
        {
            f"{File}",

            f"{ZetaDir}/define.h"
        },
        None
    )

    builder.Add(
        f"{ZetaExeBuildDir}/test_kmp.o",
        {
            f"{File}",
            f"{ZetaExeDir}/test_kmp.cpp"
        },
        lambda : compiler.cpp_to_obj(
            f"{ZetaExeBuildDir}/test_kmp.o",
            f"{ZetaExeDir}/test_kmp.cpp",
        )
    )

    builder.Add(
        f"{ZetaExeBuildDir}/test_kmp.exe",
        {
            f"{File}",
            f"{ZetaExeBuildDir}/test_kmp.o"
        },
        lambda : compiler.to_exe(
            f"{ZetaExeBuildDir}/test_kmp.exe",
            {
                f"{ZetaExeBuildDir}/test_kmp.o",
            }
        )
    )

    builder.Add(
        f"{ZetaExeDir}/test_LeftistHeap.cpp",
        {
            f"{File}",
            f"{ZetaExeDir}/LeftistHeap.h",
        },
        None
    )

    builder.Add(
        f"{ZetaExeBuildDir}/test_LeftistHeap.o",
        {
            f"{File}",
            f"{ZetaExeDir}/test_LeftistHeap.cpp",
        },
        lambda : compiler.cpp_to_obj(
            f"{ZetaExeBuildDir}/test_LeftistHeap.o",
            f"{ZetaExeDir}/test_LeftistHeap.cpp",
        )
    )

    builder.Add(
        f"{ZetaExeBuildDir}/test_LeftistHeap.exe",
        {
            f"{File}",
            f"{ZetaExeBuildDir}/test_LeftistHeap.o",
        },
        lambda : compiler.to_exe(
            f"{ZetaExeBuildDir}/test_LeftistHeap.exe",
            {
                f"{ZetaExeBuildDir}/test_LeftistHeap.o",
            }
        )
    )

    builder.Add(
        f"{ZetaExeDir}/test_log.cpp",
        {
            f"{File}",
            f"{ZetaDir}/utils.h",
        },
        None
    )

    builder.Add(
        f"{ZetaExeBuildDir}/test_log.o",
        {
            f"{File}",
            f"{ZetaExeDir}/test_log.cpp",
        },
        lambda : compiler.cpp_to_obj(
            f"{ZetaExeBuildDir}/test_log.o",
            f"{ZetaExeDir}/test_log.cpp",
        )
    )

    builder.Add(
        f"{ZetaExeBuildDir}/test_log.exe",
        {
            f"{File}",

            f"{ZetaBuildDir}/DebugStrPipe.o",
            f"{ZetaBuildDir}/Debugger.o",
            f"{ZetaBuildDir}/Logger.o",
            f"{ZetaBuildDir}/io.o",
            f"{ZetaBuildDir}/utils.o",

            f"{ZetaExeBuildDir}/test_log.o",
        },
        lambda : compiler.to_exe(
            f"{ZetaExeBuildDir}/test_log.exe",

            {
                f"{ZetaBuildDir}/DebugStrPipe.o",
                f"{ZetaBuildDir}/Debugger.o",
                f"{ZetaBuildDir}/Logger.o",
                f"{ZetaBuildDir}/io.o",
                f"{ZetaBuildDir}/utils.o",

                f"{ZetaExeBuildDir}/test_log.o",
            }
        )
    )

    builder.Add(
        f"{ZetaExeDir}/test_lru.cpp",
        {
            f"{File}",
            f"{ZetaDir}/LRUCacheManager.h",
        },
        None
    )

    builder.Add(
        f"{ZetaExeDir}/test_lrucm.cpp",
        {
            f"{File}",
            f"{ZetaDir}/DummyBlockVector.h",
            f"{ZetaDir}/DummyCacheManager.h",
            f"{ZetaDir}/LRUCacheManager.h",
            f"{ZetaDir}/RBTree.h",
            f"{ZetaExeDir}/FileBlockVector.h",
            f"{ZetaExeDir}/StdAllocator.h",
        },
        None
    )

    builder.Add(
        f"{ZetaExeBuildDir}/test_lrucm.o",
        {
            f"{File}",
            f"{ZetaExeDir}/test_lrucm.cpp",
        },
        lambda : compiler.cpp_to_obj(
            f"{ZetaExeBuildDir}/test_lrucm.o",
            f"{ZetaExeDir}/test_lrucm.cpp",
        )
    )

    builder.Add(
        f"{ZetaExeBuildDir}/test_lrucm.exe",
        {
            f"{File}",

            f"{ZetaBuildDir}/Allocator.o",
            f"{ZetaBuildDir}/BinTree.o",
            f"{ZetaBuildDir}/BlockVector.o",
            f"{ZetaBuildDir}/DebugHashTable.o",
            f"{ZetaBuildDir}/DebugPipe.o",
            f"{ZetaBuildDir}/DummyBlockVector.o",
            f"{ZetaBuildDir}/DummyCacheManager.o",
            f"{ZetaBuildDir}/LRUCacheManager.o",
            f"{ZetaBuildDir}/OrdLinkedListNode.o",
            f"{ZetaBuildDir}/OrdRBLinkedListNode.o",
            f"{ZetaBuildDir}/OrdRBTreeNode.o",
            f"{ZetaBuildDir}/RBTree.o",
            f"{ZetaBuildDir}/utils.o",

            f"{ZetaExeBuildDir}/test_lrucm.o",
        },
        lambda : compiler.to_exe(
            f"{ZetaExeBuildDir}/test_lrucm.exe",
            {
                f"{ZetaBuildDir}/Allocator.o",
                f"{ZetaBuildDir}/BinTree.o",
                f"{ZetaBuildDir}/BlockVector.o",
                f"{ZetaBuildDir}/DebugHashTable.o",
                f"{ZetaBuildDir}/DebugPipe.o",
                f"{ZetaBuildDir}/DummyBlockVector.o",
                f"{ZetaBuildDir}/DummyCacheManager.o",
                f"{ZetaBuildDir}/LRUCacheManager.o",
                f"{ZetaBuildDir}/OrdLinkedListNode.o",
                f"{ZetaBuildDir}/OrdRBLinkedListNode.o",
                f"{ZetaBuildDir}/OrdRBTreeNode.o",
                f"{ZetaBuildDir}/RBTree.o",
                f"{ZetaBuildDir}/utils.o",

                f"{ZetaExeBuildDir}/test_lrucm.o",
            }
        )
    )

    builder.Add(
        f"{ZetaExeDir}/test_lsb.cpp",
        {
            f"{File}",
            f"{ZetaDir}/utils.h",
        },
        None
    )

    builder.Add(
        f"{ZetaExeDir}/test_sort.cpp",
        {
            f"{File}",
            f"{ZetaDir}/Algorithm.h",
            f"{ZetaExeDir}/PriorityQueue.h",
        },
        None
    )

    builder.Add(
        f"{ZetaExeBuildDir}/test_sort.o",
        {
            f"{File}",
            f"{ZetaExeDir}/test_sort.cpp",
        },
        lambda : compiler.cpp_to_obj(
            f"{ZetaExeBuildDir}/test_sort.o",
            f"{ZetaExeDir}/test_sort.cpp",
        )
    )

    builder.Add(
        f"{ZetaExeBuildDir}/test_sort.exe",
        {
            f"{File}",

            f"{ZetaBuildDir}/Algorithm.o",
            f"{ZetaBuildDir}/Debugger.o",
            f"{ZetaBuildDir}/DebugStrPipe.o",
            f"{ZetaBuildDir}/GroupBinHeap.o",
            f"{ZetaBuildDir}/io.o",
            f"{ZetaBuildDir}/utils.o",

            f"{ZetaExeBuildDir}/Timer.o",

            f"{ZetaExeBuildDir}/test_sort.o",
        },
        lambda : compiler.to_exe(
            f"{ZetaExeBuildDir}/test_sort.exe",
            {
                f"{ZetaBuildDir}/Algorithm.o",
                f"{ZetaBuildDir}/Debugger.o",
                f"{ZetaBuildDir}/DebugStrPipe.o",
                f"{ZetaBuildDir}/GroupBinHeap.o",
                f"{ZetaBuildDir}/io.o",
                f"{ZetaBuildDir}/utils.o",

                f"{ZetaExeBuildDir}/Timer.o",

                f"{ZetaExeBuildDir}/test_sort.o",
            }
        )
    )

    builder.Add(
        f"{ZetaExeDir}/DyVec.c",
        {
            f"{File}",
            f"{ZetaDir}/DynamicVector.h",
        },
        None
    )

    builder.Add(
        f"{ZetaExeDir}/test_slaballoc.cpp",
        {
            f"{File}",
            f"{ZetaDir}/DebugHashTable.h",
            f"{ZetaDir}/SlabAllocator.h",
        },
        None
    )

    builder.Add(
        f"{ZetaExeBuildDir}/test_slaballoc.o",
        {
            f"{File}",
            f"{ZetaExeDir}/test_slaballoc.cpp",
        },
        lambda : compiler.cpp_to_obj(
            f"{ZetaExeBuildDir}/test_slaballoc.o",
            f"{ZetaExeDir}/test_slaballoc.cpp",
        )
    )

    builder.Add(
        f"{ZetaExeBuildDir}/test_slaballoc.exe",
        {
            f"{File}",

            f"{ZetaBuildDir}/Allocator.o",
            f"{ZetaBuildDir}/DebugHashTable.o",
            f"{ZetaBuildDir}/OrdLinkedListNode.o",
            f"{ZetaBuildDir}/OrdRBLinkedListNode.o",
            f"{ZetaBuildDir}/SlabAllocator.o",
            f"{ZetaBuildDir}/utils.o",

            f"{ZetaExeBuildDir}/test_slaballoc.o",
        },
        lambda : compiler.to_exe(
            f"{ZetaExeBuildDir}/test_slaballoc.exe",
            {
                f"{ZetaBuildDir}/Allocator.o",
                f"{ZetaBuildDir}/DebugHashTable.o",
                f"{ZetaBuildDir}/OrdLinkedListNode.o",
                f"{ZetaBuildDir}/OrdRBLinkedListNode.o",
                f"{ZetaBuildDir}/SlabAllocator.o",
                f"{ZetaBuildDir}/utils.o",

                f"{ZetaExeBuildDir}/test_slaballoc.o",
            }
        )
    )

    builder.Add(
        f"{ZetaExeDir}/test_mlv.cpp",
        {
            f"{File}",
            f"{ZetaDir}/MultiLevelTable.h",
            f"{ZetaExeDir}/StdAllocator.h",
        },
        None
    )

    builder.Add(
        f"{ZetaExeBuildDir}/test_mlv.o",
        {
            f"{File}",
            f"{ZetaDir}/MultiLevelTable.h",
            f"{ZetaExeDir}/StdAllocator.h",
            f"{ZetaExeDir}/test_mlv.cpp",
        },
        lambda : compiler.cpp_to_obj(
            f"{ZetaExeBuildDir}/test_mlv.o",
            f"{ZetaExeDir}/test_mlv.cpp",
        )
    )

    builder.Add(
        f"{ZetaExeBuildDir}/test_mlv.exe",
        {
            f"{File}",

            f"{ZetaBuildDir}/Allocator.o",
            f"{ZetaBuildDir}/DebugHashTable.o",
            f"{ZetaBuildDir}/MultiLevelTable.o",
            f"{ZetaBuildDir}/utils.o",

            f"{ZetaExeBuildDir}/test_mlv.o",
        },
        lambda : compiler.to_exe(
            f"{ZetaExeBuildDir}/test_mlv.exe",

            {
                f"{ZetaBuildDir}/Allocator.o",
                f"{ZetaBuildDir}/DebugHashTable.o",
                f"{ZetaBuildDir}/MultiLevelTable.o",
                f"{ZetaBuildDir}/utils.o",

                f"{ZetaExeBuildDir}/test_mlv.o",
            }
        )
    )

    builder.Add(
        f"{ZetaExeDir}/test_mlt.cpp",
        {
            f"{File}",
            f"{ZetaDir}/MultiLevelTable.h",
            f"{ZetaExeDir}/CppStdAllocator.h",
            f"{ZetaExeDir}/ZetaPoolAllocator.h",
        },
        None
    )

    builder.Add(
        f"{ZetaExeBuildDir}/test_mlt.o",
        {
            f"{File}",
            f"{ZetaDir}/MultiLevelTable.h",
            f"{ZetaExeDir}/StdAllocator.h",
            f"{ZetaExeDir}/test_mlt.cpp",
        },
        lambda : compiler.cpp_to_obj(
            f"{ZetaExeBuildDir}/test_mlt.o",
            f"{ZetaExeDir}/test_mlt.cpp",
        )
    )

    builder.Add(
        f"{ZetaExeBuildDir}/test_mlt.exe",
        {
            f"{File}",

            f"{ZetaBuildDir}/Allocator.o",
            f"{ZetaBuildDir}/DebugHashTable.o",
            f"{ZetaBuildDir}/MultiLevelTable.o",
            f"{ZetaBuildDir}/OrdLinkedListNode.o",
            f"{ZetaBuildDir}/utils.o",

            f"{ZetaExeBuildDir}/test_mlt.o",
        },
        lambda : compiler.to_exe(
            f"{ZetaExeBuildDir}/test_mlt.exe",

            {
                f"{ZetaBuildDir}/Allocator.o",
                f"{ZetaBuildDir}/DebugHashTable.o",
                f"{ZetaBuildDir}/MultiLevelTable.o",
                f"{ZetaBuildDir}/OrdLinkedListNode.o",
                f"{ZetaBuildDir}/utils.o",

                f"{ZetaExeBuildDir}/test_mlt.o",
            }
        )
    )

    builder.Add(
        f"{ZetaExeDir}/test_OkMap.cpp",
        {
            f"{File}",
            f"{ZetaExeDir}/OkMap.h",
        },
        None
    )

    builder.Add(
        f"{ZetaExeDir}/test_o1sh.cpp",
        {
            f"{File}",
            f"{ZetaDir}/MultiLevelTable.h",
            f"{ZetaExeDir}/StdAllocator.h",
        },
        None
    )

    builder.Add(
        f"{ZetaExeDir}/test_ordalloc.cpp",
        {
            f"{File}",
            f"{ZetaDir}/DebugHashTable.h",
            f"{ZetaDir}/OrdAllocator.h",
        },
        None
    )

    builder.Add(
        f"{ZetaExeDir}/test_pipe.c",
        {
            f"{File}",
            f"{ZetaDir}/DebugStrPipe.h",
            f"{ZetaDir}/Debugger.h",
        },
        None
    )

    builder.Add(
        f"{ZetaExeBuildDir}/test_pipe.o",
        {
            f"{File}",
            f"{ZetaExeDir}/test_pipe.c",
        },
        lambda : compiler.c_to_obj(
            f"{ZetaExeBuildDir}/test_pipe.o",
            f"{ZetaExeDir}/test_pipe.c",
        )
    )

    builder.Add(
        f"{ZetaExeBuildDir}/test_pipe.exe",
        {
            f"{File}",
            f"{ZetaBuildDir}/DebugStrPipe.o",
            f"{ZetaBuildDir}/Debugger.o",
            f"{ZetaBuildDir}/io.o",
            f"{ZetaBuildDir}/Logger.o",
            f"{ZetaBuildDir}/utils.o",
            f"{ZetaExeBuildDir}/test_pipe.o",
        },
        lambda : compiler.to_exe(
            f"{ZetaExeBuildDir}/test_pipe.exe",
            {
                f"{ZetaBuildDir}/DebugStrPipe.o",
                f"{ZetaBuildDir}/Debugger.o",
                f"{ZetaBuildDir}/io.o",
                f"{ZetaBuildDir}/Logger.o",
                f"{ZetaBuildDir}/utils.o",
                f"{ZetaExeBuildDir}/test_pipe.o",
            }
        )
    )

    builder.Add(
        f"{ZetaExeDir}/test_scheduler.cpp",
        {
            f"{File}",
            f"{ZetaExeDir}/ChainingMLTScheduler.h",
            f"{ZetaExeDir}/ChainingStdRBTScheduler.h",
            f"{ZetaExeDir}/NaiveStdRBTScheduler.h",
        },
        None
    )

    builder.Add(
        f"{ZetaExeBuildDir}/test_scheduler.o",
        {
            f"{File}",
            f"{ZetaDir}/MultiLevelTable.h",
            f"{ZetaExeDir}/StdAllocator.h",
            f"{ZetaExeDir}/test_scheduler.cpp",
        },
        lambda : compiler.cpp_to_obj(
            f"{ZetaExeBuildDir}/test_scheduler.o",
            f"{ZetaExeDir}/test_scheduler.cpp",
        )
    )

    builder.Add(
        f"{ZetaExeBuildDir}/test_scheduler.exe",
        {
            f"{File}",

            f"{ZetaBuildDir}/Allocator.o",
            f"{ZetaBuildDir}/DebugHashTable.o",
            f"{ZetaBuildDir}/MultiLevelTable.o",
            f"{ZetaBuildDir}/OrdLinkedListNode.o",
            f"{ZetaBuildDir}/utils.o",

            f"{ZetaExeBuildDir}/test_scheduler.o",
        },
        lambda : compiler.to_exe(
            f"{ZetaExeBuildDir}/test_scheduler.exe",

            {
                f"{ZetaBuildDir}/Allocator.o",
                f"{ZetaBuildDir}/DebugHashTable.o",
                f"{ZetaBuildDir}/MultiLevelTable.o",
                f"{ZetaBuildDir}/OrdLinkedListNode.o",
                f"{ZetaBuildDir}/utils.o",

                f"{ZetaExeBuildDir}/test_scheduler.o",
            }
        )
    )

    builder.Add(
        f"{ZetaExeDir}/switch.s",
        {
            f"{File}",
        },
        None
    )

    builder.Add(
        f"{ZetaExeDir}/test_cntrbt.cpp",
        {
            f"{File}",
            f"{ZetaDir}/BinTree.h",
            f"{ZetaDir}/Debugger.h",
            f"{ZetaDir}/RBTree.h",
            f"{ZetaDir}/OrdCnt3RBTreeNode.h",
            f"{ZetaDir}/OrdCntRBTreeNode.h",
            f"{ZetaDir}/utils.h",
        },
        None
    )

    builder.Add(
        f"{ZetaExeBuildDir}/test_cntrbt.o",
        {
            f"{File}",
            f"{ZetaExeDir}/test_cntrbt.cpp",
        },
        lambda : compiler.cpp_to_obj(
            f"{ZetaExeBuildDir}/test_cntrbt.o",
            f"{ZetaExeDir}/test_cntrbt.cpp",
        )
    )

    builder.Add(
        f"{ZetaExeBuildDir}/test_cntrbt.exe",
        {
            f"{File}",

            f"{ZetaBuildDir}/BinTree.o",
            f"{ZetaBuildDir}/Logger.o",
            f"{ZetaBuildDir}/Debugger.o",
            f"{ZetaBuildDir}/DebugStrPipe.o",
            f"{ZetaBuildDir}/DebugPipe.o",
            f"{ZetaBuildDir}/io.o",
            f"{ZetaBuildDir}/RBTree.o",
            f"{ZetaBuildDir}/OrdCntRBTreeNode.o",
            f"{ZetaBuildDir}/OrdCnt3RBTreeNode.o",
            f"{ZetaBuildDir}/utils.o",

            f"{ZetaExeBuildDir}/test_cntrbt.o",
        },
        lambda : compiler.to_exe(
            f"{ZetaExeBuildDir}/test_cntrbt.exe",

            {
                f"{ZetaBuildDir}/BinTree.o",
                f"{ZetaBuildDir}/Debugger.o",
                f"{ZetaBuildDir}/DebugStrPipe.o",
                f"{ZetaBuildDir}/DebugPipe.o",
                f"{ZetaBuildDir}/io.o",
                f"{ZetaBuildDir}/Logger.o",
                f"{ZetaBuildDir}/RBTree.o",
                f"{ZetaBuildDir}/OrdCntRBTreeNode.o",
                f"{ZetaBuildDir}/OrdCnt3RBTreeNode.o",
                f"{ZetaBuildDir}/utils.o",

                f"{ZetaExeBuildDir}/test_cntrbt.o",
            }
        )
    )

    builder.Add(
        f"{ZetaExeDir}/test_context.c",
        {
            f"{File}",
            f"{ZetaDir}/define.h",
        },
        None
    )

    builder.Add(
        f"{ZetaExeDir}/cpuid.s",
        {
            f"{File}",
        },
        None
    )

    builder.Add(
        f"{ZetaExeDir}/test_cpuid.c",
        {
            f"{File}",
            f"{ZetaDir}/define.h",
        },
        None
    )

    builder.Add(
        f"{ZetaExeDir}/test_crc.c",
        {
            f"{File}",
            f"{ZetaDir}/CRC.h",
        },
        None
    )

    builder.Add(
        f"{ZetaExeDir}/test_sha256.c",
        {
            f"{File}",
            f"{ZetaDir}/SHA256.h",
        },
        None
    )

    builder.Add(
        f"{ZetaExeDir}/Timer.h",
        {
            f"{File}",
        },
        None
    )

    builder.Add(
        f"{ZetaExeDir}/Timer.cpp",
        {
            f"{File}",
            f"{ZetaExeDir}/Timer.h",
        },
        None
    )

    builder.Add(
        f"{ZetaExeBuildDir}/Timer.o",
        {
            f"{File}",
            f"{ZetaExeDir}/Timer.h",
        },
        lambda : compiler.cpp_to_obj(
            f"{ZetaExeBuildDir}/Timer.o",
            f"{ZetaExeDir}/Timer.cpp",
        )
    )

    builder.Add(
        f"{ZetaExeDir}/Value.h",
        {
            f"{File}",
        },
        None
    )

    builder.Add(
        f"{ZetaExeDir}/Value.cpp",
        {
            f"{File}",
            f"{ZetaExeDir}/Value.h",
        },
        None
    )

    builder.Add(
        f"{ZetaExeBuildDir}/Value.o",
        {
            f"{File}",
            f"{ZetaExeDir}/Value.h",
        },
        lambda : compiler.cpp_to_obj(
            f"{ZetaExeBuildDir}/Value.o",
            f"{ZetaExeDir}/Value.cpp",
        )
    )

    builder.Add(
        f"{ZetaExeDir}/ZetaPoolAllocator.h",
        {
            f"{ZetaDir}/Allocator.h",
        },
        None
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

    HighLightPrint(f"args = {args}")

    mode = {
        "debug": mode_enum.debug,
        "release": mode_enum.release,
    }[args.mode]

    ZetaBuildDir = None
    ZetaExeBuildDir = None

    if mode == mode_enum.debug:
        ZetaBuildDir = f"{ZetaDevDir}/ZetaDebugBuild"
        ZetaExeBuildDir = f"{ZetaDevDir}/ZetaExeDebugBuild"

    if mode == mode_enum.release:
        ZetaBuildDir = f"{ZetaDevDir}/ZetaReleaseBuild"
        ZetaExeBuildDir = f"{ZetaDevDir}/ZetaExeReleaseBuild"

    builder = Builder()

    AddDeps(builder, ZetaBuildDir, ZetaExeBuildDir, args.verbose, mode)
    builder.Add("all", builder.GetUnits(), None)

    target = f"{ZetaExeBuildDir}/{args.target}"

    non_built, built = builder.Build(target, args.rebuild)

    ybeg = "\033[93m"
    yend = "\033[0m"

    HighLightPrint("success")

    non_built.sort()
    built.sort()

    HighLightPrint("not_built:")

    for i in non_built:
        print(f"\t{i}")

    HighLightPrint("built:")

    for i in built:
        print(f"\t{i}")

    print(f"{ybeg}target:{yend} {target}")

    if args.run_target:
        print(f"{ybeg}running{yend} {target}")
        os.system(target)

if __name__ == "__main__":
    main()
