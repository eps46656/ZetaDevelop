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
        cargs += ["-g", "-D DEBUG"]

        cppargs += ["-g", "-D DEBUG"]
    else:
        cargs += ["-O3"]

        cppargs += ["-O3"]

    Zeta.build.builder_add(builder)

    builder.add(
        f"{File}",
        set(),
        lambda : 0
    )

    builder.add(
        f"{ZetaExeDir}/test_head.h",
        {
            f"{ZetaDir}/define.h",
        },
        lambda : 0
    )

    builder.add(
        f"{ZetaExeDir}/SimpleVector.h",
        {
            f"{ZetaExeDir}/test_head.h",
        },
        lambda : 0
    )

    builder.add(
        f"{ZetaExeDir}/test_1.c",
        {
            f"{File}",
            f"{ZetaDir}/CntBinTree.h",
            f"{ZetaDir}/OrdCntRBTreeNode.h",
            f"{ZetaDir}/RBTree.h",
            f"{ZetaDir}/random.h",
            f"{ZetaDir}/utils.h",
            f"{ZetaExeDir}/SimpleVector.h",
            f"{ZetaExeDir}/test_head.h",
        },
        lambda : 0
    )

    builder.add(
        f"{ZetaExeDir}/test_1.exe",
        {
            f"{File}",
            f"{ZetaDir}/CntBinTree.o",
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
            f"{ZetaDir}/CntBinTree.o",
            f"{ZetaDir}/RBTree.o",
            f"{ZetaDir}/OrdCntRBTreeNode.o",
            f"{ZetaDir}/random.o",
            f"{ZetaDir}/utils.o",
            f"{ZetaExeDir}/test_1.c",
        ]))
    )

    builder.add(
        f"{ZetaExeDir}/test_gplist.c",
        {
            f"{File}",
            f"{ZetaDir}/CntBinTree.h",
            f"{ZetaDir}/OrdCntRBTreeNode.h",
            f"{ZetaDir}/RBTree.h",
            f"{ZetaDir}/utils.h",
            f"{ZetaExeDir}/SimpleVector.h",
            f"{ZetaExeDir}/test_head.h",
        },
        lambda : 0
    )

    builder.add(
        f"{ZetaExeDir}/test_gplist.exe",
        {
            f"{File}",
            f"{ZetaDir}/CntBinTree.o",
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
            f"{ZetaDir}/CntBinTree.o",
            f"{ZetaDir}/OrdCntRBTreeNode.o",
            f"{ZetaDir}/RBTree.o",
            f"{ZetaDir}/utils.o",
            f"{ZetaExeDir}/test_gplist.c",
        ]))
    )

    builder.add(
        f"{ZetaExeDir}/test_gplist.so",
        {
            f"{File}",
            f"{ZetaDir}/CntBinTree.o",
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
            f"{ZetaDir}/CntBinTree.o",
            f"{ZetaDir}/NearCntRBTreeNode.o",
            f"{ZetaDir}/OrdCntRBTreeNode.o",
            f"{ZetaDir}/RBTree.o",
            f"{ZetaDir}/utils.o",
            f"{ZetaExeDir}/test_gplist.c",
        ]))
    )

    builder.add(
        f"{ZetaExeDir}/OkMap.h",
        {
            f"{File}",
        },
        lambda : 0
    )

    builder.add(
        f"{ZetaExeDir}/OkMap.cpp",
        {
            f"{File}",
            f"{ZetaExeDir}/OkMap.h",
        },
        lambda : 0
    )

    builder.add(
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

    builder.add(
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

    builder.add(
        f"{ZetaExeDir}/gplist.c",
        {
            f"{File}",
            f"{ZetaDir}/CntBinTree.h",
            f"{ZetaDir}/OrdCntRBTreeNode.h",
            f"{ZetaDir}/RBTree.h",
            f"{ZetaDir}/utils.h",
        },
        lambda : 0
    )

    builder.add(
        f"{ZetaExeDir}/gplist.so",
        {
            f"{File}",
            f"{ZetaDir}/BinTree.o",
            f"{ZetaDir}/CntBinTree.o",
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
            f"{ZetaDir}/CntBinTree.o",
            f"{ZetaDir}/NearCntRBTreeNode.o",
            f"{ZetaDir}/OrdCntRBTreeNode.o",
            f"{ZetaDir}/RBTree.o",
            f"{ZetaDir}/utils.o",
            f"{ZetaExeDir}/gplist.c",
        ]))
    )

    builder.add(
        f"{ZetaExeDir}/mlmap.c",
        {
            f"{File}",
            f"{ZetaDir}/MultiLevelHashTable.h",
            f"{ZetaDir}/utils.h",
        },
        lambda : 0
    )

    builder.add(
        f"{ZetaExeDir}/mlmap.so",
        {
            f"{File}",
            f"{ZetaDir}/MultiLevelEntryTable.o",
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
            f"{ZetaDir}/MultiLevelEntryTable.o",
            f"{ZetaDir}/MultiLevelHashTable.o",
            f"{ZetaDir}/utils.o",
            f"{ZetaExeDir}/mlmap.c",
        ]))
    )

    builder.add(
        f"{ZetaExeDir}/test_hashtable.c",
        {
            f"{File}",
            f"{ZetaDir}/MultiLevelHashTable.h",
            f"{ZetaDir}/utils.h",
            f"{ZetaExeDir}/SimpleVector.h",
            f"{ZetaExeDir}/test_head.h",
        },
        lambda : 0
    )

    builder.add(
        f"{ZetaExeDir}/test_hashtable.exe",
        {
            f"{File}",
            f"{ZetaDir}/MultiLevelEntryTable.o",
            f"{ZetaDir}/MultiLevelHashTable.o",
            f"{ZetaDir}/utils.o",
            f"{ZetaExeDir}/test_hashtable.c",
        },
        lambda : os.system(" ".join([
            cc,
            f"--output {ZetaExeDir}/test_hashtable.exe",
            f"-c",
            *cargs,
            f"{ZetaDir}/MultiLevelEntryTable.o",
            f"{ZetaDir}/MultiLevelHashTable.o",
            f"{ZetaDir}/utils.o",
            f"{ZetaExeDir}/test_hashtable.c",
        ]))
    )

    builder.add(
        f"{ZetaExeDir}/test_2.c",
        {
            f"{File}",
        },
        lambda : 0
    )

    builder.add(
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

    builder.add(
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

    builder.add(
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

    builder.add(
        f"{ZetaExeDir}/test_elf.cpp",
        {
            f"{File}",
        },
        lambda : 0
    )

    builder.add(
        f"{ZetaExeDir}/test_elf.exe",
        {
            f"{File}",
            f"{ZetaExeDir}/test_elf.cpp",
        },
        lambda : os.system(" ".join([
            "clang++",
            f"--output {ZetaExeDir}/test_elf.exe",

            "--verbose",
            "-std=c++17",

            "-m64",
            "-Wall",
            "-Wextra",

            f"{ZetaExeDir}/test_elf.cpp",
        ]))
    )

    builder.add(
        f"{ZetaExeDir}/test_utf8.c",
        {
            f"{File}",
            f"{ZetaDir}/define.h"
        },
        lambda : 0
    )

    builder.add(
        f"{ZetaExeDir}/test_utf8.exe",
        {
            f"{File}",
            f"{ZetaExeDir}/test_utf8.c",
        },
        lambda : os.system(" ".join([
            cc,
            f"--output {ZetaExeDir}/test_utf8.exe",
            f"-c",
            *cargs,
            f"{ZetaExeDir}/test_utf8.c",
        ]))
    )

    builder.add(
        f"{ZetaExeDir}/FileIO.s",
        {
            f"{File}",
        },
        lambda : 0
    )

    builder.add(
        f"{ZetaExeDir}/test_io.c",
        {
            f"{File}",
        },
        lambda : 0
    )

    builder.add(
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

    builder.add(
        f"{ZetaExeDir}/test_sort.c",
        {
            f"{File}",
            f"{ZetaDir}/Algorithm.h",
        },
        lambda : 0
    )

    builder.add(
        f"{ZetaExeDir}/test_sort.exe",
        {
            f"{File}",
            f"{ZetaDir}/utils.o",
            f"{ZetaDir}/Algorithm.o",
            f"{ZetaDir}/RawVector.o",
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
            f"{ZetaDir}/RawVector.o",
            f"{ZetaDir}/random.o",
            f"{ZetaExeDir}/test_sort.c",
        ]))
    )

    builder.add(
        f"{ZetaExeDir}/test_sort.so",
        {
            f"{File}",
            f"{ZetaDir}/utils.o",
            f"{ZetaDir}/Algorithm.o",
            f"{ZetaDir}/RawVector.o",
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
            f"{ZetaDir}/RawVector.o",
            f"{ZetaDir}/random.o",
            f"{ZetaExeDir}/test_sort.c",
        ]))
    )

    builder.add(
        f"{ZetaExeDir}/DyVec.c",
        {
            f"{File}",
            f"{ZetaDir}/DynamicVector.h",
        },
        lambda : 0
    )

    builder.add(
        f"{ZetaExeDir}/DyVec.so",
        {
            f"{File}",
            f"{ZetaDir}/DynamicVector.o",
            f"{ZetaDir}/MultiLevelEntryTable.o",
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
            f"{ZetaDir}/MultiLevelEntryTable.o",
            f"{ZetaDir}/utils.o",
            f"{ZetaExeDir}/DyVec.c",
        ]))
    )

    builder.add(
        f"{ZetaExeDir}/test_slab.c",
        {
            f"{File}",
            f"{ZetaDir}/Algorithm.h",
            f"{ZetaDir}/PoolAllocator.h",
            f"{ZetaDir}/RawVector.h",
            f"{ZetaDir}/SlabAllocator.h",
            f"{ZetaDir}/random.h",
        },
        lambda : 0
    )

    builder.add(
        f"{ZetaExeDir}/test_slab.exe",
        {
            f"{File}",
            f"{ZetaDir}/utils.o",
            f"{ZetaDir}/Algorithm.o",
            f"{ZetaDir}/DoublyLinkedNode.o",
            f"{ZetaDir}/PoolAllocator.o",
            f"{ZetaDir}/RawVector.o",
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
            f"{ZetaDir}/Algorithm.o",
            f"{ZetaDir}/DoublyLinkedNode.o",
            f"{ZetaDir}/PoolAllocator.o",
            f"{ZetaDir}/RawVector.o",
            f"{ZetaDir}/random.o",
            f"{ZetaDir}/SlabAllocator.o",
            f"{ZetaExeDir}/test_slab.c",
        ]))
    )

    builder.add(
        f"{ZetaExeDir}/test_OkMap.cpp",
        {
            f"{File}",
            f"{ZetaExeDir}/OkMap.h",
        },
        lambda : 0
    )

    builder.add(
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

    builder.add(
        f"{ZetaExeDir}/test_ordalloc.cpp",
        {
            f"{File}",
            f"{ZetaDir}/DebugHashTable.h",
            f"{ZetaDir}/OrdAllocator.h",
        },
        lambda : 0
    )

    builder.add(
        f"{ZetaExeDir}/test_ordalloc.exe",
        {
            f"{File}",
            f"{ZetaDir}/Algorithm.o",
            f"{ZetaDir}/DebugTreeMap.o",
            f"{ZetaDir}/OrdAllocator.o",
            f"{ZetaDir}/OrdDoublyLinkedNode.o",
            f"{ZetaDir}/OrdRBTreeNode.o",
            f"{ZetaDir}/RawVector.o",
            f"{ZetaDir}/RBTree.o",
            f"{ZetaDir}/utils.o",

            f"{ZetaExeDir}/test_ordalloc.cpp",
        },
        lambda : os.system(" ".join([
            cppc,
            f"--output {ZetaExeDir}/test_ordalloc.exe",
            *cppargs,
            f"{ZetaDir}/Algorithm.o",
            f"{ZetaDir}/DebugTreeMap.o",
            f"{ZetaDir}/OrdAllocator.o",
            f"{ZetaDir}/OrdDoublyLinkedNode.o",
            f"{ZetaDir}/OrdRBTreeNode.o",
            f"{ZetaDir}/RawVector.o",
            f"{ZetaDir}/RBTree.o",
            f"{ZetaDir}/utils.o",

            f"{ZetaExeDir}/test_ordalloc.cpp",
        ]))
    )

    builder.add(
        f"{ZetaExeDir}/test_DebugHashTable.cpp",
        {
            f"{File}",
            f"{ZetaDir}/DebugHashTable.h",
        },
        lambda : 0
    )

    builder.add(
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

    builder.add(
        f"{ZetaExeDir}/switch.s",
        {
            f"{File}",
        },
        lambda : 0
    )

    builder.add(
        f"{ZetaExeDir}/test_context.c",
        {
            f"{File}",
            f"{ZetaDir}/define.h",
        },
        lambda : 0
    )

    builder.add(
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

    builder.add(
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

    builder.add(
        f"{ZetaExeDir}/cpuid.s",
        {
            f"{File}",
        },
        lambda : 0
    )

    builder.add(
        f"{ZetaExeDir}/test_cpuid.c",
        {
            f"{File}",
            f"{ZetaDir}/define.h",
        },
        lambda : 0
    )

    builder.add(
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

    builder.add(
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

    builder.add(
        f"{ZetaExeDir}/test_crc.c",
        {
            f"{File}",
            f"{ZetaDir}/CRC.h",
        },
        lambda : 0
    )

    builder.add(
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

    builder.add(
        f"{ZetaExeDir}/test_sha256.c",
        {
            f"{File}",
            f"{ZetaDir}/SHA256.h",
        },
        lambda : 0
    )

    builder.add(
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
    builder_add(builder)
    builder.add("all", builder.units(), lambda : 0)

    non_built, built = builder.build(GetABSPath(args.target), args.rebuild)

    print("success")
    print("non_built: ", non_built)
    print("built: ", built)
