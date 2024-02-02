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

    args = [
        "--verbose",
        "-std=gnu17",
        *[f"-I \"{path}\"" for path in INCLUDE_DIRS],
        "-m64",
        "-Wall",
        "-Wextra",

        "-ferror-limit=3", # for clang
        # "-fmax-errors=3", # for gcc

        "-masm=intel"
    ]

    if debug:
        args += ["-g", "-D DEBUG"]
    else:
        args += ["-O3"]

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
            f"-o {ZetaExeDir}/test_1.exe",
            *args,
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
            f"-o {ZetaExeDir}/test_gplist.exe",
            *args,
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
            f"-o {ZetaExeDir}/test_gplist.so",
            *args,
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
            f"-o {ZetaExeDir}/gplist.so",
            *args,
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
            f"-o {ZetaExeDir}/mlmap.so",
            *args,
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
            f"-o {ZetaExeDir}/test_hashtable.exe",
            *args,
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
            f"-o {ZetaExeDir}/test_2.s",
            f"-S",
            *args,
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
            f"-o {ZetaExeDir}/test_2.elf",
            *args,
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
            f"-o {ZetaExeDir}/test_2.exe",
            *args,
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
            f"-o {ZetaExeDir}/test_elf.exe",

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
            f"-o {ZetaExeDir}/test_utf8.exe",
            *args,
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
            f"-o {ZetaExeDir}/test_io.exe",
            *args,
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
            f"-o {ZetaExeDir}/test_sort.exe",
            *args,
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
            f"-o {ZetaExeDir}/test_sort.so",
            *args,
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
            f"-o {ZetaExeDir}/DyVec.so",
            *args,
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
            f"-o {ZetaExeDir}/test_slab.exe",
            *args,
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
        f"{ZetaExeDir}/test_ordalloc.c",
        {
            f"{File}",
            f"{ZetaDir}/OrdAllocator.h",
        },
        lambda : 0
    )

    builder.add(
        f"{ZetaExeDir}/test_ordalloc.exe",
        {
            f"{File}",
            f"{ZetaDir}/Algorithm.o",
            f"{ZetaDir}/OrdAllocator.o",
            f"{ZetaDir}/OrdDoublyLinkedNode.o",
            f"{ZetaDir}/OrdRBTreeNode.o",
            f"{ZetaDir}/RawVector.o",
            f"{ZetaDir}/RBTree.o",
            f"{ZetaDir}/utils.o",
            f"{ZetaExeDir}/test_ordalloc.c",
        },
        lambda : os.system(" ".join([
            cc,
            f"-o {ZetaExeDir}/test_ordalloc.exe",
            *args,
            f"{ZetaDir}/Algorithm.o",
            f"{ZetaDir}/OrdAllocator.o",
            f"{ZetaDir}/OrdDoublyLinkedNode.o",
            f"{ZetaDir}/OrdRBTreeNode.o",
            f"{ZetaDir}/RawVector.o",
            f"{ZetaDir}/RBTree.o",
            f"{ZetaDir}/utils.o",
            f"{ZetaExeDir}/test_ordalloc.c",
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
            f"-o {ZetaExeDir}/test_context.exe",
            *args,
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
            f"-o {ZetaExeDir}/test_context.s",
            f"-S",
            *args,
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
            f"-o {ZetaExeDir}/test_cpuid.exe",
            *args,
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
            f"-o {ZetaExeDir}/test_cpuid.s",
            f"-S",
            *args,
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
            f"-o {ZetaExeDir}/test_crc.exe",
            *args,
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
            f"-o {ZetaExeDir}/test_sha256.exe",
            *args,
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
