import os
import sys

File = os.path.abspath(__file__).replace("\\", "/")
ZetaExeDir = os.path.dirname(File).replace("\\", "/")

ZetaDevDir = f"{ZetaExeDir}/../"

import argparse

sys.path.append(ZetaDevDir)
from builder import *

ZetaDir = os.path.abspath(f"{ZetaDevDir}/Zeta").replace("\\", "/")

sys.path.append(os.path.dirname(ZetaDir))
import Zeta.build

INCLUDE_DIRS = [
]

def AddDeps(builder):
    debug = False

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
        # "-ferror-limit=3", # for clang
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
        # "-ferror-limit=3", # for clang
        # "-fmax-errors=3", # for gcc
    ]

    if debug:
        cargs += ["-O3", "-flto", "-g", "-D DEBUG"]

        cppargs += ["-O3", "-flto", "-g", "-D DEBUG"]
    else:
        cargs += ["-O3", "-flto"]

        cppargs += ["-O3", "-flto"]

    Zeta.build.AddDeps(builder)

    builder.Add(
        f"{File}",
        set(),
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
        f"{ZetaExeDir}/test_1.exe",
        {
            f"{File}",
            f"{ZetaDir}/BinTree.o",
            f"{ZetaDir}/RBTree.o",
            f"{ZetaDir}/OrdCntRBTreeNode.o",
            f"{ZetaDir}/random.o",
            f"{ZetaDir}/utils.o",
            f"{ZetaExeDir}/test_1.c",
        },
        lambda : os.system(" ".join([
            cc,
            "-c",
            f"--output {ZetaExeDir}/test_1.exe",
            f"-c",
            *cargs,
            f"{ZetaDir}/BinTree.o",
            f"{ZetaDir}/RBTree.o",
            f"{ZetaDir}/OrdCntRBTreeNode.o",
            f"{ZetaDir}/random.o",
            f"{ZetaDir}/utils.o",
            f"{ZetaExeDir}/test_1.c",
        ]))
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
        f"{ZetaExeDir}/test_binheap.exe",
        {
            f"{File}",
            f"{ZetaDir}/BinHeap.o",
            f"{ZetaDir}/BinTree.o",
            f"{ZetaDir}/OrdCntRBTreeNode.o",
            f"{ZetaDir}/utils.o",
            f"{ZetaExeDir}/test_binheap.cpp",
        },
        lambda : os.system(" ".join([
            cppc,
            f"--output {ZetaExeDir}/test_binheap.exe",
            *cppargs,
            f"{ZetaDir}/BinHeap.o",
            f"{ZetaDir}/BinTree.o",
            f"{ZetaDir}/OrdCntRBTreeNode.o",
            f"{ZetaDir}/utils.o",
            f"{ZetaExeDir}/test_binheap.cpp",
        ]))
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
        f"{ZetaExeDir}/test_gplist.exe",
        {
            f"{File}",
            f"{ZetaDir}/BinTree.o",
            f"{ZetaDir}/OrdCntRBTreeNode.o",
            f"{ZetaDir}/RBTree.o",
            f"{ZetaDir}/utils.o",
            f"{ZetaExeDir}/test_gplist.c",
        },
        lambda : os.system(" ".join([
            cc,
            f"--output {ZetaExeDir}/test_gplist.exe",
            f"-c",
            *cargs,
            f"{ZetaDir}/BinTree.o",
            f"{ZetaDir}/OrdCntRBTreeNode.o",
            f"{ZetaDir}/RBTree.o",
            f"{ZetaDir}/utils.o",
            f"{ZetaExeDir}/test_gplist.c",
        ]))
    )

    builder.Add(
        f"{ZetaExeDir}/test_gplist.so",
        {
            f"{File}",
            f"{ZetaDir}/BinTree.o",
            f"{ZetaDir}/NearCntRBTreeNode.o",
            f"{ZetaDir}/OrdCntRBTreeNode.o",
            f"{ZetaDir}/RBTree.o",
            f"{ZetaDir}/utils.o",
            f"{ZetaExeDir}/test_gplist.c",
        },
        lambda : os.system(" ".join([
            cc,
            f"--output {ZetaExeDir}/test_gplist.so",
            f"-c",
            *cargs,
            f"-shared",
            f"-fPIC",
            f"{ZetaDir}/BinTree.o",
            f"{ZetaDir}/NearCntRBTreeNode.o",
            f"{ZetaDir}/OrdCntRBTreeNode.o",
            f"{ZetaDir}/RBTree.o",
            f"{ZetaDir}/utils.o",
            f"{ZetaExeDir}/test_gplist.c",
        ]))
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
        f"{ZetaExeDir}/OkMap.o",
        {
            f"{File}",
            f"{ZetaExeDir}/OkMap.cpp",
        },
        lambda : os.system(" ".join([
            cppc,
            f"--output {ZetaExeDir}/OkMap.o",
            "--verbose",
            "-c",
            "-std=c++17",
            *[f"-I \"{path}\"" for path in INCLUDE_DIRS],
            "-m64",
            "-Wall",
            "-Wextra",
            "-ferror-limit=3", # for clang

            # f"-shared",
            # f"-fPIC",
            f"{ZetaExeDir}/OkMap.cpp",
        ]))
    )

    builder.Add(
        f"{ZetaExeDir}/OkMap.so",
        {
            f"{File}",
            f"{ZetaExeDir}/OkMap.cpp",
        },
        lambda : os.system(" ".join([
            cppc,
            f"--output {ZetaExeDir}/OkMap.so",
            "--verbose",
            "-std=c++17",
            *[f"-I \"{path}\"" for path in INCLUDE_DIRS],
            "-m64",
            "-Wall",
            "-Wextra",

            "-ferror-limit=3", # for clang
            f"-shared",
            f"-fPIC",
            f"{ZetaExeDir}/OkMap.cpp",
        ]))
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
        f"{ZetaExeDir}/gplist.so",
        {
            f"{File}",
            f"{ZetaDir}/BinTree.o",
            f"{ZetaDir}/NearCntRBTreeNode.o",
            f"{ZetaDir}/OrdCntRBTreeNode.o",
            f"{ZetaDir}/RBTree.o",
            f"{ZetaDir}/utils.o",
            f"{ZetaExeDir}/gplist.c",
        },
        lambda : os.system(" ".join([
            cc,
            f"--output {ZetaExeDir}/gplist.so",
            f"-c",
            *cargs,
            f"-shared",
            f"-fPIC",
            f"{ZetaDir}/BinTree.o",
            f"{ZetaDir}/NearCntRBTreeNode.o",
            f"{ZetaDir}/OrdCntRBTreeNode.o",
            f"{ZetaDir}/RBTree.o",
            f"{ZetaDir}/utils.o",
            f"{ZetaExeDir}/gplist.c",
        ]))
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
        f"{ZetaExeDir}/mlmap.so",
        {
            f"{File}",
            f"{ZetaDir}/MultiLevelVector.o",
            f"{ZetaDir}/MultiLevelHashTable.o",
            f"{ZetaDir}/utils.o",
            f"{ZetaExeDir}/mlmap.c",
        },
        lambda : os.system(" ".join([
            cc,
            f"--output {ZetaExeDir}/mlmap.so",
            f"-c",
            *cargs,
            f"-shared",
            f"-fPIC",
            f"{ZetaDir}/MultiLevelVector.o",
            f"{ZetaDir}/MultiLevelHashTable.o",
            f"{ZetaDir}/utils.o",
            f"{ZetaExeDir}/mlmap.c",
        ]))
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
        f"{ZetaExeDir}/test_hashtable.exe",
        {
            f"{File}",
            f"{ZetaDir}/MultiLevelVector.o",
            f"{ZetaDir}/MultiLevelHashTable.o",
            f"{ZetaDir}/utils.o",
            f"{ZetaExeDir}/test_hashtable.c",
        },
        lambda : os.system(" ".join([
            cc,
            f"--output {ZetaExeDir}/test_hashtable.exe",
            f"-c",
            *cargs,
            f"{ZetaDir}/MultiLevelVector.o",
            f"{ZetaDir}/MultiLevelHashTable.o",
            f"{ZetaDir}/utils.o",
            f"{ZetaExeDir}/test_hashtable.c",
        ]))
    )

    builder.Add(
        f"{ZetaExeDir}/test_fs.cpp",
        {
            f"{File}",
        },
        None
    )

    builder.Add(
        f"{ZetaExeDir}/test_fs.exe",
        {
            f"{File}",

            f"{ZetaDir}/BlockVector.o",
            f"{ZetaExeDir}/test_fs.cpp",
        },
        lambda : os.system(" ".join([
            cppc,

            f"--output {ZetaExeDir}/test_fs.exe",

            *cppargs,

            f"{ZetaDir}/BlockVector.o",
            f"{ZetaExeDir}/test_fs.cpp",
        ]))
    )

    builder.Add(
        f"{ZetaExeDir}/test_2.c",
        {
            f"{File}",
        },
        None
    )

    builder.Add(
        f"{ZetaExeDir}/test_2.s",
        {
            f"{File}",
            f"{ZetaExeDir}/test_2.c",
        },
        lambda : os.system(" ".join([
            cc,
            f"--output {ZetaExeDir}/test_2.s",
            f"-S",
            f"-c",
            *cargs,
            f"{ZetaExeDir}/test_2.c",
        ]))
    )

    builder.Add(
        f"{ZetaExeDir}/test_2.elf",
        {
            f"{File}",
            f"{ZetaExeDir}/test_2.c",
        },
        lambda : os.system(" ".join([
            cc,
            f"--output {ZetaExeDir}/test_2.elf",
            f"-c",
            *cargs,
            f"{ZetaExeDir}/test_2.c",
        ]))
    )

    builder.Add(
        f"{ZetaExeDir}/test_2.exe",
        {
            f"{File}",
            f"{ZetaExeDir}/test_2.c",
        },
        lambda : os.system(" ".join([
            cc,
            f"--output {ZetaExeDir}/test_2.exe",
            f"-c",
            *cargs,
            f"{ZetaExeDir}/test_2.c",
        ]))
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
        f"{ZetaExeDir}/test_3.exe",
        {
            f"{File}",

            f"{ZetaExeDir}/test_3.c",

            # f"{ZetaDir}/utils.o",
        },
        lambda : os.system(" ".join([
            cc,

            f"--output {ZetaExeDir}/test_3.exe",

            *cargs,

            f"{ZetaExeDir}/test_3.c",

            # f"{ZetaDir}/utils.o",
        ]))
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
        f"{ZetaExeDir}/test_dv.exe",
        {
            f"{File}",
            f"{ZetaDir}/DebugTreeMap.o",
            f"{ZetaDir}/DynamicVector.o",
            f"{ZetaDir}/MultiLevelVector.o",
            f"{ZetaDir}/utils.o",
            f"{ZetaExeDir}/test_dv.cpp",
        },
        lambda : os.system(" ".join([
            cppc,
            f"--output {ZetaExeDir}/test_dv.exe",
            *cppargs,

            f"{ZetaDir}/DebugTreeMap.o",
            f"{ZetaDir}/DynamicVector.o",
            f"{ZetaDir}/MultiLevelVector.o",
            f"{ZetaDir}/utils.o",

            f"{ZetaExeDir}/test_dv.cpp",
        ]))
    )

    builder.Add(
        f"{ZetaExeDir}/test_elf.cpp",
        {
            f"{File}",
        },
        None
    )

    builder.Add(
        f"{ZetaExeDir}/test_elf.exe",
        {
            f"{File}",
            f"{ZetaDir}/utils.o",
            f"{ZetaExeDir}/test_elf.cpp",
        },
        lambda : os.system(" ".join([
            cppc,
            f"--output {ZetaExeDir}/test_elf.exe",
            *cppargs,
            f"{ZetaDir}/utils.o",
            f"{ZetaExeDir}/test_elf.cpp",
        ]))
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
        f"{ZetaExeDir}/test_fat32.exe",
        {
            f"{File}",
            f"{ZetaDir}/utils.o",
            f"{ZetaDir}/DiskInfo.o",
            f"{ZetaDir}/DiskPartGPT.o",
            f"{ZetaDir}/DiskPartMBR.o",
            f"{ZetaDir}/UTF16.o",
            f"{ZetaDir}/UTF8.o",
            f"{ZetaExeDir}/test_fat32.cpp",
        },
        lambda : os.system(" ".join([
            cppc,
            f"--output {ZetaExeDir}/test_fat32.exe",
            *cppargs,
            f"{ZetaDir}/utils.o",
            f"{ZetaDir}/DiskInfo.o",
            f"{ZetaDir}/DiskPartGPT.o",
            f"{ZetaDir}/DiskPartMBR.o",
            f"{ZetaDir}/UTF16.o",
            f"{ZetaDir}/UTF8.o",
            f"{ZetaExeDir}/test_fat32.cpp",
        ]))
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
        f"{ZetaExeDir}/test_treealloc.exe",
        {
            f"{File}",

            f"{ZetaDir}/BinTree.o",
            f"{ZetaDir}/DebugTreeMap.o",
            f"{ZetaDir}/TreeAllocator.o",
            f"{ZetaDir}/RBTree.o",
            f"{ZetaDir}/OrdRBLinkedListNode.o",
            f"{ZetaDir}/OrdRBTreeNode.o",
            f"{ZetaDir}/utils.o",

            f"{ZetaExeDir}/test_treealloc.cpp",
        },
        lambda : os.system(" ".join([
            cppc,
            f"--output {ZetaExeDir}/test_treealloc.exe",
            *cppargs,

            f"{ZetaDir}/BinTree.o",
            f"{ZetaDir}/DebugTreeMap.o",
            f"{ZetaDir}/TreeAllocator.o",
            f"{ZetaDir}/RBTree.o",
            f"{ZetaDir}/OrdRBLinkedListNode.o",
            f"{ZetaDir}/OrdRBTreeNode.o",
            f"{ZetaDir}/utils.o",

            f"{ZetaExeDir}/test_treealloc.cpp",
        ]))
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
        f"{ZetaExeDir}/test_usb.exe",
        {
            f"{File}",
            f"{ZetaDir}/utils.o",
            f"{ZetaDir}/DiskInfo.o",
            f"{ZetaDir}/DiskPartGPT.o",
            f"{ZetaDir}/DiskPartMBR.o",
            f"{ZetaDir}/UTF16.o",
            f"{ZetaDir}/UTF8.o",
            f"{ZetaExeDir}/test_usb.cpp",
        },
        lambda : os.system(" ".join([
            cppc,
            f"--output {ZetaExeDir}/test_usb.exe",
            *cppargs,
            f"{ZetaDir}/utils.o",
            f"{ZetaDir}/DiskInfo.o",
            f"{ZetaDir}/DiskPartGPT.o",
            f"{ZetaDir}/DiskPartMBR.o",
            f"{ZetaDir}/UTF16.o",
            f"{ZetaDir}/UTF8.o",
            f"{ZetaExeDir}/test_usb.cpp",
        ]))
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
        f"{ZetaExeDir}/test_utf8.exe",
        {
            f"{File}",
            f"{ZetaDir}/UTF8.o",
            f"{ZetaDir}/utils.o",
            f"{ZetaExeDir}/test_utf8.c",
        },
        lambda : os.system(" ".join([
            cc,
            f"--output {ZetaExeDir}/test_utf8.exe",
            *cargs,
            f"{ZetaDir}/UTF8.o",
            f"{ZetaDir}/utils.o",
            f"{ZetaExeDir}/test_utf8.c",
        ]))
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
        f"{ZetaExeDir}/test_segvec.exe",
        {
            f"{File}",
            f"{ZetaDir}/SegVector.o",
            f"{ZetaDir}/BinTree.o",
            f"{ZetaDir}/CircularVector.o",
            f"{ZetaDir}/DebugDeque.o",
            f"{ZetaDir}/DebugTreeMap.o",
            f"{ZetaDir}/RBTree.o",
            f"{ZetaDir}/OrdCntRBTreeNode.o",
            f"{ZetaDir}/utils.o",
            f"{ZetaExeDir}/test_segvec.cpp",
        },
        lambda : os.system(" ".join([
            cppc,
            f"--output {ZetaExeDir}/test_segvec.exe",
            *cppargs,
            f"{ZetaDir}/SegVector.o",
            f"{ZetaDir}/BinTree.o",
            f"{ZetaDir}/CircularVector.o",
            f"{ZetaDir}/DebugDeque.o",
            f"{ZetaDir}/DebugTreeMap.o",
            f"{ZetaDir}/RBTree.o",
            f"{ZetaDir}/OrdCntRBTreeNode.o",
            f"{ZetaDir}/utils.o",
            f"{ZetaExeDir}/test_segvec.cpp",
        ]))
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
        f"{ZetaExeDir}/test_segvec2.exe",
        {
            f"{File}",
            f"{ZetaDir}/SegVector.o",
            f"{ZetaDir}/BinTree.o",
            f"{ZetaDir}/CircularVector.o",
            f"{ZetaDir}/DebugDeque.o",
            f"{ZetaDir}/DebugTreeMap.o",
            f"{ZetaDir}/RBTree.o",
            f"{ZetaDir}/OrdCntRBTreeNode.o",
            f"{ZetaDir}/utils.o",
            f"{ZetaExeDir}/test_segvec2.cpp",
        },
        lambda : os.system(" ".join([
            cppc,
            f"--output {ZetaExeDir}/test_segvec2.exe",
            *cppargs,
            f"{ZetaDir}/SegVector.o",
            f"{ZetaDir}/BinTree.o",
            f"{ZetaDir}/CircularVector.o",
            f"{ZetaDir}/DebugDeque.o",
            f"{ZetaDir}/DebugTreeMap.o",
            f"{ZetaDir}/RBTree.o",
            f"{ZetaDir}/OrdCntRBTreeNode.o",
            f"{ZetaDir}/utils.o",
            f"{ZetaExeDir}/test_segvec2.cpp",
        ]))
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
        f"{ZetaExeDir}/test_segvec_speed.exe",
        {
            f"{File}",
            f"{ZetaDir}/SegVector.o",
            f"{ZetaDir}/BinTree.o",
            f"{ZetaDir}/CircularVector.o",
            f"{ZetaDir}/DebugDeque.o",
            f"{ZetaDir}/DebugTreeMap.o",
            f"{ZetaDir}/RBTree.o",
            f"{ZetaDir}/OrdCntRBTreeNode.o",
            f"{ZetaDir}/utils.o",
            f"{ZetaExeDir}/test_segvec_speed.cpp",
        },
        lambda : os.system(" ".join([
            cppc,
            f"--output {ZetaExeDir}/test_segvec_speed.exe",
            *cppargs,
            f"{ZetaDir}/SegVector.o",
            f"{ZetaDir}/BinTree.o",
            f"{ZetaDir}/CircularVector.o",
            f"{ZetaDir}/DebugDeque.o",
            f"{ZetaDir}/DebugTreeMap.o",
            f"{ZetaDir}/RBTree.o",
            f"{ZetaDir}/OrdCntRBTreeNode.o",
            f"{ZetaDir}/utils.o",
            f"{ZetaExeDir}/test_segvec_speed.cpp",
        ]))
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
        f"{ZetaExeDir}/test_segvec2_speed.exe",
        {
            f"{File}",
            f"{ZetaDir}/SegVector.o",
            f"{ZetaDir}/BinTree.o",
            f"{ZetaDir}/CircularVector.o",
            f"{ZetaDir}/DebugTreeMap.o",
            f"{ZetaDir}/RBTree.o",
            f"{ZetaDir}/OrdCntRBTreeNode.o",
            f"{ZetaDir}/utils.o",
            f"{ZetaExeDir}/test_segvec2_speed.cpp",
        },
        lambda : os.system(" ".join([
            cppc,
            f"--output {ZetaExeDir}/test_segvec2_speed.exe",
            *cppargs,
            f"{ZetaDir}/SegVector.o",
            f"{ZetaDir}/BinTree.o",
            f"{ZetaDir}/CircularVector.o",
            f"{ZetaDir}/DebugTreeMap.o",
            f"{ZetaDir}/RBTree.o",
            f"{ZetaDir}/OrdCntRBTreeNode.o",
            f"{ZetaDir}/utils.o",
            f"{ZetaExeDir}/test_segvec2_speed.cpp",
        ]))
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
        f"{ZetaExeDir}/test_seqcntr.exe",
        {
            f"{File}",
            f"{ZetaDir}/BinTree.o",
            f"{ZetaDir}/CircularVector.o",
            f"{ZetaDir}/DebugTreeMap.o",
            f"{ZetaDir}/OrdCntRBTreeNode.o",
            f"{ZetaDir}/RBTree.o",
            f"{ZetaDir}/SegVector.o",
            f"{ZetaDir}/utils.o",
            f"{ZetaExeDir}/test_seqcntr.cpp",
        },
        lambda : os.system(" ".join([
            cppc,
            f"--output {ZetaExeDir}/test_seqcntr.exe",
            *cppargs,
            f"{ZetaDir}/BinTree.o",
            f"{ZetaDir}/CircularVector.o",
            f"{ZetaDir}/DebugTreeMap.o",
            f"{ZetaDir}/OrdCntRBTreeNode.o",
            f"{ZetaDir}/RBTree.o",
            f"{ZetaDir}/SegVector.o",
            f"{ZetaDir}/utils.o",
            f"{ZetaExeDir}/test_seqcntr.cpp",
        ]))
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
        f"{ZetaExeDir}/test_stdio.exe",
        {
            f"{File}",
            f"{ZetaExeDir}/test_stdio.c",
        },
        lambda : os.system(" ".join([
            cc,
            f"--output {ZetaExeDir}/test_stdio.exe",
            f"-c",
            *cargs,
            f"{ZetaExeDir}/test_stdio.c",
        ]))
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
        f"{ZetaExeDir}/test_io.exe",
        {
            f"{File}",
            f"{ZetaExeDir}/FileIO.s",
            f"{ZetaExeDir}/test_io.c",
        },
        lambda : os.system(" ".join([
            cc,
            f"--output {ZetaExeDir}/test_io.exe",
            f"-c",
            *cargs,
            f"{ZetaExeDir}/FileIO.s",
            f"{ZetaExeDir}/test_io.c",
        ]))
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
        f"{ZetaExeDir}/test_lru.exe",
        {
            f"{File}",
            f"{ZetaDir}/LRUCacheManager.o",
            f"{ZetaDir}/OrdLinkedListNode.o",
            f"{ZetaDir}/utils.o",
            f"{ZetaExeDir}/test_lru.cpp",
        },
        lambda : os.system(" ".join([
            cppc,
            f"--output {ZetaExeDir}/test_lru.exe",
            *cppargs,
            f"{ZetaDir}/LRUCacheManager.o",
            f"{ZetaDir}/OrdLinkedListNode.o",
            f"{ZetaDir}/utils.o",
            f"{ZetaExeDir}/test_lru.cpp",
        ]))
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
        f"{ZetaExeDir}/test_lrucm.exe",
        {
            f"{File}",

            f"{ZetaDir}/Allocator.o",
            f"{ZetaDir}/BinTree.o",
            f"{ZetaDir}/BlockVector.o",
            f"{ZetaDir}/CacheManager.o",
            f"{ZetaDir}/DebugPipe.o",
            f"{ZetaDir}/DummyBlockVector.o",
            f"{ZetaDir}/DummyCacheManager.o",
            f"{ZetaDir}/LRUCacheManager.o",
            f"{ZetaDir}/OrdLinkedListNode.o",
            f"{ZetaDir}/OrdRBLinkedListNode.o",
            f"{ZetaDir}/OrdRBTreeNode.o",
            f"{ZetaDir}/RBTree.o",
            f"{ZetaDir}/utils.o",

            f"{ZetaExeDir}/test_lrucm.cpp",
        },
        lambda : os.system(" ".join([
            cppc,
            f"--output {ZetaExeDir}/test_lrucm.exe",
            *cppargs,

            f"{ZetaDir}/Allocator.o",
            f"{ZetaDir}/BinTree.o",
            f"{ZetaDir}/BlockVector.o",
            f"{ZetaDir}/CacheManager.o",
            f"{ZetaDir}/DebugPipe.o",
            f"{ZetaDir}/DummyBlockVector.o",
            f"{ZetaDir}/DummyCacheManager.o",
            f"{ZetaDir}/LRUCacheManager.o",
            f"{ZetaDir}/OrdLinkedListNode.o",
            f"{ZetaDir}/OrdRBLinkedListNode.o",
            f"{ZetaDir}/OrdRBTreeNode.o",
            f"{ZetaDir}/RBTree.o",
            f"{ZetaDir}/utils.o",

            f"{ZetaExeDir}/test_lrucm.cpp",
        ]))
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
        f"{ZetaExeDir}/test_lsb.exe",
        {
            f"{File}",
            f"{ZetaDir}/utils.o",
            f"{ZetaExeDir}/test_lsb.cpp",
        },
        lambda : os.system(" ".join([
            cppc,
            f"--output {ZetaExeDir}/test_lsb.exe",
            *cppargs,
            f"{ZetaDir}/utils.o",
            f"{ZetaExeDir}/test_lsb.cpp",
        ]))
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
        f"{ZetaExeDir}/test_sort.exe",
        {
            f"{File}",
            f"{ZetaDir}/utils.o",
            f"{ZetaDir}/Algorithm.o",
            f"{ZetaDir}/random.o",
            f"{ZetaExeDir}/test_sort.c",
        },
        lambda : os.system(" ".join([
            cc,
            f"--output {ZetaExeDir}/test_sort.exe",
            f"-c",
            *cargs,
            f"{ZetaDir}/utils.o",
            f"{ZetaDir}/Algorithm.o",
            f"{ZetaDir}/random.o",
            f"{ZetaExeDir}/test_sort.c",
        ]))
    )

    builder.Add(
        f"{ZetaExeDir}/test_sort.so",
        {
            f"{File}",
            f"{ZetaDir}/utils.o",
            f"{ZetaDir}/Algorithm.o",
            f"{ZetaDir}/random.o",
            f"{ZetaExeDir}/test_sort.c",
        },
        lambda : os.system(" ".join([
            cc,
            f"--output {ZetaExeDir}/test_sort.so",
            f"-c",
            *cargs,
            f"-shared",
            f"-fPIC",
            f"{ZetaDir}/utils.o",
            f"{ZetaDir}/Algorithm.o",
            f"{ZetaDir}/random.o",
            f"{ZetaExeDir}/test_sort.c",
        ]))
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
        f"{ZetaExeDir}/DyVec.so",
        {
            f"{File}",
            f"{ZetaDir}/DynamicVector.o",
            f"{ZetaDir}/MultiLevelVector.o",
            f"{ZetaDir}/utils.o",
            f"{ZetaExeDir}/DyVec.c",
        },
        lambda : os.system(" ".join([
            cc,
            f"--output {ZetaExeDir}/DyVec.so",
            f"-c",
            *cargs,
            f"-shared",
            f"-fPIC",
            f"{ZetaDir}/DynamicVector.o",
            f"{ZetaDir}/MultiLevelVector.o",
            f"{ZetaDir}/utils.o",
            f"{ZetaExeDir}/DyVec.c",
        ]))
    )

    builder.Add(
        f"{ZetaExeDir}/test_slab.c",
        {
            f"{File}",
            f"{ZetaDir}/Algorithm.h",
            f"{ZetaDir}/PoolAllocator.h",
            f"{ZetaDir}/SlabAllocator.h",
            f"{ZetaDir}/random.h",
        },
        None
    )

    builder.Add(
        f"{ZetaExeDir}/test_slab.exe",
        {
            f"{File}",
            f"{ZetaDir}/utils.o",
            f"{ZetaDir}/OrdLinkedListNode.o",
            f"{ZetaDir}/PoolAllocator.o",
            f"{ZetaDir}/random.o",
            f"{ZetaDir}/SlabAllocator.o",
            f"{ZetaExeDir}/test_slab.c",
        },
        lambda : os.system(" ".join([
            cc,
            f"--output {ZetaExeDir}/test_slab.exe",
            f"-c",
            *cargs,
            f"{ZetaDir}/utils.o",
            f"{ZetaDir}/OrdLinkedListNode.o",
            f"{ZetaDir}/PoolAllocator.o",
            f"{ZetaDir}/random.o",
            f"{ZetaDir}/SlabAllocator.o",
            f"{ZetaExeDir}/test_slab.c",
        ]))
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
        f"{ZetaExeDir}/test_slaballoc.exe",
        {
            f"{File}",
            f"{ZetaDir}/DebugTreeMap.o",
            f"{ZetaDir}/OrdLinkedListNode.o",
            f"{ZetaDir}/OrdRBTreeNode.o",
            f"{ZetaDir}/SlabAllocator.o",
            f"{ZetaDir}/utils.o",

            f"{ZetaExeDir}/test_slaballoc.cpp",
        },
        lambda : os.system(" ".join([
            cppc,
            f"--output {ZetaExeDir}/test_slaballoc.exe",
            *cppargs,
            f"{ZetaDir}/DebugTreeMap.o",
            f"{ZetaDir}/OrdLinkedListNode.o",
            f"{ZetaDir}/OrdRBTreeNode.o",
            f"{ZetaDir}/SlabAllocator.o",
            f"{ZetaDir}/utils.o",

            f"{ZetaExeDir}/test_slaballoc.cpp",
        ]))
    )

    builder.Add(
        f"{ZetaExeDir}/test_mlv.cpp",
        {
            f"{File}",
            f"{ZetaDir}/MultiLevelVector.h",
            f"{ZetaExeDir}/StdAllocator.h",
        },
        None
    )

    builder.Add(
        f"{ZetaExeDir}/test_mlv.exe",
        {
            f"{File}",
            f"{ZetaDir}/Allocator.o",
            f"{ZetaDir}/DebugTreeMap.o",
            f"{ZetaDir}/MultiLevelVector.o",
            f"{ZetaDir}/utils.o",

            f"{ZetaExeDir}/test_mlv.cpp",
        },
        lambda : os.system(" ".join([
            cppc,
            f"--output {ZetaExeDir}/test_mlv.exe",
            *cppargs,

            f"{ZetaDir}/Allocator.o",
            f"{ZetaDir}/DebugTreeMap.o",
            f"{ZetaDir}/MultiLevelVector.o",
            f"{ZetaDir}/utils.o",

            f"{ZetaExeDir}/test_mlv.cpp",
        ]))
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
        f"{ZetaExeDir}/test_OkMap.exe",
        {
            f"{File}",
            f"{ZetaExeDir}/OkMap.o",
            f"{ZetaExeDir}/test_OkMap.cpp",
        },
        lambda : os.system(" ".join([
            cppc,
            f"--output {ZetaExeDir}/test_OkMap.exe",

            "--verbose",
            "-std=c++17",
            *[f"-I \"{path}\"" for path in INCLUDE_DIRS],
            "-m64",
            "-Wall",
            "-Wextra",

            f"{ZetaExeDir}/OkMap.o",
            f"{ZetaExeDir}/test_OkMap.cpp",
        ]))
    )

    builder.Add(
        f"{ZetaExeDir}/test_o1sh.cpp",
        {
            f"{File}",
            f"{ZetaDir}/MultiLevelVector.h",
            f"{ZetaExeDir}/StdAllocator.h",
        },
        None
    )

    builder.Add(
        f"{ZetaExeDir}/test_o1sh.exe",
        {
            f"{File}",

            f"{ZetaDir}/Allocator.o",
            f"{ZetaDir}/DebugTreeMap.o",
            f"{ZetaDir}/MultiLevelVector.o",

            f"{ZetaExeDir}/test_o1sh.cpp",
        },
        lambda : os.system(" ".join([
            cppc,
            f"--output {ZetaExeDir}/test_o1sh.exe",
            *cppargs,

            f"{ZetaDir}/Allocator.o",
            f"{ZetaDir}/DebugTreeMap.o",
            f"{ZetaDir}/MultiLevelVector.o",

            f"{ZetaExeDir}/test_o1sh.cpp",
        ]))
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
        f"{ZetaExeDir}/test_ordalloc.exe",
        {
            f"{File}",
            f"{ZetaDir}/Algorithm.o",
            f"{ZetaDir}/BinTree.o",
            f"{ZetaDir}/DebugTreeMap.o",
            f"{ZetaDir}/OrdAllocator.o",
            f"{ZetaDir}/RBTree.o",
            f"{ZetaDir}/OrdLinkedListNode.o",
            f"{ZetaDir}/OrdRBTreeNode.o",
            f"{ZetaDir}/utils.o",

            f"{ZetaExeDir}/test_ordalloc.cpp",
        },
        lambda : os.system(" ".join([
            cppc,
            f"--output {ZetaExeDir}/test_ordalloc.exe",
            *cppargs,
            f"{ZetaDir}/Algorithm.o",
            f"{ZetaDir}/BinTree.o",
            f"{ZetaDir}/DebugTreeMap.o",
            f"{ZetaDir}/OrdAllocator.o",
            f"{ZetaDir}/RBTree.o",
            f"{ZetaDir}/OrdLinkedListNode.o",
            f"{ZetaDir}/OrdRBTreeNode.o",
            f"{ZetaDir}/utils.o",

            f"{ZetaExeDir}/test_ordalloc.cpp",
        ]))
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
        f"{ZetaExeDir}/test_DebugHashTable.exe",
        {
            f"{File}",
            f"{ZetaDir}/DebugHashTable.o",

            f"{ZetaExeDir}/test_DebugHashTable.cpp",
        },
        lambda : os.system(" ".join([
            cppc,
            f"--output {ZetaExeDir}/test_DebugHashTable.exe",
            "--verbose",
            "-std=c++17",
            *[f"-I \"{path}\"" for path in INCLUDE_DIRS],
            "-m64",
            "-Wall",
            "-Wextra",

            f"{ZetaDir}/DebugHashTable.o",

            f"{ZetaExeDir}/test_DebugHashTable.cpp",
        ]))
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
        f"{ZetaExeDir}/test_cntrbt.exe",
        {
            f"{File}",
            f"{ZetaDir}/BinTree.o",
            f"{ZetaDir}/DebugPipe.o",
            f"{ZetaDir}/RBTree.o",
            f"{ZetaDir}/OrdCntRBTreeNode.o",
            f"{ZetaDir}/utils.o",

            f"{ZetaExeDir}/test_cntrbt.cpp",
        },
        lambda : os.system(" ".join([
            cppc,
            f"--output {ZetaExeDir}/test_cntrbt.exe",
            *cppargs,
            f"{ZetaDir}/BinTree.o",
            f"{ZetaDir}/DebugPipe.o",
            f"{ZetaDir}/RBTree.o",
            f"{ZetaDir}/OrdCntRBTreeNode.o",
            f"{ZetaDir}/utils.o",

            f"{ZetaExeDir}/test_cntrbt.cpp",
        ]))
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
        f"{ZetaExeDir}/test_context.exe",
        {
            f"{File}",
            f"{ZetaExeDir}/switch.s",
            f"{ZetaExeDir}/test_context.c",
        },
        lambda : os.system(" ".join([
            cc,
            f"--output {ZetaExeDir}/test_context.exe",
            f"-c",
            *cargs,
            f"{ZetaExeDir}/switch.s",
            f"{ZetaExeDir}/test_context.c",
        ]))
    )

    builder.Add(
        f"{ZetaExeDir}/test_context.s",
        {
            f"{File}",
            f"{ZetaExeDir}/switch.s",
            f"{ZetaExeDir}/test_context.c",
        },
        lambda : os.system(" ".join([
            cc,
            f"--output {ZetaExeDir}/test_context.s",
            f"-S",
            f"-c",
            *cargs,
            f"{ZetaExeDir}/switch.s",
            f"{ZetaExeDir}/test_context.c",
        ]))
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
        f"{ZetaExeDir}/test_cpuid.exe",
        {
            f"{File}",
            f"{ZetaExeDir}/cpuid.s",
            f"{ZetaExeDir}/test_cpuid.c",
        },
        lambda : os.system(" ".join([
            cc,
            f"--output {ZetaExeDir}/test_cpuid.exe",
            f"-c",
            *cargs,
            f"{ZetaExeDir}/cpuid.s",
            f"{ZetaExeDir}/test_cpuid.c",
        ]))
    )

    builder.Add(
        f"{ZetaExeDir}/test_cpuid.s",
        {
            f"{File}",
            f"{ZetaExeDir}/cpuid.s",
            f"{ZetaExeDir}/test_cpuid.c",
        },
        lambda : os.system(" ".join([
            cc,
            f"--output {ZetaExeDir}/test_cpuid.s",
            f"-S",
            f"-c",
            *cargs,
            f"{ZetaExeDir}/cpuid.s",
            f"{ZetaExeDir}/test_cpuid.c",
        ]))
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
        f"{ZetaExeDir}/test_crc.exe",
        {
            f"{File}",
            f"{ZetaDir}/CRC.o",
            f"{ZetaExeDir}/test_crc.c",
        },
        lambda : os.system(" ".join([
            cc,
            f"--output {ZetaExeDir}/test_crc.exe",
            f"-c",
            *cargs,
            f"{ZetaDir}/CRC.o",
            f"{ZetaExeDir}/test_crc.c",
        ]))
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
        f"{ZetaExeDir}/test_sha256.exe",
        {
            f"{File}",
            f"{ZetaDir}/utils.o",
            f"{ZetaDir}/SHA256.o",
            f"{ZetaExeDir}/test_sha256.c",
        },
        lambda : os.system(" ".join([
            cc,
            f"--output {ZetaExeDir}/test_sha256.exe",
            f"-c",
            *cargs,
            f"{ZetaDir}/utils.o",
            f"{ZetaDir}/SHA256.o",
            f"{ZetaExeDir}/test_sha256.c",
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
