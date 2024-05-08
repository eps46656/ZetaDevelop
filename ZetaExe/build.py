import os
import sys

File = os.path.abspath(__file__).replace("\\", "/")
ZetaExeDir = os.path.dirname(File).replace("\\", "/")

ZetaDevDir = os.path.abspath(f"{ZetaExeDir}/..").replace("\\", "/")

import argparse

sys.path.append(ZetaDevDir)
from builder import *

ZetaDir = os.path.abspath(f"{ZetaDevDir}/Zeta").replace("\\", "/")

sys.path.append(os.path.dirname(ZetaDir))
import Zeta.build

DEBUG_MODE = 0
RELEASE_MODE = 1

def AddDeps(builder, ZetaBuildDir, ZetaExeBuildDir, mode):
    c_include_dirs = []
    cpp_include_dirs = []

    c_to_ll_args = [
        "--verbose",
        "-std=c2x",
        *[f"-I \"{dir}\"" for dir in c_include_dirs],
        "-m64",
        "-O3",
    ]

    cpp_to_ll_args = [
        "--verbose",
        "-std=c++17",
        *[f"-I \"{dir}\"" for dir in cpp_include_dirs],
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

        rc = os.system(" ".join([
            f"clang",
            f"-o {dst}",
            f"-emit-llvm -S",
            *c_to_ll_args,
            src,
        ]))

        return rc

    def cpp_to_ll_func(dst, src):
        os.makedirs(os.path.dirname(dst), exist_ok=True)

        rc = os.system(" ".join([
            f"clang++",
            f"-o {dst}",
            f"-emit-llvm -S",
            *cpp_to_ll_args,
            src,
        ]))

        return rc

    runtime_libs = []

    if os.name == "nt":
        runtime_libs.append(
            "C:/Program Files/clang+llvm-18.1.4-x86_64-pc-windows-msvc/lib/clang/18/lib/windows/clang_rt.builtins-x86_64.lib")

    def lls_to_exe_func_(dst, srcs, linked_tmp_file, opted_tmp_file):
        os.makedirs(os.path.dirname(dst), exist_ok=True)
        os.makedirs(os.path.dirname(linked_tmp_file), exist_ok=True)
        os.makedirs(os.path.dirname(opted_tmp_file), exist_ok=True)

        rc = os.system(" ".join([
            f"llvm-link",
            f"-o {linked_tmp_file}",
            f"-v",
            f"-S",
            *srcs
        ]))

        if rc != 0:
            return rc

        rc = os.system(" ".join([
            f"opt",
            f"-o {opted_tmp_file}",
            f"-S",
            f"--O3",
            linked_tmp_file,
        ]))

        if rc != 0:
            return rc

        rc = os.system(" ".join([
            f"clang",
            f"-o {dst}",
            "--verbose",
            *[f"\"{lib}\"" for lib in runtime_libs],
            "-m64",
            "-O3",
            opted_tmp_file,
        ]))

        return rc

    def lls_to_exe_func(dst, srcs):
        linked_tmp_file = f"{ZetaExeBuildDir}/tmp.ll"
        opted_tmp_file = f"{ZetaExeBuildDir}/tmp.opt.ll"
        rc = lls_to_exe_func_(dst, srcs, linked_tmp_file, opted_tmp_file)
        # os.remove(linked_tmp_file)
        # os.remove(opted_tmp_file)
        return rc

    # --------------------------------------------------------------------------

    Zeta.build.AddDeps(builder, ZetaBuildDir, mode)

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
            f"{ZetaExeDir}/test_head.h",
        },
        None
    )

    builder.Add(
        f"{ZetaExeDir}/SimpleVector.h",
        {
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
            f"{ZetaExeDir}/SimpleVector.h",
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
        lambda : c_to_ll_func(
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
        lambda : cpp_to_ll_func(
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
            f"{ZetaExeDir}/SimpleVector.h",
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
            f"{ZetaExeDir}/SimpleVector.h",
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
        lambda : c_to_ll_func(
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
        lambda : lls_to_exe_func(
            f"{ZetaExeBuildDir}/test_3.exe",
            [
                f"{ZetaExeBuildDir}/test_3.ll",
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
            f"{ZetaDir}/DebugHashTable.h",
            f"{ZetaDir}/TreeAllocator.h",
        },
        None
    )

    builder.Add(
        f"{ZetaExeBuildDir}/test_treealloc.ll",
        {
            f"{File}",
            f"{ZetaExeDir}/test_treealloc.cpp",
        },
        lambda : cpp_to_ll_func(
            f"{ZetaExeBuildDir}/test_treealloc.ll",
            f"{ZetaExeDir}/test_treealloc.cpp",
        )
    )

    builder.Add(
        f"{ZetaExeBuildDir}/test_treealloc.exe",
        {
            f"{File}",

            f"{ZetaBuildDir}/BinTree.ll",
            f"{ZetaBuildDir}/DebugPipe.ll",
            f"{ZetaBuildDir}/DebugTreeMap.ll",
            f"{ZetaBuildDir}/OrdRBLinkedListNode.ll",
            f"{ZetaBuildDir}/OrdRBTreeNode.ll",
            f"{ZetaBuildDir}/RBTree.ll",
            f"{ZetaBuildDir}/TreeAllocator.ll",
            f"{ZetaBuildDir}/utils.ll",

            f"{ZetaExeDir}/test_treealloc.ll",
        },
        lambda : lls_to_exe_func(
            f"{ZetaExeBuildDir}/test_treealloc.exe",
            [
                f"{ZetaBuildDir}/BinTree.ll",
                f"{ZetaBuildDir}/DebugPipe.ll",
                f"{ZetaBuildDir}/DebugTreeMap.ll",
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
        lambda : cpp_to_ll_func(
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
            f"{ZetaBuildDir}/DebugPipe.ll",
            f"{ZetaBuildDir}/DebugTreeMap.ll",
            f"{ZetaBuildDir}/RBTree.ll",
            f"{ZetaBuildDir}/OrdCntRBTreeNode.ll",
            f"{ZetaBuildDir}/utils.ll",

            f"{ZetaExeBuildDir}/test_segvec.ll",
        },
        lambda : lls_to_exe_func(
            f"{ZetaExeBuildDir}/test_segvec.exe",
            [
                f"{ZetaBuildDir}/SegVector.ll",
                f"{ZetaBuildDir}/BinTree.ll",
                f"{ZetaBuildDir}/CircularVector.ll",
                f"{ZetaBuildDir}/DebugDeque.ll",
                f"{ZetaBuildDir}/DebugPipe.ll",
                f"{ZetaBuildDir}/DebugTreeMap.ll",
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
        lambda : cpp_to_ll_func(
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
            f"{ZetaBuildDir}/DebugPipe.ll",
            f"{ZetaBuildDir}/DebugTreeMap.ll",
            f"{ZetaBuildDir}/RBTree.ll",
            f"{ZetaBuildDir}/OrdCntRBTreeNode.ll",
            f"{ZetaBuildDir}/utils.ll",

            f"{ZetaExeBuildDir}/test_segvec_speed.ll",
        },
        lambda : lls_to_exe_func(
            f"{ZetaExeBuildDir}/test_segvec_speed.exe",
            [
                f"{ZetaBuildDir}/SegVector.ll",
                f"{ZetaBuildDir}/BinTree.ll",
                f"{ZetaBuildDir}/CircularVector.ll",
                f"{ZetaBuildDir}/DebugDeque.ll",
                f"{ZetaBuildDir}/DebugPipe.ll",
                f"{ZetaBuildDir}/DebugTreeMap.ll",
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
        lambda : cpp_to_ll_func(
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
            f"{ZetaBuildDir}/DebugPipe.ll",
            f"{ZetaBuildDir}/DebugTreeMap.ll",
            f"{ZetaBuildDir}/RBTree.ll",
            f"{ZetaBuildDir}/OrdCntRBTreeNode.ll",
            f"{ZetaBuildDir}/utils.ll",

            f"{ZetaExeBuildDir}/test_segvec2_speed.ll",
        },
        lambda : lls_to_exe_func(
            f"{ZetaExeBuildDir}/test_segvec2_speed.exe",
            [
                f"{ZetaBuildDir}/SegVector.ll",
                f"{ZetaBuildDir}/BinTree.ll",
                f"{ZetaBuildDir}/CircularVector.ll",
                f"{ZetaBuildDir}/DebugPipe.ll",
                f"{ZetaBuildDir}/DebugTreeMap.ll",
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
            f"{ZetaDir}/CircularVector.h",
            f"{ZetaDir}/SegVector.h",
            f"{ZetaExeDir}/StdAllocator.h"
        },
        None
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
            f"{ZetaDir}/DebugHashTable.h",
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
        lambda : cpp_to_ll_func(
            f"{ZetaExeBuildDir}/test_slaballoc.ll",
            f"{ZetaExeDir}/test_slaballoc.cpp",
        )
    )

    builder.Add(
        f"{ZetaExeBuildDir}/test_slaballoc.exe",
        {
            f"{File}",

            f"{ZetaBuildDir}/Allocator.ll",
            f"{ZetaBuildDir}/DebugTreeMap.ll",
            f"{ZetaBuildDir}/OrdLinkedListNode.ll",
            f"{ZetaBuildDir}/OrdRBLinkedListNode.ll",
            f"{ZetaBuildDir}/SlabAllocator.ll",
            f"{ZetaBuildDir}/utils.ll",

            f"{ZetaExeBuildDir}/test_slaballoc.ll",
        },
        lambda : lls_to_exe_func(
            f"{ZetaExeBuildDir}/test_slaballoc.exe",
            [
                f"{ZetaBuildDir}/Allocator.ll",
                f"{ZetaBuildDir}/DebugTreeMap.ll",
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
        lambda : cpp_to_ll_func(
            f"{ZetaExeBuildDir}/test_mlv.ll",
            f"{ZetaExeDir}/test_mlv.cpp",
        )
    )

    builder.Add(
        f"{ZetaExeBuildDir}/test_mlv.exe",
        {
            f"{File}",

            f"{ZetaBuildDir}/Allocator.ll",
            f"{ZetaBuildDir}/DebugTreeMap.ll",
            f"{ZetaBuildDir}/MultiLevelTable.ll",
            f"{ZetaBuildDir}/utils.ll",

            f"{ZetaExeBuildDir}/test_mlv.ll",
        },
        lambda : lls_to_exe_func(
            f"{ZetaExeBuildDir}/test_mlv.exe",

            [
                f"{ZetaBuildDir}/Allocator.ll",
                f"{ZetaBuildDir}/DebugTreeMap.ll",
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
        lambda : cpp_to_ll_func(
            f"{ZetaExeBuildDir}/test_mlt.ll",
            f"{ZetaExeDir}/test_mlt.cpp",
        )
    )

    builder.Add(
        f"{ZetaExeBuildDir}/test_mlt.exe",
        {
            f"{File}",

            f"{ZetaBuildDir}/Allocator.ll",
            f"{ZetaBuildDir}/DebugTreeMap.ll",
            f"{ZetaBuildDir}/MultiLevelTable.ll",
            f"{ZetaBuildDir}/OrdLinkedListNode.ll",
            f"{ZetaBuildDir}/utils.ll",

            f"{ZetaExeBuildDir}/test_mlt.ll",
        },
        lambda : lls_to_exe_func(
            f"{ZetaExeBuildDir}/test_mlt.exe",

            [
                f"{ZetaBuildDir}/Allocator.ll",
                f"{ZetaBuildDir}/DebugTreeMap.ll",
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
            f"{ZetaDir}/DebugHashTable.h",
            f"{ZetaDir}/OrdAllocator.h",
        },
        None
    )

    builder.Add(
        f"{ZetaExeDir}/test_DebugHashTable.cpp",
        {
            f"{File}",
            f"{ZetaDir}/DebugHashTable.h",
        },
        None
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
            f"{ZetaDir}/OrdCntRBTreeNode.h",
            f"{ZetaDir}/utils.h",
        },
        None
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
    ZetaExeBuildDir = None

    if mode == DEBUG_MODE:
        ZetaBuildDir = f"{ZetaDevDir}/ZetaDebugBuild"
        ZetaExeBuildDir = f"{ZetaDevDir}/ZetaExeDebugBuild"

    if mode == RELEASE_MODE:
        ZetaBuildDir = f"{ZetaDevDir}/ZetaReleaseBuild"
        ZetaExeBuildDir = f"{ZetaDevDir}/ZetaExeReleaseBuild"

    builder = Builder()

    AddDeps(builder, ZetaBuildDir, ZetaExeBuildDir, mode)
    builder.Add("all", builder.units(), None)

    target = f"{ZetaExeBuildDir}/{args.target}"

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
