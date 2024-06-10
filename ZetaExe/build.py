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

def AddDeps(builder, ZetaBuildDir, ZetaExeBuildDir, verbose, mode):
    c_include_dirs = []
    cpp_include_dirs = []

    c_to_ll_args = [
        "--verbose" if verbose else "",
        "-std=c2x",
        *[f"-I {ToPath(dir)}" for dir in c_include_dirs],
        "-m64",
        "-O3",
    ]

    cpp_to_ll_args = [
        "--verbose" if verbose else "",
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

    runtime_libs = []

    if os.name == "nt":
        runtime_libs.append(
            "C:/Program Files/clang+llvm-18.1.4-x86_64-pc-windows-msvc/lib/clang/18/lib/windows/clang_rt.builtins-x86_64.lib")

    def lls_to_exe_func_(dst, srcs, linked_tmp_file, opted_tmp_file):
        os.makedirs(os.path.dirname(dst), exist_ok=True)
        os.makedirs(os.path.dirname(linked_tmp_file), exist_ok=True)
        os.makedirs(os.path.dirname(opted_tmp_file), exist_ok=True)

        cmd = " ".join([
            f"llvm-link",
            f"-o {ToPath(linked_tmp_file)}",
            f"-v" if verbose else "",
            f"-S",
            *[ToPath(src) for src in srcs],
        ])

        HighLightPrint(f"cmd = {cmd}")

        rc = os.system(cmd)

        if rc != 0:
            return rc

        cmd = " ".join([
            f"opt",
            f"-o {ToPath(opted_tmp_file)}",
            f"-S",
            f"--O3",
            ToPath(linked_tmp_file),
        ])

        HighLightPrint(f"cmd = {cmd}")

        rc = os.system(cmd)

        if rc != 0:
            return rc

        cmd = " ".join([
            f"clang",
            f"-o {ToPath(dst)}",
            "--verbose" if verbose else "",
            "-m64",
            "-O3",
            *[ToPath(lib) for lib in runtime_libs],
            "-lstdc++",
            ToPath(opted_tmp_file),
        ])

        HighLightPrint(f"cmd = {cmd}")

        rc = os.system(cmd)

        return rc

    def lls_to_exe_func(dst, srcs):
        linked_tmp_file = f"{ZetaExeBuildDir}/tmp.ll"
        opted_tmp_file = f"{ZetaExeBuildDir}/tmp.opt.ll"
        rc = lls_to_exe_func_(dst, srcs, linked_tmp_file, opted_tmp_file)
        # os.remove(linked_tmp_file)
        # os.remove(opted_tmp_file)
        return rc

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
        lambda : c_to_ll_func(
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
            f"{ZetaBuildDir}/DebugHashMap.ll",
            f"{ZetaBuildDir}/DebugPipe.ll",
            f"{ZetaBuildDir}/OrdRBLinkedListNode.ll",
            f"{ZetaBuildDir}/OrdRBTreeNode.ll",
            f"{ZetaBuildDir}/RBTree.ll",
            f"{ZetaBuildDir}/TreeAllocator.ll",
            f"{ZetaBuildDir}/utils.ll",

            f"{ZetaExeBuildDir}/test_treealloc.ll",
        },
        lambda : lls_to_exe_func(
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
            f"{ZetaBuildDir}/DebugHashMap.ll",
            f"{ZetaBuildDir}/DebugPipe.ll",
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
        lambda : cpp_to_ll_func(
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
        lambda : lls_to_exe_func(
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
            f"{ZetaBuildDir}/DebugHashMap.ll",
            f"{ZetaBuildDir}/DebugPipe.ll",
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
            f"{ZetaBuildDir}/DebugHashMap.ll",
            f"{ZetaBuildDir}/DebugPipe.ll",
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
        lambda : cpp_to_ll_func(
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
        lambda : lls_to_exe_func(
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
        lambda : cpp_to_ll_func(
            f"{ZetaExeBuildDir}/test_stagevec.ll",
            f"{ZetaExeDir}/test_stagevec.cpp",
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
            f"{ZetaBuildDir}/StageVector.ll",
            f"{ZetaBuildDir}/utils.ll",

            f"{ZetaExeBuildDir}/test_stagevec.ll",
        },
        lambda : lls_to_exe_func(
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
        lambda : cpp_to_ll_func(
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
        lambda : lls_to_exe_func(
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
            f"{ZetaBuildDir}/DebugHashMap.ll",
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
            f"{ZetaBuildDir}/DebugHashMap.ll",
            f"{ZetaBuildDir}/MultiLevelTable.ll",
            f"{ZetaBuildDir}/utils.ll",

            f"{ZetaExeBuildDir}/test_mlv.ll",
        },
        lambda : lls_to_exe_func(
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
            f"{ZetaBuildDir}/DebugHashMap.ll",
            f"{ZetaBuildDir}/MultiLevelTable.ll",
            f"{ZetaBuildDir}/OrdLinkedListNode.ll",
            f"{ZetaBuildDir}/utils.ll",

            f"{ZetaExeBuildDir}/test_mlt.ll",
        },
        lambda : lls_to_exe_func(
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
        lambda : cpp_to_ll_func(
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
        lambda : lls_to_exe_func(
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
        lambda : cpp_to_ll_func(
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
        lambda : lls_to_exe_func(
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
