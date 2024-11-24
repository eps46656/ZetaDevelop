import argparse
import os
import sys

import termcolor
from typeguard import typechecked

import Zeta
from Builder import Builder
from config import ModeEnum, target
from LLVMCompiler import LLVMCompiler, LLVMCompilerConfig
from utils import GetRealPath

File = GetRealPath(os.path.realpath(__file__))
Dir = GetRealPath(os.path.realpath(os.path.dirname(__file__)))

ZetaExeDir = GetRealPath(Dir)
ZetaDevDir = GetRealPath(f"{ZetaExeDir}/..")
ZetaDir = GetRealPath(f"{ZetaDevDir}/Zeta")


@typechecked
def AddDeps(builder: Builder, ZetaBuildDir: str, ZetaExeBuildDir: str, verbose: bool, mode: ModeEnum):
    compiler = LLVMCompiler(LLVMCompilerConfig(
        verbose=verbose,

        target=target,

        mode=mode,

        build_dir=ZetaExeBuildDir,

        working_dirs={
            ZetaDevDir,
            ZetaDir,
            ZetaExeDir,
        },

        c_include_dirs={
            f"{ZetaDir}",
        },

        cpp_include_dirs={
            f"{ZetaDir}",
        },
    ))

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
        f"{ZetaExeDir}/assoc_cntr_utils.h",
        {
            f"{File}",
            f"{ZetaDir}/assoc_cntr.h",
            f"{ZetaDir}/debugger.h",
        },
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
        f"{ZetaExeDir}/cpp_std_allocator.h",
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
            f"{ZetaDir}/ord_linked_list_node.h",
            f"{ZetaExeDir}/zeta_pool_allocator.h",
        },
        None
    )

    builder.Add(
        f"{ZetaExeDir}/ChainingStdRBTScheduler.h",
        {
            f"{File}",
            f"{ZetaDir}/ord_linked_list_node.h",
            f"{ZetaExeDir}/cpp_std_allocator.h",
        },
        None
    )

    builder.Add(
        f"{ZetaExeDir}/cas_alloc_utils.h",
        {
            f"{File}",
            f"{ZetaDir}/cascade_allocator.h",
            f"{ZetaDir}/memory.h",
            f"{ZetaExeDir}/std_allocator.h",
        },
        None
    )

    builder.Add(
        f"{ZetaExeDir}/circular_array_utils.h",
        {
            f"{File}",
            f"{ZetaDir}/circular_array.h",
            f"{ZetaExeDir}/seq_cntr_utils.h",
        },
        None
    )

    builder.Add(
        f"{ZetaExeDir}/debug_deque_utils.h",
        {
            f"{File}",
            f"{ZetaDir}/debug_deque.h",
            f"{ZetaExeDir}/seq_cntr_utils.h",
        },
        None
    )

    builder.Add(
        f"{ZetaExeDir}/debug_hash_table_utils.h",
        {
            f"{File}",
            f"{ZetaDir}/debug_hash_table.h",
            f"{ZetaDir}/debugger.h",
            f"{ZetaExeDir}/assoc_cntr_utils.h",
        },
        None
    )

    builder.Add(
        f"{ZetaExeDir}/dynamic_hash_table_utils.h",
        {
            f"{File}",
            f"{ZetaDir}/dynamic_hash_table.h",
            f"{ZetaExeDir}/std_allocator.h",
        },
        None
    )

    builder.Add(
        f"{ZetaExeDir}/dynamic_vector_utils.h",
        {
            f"{File}",
            f"{ZetaDir}/dynamic_vector.h",
            f"{ZetaDir}/mem_check_utils.h",
            f"{ZetaExeDir}/seq_cntr_utils.h",
            f"{ZetaExeDir}/std_allocator.h",
        },
        None
    )

    builder.Add(
        f"{ZetaExeDir}/file_block_vector.h",
        {
            f"{ZetaDir}/block_vector.h",
        },
        None
    )

    builder.Add(
        f"{ZetaExeDir}/PriorityQueue.h",
        {
            f"{File}",
            f"{ZetaDir}/debugger.h",
            f"{ZetaDir}/Groupbin_heap.h",
        },
        None
    )

    builder.Add(
        f"{ZetaExeDir}/random.h",
        {
            f"{File}",
            f"{ZetaDir}/define.h",
        },
        None
    )

    builder.Add(
        f"{ZetaExeDir}/staging_vector_utils.h",
        {
            f"{File}",
            f"{ZetaDir}/staging_vector.h",
            f"{ZetaExeDir}/seq_cntr_utils.h",
            f"{ZetaExeDir}/std_allocator.h",
        },
        None
    )

    builder.Add(
        f"{ZetaExeDir}/std_allocator.h",
        {
            f"{ZetaDir}/allocator.h",
            f"{ZetaDir}/debug_hash_table.h",
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
            f"{ZetaDir}/bin_tree.h",
            f"{ZetaDir}/ord_cnt_rb_tree_node.h",
            f"{ZetaDir}/rbtree.h",
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
        lambda: compiler.c_to_obj(
            f"{ZetaExeBuildDir}/test_1.o",
            f"{ZetaExeDir}/test_1.c",
        )
    )

    builder.Add(
        f"{ZetaExeDir}/test_binheap.cpp",
        {
            f"{File}",
            f"{ZetaDir}/bin_heap.h",
            f"{ZetaDir}/ord_cnt_rb_tree_node.h"
        },
        None
    )

    builder.Add(
        f"{ZetaExeBuildDir}/test_binheap.o",
        {
            f"{File}",
            f"{ZetaExeDir}/test_binheap.cpp",
        },
        lambda: compiler.cpp_to_obj(
            f"{ZetaExeBuildDir}/test_binheap.o",
            f"{ZetaExeDir}/test_binheap.cpp",
        )
    )

    builder.Add(
        f"{ZetaExeDir}/test_gplist.c",
        {
            f"{File}",
            f"{ZetaDir}/bin_tree.h",
            f"{ZetaDir}/ord_cnt_rb_tree_node.h",
            f"{ZetaDir}/rbtree.h",
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
            f"{ZetaDir}/bin_tree.h",
            f"{ZetaDir}/ord_cnt_rb_tree_node.h",
            f"{ZetaDir}/rbtree.h",
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
        f"{ZetaExeDir}/NaiveStdRBTScheduler.h",
        {
            f"{File}",
            f"{ZetaExeDir}/cpp_std_allocator.h",
        },
        None
    )

    builder.Add(
        f"{ZetaExeDir}/seg_vector_utils.h",
        {
            f"{File}",
            f"{ZetaDir}/seg_vector.h",
            f"{ZetaExeDir}/seq_cntr_utils.h",
            f"{ZetaExeDir}/std_allocator.h",
        },
        None
    )

    builder.Add(
        f"{ZetaExeDir}/seq_cntr_utils.h",
        {
            f"{File}",
        },
        None
    )

    builder.Add(
        f"{ZetaExeDir}/test_hashtable.c",
        {
            f"{File}",
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
        lambda: compiler.c_to_obj(
            f"{ZetaExeBuildDir}/test_2.o",
            f"{ZetaExeDir}/test_2.c",
        )
    )

    builder.Add(
        f"{ZetaExeBuildDir}/test_2.exe",
        {
            f"{File}",
            f"{ZetaBuildDir}/debug_str_pipe.o",
            f"{ZetaBuildDir}/debugger.o",
            f"{ZetaBuildDir}/logger.o",
            f"{ZetaBuildDir}/io.o",
            f"{ZetaBuildDir}/utils.o",
            f"{ZetaExeBuildDir}/test_2.o"
        },
        lambda: compiler.to_exe(
            f"{ZetaExeBuildDir}/test_2.exe",
            {
                f"{ZetaBuildDir}/debug_str_pipe.o",
                f"{ZetaBuildDir}/debugger.o",
                f"{ZetaBuildDir}/logger.o",
                f"{ZetaBuildDir}/io.o",
                f"{ZetaBuildDir}/utils.o",
                f"{ZetaExeBuildDir}/test_2.o",
            }
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
        lambda: compiler.c_to_obj(
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
        lambda: compiler.to_exe(
            f"{ZetaExeBuildDir}/test_3.exe",
            {
                f"{ZetaExeBuildDir}/test_3.o",
            }
        )
    )

    builder.Add(
        f"{ZetaExeDir}/test_datetime.cpp",
        {
            f"{File}",
            f"{ZetaDir}/datetime.h",
        },
        None
    )

    builder.Add(
        f"{ZetaExeBuildDir}/test_datetime.o",
        {
            f"{File}",
            f"{ZetaExeDir}/test_datetime.cpp",
        },
        lambda: compiler.cpp_to_obj(
            f"{ZetaExeBuildDir}/test_datetime.o",
            f"{ZetaExeDir}/test_datetime.cpp",
        )
    )

    builder.Add(
        f"{ZetaExeBuildDir}/test_datetime.exe",
        {
            f"{File}",

            f"{ZetaBuildDir}/datetime.o",

            f"{ZetaExeBuildDir}/test_datetime.o",
        },
        lambda: compiler.to_exe(
            f"{ZetaExeBuildDir}/test_datetime.exe",
            {
                f"{ZetaBuildDir}/datetime.o",

                f"{ZetaExeBuildDir}/test_datetime.o",
            }
        )
    )

    builder.Add(
        f"{ZetaExeDir}/test_dht.cpp",
        {
            f"{File}",

            f"{ZetaDir}/debug_deque.h",
            f"{ZetaDir}/debugger.h",
            f"{ZetaDir}/mem_check_utils.h",
            f"{ZetaExeDir}/assoc_cntr_utils.h",
            f"{ZetaExeDir}/debug_hash_table_utils.h",
            f"{ZetaExeDir}/dynamic_hash_table_utils.h",
            f"{ZetaExeDir}/pod_value.h",
            f"{ZetaExeDir}/random.h",
            f"{ZetaExeDir}/std_allocator.h",
            f"{ZetaExeDir}/timer.h",
        },
        None
    )

    builder.Add(
        f"{ZetaExeBuildDir}/test_dht.o",
        {
            f"{File}",
            f"{ZetaExeDir}/test_dht.cpp",
        },
        lambda: compiler.cpp_to_obj(
            f"{ZetaExeBuildDir}/test_dht.o",
            f"{ZetaExeDir}/test_dht.cpp",
        )
    )

    builder.Add(
        f"{ZetaExeBuildDir}/test_dht.exe",
        {
            f"{File}",
            f"{ZetaBuildDir}/assoc_cntr.o",
            f"{ZetaBuildDir}/bin_tree.o",
            f"{ZetaBuildDir}/debug_hash_table.o",
            f"{ZetaBuildDir}/debug_str_pipe.o",
            f"{ZetaBuildDir}/debugger.o",
            f"{ZetaBuildDir}/dynamic_hash_table.o",
            f"{ZetaBuildDir}/generic_hash_table.o",
            f"{ZetaBuildDir}/io.o",
            f"{ZetaBuildDir}/logger.o",
            f"{ZetaBuildDir}/mem_check_utils.o",
            f"{ZetaBuildDir}/ord_linked_list_node.o",
            f"{ZetaBuildDir}/ord_rb_tree_node.o",
            f"{ZetaBuildDir}/rbtree.o",
            f"{ZetaBuildDir}/utils.o",
            f"{ZetaExeBuildDir}/timer.o",
            f"{ZetaExeBuildDir}/test_dht.o",
        },
        lambda: compiler.to_exe(
            f"{ZetaExeBuildDir}/test_dht.exe",

            {
                f"{ZetaBuildDir}/assoc_cntr.o",
                f"{ZetaBuildDir}/bin_tree.o",
                f"{ZetaBuildDir}/debug_hash_table.o",
                f"{ZetaBuildDir}/debug_str_pipe.o",
                f"{ZetaBuildDir}/debugger.o",
                f"{ZetaBuildDir}/dynamic_hash_table.o",
                f"{ZetaBuildDir}/generic_hash_table.o",
                f"{ZetaBuildDir}/io.o",
                f"{ZetaBuildDir}/logger.o",
                f"{ZetaBuildDir}/mem_check_utils.o",
                f"{ZetaBuildDir}/ord_linked_list_node.o",
                f"{ZetaBuildDir}/ord_rb_tree_node.o",
                f"{ZetaBuildDir}/rbtree.o",
                f"{ZetaBuildDir}/utils.o",
                f"{ZetaExeBuildDir}/timer.o",
                f"{ZetaExeBuildDir}/test_dht.o",
            }
        )
    )

    builder.Add(
        f"{ZetaExeDir}/test_dv.cpp",
        {
            f"{File}",
            f"{ZetaDir}/dynamic_vector.h",
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
            f"{ZetaDir}/disk_info.h",
            f"{ZetaDir}/disk_part_gpt.h",
            f"{ZetaDir}/disk_part_mbt.h",
        },
        None
    )

    builder.Add(
        f"{ZetaExeDir}/test_jump.cpp",
        {
            f"{File}",
            f"{ZetaDir}/jump.h",
        },
        None
    )

    builder.Add(
        f"{ZetaExeBuildDir}/test_jump.o",
        {
            f"{File}",
            f"{ZetaExeDir}/test_jump.cpp",
        },
        lambda: compiler.cpp_to_obj(
            f"{ZetaExeBuildDir}/test_jump.o",
            f"{ZetaExeDir}/test_jump.cpp",
        )
    )

    builder.Add(
        f"{ZetaExeBuildDir}/test_jump.exe",
        {
            f"{File}",

            f"{ZetaBuildDir}/debug_str_pipe.o",
            f"{ZetaBuildDir}/debugger.o",
            f"{ZetaBuildDir}/io.o",
            f"{ZetaBuildDir}/jump.o",
            f"{ZetaBuildDir}/logger.o",

            f"{ZetaExeBuildDir}/test_jump.o",
        },
        lambda: compiler.to_exe(
            f"{ZetaExeBuildDir}/test_jump.exe",
            {
                f"{ZetaBuildDir}/debug_str_pipe.o",
                f"{ZetaBuildDir}/debugger.o",
                f"{ZetaBuildDir}/io.o",
                f"{ZetaBuildDir}/jump.o",
                f"{ZetaBuildDir}/logger.o",

                f"{ZetaExeBuildDir}/test_jump.o",
            }
        )
    )

    builder.Add(
        f"{ZetaExeDir}/test_tree_alloc.cpp",
        {
            f"{File}",
            f"{ZetaDir}/debug_hash_table.h",
            f"{ZetaDir}/debugger.h",
            f"{ZetaDir}/tree_allocator.h",
        },
        None
    )

    builder.Add(
        f"{ZetaExeBuildDir}/test_tree_alloc.o",
        {
            f"{File}",
            f"{ZetaExeDir}/test_tree_alloc.cpp",
        },
        lambda: compiler.cpp_to_obj(
            f"{ZetaExeBuildDir}/test_tree_alloc.o",
            f"{ZetaExeDir}/test_tree_alloc.cpp",
        )
    )

    builder.Add(
        f"{ZetaExeBuildDir}/test_tree_alloc.exe",
        {
            f"{File}",

            f"{ZetaBuildDir}/allocator.o",
            f"{ZetaBuildDir}/assoc_cntr.o",
            f"{ZetaBuildDir}/bin_tree.o",
            f"{ZetaBuildDir}/debug_hash_table.o",
            f"{ZetaBuildDir}/debug_hash_table.o",
            f"{ZetaBuildDir}/debug_str_pipe.o",
            f"{ZetaBuildDir}/debugger.o",
            f"{ZetaBuildDir}/io.o",
            f"{ZetaBuildDir}/logger.o",
            f"{ZetaBuildDir}/mem_check_utils.o",
            f"{ZetaBuildDir}/ord_rb_linked_list_node.o",
            f"{ZetaBuildDir}/ord_rb_tree_node.o",
            f"{ZetaBuildDir}/rbtree.o",
            f"{ZetaBuildDir}/tree_allocator.o",
            f"{ZetaBuildDir}/utils.o",

            f"{ZetaExeBuildDir}/test_tree_alloc.o",
        },
        lambda: compiler.to_exe(
            f"{ZetaExeBuildDir}/test_tree_alloc.exe",
            {
                f"{ZetaBuildDir}/allocator.o",
                f"{ZetaBuildDir}/assoc_cntr.o",
                f"{ZetaBuildDir}/bin_tree.o",
                f"{ZetaBuildDir}/debug_hash_table.o",
                f"{ZetaBuildDir}/debug_hash_table.o",
                f"{ZetaBuildDir}/debug_str_pipe.o",
                f"{ZetaBuildDir}/debugger.o",
                f"{ZetaBuildDir}/io.o",
                f"{ZetaBuildDir}/logger.o",
                f"{ZetaBuildDir}/mem_check_utils.o",
                f"{ZetaBuildDir}/ord_rb_linked_list_node.o",
                f"{ZetaBuildDir}/ord_rb_tree_node.o",
                f"{ZetaBuildDir}/rbtree.o",
                f"{ZetaBuildDir}/tree_allocator.o",
                f"{ZetaBuildDir}/utils.o",

                f"{ZetaExeBuildDir}/test_tree_alloc.o",
            }
        )
    )

    builder.Add(
        f"{ZetaExeDir}/test_usb.cpp",
        {
            f"{File}",
            f"{ZetaDir}/disk_info.h",
            f"{ZetaDir}/disk_part_gpt.h",
            f"{ZetaDir}/disk_part_mbt.h",
            f"{ZetaDir}/define.h"
        },
        None
    )

    builder.Add(
        f"{ZetaExeDir}/test_utf8.c",
        {
            f"{File}",

            f"{ZetaDir}/debugger.h",
            f"{ZetaDir}/utf8.h",
        },
        None
    )

    builder.Add(
        f"{ZetaExeBuildDir}/test_utf8.o",
        {
            f"{File}",

            f"{ZetaExeDir}/test_utf8.c",

            f"{ZetaDir}/utf8.h",
        },

        lambda: compiler.cpp_to_obj(
            f"{ZetaExeBuildDir}/test_utf8.o",
            f"{ZetaExeDir}/test_utf8.c",
        )
    )

    builder.Add(
        f"{ZetaExeBuildDir}/test_utf8.exe",
        {
            f"{File}",

            f"{ZetaBuildDir}/debugger.o",
            f"{ZetaBuildDir}/debug_str_pipe.o",
            f"{ZetaBuildDir}/logger.o",
            f"{ZetaBuildDir}/io.o",
            f"{ZetaBuildDir}/utf8.o",
            f"{ZetaBuildDir}/utils.o",

            f"{ZetaExeBuildDir}/test_utf8.o",
        },
        lambda: compiler.to_exe(
            f"{ZetaExeBuildDir}/test_utf8.exe",
            {
                f"{ZetaBuildDir}/debugger.o",
                f"{ZetaBuildDir}/debug_str_pipe.o",
                f"{ZetaBuildDir}/logger.o",
                f"{ZetaBuildDir}/io.o",
                f"{ZetaBuildDir}/utf8.o",
                f"{ZetaBuildDir}/utils.o",

                f"{ZetaExeBuildDir}/test_utf8.o",
            }
        )
    )

    builder.Add(
        f"{ZetaExeDir}/test_utf16.c",
        {
            f"{File}",

            f"{ZetaDir}/debugger.h",
            f"{ZetaDir}/utf16.h",
        },
        None
    )

    builder.Add(
        f"{ZetaExeBuildDir}/test_utf16.o",
        {
            f"{File}",

            f"{ZetaExeDir}/test_utf16.c",

            f"{ZetaDir}/utf16.h",
        },

        lambda: compiler.cpp_to_obj(
            f"{ZetaExeBuildDir}/test_utf16.o",
            f"{ZetaExeDir}/test_utf16.c",
        )
    )

    builder.Add(
        f"{ZetaExeBuildDir}/test_utf16.exe",
        {
            f"{File}",

            f"{ZetaBuildDir}/debugger.o",
            f"{ZetaBuildDir}/debug_str_pipe.o",
            f"{ZetaBuildDir}/logger.o",
            f"{ZetaBuildDir}/io.o",
            f"{ZetaBuildDir}/utf16.o",
            f"{ZetaBuildDir}/utils.o",

            f"{ZetaExeBuildDir}/test_utf16.o",
        },
        lambda: compiler.to_exe(
            f"{ZetaExeBuildDir}/test_utf16.exe",
            {
                f"{ZetaBuildDir}/debugger.o",
                f"{ZetaBuildDir}/debug_str_pipe.o",
                f"{ZetaBuildDir}/logger.o",
                f"{ZetaBuildDir}/io.o",
                f"{ZetaBuildDir}/utf16.o",
                f"{ZetaBuildDir}/utils.o",

                f"{ZetaExeBuildDir}/test_utf16.o",
            }
        )
    )

    builder.Add(
        f"{ZetaExeDir}/test_segvec.cpp",
        {
            f"{File}",
            f"{ZetaDir}/seg_vector.h",
            f"{ZetaExeDir}/std_allocator.h"
        },
        None
    )

    builder.Add(
        f"{ZetaExeBuildDir}/test_segvec.o",
        {
            f"{File}",
            f"{ZetaExeDir}/test_segvec.cpp",
        },
        lambda: compiler.cpp_to_obj(
            f"{ZetaExeBuildDir}/test_segvec.o",
            f"{ZetaExeDir}/test_segvec.cpp",
        )
    )

    builder.Add(
        f"{ZetaExeBuildDir}/test_segvec.exe",
        {
            f"{File}",

            f"{ZetaBuildDir}/seg_vector.o",
            f"{ZetaBuildDir}/bin_tree.o",
            f"{ZetaBuildDir}/circular_array.o",
            f"{ZetaBuildDir}/debug_deque.o",
            f"{ZetaBuildDir}/debug_hash_table.o",
            f"{ZetaBuildDir}/rbtree.o",
            f"{ZetaBuildDir}/ord_cnt_rb_tree_node.o",
            f"{ZetaBuildDir}/utils.o",

            f"{ZetaExeBuildDir}/test_segvec.o",
        },
        lambda: compiler.to_exe(
            f"{ZetaExeBuildDir}/test_segvec.exe",
            {
                f"{ZetaBuildDir}/seg_vector.o",
                f"{ZetaBuildDir}/bin_tree.o",
                f"{ZetaBuildDir}/circular_array.o",
                f"{ZetaBuildDir}/debug_deque.o",
                f"{ZetaBuildDir}/debug_hash_table.o",
                f"{ZetaBuildDir}/rbtree.o",
                f"{ZetaBuildDir}/ord_cnt_rb_tree_node.o",
                f"{ZetaBuildDir}/utils.o",

                f"{ZetaExeBuildDir}/test_segvec.o",
            }
        )
    )

    builder.Add(
        f"{ZetaExeDir}/test_segvec2.cpp",
        {
            f"{File}",
            f"{ZetaDir}/seg_vector.h",
            f"{ZetaExeDir}/std_allocator.h"
        },
        None
    )

    builder.Add(
        f"{ZetaExeBuildDir}/test_segvec2.o",
        {
            f"{File}",
            f"{ZetaExeDir}/test_segvec2.cpp",
        },
        lambda: compiler.cpp_to_obj(
            f"{ZetaExeBuildDir}/test_segvec2.o",
            f"{ZetaExeDir}/test_segvec2.cpp",
        )
    )

    builder.Add(
        f"{ZetaExeBuildDir}/test_segvec2.exe",
        {
            f"{File}",

            f"{ZetaBuildDir}/bin_tree.o",
            f"{ZetaBuildDir}/circular_array.o",
            f"{ZetaBuildDir}/debug_deque.o",
            f"{ZetaBuildDir}/debug_hash_table.o",
            f"{ZetaBuildDir}/ord_cnt_rb_tree_node.o",
            f"{ZetaBuildDir}/rbtree.o",
            f"{ZetaBuildDir}/seg_vector.o",
            f"{ZetaBuildDir}/utils.o",

            f"{ZetaExeBuildDir}/test_segvec2.o",
        },
        lambda: compiler.to_exe(
            f"{ZetaExeBuildDir}/test_segvec2.exe",
            {
                f"{ZetaBuildDir}/bin_tree.o",
                f"{ZetaBuildDir}/circular_array.o",
                f"{ZetaBuildDir}/debug_deque.o",
                f"{ZetaBuildDir}/debug_hash_table.o",
                f"{ZetaBuildDir}/ord_cnt_rb_tree_node.o",
                f"{ZetaBuildDir}/rbtree.o",
                f"{ZetaBuildDir}/seg_vector.o",
                f"{ZetaBuildDir}/utils.o",

                f"{ZetaExeBuildDir}/test_segvec2.o",
            }
        )
    )

    builder.Add(
        f"{ZetaExeDir}/test_segvec_speed.cpp",
        {
            f"{File}",
            f"{ZetaDir}/seg_vector.h",
            f"{ZetaExeDir}/std_allocator.h"
        },
        None
    )

    builder.Add(
        f"{ZetaExeBuildDir}/test_segvec_speed.o",
        {
            f"{File}",
            f"{ZetaExeDir}/test_segvec_speed.cpp",
        },
        lambda: compiler.cpp_to_obj(
            f"{ZetaExeBuildDir}/test_segvec_speed.o",
            f"{ZetaExeDir}/test_segvec_speed.cpp",
        )
    )

    builder.Add(
        f"{ZetaExeBuildDir}/test_segvec_speed.exe",
        {
            f"{File}",

            f"{ZetaBuildDir}/seg_vector.o",
            f"{ZetaBuildDir}/bin_tree.o",
            f"{ZetaBuildDir}/circular_array.o",
            f"{ZetaBuildDir}/debug_deque.o",
            f"{ZetaBuildDir}/debug_hash_table.o",
            f"{ZetaBuildDir}/rbtree.o",
            f"{ZetaBuildDir}/ord_cnt_rb_tree_node.o",
            f"{ZetaBuildDir}/utils.o",

            f"{ZetaExeBuildDir}/test_segvec_speed.o",
        },
        lambda: compiler.to_exe(
            f"{ZetaExeBuildDir}/test_segvec_speed.exe",
            {
                f"{ZetaBuildDir}/seg_vector.o",
                f"{ZetaBuildDir}/bin_tree.o",
                f"{ZetaBuildDir}/circular_array.o",
                f"{ZetaBuildDir}/debug_deque.o",
                f"{ZetaBuildDir}/debug_hash_table.o",
                f"{ZetaBuildDir}/rbtree.o",
                f"{ZetaBuildDir}/ord_cnt_rb_tree_node.o",
                f"{ZetaBuildDir}/utils.o",

                f"{ZetaExeBuildDir}/test_segvec_speed.o",
            }
        )
    )

    builder.Add(
        f"{ZetaExeDir}/test_segvec2_speed.cpp",
        {
            f"{File}",
            f"{ZetaDir}/seg_vector.h",
            f"{ZetaExeDir}/std_allocator.h"
        },
        None
    )

    builder.Add(
        f"{ZetaExeBuildDir}/test_segvec2_speed.o",
        {
            f"{File}",
            f"{ZetaExeDir}/test_segvec2_speed.cpp",
        },
        lambda: compiler.cpp_to_obj(
            f"{ZetaExeBuildDir}/test_segvec2_speed.o",
            f"{ZetaExeDir}/test_segvec2_speed.cpp",
        )
    )

    builder.Add(
        f"{ZetaExeBuildDir}/test_segvec2_speed.exe",
        {
            f"{File}",

            f"{ZetaBuildDir}/seg_vector.o",
            f"{ZetaBuildDir}/bin_tree.o",
            f"{ZetaBuildDir}/circular_array.o",
            f"{ZetaBuildDir}/debug_hash_table.o",
            f"{ZetaBuildDir}/rbtree.o",
            f"{ZetaBuildDir}/ord_cnt_rb_tree_node.o",
            f"{ZetaBuildDir}/utils.o",

            f"{ZetaExeBuildDir}/test_segvec2_speed.o",
        },
        lambda: compiler.to_exe(
            f"{ZetaExeBuildDir}/test_segvec2_speed.exe",
            {
                f"{ZetaBuildDir}/seg_vector.o",
                f"{ZetaBuildDir}/bin_tree.o",
                f"{ZetaBuildDir}/circular_array.o",
                f"{ZetaBuildDir}/debug_hash_table.o",
                f"{ZetaBuildDir}/rbtree.o",
                f"{ZetaBuildDir}/ord_cnt_rb_tree_node.o",
                f"{ZetaBuildDir}/utils.o",

                f"{ZetaExeBuildDir}/test_segvec2_speed.o",
            }
        )
    )

    builder.Add(
        f"{ZetaExeDir}/test_seqcntr.cpp",
        {
            f"{File}",
            f"{ZetaDir}/debug_deque.h",
            f"{ZetaDir}/circular_array.h",
            f"{ZetaDir}/seg_vector.h",
            f"{ZetaExeDir}/std_allocator.h"
        },
        None
    )

    builder.Add(
        f"{ZetaExeBuildDir}/test_seqcntr.o",
        {
            f"{File}",
            f"{ZetaExeDir}/test_seqcntr.cpp",
        },
        lambda: compiler.cpp_to_obj(
            f"{ZetaExeBuildDir}/test_seqcntr.o",
            f"{ZetaExeDir}/test_seqcntr.cpp",
        )
    )

    builder.Add(
        f"{ZetaExeBuildDir}/test_seqcntr.exe",
        {
            f"{File}",

            f"{ZetaBuildDir}/bin_tree.o",
            f"{ZetaBuildDir}/circular_array.o",
            f"{ZetaBuildDir}/debug_deque.o",
            f"{ZetaBuildDir}/debug_hash_table.o",
            f"{ZetaBuildDir}/rbtree.o",
            f"{ZetaBuildDir}/ord_cnt_rb_tree_node.o",
            f"{ZetaBuildDir}/seg_vector.o",
            f"{ZetaBuildDir}/utils.o",

            f"{ZetaExeBuildDir}/test_seqcntr.o",
        },
        lambda: compiler.to_exe(
            f"{ZetaExeBuildDir}/test_seqcntr.exe",
            {
                f"{ZetaBuildDir}/bin_tree.o",
                f"{ZetaBuildDir}/circular_array.o",
                f"{ZetaBuildDir}/debug_deque.o",
                f"{ZetaBuildDir}/debug_hash_table.o",
                f"{ZetaBuildDir}/rbtree.o",
                f"{ZetaBuildDir}/ord_cnt_rb_tree_node.o",
                f"{ZetaBuildDir}/seg_vector.o",
                f"{ZetaBuildDir}/utils.o",

                f"{ZetaExeBuildDir}/test_seqcntr.o",
            }
        )
    )

    builder.Add(
        f"{ZetaExeDir}/test_stagevec.cpp",
        {
            f"{File}",
            f"{ZetaDir}/debug_deque.h",
            f"{ZetaDir}/debugger.h",
            f"{ZetaDir}/logger.h",
            f"{ZetaExeDir}/cas_alloc_utils.h",
            f"{ZetaExeDir}/circular_array_utils.h",
            f"{ZetaExeDir}/debug_deque_utils.h",
            f"{ZetaExeDir}/dynamic_vector_utils.h",
            f"{ZetaExeDir}/pod_value.h",
            f"{ZetaExeDir}/random.h",
            f"{ZetaExeDir}/seg_vector_utils.h",
            f"{ZetaExeDir}/seq_cntr_utils.h",
            f"{ZetaExeDir}/staging_vector_utils.h",
            f"{ZetaExeDir}/timer.h",
        },
        None
    )

    builder.Add(
        f"{ZetaExeBuildDir}/test_stagevec.o",
        {
            f"{File}",
            f"{ZetaExeDir}/test_stagevec.cpp",
        },
        lambda: compiler.cpp_to_obj(
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
        lambda: compiler.ll_to_asm(
            f"{ZetaExeBuildDir}/test_stagevec.s",
            f"{ZetaExeBuildDir}/test_stagevec.o",
        )
    )

    builder.Add(
        f"{ZetaExeBuildDir}/test_stagevec.exe",
        {
            f"{File}",
            f"{ZetaBuildDir}/algorithm.o",
            f"{ZetaBuildDir}/allocator.o",
            f"{ZetaBuildDir}/assoc_cntr.o",
            f"{ZetaBuildDir}/bin_tree.o",
            f"{ZetaBuildDir}/cascade_allocator.o",
            f"{ZetaBuildDir}/circular_array.o",
            f"{ZetaBuildDir}/debug_deque.o",
            f"{ZetaBuildDir}/debug_hash_table.o",
            f"{ZetaBuildDir}/debug_str_pipe.o",
            f"{ZetaBuildDir}/debugger.o",
            f"{ZetaBuildDir}/dummy_vector.o",
            f"{ZetaBuildDir}/dynamic_hash_table.o",
            f"{ZetaBuildDir}/dynamic_vector.o",
            f"{ZetaBuildDir}/generic_hash_table.o",
            f"{ZetaBuildDir}/io.o",
            f"{ZetaBuildDir}/logger.o",
            f"{ZetaBuildDir}/mem_check_utils.o",
            f"{ZetaBuildDir}/memory.o",
            f"{ZetaBuildDir}/ord_cnt_3rb_tree_node.o",
            f"{ZetaBuildDir}/ord_cnt_rb_tree_node.o",
            f"{ZetaBuildDir}/ord_linked_list_node.o",
            f"{ZetaBuildDir}/ord_rb_tree_node.o",
            f"{ZetaBuildDir}/pool_allocator.o",
            f"{ZetaBuildDir}/rbtree.o",
            f"{ZetaBuildDir}/seg_utils.o",
            f"{ZetaBuildDir}/seg_vector.o",
            f"{ZetaBuildDir}/seq_cntr.o",
            f"{ZetaBuildDir}/staging_vector.o",
            f"{ZetaBuildDir}/utils.o",
            f"{ZetaExeBuildDir}/test_stagevec.o",
            f"{ZetaExeBuildDir}/timer.o",
        },
        lambda: compiler.to_exe(
            f"{ZetaExeBuildDir}/test_stagevec.exe",

            {
                f"{ZetaBuildDir}/algorithm.o",
                f"{ZetaBuildDir}/allocator.o",
                f"{ZetaBuildDir}/assoc_cntr.o",
                f"{ZetaBuildDir}/bin_tree.o",
                f"{ZetaBuildDir}/cascade_allocator.o",
                f"{ZetaBuildDir}/circular_array.o",
                f"{ZetaBuildDir}/debug_deque.o",
                f"{ZetaBuildDir}/debug_hash_table.o",
                f"{ZetaBuildDir}/debug_str_pipe.o",
                f"{ZetaBuildDir}/debugger.o",
                f"{ZetaBuildDir}/dummy_vector.o",
                f"{ZetaBuildDir}/dynamic_hash_table.o",
                f"{ZetaBuildDir}/dynamic_vector.o",
                f"{ZetaBuildDir}/generic_hash_table.o",
                f"{ZetaBuildDir}/io.o",
                f"{ZetaBuildDir}/logger.o",
                f"{ZetaBuildDir}/mem_check_utils.o",
                f"{ZetaBuildDir}/memory.o",
                f"{ZetaBuildDir}/ord_cnt_3rb_tree_node.o",
                f"{ZetaBuildDir}/ord_cnt_rb_tree_node.o",
                f"{ZetaBuildDir}/ord_linked_list_node.o",
                f"{ZetaBuildDir}/ord_rb_tree_node.o",
                f"{ZetaBuildDir}/pool_allocator.o",
                f"{ZetaBuildDir}/rbtree.o",
                f"{ZetaBuildDir}/seg_utils.o",
                f"{ZetaBuildDir}/seg_vector.o",
                f"{ZetaBuildDir}/seq_cntr.o",
                f"{ZetaBuildDir}/staging_vector.o",
                f"{ZetaBuildDir}/utils.o",
                f"{ZetaExeBuildDir}/test_stagevec.o",
                f"{ZetaExeBuildDir}/timer.o",
            }
        )
    )

    builder.Add(
        f"{ZetaExeDir}/test_stagevec_speed.cpp",
        {
            f"{File}",
            f"{ZetaDir}/circular_array.h",
            f"{ZetaDir}/debug_deque.h",
            f"{ZetaDir}/debugger.h",
            f"{ZetaDir}/logger.h",
            f"{ZetaDir}/seg_vector.h",
            f"{ZetaDir}/staging_vector.h",
            f"{ZetaExeDir}/std_allocator.h",
        },
        None
    )

    builder.Add(
        f"{ZetaExeBuildDir}/test_stagevec_speed.o",
        {
            f"{File}",
            f"{ZetaExeDir}/test_stagevec_speed.cpp",
        },
        lambda: compiler.cpp_to_obj(
            f"{ZetaExeBuildDir}/test_stagevec_speed.o",
            f"{ZetaExeDir}/test_stagevec_speed.cpp",
        )
    )

    builder.Add(
        f"{ZetaExeBuildDir}/test_stagevec_speed.exe",
        {
            f"{File}",
            f"{ZetaBuildDir}/bin_tree.o",
            f"{ZetaBuildDir}/circular_array.o",
            f"{ZetaBuildDir}/debug_deque.o",
            f"{ZetaBuildDir}/debug_hash_table.o",
            f"{ZetaBuildDir}/debug_str_pipe.o",
            f"{ZetaBuildDir}/debugger.o",
            f"{ZetaBuildDir}/dummy_vector.o",
            f"{ZetaBuildDir}/io.o",
            f"{ZetaBuildDir}/logger.o",
            f"{ZetaBuildDir}/ord_cnt_3rb_tree_node.o",
            f"{ZetaBuildDir}/ord_cnt_rb_tree_node.o",
            f"{ZetaBuildDir}/rbtree.o",
            f"{ZetaBuildDir}/seg_utils.o",
            f"{ZetaBuildDir}/seq_cntr.o",
            f"{ZetaBuildDir}/staging_vector.o",
            f"{ZetaBuildDir}/utils.o",
            f"{ZetaExeBuildDir}/test_stagevec_speed.o",
        },
        lambda: compiler.to_exe(
            f"{ZetaExeBuildDir}/test_stagevec_speed.exe",

            {
                f"{ZetaBuildDir}/bin_tree.o",
                f"{ZetaBuildDir}/circular_array.o",
                f"{ZetaBuildDir}/debug_deque.o",
                f"{ZetaBuildDir}/debug_hash_table.o",
                f"{ZetaBuildDir}/debug_str_pipe.o",
                f"{ZetaBuildDir}/debugger.o",
                f"{ZetaBuildDir}/dummy_vector.o",
                f"{ZetaBuildDir}/io.o",
                f"{ZetaBuildDir}/logger.o",
                f"{ZetaBuildDir}/ord_cnt_3rb_tree_node.o",
                f"{ZetaBuildDir}/ord_cnt_rb_tree_node.o",
                f"{ZetaBuildDir}/rbtree.o",
                f"{ZetaBuildDir}/seg_utils.o",
                f"{ZetaBuildDir}/seq_cntr.o",
                f"{ZetaBuildDir}/staging_vector.o",
                f"{ZetaBuildDir}/utils.o",
                f"{ZetaExeBuildDir}/test_stagevec_speed.o",
            }
        )
    )

    builder.Add(
        f"{ZetaExeDir}/test_stdio.c",
        {
            f"{File}",
            f"{ZetaDir}/define.h",
            f"{ZetaDir}/pipe.h",
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
        lambda: compiler.cpp_to_obj(
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
        lambda: compiler.to_exe(
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
        lambda: compiler.cpp_to_obj(
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
        lambda: compiler.to_exe(
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
        lambda: compiler.cpp_to_obj(
            f"{ZetaExeBuildDir}/test_log.o",
            f"{ZetaExeDir}/test_log.cpp",
        )
    )

    builder.Add(
        f"{ZetaExeBuildDir}/test_log.exe",
        {
            f"{File}",

            f"{ZetaBuildDir}/debug_str_pipe.o",
            f"{ZetaBuildDir}/debugger.o",
            f"{ZetaBuildDir}/logger.o",
            f"{ZetaBuildDir}/io.o",
            f"{ZetaBuildDir}/utils.o",

            f"{ZetaExeBuildDir}/test_log.o",
        },
        lambda: compiler.to_exe(
            f"{ZetaExeBuildDir}/test_log.exe",

            {
                f"{ZetaBuildDir}/debug_str_pipe.o",
                f"{ZetaBuildDir}/debugger.o",
                f"{ZetaBuildDir}/logger.o",
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
            f"{ZetaDir}/lru_cache_manager.h",
        },
        None
    )

    builder.Add(
        f"{ZetaExeDir}/test_lrucm.cpp",
        {
            f"{File}",
            f"{ZetaDir}/dummy_block_vector.h",
            f"{ZetaDir}/dummy_cache_manager.h",
            f"{ZetaDir}/lru_cache_manager.h",
            f"{ZetaDir}/rbtree.h",
            f"{ZetaExeDir}/file_block_vector.h",
            f"{ZetaExeDir}/std_allocator.h",
        },
        None
    )

    builder.Add(
        f"{ZetaExeBuildDir}/test_lrucm.o",
        {
            f"{File}",
            f"{ZetaExeDir}/test_lrucm.cpp",
        },
        lambda: compiler.cpp_to_obj(
            f"{ZetaExeBuildDir}/test_lrucm.o",
            f"{ZetaExeDir}/test_lrucm.cpp",
        )
    )

    builder.Add(
        f"{ZetaExeBuildDir}/test_lrucm.exe",
        {
            f"{File}",

            f"{ZetaBuildDir}/allocator.o",
            f"{ZetaBuildDir}/bin_tree.o",
            f"{ZetaBuildDir}/block_vector.o",
            f"{ZetaBuildDir}/debug_hash_table.o",
            f"{ZetaBuildDir}/dummy_block_vector.o",
            f"{ZetaBuildDir}/dummy_cache_manager.o",
            f"{ZetaBuildDir}/lru_cache_manager.o",
            f"{ZetaBuildDir}/ord_linked_list_node.o",
            f"{ZetaBuildDir}/ord_rb_linked_list_node.o",
            f"{ZetaBuildDir}/ord_rb_tree_node.o",
            f"{ZetaBuildDir}/rbtree.o",
            f"{ZetaBuildDir}/utils.o",

            f"{ZetaExeBuildDir}/test_lrucm.o",
        },
        lambda: compiler.to_exe(
            f"{ZetaExeBuildDir}/test_lrucm.exe",
            {
                f"{ZetaBuildDir}/allocator.o",
                f"{ZetaBuildDir}/bin_tree.o",
                f"{ZetaBuildDir}/block_vector.o",
                f"{ZetaBuildDir}/debug_hash_table.o",
                f"{ZetaBuildDir}/dummy_block_vector.o",
                f"{ZetaBuildDir}/dummy_cache_manager.o",
                f"{ZetaBuildDir}/lru_cache_manager.o",
                f"{ZetaBuildDir}/ord_linked_list_node.o",
                f"{ZetaBuildDir}/ord_rb_linked_list_node.o",
                f"{ZetaBuildDir}/ord_rb_tree_node.o",
                f"{ZetaBuildDir}/rbtree.o",
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
            f"{ZetaDir}/algorithm.h",
            f"{ZetaDir}/cascade_allocator.h",
            f"{ZetaDir}/debugger.h",
            f"{ZetaDir}/memory.h",
            f"{ZetaExeDir}/cas_alloc_utils.h",
            f"{ZetaExeDir}/timer.h",
        },
        None
    )

    builder.Add(
        f"{ZetaExeBuildDir}/test_sort.o",
        {
            f"{File}",
            f"{ZetaExeDir}/test_sort.cpp",
        },
        lambda: compiler.cpp_to_obj(
            f"{ZetaExeBuildDir}/test_sort.o",
            f"{ZetaExeDir}/test_sort.cpp",
        )
    )

    builder.Add(
        f"{ZetaExeBuildDir}/test_sort.exe",
        {
            f"{File}",

            f"{ZetaBuildDir}/algorithm.o",
            f"{ZetaBuildDir}/allocator.o",
            f"{ZetaBuildDir}/cascade_allocator.o",
            f"{ZetaBuildDir}/debug_str_pipe.o",
            f"{ZetaBuildDir}/debugger.o",
            f"{ZetaBuildDir}/io.o",
            f"{ZetaBuildDir}/mem_check_utils.o",
            f"{ZetaBuildDir}/memory.o",
            f"{ZetaBuildDir}/ord_linked_list_node.o",
            f"{ZetaBuildDir}/utils.o",

            f"{ZetaExeBuildDir}/timer.o",

            f"{ZetaExeBuildDir}/test_sort.o",
        },
        lambda: compiler.to_exe(
            f"{ZetaExeBuildDir}/test_sort.exe",
            {
                f"{ZetaBuildDir}/algorithm.o",
                f"{ZetaBuildDir}/allocator.o",
                f"{ZetaBuildDir}/cascade_allocator.o",
                f"{ZetaBuildDir}/debug_str_pipe.o",
                f"{ZetaBuildDir}/debugger.o",
                f"{ZetaBuildDir}/io.o",
                f"{ZetaBuildDir}/mem_check_utils.o",
                f"{ZetaBuildDir}/memory.o",
                f"{ZetaBuildDir}/ord_linked_list_node.o",
                f"{ZetaBuildDir}/utils.o",

                f"{ZetaExeBuildDir}/timer.o",

                f"{ZetaExeBuildDir}/test_sort.o",
            }
        )
    )

    builder.Add(
        f"{ZetaExeDir}/DyVec.c",
        {
            f"{File}",
            f"{ZetaDir}/dynamic_vector.h",
        },
        None
    )

    builder.Add(
        f"{ZetaExeDir}/test_slaballoc.cpp",
        {
            f"{File}",
            f"{ZetaDir}/debug_hash_table.h",
            f"{ZetaDir}/Slaballocator.h",
        },
        None
    )

    builder.Add(
        f"{ZetaExeBuildDir}/test_slaballoc.o",
        {
            f"{File}",
            f"{ZetaExeDir}/test_slaballoc.cpp",
        },
        lambda: compiler.cpp_to_obj(
            f"{ZetaExeBuildDir}/test_slaballoc.o",
            f"{ZetaExeDir}/test_slaballoc.cpp",
        )
    )

    builder.Add(
        f"{ZetaExeBuildDir}/test_slaballoc.exe",
        {
            f"{File}",

            f"{ZetaBuildDir}/allocator.o",
            f"{ZetaBuildDir}/debug_hash_table.o",
            f"{ZetaBuildDir}/ord_linked_list_node.o",
            f"{ZetaBuildDir}/ord_rb_linked_list_node.o",
            f"{ZetaBuildDir}/Slaballocator.o",
            f"{ZetaBuildDir}/utils.o",

            f"{ZetaExeBuildDir}/test_slaballoc.o",
        },
        lambda: compiler.to_exe(
            f"{ZetaExeBuildDir}/test_slaballoc.exe",
            {
                f"{ZetaBuildDir}/allocator.o",
                f"{ZetaBuildDir}/debug_hash_table.o",
                f"{ZetaBuildDir}/ord_linked_list_node.o",
                f"{ZetaBuildDir}/ord_rb_linked_list_node.o",
                f"{ZetaBuildDir}/Slaballocator.o",
                f"{ZetaBuildDir}/utils.o",

                f"{ZetaExeBuildDir}/test_slaballoc.o",
            }
        )
    )

    builder.Add(
        f"{ZetaExeDir}/test_lin_space_allocator.cpp",
        {
            f"{File}",
            f"{ZetaDir}/lin_space_allocator.h",
            f"{ZetaExeDir}/random.h",
            f"{ZetaExeDir}/std_allocator.h",
        },
        None
    )

    builder.Add(
        f"{ZetaExeBuildDir}/test_lin_space_allocator.o",
        {
            f"{File}",
            f"{ZetaExeDir}/std_allocator.h",
            f"{ZetaExeDir}/test_lin_space_allocator.cpp",
        },
        lambda: compiler.cpp_to_obj(
            f"{ZetaExeBuildDir}/test_lin_space_allocator.o",
            f"{ZetaExeDir}/test_lin_space_allocator.cpp",
        )
    )

    builder.Add(
        f"{ZetaExeBuildDir}/test_lin_space_allocator.exe",
        {
            f"{File}",

            f"{ZetaBuildDir}/allocator.o",
            f"{ZetaBuildDir}/bin_tree.o",
            f"{ZetaBuildDir}/debug_str_pipe.o",
            f"{ZetaBuildDir}/debugger.o",
            f"{ZetaBuildDir}/io.o",
            f"{ZetaBuildDir}/lin_space_allocator.o",
            f"{ZetaBuildDir}/logger.o",
            f"{ZetaBuildDir}/mem_check_utils.o",
            f"{ZetaBuildDir}/rbtree.o",
            f"{ZetaBuildDir}/utils.o",

            f"{ZetaExeBuildDir}/test_lin_space_allocator.o",
        },
        lambda: compiler.to_exe(
            f"{ZetaExeBuildDir}/test_lin_space_allocator.exe",

            {
                f"{ZetaBuildDir}/allocator.o",
                f"{ZetaBuildDir}/bin_tree.o",
                f"{ZetaBuildDir}/debug_str_pipe.o",
                f"{ZetaBuildDir}/debugger.o",
                f"{ZetaBuildDir}/io.o",
                f"{ZetaBuildDir}/lin_space_allocator.o",
                f"{ZetaBuildDir}/logger.o",
                f"{ZetaBuildDir}/mem_check_utils.o",
                f"{ZetaBuildDir}/rbtree.o",
                f"{ZetaBuildDir}/utils.o",

                f"{ZetaExeBuildDir}/test_lin_space_allocator.o",
            }
        )
    )

    builder.Add(
        f"{ZetaExeDir}/test_mlv.cpp",
        {
            f"{File}",
            f"{ZetaDir}/MultiLevelTable.h",
            f"{ZetaExeDir}/std_allocator.h",
        },
        None
    )

    builder.Add(
        f"{ZetaExeBuildDir}/test_mlv.o",
        {
            f"{File}",
            f"{ZetaDir}/MultiLevelTable.h",
            f"{ZetaExeDir}/std_allocator.h",
            f"{ZetaExeDir}/test_mlv.cpp",
        },
        lambda: compiler.cpp_to_obj(
            f"{ZetaExeBuildDir}/test_mlv.o",
            f"{ZetaExeDir}/test_mlv.cpp",
        )
    )

    builder.Add(
        f"{ZetaExeBuildDir}/test_mlv.exe",
        {
            f"{File}",

            f"{ZetaBuildDir}/allocator.o",
            f"{ZetaBuildDir}/debug_hash_table.o",
            f"{ZetaBuildDir}/MultiLevelTable.o",
            f"{ZetaBuildDir}/utils.o",

            f"{ZetaExeBuildDir}/test_mlv.o",
        },
        lambda: compiler.to_exe(
            f"{ZetaExeBuildDir}/test_mlv.exe",

            {
                f"{ZetaBuildDir}/allocator.o",
                f"{ZetaBuildDir}/debug_hash_table.o",
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
            f"{ZetaExeDir}/cpp_std_allocator.h",
            f"{ZetaExeDir}/zeta_pool_allocator.h",
        },
        None
    )

    builder.Add(
        f"{ZetaExeBuildDir}/test_mlt.o",
        {
            f"{File}",
            f"{ZetaDir}/MultiLevelTable.h",
            f"{ZetaExeDir}/std_allocator.h",
            f"{ZetaExeDir}/test_mlt.cpp",
        },
        lambda: compiler.cpp_to_obj(
            f"{ZetaExeBuildDir}/test_mlt.o",
            f"{ZetaExeDir}/test_mlt.cpp",
        )
    )

    builder.Add(
        f"{ZetaExeBuildDir}/test_mlt.exe",
        {
            f"{File}",

            f"{ZetaBuildDir}/allocator.o",
            f"{ZetaBuildDir}/debug_hash_table.o",
            f"{ZetaBuildDir}/MultiLevelTable.o",
            f"{ZetaBuildDir}/ord_linked_list_node.o",
            f"{ZetaBuildDir}/utils.o",

            f"{ZetaExeBuildDir}/test_mlt.o",
        },
        lambda: compiler.to_exe(
            f"{ZetaExeBuildDir}/test_mlt.exe",

            {
                f"{ZetaBuildDir}/allocator.o",
                f"{ZetaBuildDir}/debug_hash_table.o",
                f"{ZetaBuildDir}/MultiLevelTable.o",
                f"{ZetaBuildDir}/ord_linked_list_node.o",
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
            f"{ZetaExeDir}/std_allocator.h",
        },
        None
    )

    builder.Add(
        f"{ZetaExeDir}/test_pipe.c",
        {
            f"{File}",
            f"{ZetaDir}/debug_str_pipe.h",
            f"{ZetaDir}/debugger.h",
        },
        None
    )

    builder.Add(
        f"{ZetaExeBuildDir}/test_pipe.o",
        {
            f"{File}",
            f"{ZetaExeDir}/test_pipe.c",
        },
        lambda: compiler.c_to_obj(
            f"{ZetaExeBuildDir}/test_pipe.o",
            f"{ZetaExeDir}/test_pipe.c",
        )
    )

    builder.Add(
        f"{ZetaExeBuildDir}/test_pipe.exe",
        {
            f"{File}",
            f"{ZetaBuildDir}/debug_str_pipe.o",
            f"{ZetaBuildDir}/debugger.o",
            f"{ZetaBuildDir}/io.o",
            f"{ZetaBuildDir}/logger.o",
            f"{ZetaBuildDir}/utils.o",
            f"{ZetaExeBuildDir}/test_pipe.o",
        },
        lambda: compiler.to_exe(
            f"{ZetaExeBuildDir}/test_pipe.exe",
            {
                f"{ZetaBuildDir}/debug_str_pipe.o",
                f"{ZetaBuildDir}/debugger.o",
                f"{ZetaBuildDir}/io.o",
                f"{ZetaBuildDir}/logger.o",
                f"{ZetaBuildDir}/utils.o",
                f"{ZetaExeBuildDir}/test_pipe.o",
            }
        )
    )

    builder.Add(
        f"{ZetaExeDir}/test_qsort.cpp",
        {
            f"{File}",
            f"{ZetaDir}/algorithm.h",
        },
        None
    )

    builder.Add(
        f"{ZetaExeBuildDir}/test_qsort.o",
        {
            f"{File}",
            f"{ZetaExeDir}/test_qsort.cpp",
        },
        lambda: compiler.cpp_to_obj(
            f"{ZetaExeBuildDir}/test_qsort.o",
            f"{ZetaExeDir}/test_qsort.cpp",
        )
    )

    builder.Add(
        f"{ZetaExeBuildDir}/test_qsort.exe",
        {
            f"{File}",
            f"{ZetaBuildDir}/algorithm.o",
            f"{ZetaBuildDir}/debugger.o",
            f"{ZetaBuildDir}/debug_str_pipe.o",
            f"{ZetaBuildDir}/io.o",
            f"{ZetaBuildDir}/utils.o",
            f"{ZetaExeBuildDir}/test_qsort.o",
        },
        lambda: compiler.to_exe(
            f"{ZetaExeBuildDir}/test_qsort.exe",

            {
                f"{ZetaBuildDir}/algorithm.o",
                f"{ZetaBuildDir}/debugger.o",
                f"{ZetaBuildDir}/debug_str_pipe.o",
                f"{ZetaBuildDir}/io.o",
                f"{ZetaBuildDir}/utils.o",
                f"{ZetaExeBuildDir}/test_qsort.o",
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
            f"{ZetaExeDir}/std_allocator.h",
            f"{ZetaExeDir}/test_scheduler.cpp",
        },
        lambda: compiler.cpp_to_obj(
            f"{ZetaExeBuildDir}/test_scheduler.o",
            f"{ZetaExeDir}/test_scheduler.cpp",
        )
    )

    builder.Add(
        f"{ZetaExeBuildDir}/test_scheduler.exe",
        {
            f"{File}",

            f"{ZetaBuildDir}/allocator.o",
            f"{ZetaBuildDir}/debug_hash_table.o",
            f"{ZetaBuildDir}/MultiLevelTable.o",
            f"{ZetaBuildDir}/ord_linked_list_node.o",
            f"{ZetaBuildDir}/utils.o",

            f"{ZetaExeBuildDir}/test_scheduler.o",
        },
        lambda: compiler.to_exe(
            f"{ZetaExeBuildDir}/test_scheduler.exe",

            {
                f"{ZetaBuildDir}/allocator.o",
                f"{ZetaBuildDir}/debug_hash_table.o",
                f"{ZetaBuildDir}/MultiLevelTable.o",
                f"{ZetaBuildDir}/ord_linked_list_node.o",
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
        f"{ZetaExeDir}/test_cas_alloc.cpp",
        {
            f"{File}",
            f"{ZetaDir}/cascade_allocator.h",
            f"{ZetaDir}/debug_hash_table.h",
            f"{ZetaDir}/debugger.h",
            f"{ZetaDir}/mem_check_utils.h",
        },
        None
    )

    builder.Add(
        f"{ZetaExeBuildDir}/test_cas_alloc.o",
        {
            f"{File}",
            f"{ZetaExeDir}/test_cas_alloc.cpp",
        },
        lambda: compiler.cpp_to_obj(
            f"{ZetaExeBuildDir}/test_cas_alloc.o",
            f"{ZetaExeDir}/test_cas_alloc.cpp",
        )
    )

    builder.Add(
        f"{ZetaExeBuildDir}/test_cas_alloc.exe",
        {
            f"{File}",

            f"{ZetaBuildDir}/allocator.o",
            f"{ZetaBuildDir}/cascade_allocator.o",
            f"{ZetaBuildDir}/debug_str_pipe.o",
            f"{ZetaBuildDir}/debugger.o",
            f"{ZetaBuildDir}/io.o",
            f"{ZetaBuildDir}/logger.o",
            f"{ZetaBuildDir}/mem_check_utils.o",
            f"{ZetaBuildDir}/ord_linked_list_node.o",
            f"{ZetaBuildDir}/utils.o",

            f"{ZetaExeBuildDir}/test_cas_alloc.o",
        },
        lambda: compiler.to_exe(
            f"{ZetaExeBuildDir}/test_cas_alloc.exe",

            {
                f"{ZetaBuildDir}/allocator.o",
                f"{ZetaBuildDir}/cascade_allocator.o",
                f"{ZetaBuildDir}/debug_str_pipe.o",
                f"{ZetaBuildDir}/debugger.o",
                f"{ZetaBuildDir}/io.o",
                f"{ZetaBuildDir}/logger.o",
                f"{ZetaBuildDir}/mem_check_utils.o",
                f"{ZetaBuildDir}/ord_linked_list_node.o",
                f"{ZetaBuildDir}/utils.o",

                f"{ZetaExeBuildDir}/test_cas_alloc.o",
            }
        )
    )

    builder.Add(
        f"{ZetaExeDir}/test_cntrbt.cpp",
        {
            f"{File}",
            f"{ZetaDir}/debugger.h",
            f"{ZetaDir}/rbtree.h",
            f"{ZetaDir}/ord_cnt_3rb_tree_node.h",
            f"{ZetaDir}/ord_cnt_rb_tree_node.h",
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
        lambda: compiler.cpp_to_obj(
            f"{ZetaExeBuildDir}/test_cntrbt.o",
            f"{ZetaExeDir}/test_cntrbt.cpp",
        )
    )

    builder.Add(
        f"{ZetaExeBuildDir}/test_cntrbt.exe",
        {
            f"{File}",

            f"{ZetaBuildDir}/bin_tree.o",
            f"{ZetaBuildDir}/debug_str_pipe.o",
            f"{ZetaBuildDir}/debugger.o",
            f"{ZetaBuildDir}/io.o",
            f"{ZetaBuildDir}/logger.o",
            f"{ZetaBuildDir}/mem_check_utils.o",
            f"{ZetaBuildDir}/ord_cnt_3rb_tree_node.o",
            f"{ZetaBuildDir}/ord_cnt_rb_tree_node.o",
            f"{ZetaBuildDir}/rbtree.o",
            f"{ZetaBuildDir}/utils.o",

            f"{ZetaExeBuildDir}/test_cntrbt.o",
        },
        lambda: compiler.to_exe(
            f"{ZetaExeBuildDir}/test_cntrbt.exe",

            {
                f"{ZetaBuildDir}/bin_tree.o",
                f"{ZetaBuildDir}/debug_str_pipe.o",
                f"{ZetaBuildDir}/debugger.o",
                f"{ZetaBuildDir}/io.o",
                f"{ZetaBuildDir}/logger.o",
                f"{ZetaBuildDir}/mem_check_utils.o",
                f"{ZetaBuildDir}/ord_cnt_3rb_tree_node.o",
                f"{ZetaBuildDir}/ord_cnt_rb_tree_node.o",
                f"{ZetaBuildDir}/rbtree.o",
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
            f"{ZetaDir}/crc.h",
        },
        None
    )

    builder.Add(
        f"{ZetaExeDir}/test_sha256.c",
        {
            f"{File}",
            f"{ZetaDir}/sha256.h",
        },
        None
    )

    builder.Add(
        f"{ZetaExeDir}/timer.h",
        {
            f"{File}",
        },
        None
    )

    builder.Add(
        f"{ZetaExeDir}/timer.cpp",
        {
            f"{File}",
            f"{ZetaExeDir}/timer.h",
        },
        None
    )

    builder.Add(
        f"{ZetaExeBuildDir}/timer.o",
        {
            f"{File}",
            f"{ZetaExeDir}/timer.h",
        },
        lambda: compiler.cpp_to_obj(
            f"{ZetaExeBuildDir}/timer.o",
            f"{ZetaExeDir}/timer.cpp",
        )
    )

    builder.Add(
        f"{ZetaExeDir}/pod_value.h",
        {
            f"{File}",
        },
        None
    )

    builder.Add(
        f"{ZetaExeDir}/pod_value.cpp",
        {
            f"{File}",
            f"{ZetaExeDir}/pod_value.h",
        },
        None
    )

    builder.Add(
        f"{ZetaExeBuildDir}/pod_value.o",
        {
            f"{File}",
            f"{ZetaExeDir}/pod_value.h",
        },
        lambda: compiler.cpp_to_obj(
            f"{ZetaExeBuildDir}/pod_value.o",
            f"{ZetaExeDir}/pod_value.cpp",
        )
    )

    builder.Add(
        f"{ZetaExeDir}/zeta_pool_allocator.h",
        {
            f"{ZetaDir}/allocator.h",
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

    print(termcolor.colored(f"{args=}", "yellow"))

    mode = ModeEnum[args.mode.upper()]

    ZetaBuildDir = None
    ZetaExeBuildDir = None

    if mode == ModeEnum.DEBUG:
        ZetaBuildDir = f"{ZetaDevDir}/ZetaDebugBuild"
        ZetaExeBuildDir = f"{ZetaDevDir}/ZetaExeDebugBuild"

    if mode == ModeEnum.RELEASE:
        ZetaBuildDir = f"{ZetaDevDir}/ZetaReleaseBuild"
        ZetaExeBuildDir = f"{ZetaDevDir}/ZetaExeReleaseBuild"

    builder = Builder()

    AddDeps(builder, ZetaBuildDir, ZetaExeBuildDir, args.verbose, mode)
    builder.Add("all", builder.GetUnits(), None)

    builder.Check()

    target = f"{ZetaExeBuildDir}/{args.target}"

    is_success, units = builder.Build(target, args.rebuild)

    status = "success" if is_success else "failed"

    print(termcolor.colored(status, "yellow"))

    for unit_state, units in units.items():
        print(termcolor.colored(f"{unit_state} build units:", "yellow"))

        for i in sorted(units):
            print(f"\t{i}")

    print(termcolor.colored(f"target: ", "yellow") + f"{target}")

    print(termcolor.colored(status, "yellow"))

    if not is_success:
        return

    if args.run_target:
        print(termcolor.colored(f"running: ", "yellow") + f"{target}")
        os.system(target)


if __name__ == "__main__":
    main()
