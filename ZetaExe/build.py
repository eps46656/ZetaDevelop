import os
import sys

File = os.path.abspath(__file__).replace("\\", "/")
ZetaExeDir = os.path.dirname(File).replace("\\", "/")

ZetaDevDir = os.path.abspath(f"{ZetaExeDir}/..").replace("\\", "/")

import argparse

sys.path.append(ZetaDevDir)

from Builder import *
from Compiler import *

ZetaDir = os.path.abspath(f"{ZetaDevDir}/Zeta").replace("\\", "/")

sys.path.append(os.path.dirname(ZetaDir))
import Zeta.build

def AddDeps(builder, ZetaBuildDir, ZetaExeBuildDir, verbose, mode):
    compiler_config = EmptyClass

    compiler_config.verbose = verbose
    compiler_config.mode = mode
    compiler_config.build_dir = ZetaExeBuildDir

    compiler_config.target = EmptyClass
    compiler_config.target.arch = target.arch
    compiler_config.target.vendor = target.vendor
    compiler_config.target.sys = target.sys
    compiler_config.target.env = target.env

    compiler = Compiler(compiler_config)

    '''
    if os.name == "nt":
        runtime_libs.append(
            "C:/Program Files/clang+llvm-18.1.4-x86_64-pc-windows-msvc/lib/clang/18/lib/windows/clang_rt.builtins-x86_64.lib")
    '''

    # --------------------------------------------------------------------------

    Zeta.build.AddDeps(builder, ZetaBuildDir, verbose, mode)

    builder.Add(
        f"{File}",
        set(),
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
        f"{ZetaExeDir}/FileBlockVector.h",
        {
            f"{ZetaDir}/BlockVector.h",
        },
        None
    )

    builder.Add(
        f"{ZetaExeDir}/StdAllocator.h",
        {
            f"{ZetaDir}/Allocator.h",
            f"{ZetaDir}/DebugHashMap.h",
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
        f"{ZetaExeBuildDir}/test_1.ll",
        {
            f"{File}",
            f"{ZetaExeDir}/test_1.c",
        },
        lambda : compiler.c_to_ll(
            f"{ZetaExeBuildDir}/test_1.ll",
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
        f"{ZetaExeBuildDir}/test_binheap.ll",
        {
            f"{File}",
            f"{ZetaExeDir}/test_binheap.c",
        },
        lambda : compiler.cpp_to_ll(
            f"{ZetaExeBuildDir}/test_binheap.ll",
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
        f"{ZetaExeBuildDir}/test_2.ll",
        {
            f"{File}",
            f"{ZetaExeDir}/test_2.c"
        },
        lambda : compiler.c_to_ll(
            f"{ZetaExeBuildDir}/test_2.ll",
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
        f"{ZetaExeBuildDir}/test_3.ll",
        {
            f"{File}",
            f"{ZetaExeDir}/test_3.c"
        },
        lambda : compiler.c_to_ll(
            f"{ZetaExeBuildDir}/test_3.ll",
            f"{ZetaExeDir}/test_3.c",
        )
    )

    builder.Add(
        f"{ZetaExeBuildDir}/test_3.exe",
        {
            f"{File}",
            f"{ZetaExeBuildDir}/test_3.ll"
        },
        lambda : compiler.lls_to_exe(
            f"{ZetaExeBuildDir}/test_3.exe",
            [
                f"{ZetaExeBuildDir}/test_3.ll",
            ]
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
        f"{ZetaExeBuildDir}/test_btree.ll",
        {
            f"{File}",
            f"{ZetaExeDir}/test_btree.cpp"
        },
        lambda : compiler.cpp_to_ll(
            f"{ZetaExeBuildDir}/test_btree.ll",
            f"{ZetaExeDir}/test_btree.cpp",
        )
    )

    builder.Add(
        f"{ZetaExeBuildDir}/test_btree.exe",
        {
            f"{File}",
            f"{ZetaExeBuildDir}/test_btree.ll",
            f"{ZetaBuildDir}/BTree.ll",
            f"{ZetaBuildDir}/utils.ll",
        },
        lambda : compiler.lls_to_exe(
            f"{ZetaExeBuildDir}/test_btree.exe",
            [
                f"{ZetaExeBuildDir}/test_btree.ll",
                f"{ZetaBuildDir}/BTree.ll",
                f"{ZetaBuildDir}/utils.ll",
            ]
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
        f"{ZetaExeBuildDir}/test_datetime.ll",
        {
            f"{File}",
            f"{ZetaExeDir}/test_datetime.cpp",
        },
        lambda : compiler.cpp_to_ll(
            f"{ZetaExeBuildDir}/test_datetime.ll",
            f"{ZetaExeDir}/test_datetime.cpp",
        )
    )

    builder.Add(
        f"{ZetaExeBuildDir}/test_datetime.exe",
        {
            f"{File}",

            f"{ZetaBuildDir}/DateTime.ll",

            f"{ZetaExeBuildDir}/test_datetime.ll",
        },
        lambda : compiler.lls_to_exe(
            f"{ZetaExeBuildDir}/test_datetime.exe",
            [
                f"{ZetaBuildDir}/DateTime.ll",

                f"{ZetaExeBuildDir}/test_datetime.ll",
            ]
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
            f"{ZetaDir}/DebugHashMap.h",
            f"{ZetaDir}/TreeAllocator.h",
            f"{ZetaExeDir}/MemAllocatorCheck.h",
        },
        None
    )

    builder.Add(
        f"{ZetaExeBuildDir}/test_treealloc.ll",
        {
            f"{File}",
            f"{ZetaExeDir}/test_treealloc.cpp",
        },
        lambda : compiler.cpp_to_ll(
            f"{ZetaExeBuildDir}/test_treealloc.ll",
            f"{ZetaExeDir}/test_treealloc.cpp",
        )
    )

    builder.Add(
        f"{ZetaExeBuildDir}/test_treealloc.exe",
        {
            f"{File}",

            f"{ZetaBuildDir}/BinTree.ll",
            f"{ZetaBuildDir}/DebugHashMap.ll",
            f"{ZetaBuildDir}/DebugPipe.ll",
            f"{ZetaBuildDir}/OrdRBLinkedListNode.ll",
            f"{ZetaBuildDir}/OrdRBTreeNode.ll",
            f"{ZetaBuildDir}/RBTree.ll",
            f"{ZetaBuildDir}/TreeAllocator.ll",
            f"{ZetaBuildDir}/utils.ll",

            f"{ZetaExeBuildDir}/test_treealloc.ll",
        },
        lambda : compiler.lls_to_exe(
            f"{ZetaExeBuildDir}/test_treealloc.exe",
            [
                f"{ZetaBuildDir}/BinTree.ll",
                f"{ZetaBuildDir}/DebugHashMap.ll",
                f"{ZetaBuildDir}/DebugPipe.ll",
                f"{ZetaBuildDir}/OrdRBLinkedListNode.ll",
                f"{ZetaBuildDir}/OrdRBTreeNode.ll",
                f"{ZetaBuildDir}/RBTree.ll",
                f"{ZetaBuildDir}/TreeAllocator.ll",
                f"{ZetaBuildDir}/utils.ll",

                f"{ZetaExeBuildDir}/test_treealloc.ll",
            ]
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
            f"{ZetaDir}/define.h"
        },
        None
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
        f"{ZetaExeBuildDir}/test_segvec.ll",
        {
            f"{File}",
            f"{ZetaExeDir}/test_segvec.cpp",
        },
        lambda : compiler.cpp_to_ll(
            f"{ZetaExeBuildDir}/test_segvec.ll",
            f"{ZetaExeDir}/test_segvec.cpp",
        )
    )

    builder.Add(
        f"{ZetaExeBuildDir}/test_segvec.exe",
        {
            f"{File}",

            f"{ZetaBuildDir}/SegVector.ll",
            f"{ZetaBuildDir}/BinTree.ll",
            f"{ZetaBuildDir}/CircularVector.ll",
            f"{ZetaBuildDir}/DebugDeque.ll",
            f"{ZetaBuildDir}/DebugHashMap.ll",
            f"{ZetaBuildDir}/DebugPipe.ll",
            f"{ZetaBuildDir}/RBTree.ll",
            f"{ZetaBuildDir}/OrdCntRBTreeNode.ll",
            f"{ZetaBuildDir}/utils.ll",

            f"{ZetaExeBuildDir}/test_segvec.ll",
        },
        lambda : compiler.lls_to_exe(
            f"{ZetaExeBuildDir}/test_segvec.exe",
            [
                f"{ZetaBuildDir}/SegVector.ll",
                f"{ZetaBuildDir}/BinTree.ll",
                f"{ZetaBuildDir}/CircularVector.ll",
                f"{ZetaBuildDir}/DebugDeque.ll",
                f"{ZetaBuildDir}/DebugHashMap.ll",
                f"{ZetaBuildDir}/DebugPipe.ll",
                f"{ZetaBuildDir}/RBTree.ll",
                f"{ZetaBuildDir}/OrdCntRBTreeNode.ll",
                f"{ZetaBuildDir}/utils.ll",

                f"{ZetaExeBuildDir}/test_segvec.ll",
            ]
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
        f"{ZetaExeBuildDir}/test_segvec2.ll",
        {
            f"{File}",
            f"{ZetaExeDir}/test_segvec2.cpp",
        },
        lambda : compiler.cpp_to_ll(
            f"{ZetaExeBuildDir}/test_segvec2.ll",
            f"{ZetaExeDir}/test_segvec2.cpp",
        )
    )

    builder.Add(
        f"{ZetaExeBuildDir}/test_segvec2.exe",
        {
            f"{File}",

            f"{ZetaBuildDir}/BinTree.ll",
            f"{ZetaBuildDir}/CircularVector.ll",
            f"{ZetaBuildDir}/DebugDeque.ll",
            f"{ZetaBuildDir}/DebugHashMap.ll",
            f"{ZetaBuildDir}/DebugPipe.ll",
            f"{ZetaBuildDir}/OrdCntRBTreeNode.ll",
            f"{ZetaBuildDir}/RBTree.ll",
            f"{ZetaBuildDir}/SegVector.ll",
            f"{ZetaBuildDir}/utils.ll",

            f"{ZetaExeBuildDir}/test_segvec2.ll",
        },
        lambda : compiler.lls_to_exe(
            f"{ZetaExeBuildDir}/test_segvec2.exe",
            [
                f"{ZetaBuildDir}/BinTree.ll",
                f"{ZetaBuildDir}/CircularVector.ll",
                f"{ZetaBuildDir}/DebugDeque.ll",
                f"{ZetaBuildDir}/DebugHashMap.ll",
                f"{ZetaBuildDir}/DebugPipe.ll",
                f"{ZetaBuildDir}/OrdCntRBTreeNode.ll",
                f"{ZetaBuildDir}/RBTree.ll",
                f"{ZetaBuildDir}/SegVector.ll",
                f"{ZetaBuildDir}/utils.ll",

                f"{ZetaExeBuildDir}/test_segvec2.ll",
            ]
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
        f"{ZetaExeBuildDir}/test_segvec_speed.ll",
        {
            f"{File}",
            f"{ZetaExeDir}/test_segvec_speed.cpp",
        },
        lambda : compiler.cpp_to_ll(
            f"{ZetaExeBuildDir}/test_segvec_speed.ll",
            f"{ZetaExeDir}/test_segvec_speed.cpp",
        )
    )

    builder.Add(
        f"{ZetaExeBuildDir}/test_segvec_speed.exe",
        {
            f"{File}",

            f"{ZetaBuildDir}/SegVector.ll",
            f"{ZetaBuildDir}/BinTree.ll",
            f"{ZetaBuildDir}/CircularVector.ll",
            f"{ZetaBuildDir}/DebugDeque.ll",
            f"{ZetaBuildDir}/DebugHashMap.ll",
            f"{ZetaBuildDir}/DebugPipe.ll",
            f"{ZetaBuildDir}/RBTree.ll",
            f"{ZetaBuildDir}/OrdCntRBTreeNode.ll",
            f"{ZetaBuildDir}/utils.ll",

            f"{ZetaExeBuildDir}/test_segvec_speed.ll",
        },
        lambda : compiler.lls_to_exe(
            f"{ZetaExeBuildDir}/test_segvec_speed.exe",
            [
                f"{ZetaBuildDir}/SegVector.ll",
                f"{ZetaBuildDir}/BinTree.ll",
                f"{ZetaBuildDir}/CircularVector.ll",
                f"{ZetaBuildDir}/DebugDeque.ll",
                f"{ZetaBuildDir}/DebugHashMap.ll",
                f"{ZetaBuildDir}/DebugPipe.ll",
                f"{ZetaBuildDir}/RBTree.ll",
                f"{ZetaBuildDir}/OrdCntRBTreeNode.ll",
                f"{ZetaBuildDir}/utils.ll",

                f"{ZetaExeBuildDir}/test_segvec_speed.ll",
            ]
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
        f"{ZetaExeBuildDir}/test_segvec2_speed.ll",
        {
            f"{File}",
            f"{ZetaExeDir}/test_segvec2_speed.cpp",
        },
        lambda : compiler.cpp_to_ll(
            f"{ZetaExeBuildDir}/test_segvec2_speed.ll",
            f"{ZetaExeDir}/test_segvec2_speed.cpp",
        )
    )

    builder.Add(
        f"{ZetaExeBuildDir}/test_segvec2_speed.exe",
        {
            f"{File}",

            f"{ZetaBuildDir}/SegVector.ll",
            f"{ZetaBuildDir}/BinTree.ll",
            f"{ZetaBuildDir}/CircularVector.ll",
            f"{ZetaBuildDir}/DebugHashMap.ll",
            f"{ZetaBuildDir}/DebugPipe.ll",
            f"{ZetaBuildDir}/RBTree.ll",
            f"{ZetaBuildDir}/OrdCntRBTreeNode.ll",
            f"{ZetaBuildDir}/utils.ll",

            f"{ZetaExeBuildDir}/test_segvec2_speed.ll",
        },
        lambda : compiler.lls_to_exe(
            f"{ZetaExeBuildDir}/test_segvec2_speed.exe",
            [
                f"{ZetaBuildDir}/SegVector.ll",
                f"{ZetaBuildDir}/BinTree.ll",
                f"{ZetaBuildDir}/CircularVector.ll",
                f"{ZetaBuildDir}/DebugHashMap.ll",
                f"{ZetaBuildDir}/DebugPipe.ll",
                f"{ZetaBuildDir}/RBTree.ll",
                f"{ZetaBuildDir}/OrdCntRBTreeNode.ll",
                f"{ZetaBuildDir}/utils.ll",

                f"{ZetaExeBuildDir}/test_segvec2_speed.ll",
            ]
        )
    )

    builder.Add(
        f"{ZetaExeDir}/test_seqcntr.cpp",
        {
            f"{File}",
            f"{ZetaDir}/DebugDeque.h",
            f"{ZetaDir}/CircularVector.h",
            f"{ZetaDir}/SegVector.h",
            f"{ZetaExeDir}/StdAllocator.h"
        },
        None
    )

    builder.Add(
        f"{ZetaExeBuildDir}/test_seqcntr.ll",
        {
            f"{File}",
            f"{ZetaExeDir}/test_seqcntr.cpp",
        },
        lambda : compiler.cpp_to_ll(
            f"{ZetaExeBuildDir}/test_seqcntr.ll",
            f"{ZetaExeDir}/test_seqcntr.cpp",
        )
    )

    builder.Add(
        f"{ZetaExeBuildDir}/test_seqcntr.exe",
        {
            f"{File}",

            f"{ZetaBuildDir}/BinTree.ll",
            f"{ZetaBuildDir}/CircularVector.ll",
            f"{ZetaBuildDir}/DebugDeque.ll",
            f"{ZetaBuildDir}/DebugHashMap.ll",
            f"{ZetaBuildDir}/DebugPipe.ll",
            f"{ZetaBuildDir}/RBTree.ll",
            f"{ZetaBuildDir}/OrdCntRBTreeNode.ll",
            f"{ZetaBuildDir}/SegVector.ll",
            f"{ZetaBuildDir}/utils.ll",

            f"{ZetaExeBuildDir}/test_seqcntr.ll",
        },
        lambda : compiler.lls_to_exe(
            f"{ZetaExeBuildDir}/test_seqcntr.exe",
            [
                f"{ZetaBuildDir}/BinTree.ll",
                f"{ZetaBuildDir}/CircularVector.ll",
                f"{ZetaBuildDir}/DebugDeque.ll",
                f"{ZetaBuildDir}/DebugHashMap.ll",
                f"{ZetaBuildDir}/DebugPipe.ll",
                f"{ZetaBuildDir}/RBTree.ll",
                f"{ZetaBuildDir}/OrdCntRBTreeNode.ll",
                f"{ZetaBuildDir}/SegVector.ll",
                f"{ZetaBuildDir}/utils.ll",

                f"{ZetaExeBuildDir}/test_seqcntr.ll",
            ]
        )
    )

    builder.Add(
        f"{ZetaExeDir}/test_stagevec.cpp",
        {
            f"{File}",
            f"{ZetaDir}/DebugDeque.h",
            f"{ZetaDir}/CircularVector.h",
            f"{ZetaDir}/SegVector.h",
            f"{ZetaDir}/StageVector.h",
            f"{ZetaExeDir}/MemAllocatorCheck.h",
            f"{ZetaExeDir}/StdAllocator.h",
        },
        None
    )

    builder.Add(
        f"{ZetaExeBuildDir}/test_stagevec.ll",
        {
            f"{File}",
            f"{ZetaExeDir}/test_stagevec.cpp",
        },
        lambda : compiler.cpp_to_ll(
            f"{ZetaExeBuildDir}/test_stagevec.ll",
            f"{ZetaExeDir}/test_stagevec.cpp",
        )
    )

    builder.Add(
        f"{ZetaExeBuildDir}/test_stagevec.s",
        {
            f"{File}",
            f"{ZetaExeBuildDir}/test_stagevec.ll",
        },
        lambda : compiler.ll_to_asm(
            f"{ZetaExeBuildDir}/test_stagevec.s",
            f"{ZetaExeBuildDir}/test_stagevec.ll",
        )
    )

    builder.Add(
        f"{ZetaExeBuildDir}/test_stagevec.o",
        {
            f"{File}",
            f"{ZetaExeBuildDir}/test_stagevec.ll",
        },
        lambda : compiler.ll_to_obj(
            f"{ZetaExeBuildDir}/test_stagevec.o",
            f"{ZetaExeBuildDir}/test_stagevec.ll",
        )
    )

    builder.Add(
        f"{ZetaExeBuildDir}/test_stagevec.exe",
        {
            f"{File}",

            f"{ZetaBuildDir}/BinTree.ll",
            f"{ZetaBuildDir}/CircularVector.ll",
            f"{ZetaBuildDir}/DebugDeque.ll",
            f"{ZetaBuildDir}/DebugHashMap.ll",
            f"{ZetaBuildDir}/DebugPipe.ll",
            f"{ZetaBuildDir}/RBTree.ll",
            f"{ZetaBuildDir}/OrdCnt3RBTreeNode.ll",
            f"{ZetaBuildDir}/OrdCntRBTreeNode.ll",
            f"{ZetaBuildDir}/SegVector.ll",
            f"{ZetaBuildDir}/SegUtils.ll",
            f"{ZetaBuildDir}/StageVector.ll",
            f"{ZetaBuildDir}/utils.ll",

            f"{ZetaExeBuildDir}/test_stagevec.ll",
        },
        lambda : compiler.lls_to_exe(
            f"{ZetaExeBuildDir}/test_stagevec.exe",
            [
                f"{ZetaBuildDir}/BinTree.ll",
                f"{ZetaBuildDir}/CircularVector.ll",
                f"{ZetaBuildDir}/DebugDeque.ll",
                f"{ZetaBuildDir}/DebugHashMap.ll",
                f"{ZetaBuildDir}/DebugPipe.ll",
                f"{ZetaBuildDir}/RBTree.ll",
                f"{ZetaBuildDir}/OrdCnt3RBTreeNode.ll",
                f"{ZetaBuildDir}/OrdCntRBTreeNode.ll",
                f"{ZetaBuildDir}/SegVector.ll",
                f"{ZetaBuildDir}/SegUtils.ll",
                f"{ZetaBuildDir}/StageVector.ll",
                f"{ZetaBuildDir}/utils.ll",

                f"{ZetaExeBuildDir}/test_stagevec.ll",
            ]
        )
    )

    builder.Add(
        f"{ZetaExeDir}/test_stdio.c",
        {
            f"{File}",
            f"{ZetaDir}/define.h"
            f"{ZetaDir}/Stream.h"
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
        f"{ZetaExeBuildDir}/test_kmp.ll",
        {
            f"{File}",
            f"{ZetaExeDir}/test_kmp.cpp"
        },
        lambda : compiler.cpp_to_ll(
            f"{ZetaExeBuildDir}/test_kmp.ll",
            f"{ZetaExeDir}/test_kmp.cpp",
        )
    )

    builder.Add(
        f"{ZetaExeBuildDir}/test_kmp.exe",
        {
            f"{File}",
            f"{ZetaExeBuildDir}/test_kmp.ll"
        },
        lambda : compiler.lls_to_exe(
            f"{ZetaExeBuildDir}/test_kmp.exe",
            [
                f"{ZetaExeBuildDir}/test_kmp.ll",
            ]
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
        f"{ZetaExeBuildDir}/test_LeftistHeap.ll",
        {
            f"{File}",
            f"{ZetaExeDir}/test_LeftistHeap.cpp",
        },
        lambda : compiler.cpp_to_ll(
            f"{ZetaExeBuildDir}/test_LeftistHeap.ll",
            f"{ZetaExeDir}/test_LeftistHeap.cpp",
        )
    )

    builder.Add(
        f"{ZetaExeBuildDir}/test_LeftistHeap.exe",
        {
            f"{File}",
            f"{ZetaExeBuildDir}/test_LeftistHeap.ll",
        },
        lambda : compiler.lls_to_exe(
            f"{ZetaExeBuildDir}/test_LeftistHeap.exe",
            [
                f"{ZetaExeBuildDir}/test_LeftistHeap.ll",
            ]
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
        f"{ZetaExeBuildDir}/test_lrucm.ll",
        {
            f"{File}",
            f"{ZetaExeDir}/test_lrucm.cpp",
        },
        lambda : compiler.cpp_to_ll(
            f"{ZetaExeBuildDir}/test_lrucm.ll",
            f"{ZetaExeDir}/test_lrucm.cpp",
        )
    )

    builder.Add(
        f"{ZetaExeBuildDir}/test_lrucm.exe",
        {
            f"{File}",

            f"{ZetaBuildDir}/Allocator.ll",
            f"{ZetaBuildDir}/BinTree.ll",
            f"{ZetaBuildDir}/BlockVector.ll",
            f"{ZetaBuildDir}/DebugHashMap.ll",
            f"{ZetaBuildDir}/DebugPipe.ll",
            f"{ZetaBuildDir}/DummyBlockVector.ll",
            f"{ZetaBuildDir}/DummyCacheManager.ll",
            f"{ZetaBuildDir}/LRUCacheManager.ll",
            f"{ZetaBuildDir}/OrdLinkedListNode.ll",
            f"{ZetaBuildDir}/OrdRBLinkedListNode.ll",
            f"{ZetaBuildDir}/OrdRBTreeNode.ll",
            f"{ZetaBuildDir}/RBTree.ll",
            f"{ZetaBuildDir}/utils.ll",

            f"{ZetaExeBuildDir}/test_lrucm.ll",
        },
        lambda : compiler.lls_to_exe(
            f"{ZetaExeBuildDir}/test_lrucm.exe",
            [
                f"{ZetaBuildDir}/Allocator.ll",
                f"{ZetaBuildDir}/BinTree.ll",
                f"{ZetaBuildDir}/BlockVector.ll",
                f"{ZetaBuildDir}/DebugHashMap.ll",
                f"{ZetaBuildDir}/DebugPipe.ll",
                f"{ZetaBuildDir}/DummyBlockVector.ll",
                f"{ZetaBuildDir}/DummyCacheManager.ll",
                f"{ZetaBuildDir}/LRUCacheManager.ll",
                f"{ZetaBuildDir}/OrdLinkedListNode.ll",
                f"{ZetaBuildDir}/OrdRBLinkedListNode.ll",
                f"{ZetaBuildDir}/OrdRBTreeNode.ll",
                f"{ZetaBuildDir}/RBTree.ll",
                f"{ZetaBuildDir}/utils.ll",

                f"{ZetaExeBuildDir}/test_lrucm.ll",
            ]
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
        f"{ZetaExeDir}/test_sort.c",
        {
            f"{File}",
            f"{ZetaDir}/Algorithm.h",
        },
        None
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
            f"{ZetaDir}/DebugHashMap.h",
            f"{ZetaDir}/SlabAllocator.h",
        },
        None
    )

    builder.Add(
        f"{ZetaExeBuildDir}/test_slaballoc.ll",
        {
            f"{File}",
            f"{ZetaExeDir}/test_slaballoc.cpp",
        },
        lambda : compiler.cpp_to_ll(
            f"{ZetaExeBuildDir}/test_slaballoc.ll",
            f"{ZetaExeDir}/test_slaballoc.cpp",
        )
    )

    builder.Add(
        f"{ZetaExeBuildDir}/test_slaballoc.exe",
        {
            f"{File}",

            f"{ZetaBuildDir}/Allocator.ll",
            f"{ZetaBuildDir}/DebugHashMap.ll",
            f"{ZetaBuildDir}/OrdLinkedListNode.ll",
            f"{ZetaBuildDir}/OrdRBLinkedListNode.ll",
            f"{ZetaBuildDir}/SlabAllocator.ll",
            f"{ZetaBuildDir}/utils.ll",

            f"{ZetaExeBuildDir}/test_slaballoc.ll",
        },
        lambda : compiler.lls_to_exe(
            f"{ZetaExeBuildDir}/test_slaballoc.exe",
            [
                f"{ZetaBuildDir}/Allocator.ll",
                f"{ZetaBuildDir}/DebugHashMap.ll",
                f"{ZetaBuildDir}/OrdLinkedListNode.ll",
                f"{ZetaBuildDir}/OrdRBLinkedListNode.ll",
                f"{ZetaBuildDir}/SlabAllocator.ll",
                f"{ZetaBuildDir}/utils.ll",

                f"{ZetaExeBuildDir}/test_slaballoc.ll",
            ]
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
        f"{ZetaExeBuildDir}/test_mlv.ll",
        {
            f"{File}",
            f"{ZetaDir}/MultiLevelTable.h",
            f"{ZetaExeDir}/StdAllocator.h",
            f"{ZetaExeDir}/test_mlv.cpp",
        },
        lambda : compiler.cpp_to_ll(
            f"{ZetaExeBuildDir}/test_mlv.ll",
            f"{ZetaExeDir}/test_mlv.cpp",
        )
    )

    builder.Add(
        f"{ZetaExeBuildDir}/test_mlv.exe",
        {
            f"{File}",

            f"{ZetaBuildDir}/Allocator.ll",
            f"{ZetaBuildDir}/DebugHashMap.ll",
            f"{ZetaBuildDir}/MultiLevelTable.ll",
            f"{ZetaBuildDir}/utils.ll",

            f"{ZetaExeBuildDir}/test_mlv.ll",
        },
        lambda : compiler.lls_to_exe(
            f"{ZetaExeBuildDir}/test_mlv.exe",

            [
                f"{ZetaBuildDir}/Allocator.ll",
                f"{ZetaBuildDir}/DebugHashMap.ll",
                f"{ZetaBuildDir}/MultiLevelTable.ll",
                f"{ZetaBuildDir}/utils.ll",

                f"{ZetaExeBuildDir}/test_mlv.ll",
            ]
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
        f"{ZetaExeBuildDir}/test_mlt.ll",
        {
            f"{File}",
            f"{ZetaDir}/MultiLevelTable.h",
            f"{ZetaExeDir}/StdAllocator.h",
            f"{ZetaExeDir}/test_mlt.cpp",
        },
        lambda : compiler.cpp_to_ll(
            f"{ZetaExeBuildDir}/test_mlt.ll",
            f"{ZetaExeDir}/test_mlt.cpp",
        )
    )

    builder.Add(
        f"{ZetaExeBuildDir}/test_mlt.exe",
        {
            f"{File}",

            f"{ZetaBuildDir}/Allocator.ll",
            f"{ZetaBuildDir}/DebugHashMap.ll",
            f"{ZetaBuildDir}/MultiLevelTable.ll",
            f"{ZetaBuildDir}/OrdLinkedListNode.ll",
            f"{ZetaBuildDir}/utils.ll",

            f"{ZetaExeBuildDir}/test_mlt.ll",
        },
        lambda : compiler.lls_to_exe(
            f"{ZetaExeBuildDir}/test_mlt.exe",

            [
                f"{ZetaBuildDir}/Allocator.ll",
                f"{ZetaBuildDir}/DebugHashMap.ll",
                f"{ZetaBuildDir}/MultiLevelTable.ll",
                f"{ZetaBuildDir}/OrdLinkedListNode.ll",
                f"{ZetaBuildDir}/utils.ll",

                f"{ZetaExeBuildDir}/test_mlt.ll",
            ]
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
            f"{ZetaDir}/DebugHashMap.h",
            f"{ZetaDir}/OrdAllocator.h",
        },
        None
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
        f"{ZetaExeBuildDir}/test_scheduler.ll",
        {
            f"{File}",
            f"{ZetaDir}/MultiLevelTable.h",
            f"{ZetaExeDir}/StdAllocator.h",
            f"{ZetaExeDir}/test_scheduler.cpp",
        },
        lambda : compiler.cpp_to_ll(
            f"{ZetaExeBuildDir}/test_scheduler.ll",
            f"{ZetaExeDir}/test_scheduler.cpp",
        )
    )

    builder.Add(
        f"{ZetaExeBuildDir}/test_scheduler.exe",
        {
            f"{File}",

            f"{ZetaBuildDir}/Allocator.ll",
            f"{ZetaBuildDir}/DebugHashMap.ll",
            f"{ZetaBuildDir}/MultiLevelTable.ll",
            f"{ZetaBuildDir}/OrdLinkedListNode.ll",
            f"{ZetaBuildDir}/utils.ll",

            f"{ZetaExeBuildDir}/test_scheduler.ll",
        },
        lambda : compiler.lls_to_exe(
            f"{ZetaExeBuildDir}/test_scheduler.exe",

            [
                f"{ZetaBuildDir}/Allocator.ll",
                f"{ZetaBuildDir}/DebugHashMap.ll",
                f"{ZetaBuildDir}/MultiLevelTable.ll",
                f"{ZetaBuildDir}/OrdLinkedListNode.ll",
                f"{ZetaBuildDir}/utils.ll",

                f"{ZetaExeBuildDir}/test_scheduler.ll",
            ]
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
            f"{ZetaDir}/RBTree.h",
            f"{ZetaDir}/OrdCnt3RBTreeNode.h",
            f"{ZetaDir}/OrdCntRBTreeNode.h",
            f"{ZetaDir}/utils.h",
        },
        None
    )

    builder.Add(
        f"{ZetaExeBuildDir}/test_cntrbt.ll",
        {
            f"{File}",
            f"{ZetaExeDir}/test_cntrbt.cpp",
        },
        lambda : compiler.cpp_to_ll(
            f"{ZetaExeBuildDir}/test_cntrbt.ll",
            f"{ZetaExeDir}/test_cntrbt.cpp",
        )
    )

    builder.Add(
        f"{ZetaExeBuildDir}/test_cntrbt.exe",
        {
            f"{File}",

            f"{ZetaBuildDir}/BinTree.ll",
            f"{ZetaBuildDir}/DebugPipe.ll",
            f"{ZetaBuildDir}/RBTree.ll",
            f"{ZetaBuildDir}/OrdCntRBTreeNode.ll",
            f"{ZetaBuildDir}/OrdCnt3RBTreeNode.ll",
            f"{ZetaBuildDir}/utils.ll",

            f"{ZetaExeBuildDir}/test_cntrbt.ll",
        },
        lambda : compiler.lls_to_exe(
            f"{ZetaExeBuildDir}/test_cntrbt.exe",

            [
                f"{ZetaBuildDir}/BinTree.ll",
                f"{ZetaBuildDir}/DebugPipe.ll",
                f"{ZetaBuildDir}/RBTree.ll",
                f"{ZetaBuildDir}/OrdCntRBTreeNode.ll",
                f"{ZetaBuildDir}/OrdCnt3RBTreeNode.ll",
                f"{ZetaBuildDir}/utils.ll",

                f"{ZetaExeBuildDir}/test_cntrbt.ll",
            ]
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
                        required=True,)

    parser.add_argument("--mode",
                        dest="mode",
                        action="store",
                        choices=["debug", "release"],
                        required=True,)

    parser.add_argument("--verbose",
                        dest="verbose",
                        action="store_true")

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
    builder.Add("all", builder.units(), None)

    target = f"{ZetaExeBuildDir}/{args.target}"

    non_built, built = builder.build(target, args.rebuild)

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
