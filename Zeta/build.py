import argparse
import os
import sys

import termcolor
from typeguard import typechecked

from Builder import Builder
from config import ModeEnum, target
from LLVMCompiler import LLVMCompiler, LLVMCompilerConfig
from utils import GetRealPath

File = GetRealPath(os.path.realpath(__file__))
Dir = GetRealPath(os.path.realpath(os.path.dirname(__file__)))

ZetaDir = GetRealPath(Dir)
ZetaDevDir = GetRealPath(f"{ZetaDir}/..")


@typechecked
def AddDeps(builder: Builder, ZetaBuildDir: str, verbose: bool, mode: ModeEnum):
    compiler = LLVMCompiler(LLVMCompilerConfig(
        verbose=verbose,

        target=target,

        mode=mode,

        base_dir=ZetaDevDir,

        build_dir=ZetaBuildDir,

        working_dirs=[
            ZetaDevDir,
            ZetaDir,
        ],

        c_include_dirs=[],
        cpp_include_dirs=[],
    ))

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
        f"{ZetaDir}/algorithm.h",
        {
            f"{File}",
            f"{ZetaDir}/utils.h",
        },
        None
    )

    builder.Add(
        f"{ZetaDir}/algorithm.c",
        {
            f"{File}",
            f"{ZetaDir}/algorithm.h",
            f"{ZetaDir}/debugger.h",
        },
        None
    )

    builder.Add(
        f"{ZetaBuildDir}/algorithm.o",
        {
            f"{File}",
            f"{ZetaDir}/algorithm.c",
        },
        lambda: compiler.c_to_obj(
            f"{ZetaBuildDir}/algorithm.o",
            f"{ZetaDir}/algorithm.c",
        )
    )

    builder.Add(
        f"{ZetaDir}/allocator.h",
        {
            f"{File}",
            f"{ZetaDir}/mem_check_utils.h",
        },
        None
    )

    builder.Add(
        f"{ZetaDir}/allocator.c",
        {
            f"{File}",
            f"{ZetaDir}/allocator.h",
            f"{ZetaDir}/debugger.h",
        },
        None
    )

    builder.Add(
        f"{ZetaBuildDir}/allocator.o",
        {
            f"{File}",
            f"{ZetaDir}/allocator.c",
        },
        lambda: compiler.c_to_obj(
            f"{ZetaBuildDir}/allocator.o",
            f"{ZetaDir}/allocator.c",
        )
    )

    builder.Add(
        f"{ZetaDir}/assoc_cntr.h",
        {
            f"{File}",
            f"{ZetaDir}/define.h",
        },
        None
    )

    builder.Add(
        f"{ZetaDir}/assoc_cntr.c",
        {
            f"{File}",
            f"{ZetaDir}/assoc_cntr.h",
            f"{ZetaDir}/debugger.h",
        },
        None
    )

    builder.Add(
        f"{ZetaBuildDir}/assoc_cntr.o",
        {
            f"{File}",
            f"{ZetaDir}/assoc_cntr.c",
        },
        lambda: compiler.c_to_obj(
            f"{ZetaBuildDir}/assoc_cntr.o",
            f"{ZetaDir}/assoc_cntr.c",
        )
    )

    builder.Add(
        f"{ZetaDir}/bin_heap.h",
        {
            f"{File}",
            f"{ZetaDir}/define.h",
        },
        None
    )

    builder.Add(
        f"{ZetaDir}/bin_heap.c",
        {
            f"{File}",
            f"{ZetaDir}/bin_heap.h",
            f"{ZetaDir}/debugger.h",
            f"{ZetaDir}/utils.h",
        },
        None
    )

    builder.Add(
        f"{ZetaBuildDir}/bin_heap.o",
        {
            f"{File}",
            f"{ZetaDir}/bin_heap.c",
        },
        lambda: compiler.c_to_obj(
            f"{ZetaBuildDir}/bin_heap.o",
            f"{ZetaDir}/bin_heap.c",
        )
    )

    builder.Add(
        f"{ZetaDir}/bin_tree.h",
        {
            f"{File}",
            f"{ZetaDir}/define.h",
        },
        None
    )

    builder.Add(
        f"{ZetaDir}/bin_tree.c",
        {
            f"{File}",
            f"{ZetaDir}/bin_tree.h",
            f"{ZetaDir}/debugger.h",
            f"{ZetaDir}/utils.h",
        },
        None
    )

    builder.Add(
        f"{ZetaBuildDir}/bin_tree.o",
        {
            f"{File}",
            f"{ZetaDir}/bin_tree.c",
        },
        lambda: compiler.c_to_obj(
            f"{ZetaBuildDir}/bin_tree.o",
            f"{ZetaDir}/bin_tree.c",
        )
    )

    builder.Add(
        f"{ZetaDir}/block_vector.h",
        {
            f"{File}",

            f"{ZetaDir}/define.h",
        },
        None
    )

    builder.Add(
        f"{ZetaDir}/block_vector.c",
        {
            f"{File}",
            f"{ZetaDir}/block_vector.h",
        },
        None
    )

    builder.Add(
        f"{ZetaBuildDir}/block_vector.o",
        {
            f"{File}",
            f"{ZetaDir}/block_vector.c",
        },
        lambda: compiler.c_to_obj(
            f"{ZetaBuildDir}/block_vector.o",
            f"{ZetaDir}/block_vector.c",
        )
    )

    builder.Add(
        f"{ZetaDir}/cache_manager.h",
        {
            f"{File}",
            f"{ZetaDir}/define.h",
        },
        None
    )

    builder.Add(
        f"{ZetaDir}/cache_manager.c",
        {
            f"{File}",
            f"{ZetaDir}/cache_manager.h",
        },
        None
    )

    builder.Add(
        f"{ZetaBuildDir}/cache_manager.o",
        {
            f"{File}",
            f"{ZetaDir}/cache_manager.c",
        },
        lambda: compiler.c_to_obj(
            f"{ZetaBuildDir}/cache_manager.o",
            f"{ZetaDir}/cache_manager.c",
        )
    )

    builder.Add(
        f"{ZetaDir}/cascade_allocator.h",
        {
            f"{File}",
            f"{ZetaDir}/allocator.h",
        },
        None
    )

    builder.Add(
        f"{ZetaDir}/cascade_allocator.c",
        {
            f"{File}",
            f"{ZetaDir}/cascade_allocator.h",
            f"{ZetaDir}/debugger.h",
            f"{ZetaDir}/ord_linked_list_node.h",
            f"{ZetaDir}/utils.h",
        },
        None
    )

    builder.Add(
        f"{ZetaBuildDir}/cascade_allocator.o",
        {
            f"{File}",
            f"{ZetaDir}/cascade_allocator.c",
        },
        lambda: compiler.c_to_obj(
            f"{ZetaBuildDir}/cascade_allocator.o",
            f"{ZetaDir}/cascade_allocator.c",
        )
    )

    builder.Add(
        f"{ZetaDir}/circular_array.h",
        {
            f"{File}",
            f"{ZetaDir}/seq_cntr.h",
        },
        None
    )

    builder.Add(
        f"{ZetaDir}/circular_array.c",
        {
            f"{File}",
            f"{ZetaDir}/circular_array.h",
            f"{ZetaDir}/debugger.h",
            f"{ZetaDir}/utils.h",
        },
        None
    )

    builder.Add(
        f"{ZetaBuildDir}/circular_array.o",
        {
            f"{File}",
            f"{ZetaDir}/circular_array.c",
        },
        lambda: compiler.c_to_obj(
            f"{ZetaBuildDir}/circular_array.o",
            f"{ZetaDir}/circular_array.c",
        )
    )

    builder.Add(
        f"{ZetaDir}/crc.h",
        {
            f"{File}",
            f"{ZetaDir}/define.h",
        },
        None
    )

    builder.Add(
        f"{ZetaDir}/crc.c",
        {
            f"{File}",
            f"{ZetaDir}/crc.h",
            f"{ZetaDir}/debugger.h",
        },
        None
    )

    builder.Add(
        f"{ZetaBuildDir}/crc.o",
        {
            f"{File}",
            f"{ZetaDir}/crc.c",
        },
        lambda: compiler.c_to_obj(
            f"{ZetaBuildDir}/crc.o",
            f"{ZetaDir}/crc.c",
        )
    )

    builder.Add(
        f"{ZetaDir}/datetime.h",
        {
            f"{File}",
            f"{ZetaDir}/define.h",
        },
        None
    )

    builder.Add(
        f"{ZetaDir}/datetime.c",
        {
            f"{File}",
            f"{ZetaDir}/datetime.h",
            f"{ZetaDir}/debugger.h",
        },
        None
    )

    builder.Add(
        f"{ZetaBuildDir}/datetime.o",
        {
            f"{File}",
            f"{ZetaDir}/datetime.c",
        },
        lambda: compiler.c_to_obj(
            f"{ZetaBuildDir}/datetime.o",
            f"{ZetaDir}/datetime.c",
        )
    )

    builder.Add(
        f"{ZetaDir}/debug_deque.h",
        {
            f"{File}",
            f"{ZetaDir}/seq_cntr.h",
        },
        None
    )

    builder.Add(
        f"{ZetaDir}/debug_deque.cpp",
        {
            f"{File}",
            f"{ZetaDir}/debug_deque.h",
            f"{ZetaDir}/debugger.h",
            f"{ZetaDir}/utils.h",
        },
        None
    )

    builder.Add(
        f"{ZetaBuildDir}/debug_deque.o",
        {
            f"{File}",
            f"{ZetaDir}/debug_deque.cpp",
        },
        lambda: compiler.cpp_to_obj(
            f"{ZetaBuildDir}/debug_deque.o",
            f"{ZetaDir}/debug_deque.cpp",
        )
    )

    builder.Add(
        f"{ZetaDir}/debug_hash_table.h",
        {
            f"{File}",
            f"{ZetaDir}/assoc_cntr.h",
        },
        None
    )

    builder.Add(
        f"{ZetaDir}/debug_hash_table.cpp",
        {
            f"{File}",
            f"{ZetaDir}/debug_hash_table.h",
            f"{ZetaDir}/debugger.h",
        },
        None
    )

    builder.Add(
        f"{ZetaBuildDir}/debug_hash_table.o",
        {
            f"{File}",
            f"{ZetaDir}/debug_hash_table.cpp",
        },
        lambda: compiler.cpp_to_obj(
            f"{ZetaBuildDir}/debug_hash_table.o",
            f"{ZetaDir}/debug_hash_table.cpp",
        )
    )

    builder.Add(
        f"{ZetaDir}/debug_str_pipe.h",
        {
            f"{File}",
            f"{ZetaDir}/pipe.h",
        },
        None
    )

    builder.Add(
        f"{ZetaDir}/debug_str_pipe.cpp",
        {
            f"{File}",
            f"{ZetaDir}/debug_str_pipe.h",
            f"{ZetaDir}/debugger.h",
        },
        None
    )

    builder.Add(
        f"{ZetaBuildDir}/debug_str_pipe.o",
        {
            f"{File}",
            f"{ZetaDir}/debug_str_pipe.cpp",
        },
        lambda: compiler.cpp_to_obj(
            f"{ZetaBuildDir}/debug_str_pipe.o",
            f"{ZetaDir}/debug_str_pipe.cpp",
        )
    )

    builder.Add(
        f"{ZetaDir}/debugger.h",
        {
            f"{File}",
            f"{ZetaDir}/logger.h",
        },
        None
    )

    builder.Add(
        f"{ZetaDir}/debugger.c",
        {
            f"{File}",
            f"{ZetaDir}/debugger.h",
            f"{ZetaDir}/debug_str_pipe.h",
            f"{ZetaDir}/pipe.h",
        },
        None
    )

    builder.Add(
        f"{ZetaBuildDir}/debugger.o",
        {
            f"{File}",
            f"{ZetaDir}/debugger.c",
        },
        lambda: compiler.c_to_obj(
            f"{ZetaBuildDir}/debugger.o",
            f"{ZetaDir}/debugger.c",
        )
    )

    builder.Add(
        f"{ZetaDir}/disk_info.h",
        {
            f"{File}",
            f"{ZetaDir}/define.h",
        },
        None
    )

    builder.Add(
        f"{ZetaDir}/disk_info.c",
        {
            f"{File}",
            f"{ZetaDir}/disk_part_gpt.h",
            f"{ZetaDir}/utils.h",
        },
        None
    )

    builder.Add(
        f"{ZetaBuildDir}/disk_info.o",
        {
            f"{File}",
            f"{ZetaDir}/disk_info.c",
        },
        lambda: compiler.c_to_obj(
            f"{ZetaBuildDir}/disk_info.o",
            f"{ZetaDir}/disk_info.c",
        )
    )

    builder.Add(
        f"{ZetaDir}/disk_part_gpt.h",
        {
            f"{File}",
            f"{ZetaDir}/define.h",
        },
        None
    )

    builder.Add(
        f"{ZetaDir}/disk_part_gpt.c",
        {
            f"{File}",
            f"{ZetaDir}/disk_part_gpt.h",
            f"{ZetaDir}/utils.h",
        },
        None
    )

    builder.Add(
        f"{ZetaBuildDir}/disk_part_gpt.o",
        {
            f"{File}",
            f"{ZetaDir}/disk_part_gpt.c",
        },
        lambda: compiler.c_to_obj(
            f"{ZetaBuildDir}/disk_part_gpt.o",
            f"{ZetaDir}/disk_part_gpt.c",
        )
    )

    builder.Add(
        f"{ZetaDir}/disk_part_mbt.h",
        {
            f"{File}",
            f"{ZetaDir}/define.h",
        },
        None
    )

    builder.Add(
        f"{ZetaDir}/disk_part_mbt.c",
        {
            f"{File}",
            f"{ZetaDir}/disk_part_mbt.h",
            f"{ZetaDir}/utils.h",
        },
        None
    )

    builder.Add(
        f"{ZetaBuildDir}/disk_part_mbt.o",
        {
            f"{File}",
            f"{ZetaDir}/disk_part_mbt.c",
        },
        lambda: compiler.c_to_obj(
            f"{ZetaBuildDir}/disk_part_mbt.o",
            f"{ZetaDir}/disk_part_mbt.c",
        )
    )

    builder.Add(
        f"{ZetaDir}/dummy_block_vector.h",
        {
            f"{File}",
            f"{ZetaDir}/block_vector.h",
        },
        None
    )

    builder.Add(
        f"{ZetaDir}/dummy_block_vector.c",
        {
            f"{File}",
            f"{ZetaDir}/dummy_block_vector.h",
            f"{ZetaDir}/debugger.h",
            f"{ZetaDir}/utils.h",
        },
        None
    )

    builder.Add(
        f"{ZetaBuildDir}/dummy_block_vector.o",
        {
            f"{File}",
            f"{ZetaDir}/dummy_block_vector.c",
        },
        lambda: compiler.c_to_obj(
            f"{ZetaBuildDir}/dummy_block_vector.o",
            f"{ZetaDir}/dummy_block_vector.c",
        )
    )

    builder.Add(
        f"{ZetaDir}/dummy_cache_manager.h",
        {
            f"{File}",
            f"{ZetaDir}/block_vector.h",
            f"{ZetaDir}/cache_manager.h",
        },
        None
    )

    builder.Add(
        f"{ZetaDir}/dummy_cache_manager.c",
        {
            f"{File}",
            f"{ZetaDir}/dummy_cache_manager.h",
            f"{ZetaDir}/debugger.h",
        },
        None
    )

    builder.Add(
        f"{ZetaBuildDir}/dummy_cache_manager.o",
        {
            f"{File}",
            f"{ZetaDir}/dummy_cache_manager.c",
        },
        lambda: compiler.c_to_obj(
            f"{ZetaBuildDir}/dummy_cache_manager.o",
            f"{ZetaDir}/dummy_cache_manager.c",
        )
    )

    builder.Add(
        f"{ZetaDir}/dummy_vector.h",
        {
            f"{File}",
            f"{ZetaDir}/seq_cntr.h",
        },
        None
    )

    builder.Add(
        f"{ZetaDir}/dummy_vector.c",
        {
            f"{File}",
            f"{ZetaDir}/dummy_vector.h",
            f"{ZetaDir}/debugger.h",
        },
        None
    )

    builder.Add(
        f"{ZetaBuildDir}/dummy_vector.o",
        {
            f"{File}",
            f"{ZetaDir}/dummy_vector.c",
        },
        lambda: compiler.c_to_obj(
            f"{ZetaBuildDir}/dummy_vector.o",
            f"{ZetaDir}/dummy_vector.c",
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
        lambda: compiler.c_to_obj(
            f"{ZetaBuildDir}/DiffLinkedListNode.o",
            f"{ZetaDir}/DiffLinkedListNode.c",
        )
    )

    builder.Add(
        f"{ZetaDir}/dynamic_hash_table.h",
        {
            f"{File}",
            f"{ZetaDir}/allocator.h",
            f"{ZetaDir}/assoc_cntr.h",
            f"{ZetaDir}/generic_hash_table.h",
            f"{ZetaDir}/mem_check_utils.h",
            f"{ZetaDir}/ord_linked_list_node.h",
        },
        None
    )

    builder.Add(
        f"{ZetaDir}/dynamic_hash_table.c",
        {
            f"{File}",

            f"{ZetaDir}/dynamic_hash_table.h",
            f"{ZetaDir}/debugger.h",
            f"{ZetaDir}/utils.h",
        },
        None
    )

    builder.Add(
        f"{ZetaBuildDir}/dynamic_hash_table.o",
        {
            f"{File}",
            f"{ZetaDir}/dynamic_hash_table.c",
        },
        lambda: compiler.c_to_obj(
            f"{ZetaBuildDir}/dynamic_hash_table.o",
            f"{ZetaDir}/dynamic_hash_table.c",
        )
    )

    builder.Add(
        f"{ZetaDir}/dynamic_vector.h",
        {
            f"{File}",
            f"{ZetaDir}/allocator.h",
            f"{ZetaDir}/mem_check_utils.h",
            f"{ZetaDir}/seq_cntr.h",
        },
        None
    )

    builder.Add(
        f"{ZetaDir}/dynamic_vector.c",
        {
            f"{File}",
            f"{ZetaDir}/dynamic_vector.h",
            f"{ZetaDir}/circular_array.h",
            f"{ZetaDir}/debugger.h",
            f"{ZetaDir}/utils.h",
        },
        None
    )

    builder.Add(
        f"{ZetaBuildDir}/dynamic_vector.o",
        {
            f"{File}",
            f"{ZetaDir}/dynamic_vector.c",
        },
        lambda: compiler.c_to_obj(
            f"{ZetaBuildDir}/dynamic_vector.o",
            f"{ZetaDir}/dynamic_vector.c",
        )
    )

    builder.Add(
        f"{ZetaDir}/elf_utils.h",
        {
            f"{File}",
            f"{ZetaDir}/define.h",
        },
        None
    )

    builder.Add(
        f"{ZetaDir}/elf_utils.c",
        {
            f"{File}",
            f"{ZetaDir}/elf_utils.h",
            f"{ZetaDir}/debugger.h",
            f"{ZetaDir}/utils.h",
        },
        None
    )

    builder.Add(
        f"{ZetaBuildDir}/elf_utils.o",
        {
            f"{File}",
            f"{ZetaDir}/elf_utils.c",
        },
        lambda: compiler.c_to_obj(
            f"{ZetaBuildDir}/elf_utils.o",
            f"{ZetaDir}/elf_utils.c",
        )
    )

    builder.Add(
        f"{ZetaDir}/generic_hash_table.h",
        {
            f"{File}",
            f"{ZetaDir}/allocator.h",
            f"{ZetaDir}/mem_check_utils.h",
            f"{ZetaDir}/ord_rb_tree_node.h",
        },
        None
    )

    builder.Add(
        f"{ZetaDir}/generic_hash_table.c",
        {
            f"{File}",
            f"{ZetaDir}/generic_hash_table.h",
            f"{ZetaDir}/debugger.h",
            f"{ZetaDir}/mem_check_utils.h",
            f"{ZetaDir}/rbtree.h",
            f"{ZetaDir}/utils.h",
        },
        None
    )

    builder.Add(
        f"{ZetaBuildDir}/generic_hash_table.o",
        {
            f"{File}",
            f"{ZetaDir}/generic_hash_table.c",
        },
        lambda: compiler.c_to_obj(
            f"{ZetaBuildDir}/generic_hash_table.o",
            f"{ZetaDir}/generic_hash_table.c",
        )
    )

    builder.Add(
        f"{ZetaDir}/Groupbin_heap.h",
        {
            f"{File}",
            f"{ZetaDir}/define.h",
        },
        None
    )

    builder.Add(
        f"{ZetaDir}/Groupbin_heap.c",
        {
            f"{File}",
            f"{ZetaDir}/Groupbin_heap.h",

            f"{ZetaDir}/debugger.h",
            f"{ZetaDir}/utils.h",
        },
        None
    )

    builder.Add(
        f"{ZetaBuildDir}/Groupbin_heap.o",
        {
            f"{File}",
            f"{ZetaDir}/Groupbin_heap.c",
        },
        lambda: compiler.c_to_obj(
            f"{ZetaBuildDir}/Groupbin_heap.o",
            f"{ZetaDir}/Groupbin_heap.c",
        )
    )

    builder.Add(
        f"{ZetaDir}/FileSysFAT32.h",
        {
            f"{File}",
            f"{ZetaDir}/define.h",
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
        lambda: compiler.c_to_obj(
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
        lambda: compiler.c_to_obj(
            f"{ZetaBuildDir}/MultiLevelTable.o",
            f"{ZetaDir}/MultiLevelTable.c",
        )
    )

    builder.Add(
        f"{ZetaDir}/io.h",
        {
            f"{File}",
            f"{ZetaDir}/pipe.h",
        },
        None
    )

    builder.Add(
        f"{ZetaDir}/io.c",
        {
            f"{File}",
            f"{ZetaDir}/io.h",
            f"{ZetaDir}/debugger.h",
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
        lambda: compiler.c_to_obj(
            f"{ZetaBuildDir}/io.o",
            f"{ZetaDir}/io.c",
        )
    )

    builder.Add(
        f"{ZetaDir}/jump.h",
        {
            f"{File}",

            f"{ZetaDir}/define.h",
        },
        None
    )

    builder.Add(
        f"{ZetaDir}/Jump_x86_64.s",
        {
            f"{File}",
        },
        None
    )

    builder.Add(
        f"{ZetaBuildDir}/jump.o",
        {
            f"{File}",
            f"{ZetaDir}/jump.h",
            f"{ZetaDir}/Jump_x86_64.s",
        },
        lambda: compiler.asm_to_obj(
            f"{ZetaBuildDir}/jump.o",
            f"{ZetaDir}/Jump_x86_64.s",
        )
    )

    builder.Add(
        f"{ZetaDir}/lin_space_allocator.h",
        {
            f"{File}",
            f"{ZetaDir}/allocator.h",
            f"{ZetaDir}/assoc_cntr.h",
            f"{ZetaDir}/mem_check_utils.h",
        },
        None
    )

    builder.Add(
        f"{ZetaDir}/lin_space_allocator.c",
        {
            f"{File}",
            f"{ZetaDir}/lin_space_allocator.h",
            f"{ZetaDir}/debugger.h",
            f"{ZetaDir}/rbtree.h",
            f"{ZetaDir}/ptr_utils.h",
            f"{ZetaDir}/utils.h",
        },
        None
    )

    builder.Add(
        f"{ZetaBuildDir}/lin_space_allocator.o",
        {
            f"{File}",
            f"{ZetaDir}/lin_space_allocator.c",
        },
        lambda: compiler.c_to_obj(
            f"{ZetaBuildDir}/lin_space_allocator.o",
            f"{ZetaDir}/lin_space_allocator.c",
        )
    )

    builder.Add(
        f"{ZetaDir}/logger.h",
        {
            f"{File}",
            f"{ZetaDir}/io.h",
        },
        None
    )

    builder.Add(
        f"{ZetaDir}/logger.c",
        {
            f"{File}",
            f"{ZetaDir}/logger.h",
            f"{ZetaDir}/pipe.h",
        },
        None
    )

    builder.Add(
        f"{ZetaBuildDir}/logger.o",
        {
            f"{File}",
            f"{ZetaDir}/logger.c",
        },
        lambda: compiler.c_to_obj(
            f"{ZetaBuildDir}/logger.o",
            f"{ZetaDir}/logger.c",
        )
    )

    builder.Add(
        f"{ZetaDir}/lru_cache_manager.h",
        {
            f"{File}",

            f"{ZetaDir}/allocator.h",
            f"{ZetaDir}/block_vector.h",
            f"{ZetaDir}/cache_manager.h",
            f"{ZetaDir}/ord_bin_tree_node.h",
            f"{ZetaDir}/ord_linked_list_node.h",
            f"{ZetaDir}/ord_rb_tree_node.h",
        },
        None
    )

    builder.Add(
        f"{ZetaDir}/lru_cache_manager.c",
        {
            f"{File}",

            f"{ZetaDir}/lru_cache_manager.h",

            f"{ZetaDir}/debugger.h",
            f"{ZetaDir}/rbtree.h",
            f"{ZetaDir}/utils.h",
        },
        None
    )

    builder.Add(
        f"{ZetaBuildDir}/lru_cache_manager.o",
        {
            f"{File}",
            f"{ZetaDir}/lru_cache_manager.c",
        },
        lambda: compiler.c_to_obj(
            f"{ZetaBuildDir}/lru_cache_manager.o",
            f"{ZetaDir}/lru_cache_manager.c",
        )
    )

    builder.Add(
        f"{ZetaDir}/mem_check_utils.h",
        {
            f"{File}",
            f"{ZetaDir}/define.h",
        },
        None
    )

    builder.Add(
        f"{ZetaDir}/mem_check_utils.cpp",
        {
            f"{File}",
            f"{ZetaDir}/mem_check_utils.h",
            f"{ZetaDir}/debugger.h",
            f"{ZetaDir}/utils.h",
        },
        None
    )

    builder.Add(
        f"{ZetaBuildDir}/mem_check_utils.o",
        {
            f"{File}",
            f"{ZetaDir}/mem_check_utils.cpp"
            if mode == ModeEnum.DEBUG
            else f"{ZetaDir}/null.cpp",
        },
        lambda: compiler.cpp_to_obj(
            f"{ZetaBuildDir}/mem_check_utils.o",
            f"{ZetaDir}/mem_check_utils.cpp"
            if mode == ModeEnum.DEBUG
            else f"{ZetaDir}/null.cpp",
        )
    )

    builder.Add(
        f"{ZetaDir}/memory.h",
        {
            f"{File}",
            f"{ZetaDir}/allocator.h",
        },
        None
    )

    builder.Add(
        f"{ZetaDir}/memory.c",
        {
            f"{File}",
            f"{ZetaDir}/memory.h",
        },
        None
    )

    builder.Add(
        f"{ZetaBuildDir}/memory.o",
        {
            f"{File}",
            f"{ZetaDir}/memory.c"
        },
        lambda: compiler.c_to_obj(
            f"{ZetaBuildDir}/memory.o",
            f"{ZetaDir}/memory.c"
        )
    )

    builder.Add(
        f"{ZetaDir}/Ordallocator.h",
        {
            f"{File}",
            f"{ZetaDir}/allocator.h",
        },
        None
    )

    builder.Add(
        f"{ZetaDir}/null.c",
        {
            f"{File}",
        },
        None
    )

    builder.Add(
        f"{ZetaDir}/null.cpp",
        {
            f"{File}",
        },
        None
    )

    builder.Add(
        f"{ZetaDir}/ord_bin_tree_node.h",
        {
            f"{File}",

            f"{ZetaDir}/bin_tree.h",
        },
        None
    )

    builder.Add(
        f"{ZetaDir}/ord_bin_tree_node.c",
        {
            f"{File}",

            f"{ZetaDir}/ord_bin_tree_node.h",
        },
        None
    )

    builder.Add(
        f"{ZetaBuildDir}/ord_bin_tree_node.o",
        {
            f"{File}",
            f"{ZetaDir}/ord_bin_tree_node.c",
        },
        lambda: compiler.c_to_obj(
            f"{ZetaBuildDir}/ord_bin_tree_node.o",
            f"{ZetaDir}/ord_bin_tree_node.c",
        )
    )

    builder.Add(
        f"{ZetaDir}/ord_linked_list_node.h",
        {
            f"{File}",
            f"{ZetaDir}/define.h",
        },
        None
    )

    builder.Add(
        f"{ZetaDir}/ord_linked_list_node.c",
        {
            f"{File}",
            f"{ZetaDir}/ord_linked_list_node.h",
        },
        None
    )

    builder.Add(
        f"{ZetaBuildDir}/ord_linked_list_node.o",
        {
            f"{File}",
            f"{ZetaDir}/ord_linked_list_node.c",
        },
        lambda: compiler.c_to_obj(
            f"{ZetaBuildDir}/ord_linked_list_node.o",
            f"{ZetaDir}/ord_linked_list_node.c",
        )
    )

    builder.Add(
        f"{ZetaDir}/ord_rb_linked_list_node.h",
        {
            f"{File}",
            f"{ZetaDir}/define.h",
        },
        None
    )

    builder.Add(
        f"{ZetaDir}/ord_rb_linked_list_node.c",
        {
            f"{File}",
            f"{ZetaDir}/ord_rb_linked_list_node.h",
        },
        None
    )

    builder.Add(
        f"{ZetaBuildDir}/ord_rb_linked_list_node.o",
        {
            f"{File}",
            f"{ZetaDir}/ord_rb_linked_list_node.c",
        },
        lambda: compiler.c_to_obj(
            f"{ZetaBuildDir}/ord_rb_linked_list_node.o",
            f"{ZetaDir}/ord_rb_linked_list_node.c",
        )
    )

    builder.Add(
        f"{ZetaDir}/ord_rb_tree_node.h",
        {
            f"{File}",
            f"{ZetaDir}/bin_tree.h",
        },
        None
    )

    builder.Add(
        f"{ZetaDir}/ord_rb_tree_node.c",
        {
            f"{File}",
            f"{ZetaDir}/ord_rb_tree_node.h",
            f"{ZetaDir}/debugger.h",
        },
        None
    )

    builder.Add(
        f"{ZetaBuildDir}/ord_rb_tree_node.o",
        {
            f"{File}",
            f"{ZetaDir}/ord_rb_tree_node.c",
        },
        lambda: compiler.c_to_obj(
            f"{ZetaBuildDir}/ord_rb_tree_node.o",
            f"{ZetaDir}/ord_rb_tree_node.c",
        )
    )

    builder.Add(
        f"{ZetaDir}/ord_cnt_3rb_tree_node.h",
        {
            f"{File}",
            f"{ZetaDir}/bin_tree.h",
        },
        None
    )

    builder.Add(
        f"{ZetaDir}/ord_cnt_3rb_tree_node.c",
        {
            f"{File}",
            f"{ZetaDir}/ord_cnt_rb_tree_node.h",
            f"{ZetaDir}/debugger.h",
        },
        None
    )

    builder.Add(
        f"{ZetaBuildDir}/ord_cnt_3rb_tree_node.o",
        {
            f"{File}",
            f"{ZetaDir}/ord_cnt_3rb_tree_node.c",
        },
        lambda: compiler.c_to_obj(
            f"{ZetaBuildDir}/ord_cnt_3rb_tree_node.o",
            f"{ZetaDir}/ord_cnt_3rb_tree_node.c",
        )
    )

    builder.Add(
        f"{ZetaDir}/ord_cnt_rb_tree_node.h",
        {
            f"{File}",
            f"{ZetaDir}/bin_tree.h",
        },
        None
    )

    builder.Add(
        f"{ZetaDir}/ord_cnt_rb_tree_node.c",
        {
            f"{File}",
            f"{ZetaDir}/ord_cnt_rb_tree_node.h",
        },
        None
    )

    builder.Add(
        f"{ZetaBuildDir}/ord_cnt_rb_tree_node.o",
        {
            f"{File}",
            f"{ZetaDir}/ord_cnt_rb_tree_node.c",
        },
        lambda: compiler.c_to_obj(
            f"{ZetaBuildDir}/ord_cnt_rb_tree_node.o",
            f"{ZetaDir}/ord_cnt_rb_tree_node.c",
        )
    )

    builder.Add(
        f"{ZetaDir}/pipe.h",
        {
            f"{File}",
            f"{ZetaDir}/define.h",
        },
        None
    )

    builder.Add(
        f"{ZetaDir}/pool_allocator.h",
        {
            f"{File}",
            f"{ZetaDir}/allocator.h",
        },
        None
    )

    builder.Add(
        f"{ZetaDir}/pool_allocator.c",
        {
            f"{File}",
            f"{ZetaDir}/pool_allocator.h",
            f"{ZetaDir}/debugger.h",
        },
        None
    )

    builder.Add(
        f"{ZetaBuildDir}/pool_allocator.o",
        {
            f"{File}",
            f"{ZetaDir}/pool_allocator.c",
        },
        lambda: compiler.c_to_obj(
            f"{ZetaBuildDir}/pool_allocator.o",
            f"{ZetaDir}/pool_allocator.c",
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
            f"{ZetaDir}/circular_array.h",
            f"{ZetaDir}/seg_utils.h",
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
        lambda: compiler.c_to_obj(
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
        lambda: compiler.c_to_obj(
            f"{ZetaBuildDir}/random.o",
            f"{ZetaDir}/random.c",
        )
    )

    builder.Add(
        f"{ZetaDir}/rbtree.h",
        {
            f"{File}",
            f"{ZetaDir}/bin_tree.h",
        },
        None
    )

    builder.Add(
        f"{ZetaDir}/rbtree.c",
        {
            f"{File}",
            f"{ZetaDir}/rbtree.h",
            f"{ZetaDir}/debugger.h",
            f"{ZetaDir}/utils.h",
        },
        None
    )

    builder.Add(
        f"{ZetaBuildDir}/rbtree.o",
        {
            f"{File}",
            f"{ZetaDir}/rbtree.c",
        },
        lambda: compiler.c_to_obj(
            f"{ZetaBuildDir}/rbtree.o",
            f"{ZetaDir}/rbtree.c",
        )
    )

    builder.Add(
        f"{ZetaDir}/ptr_utils.h",
        {
            f"{File}",
            f"{ZetaDir}/define.h",
        },
        None
    )

    builder.Add(
        f"{ZetaDir}/SegList.h",
        {
            f"{File}",
            f"{ZetaDir}/allocator.h",
            f"{ZetaDir}/debug_hash_table.h",
            f"{ZetaDir}/ord_linked_list_node.h",
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
        lambda: compiler.c_to_obj(
            f"{ZetaBuildDir}/SegList.o",
            f"{ZetaDir}/SegList.c",
        )
    )

    builder.Add(
        f"{ZetaDir}/seg_staging_vector_temp.h",
        {
            f"{File}",
            f"{ZetaDir}/allocator.h",
            f"{ZetaDir}/mem_check_utils.h",
            f"{ZetaDir}/ord_cnt_3rb_tree_node.h",
            f"{ZetaDir}/ord_cnt_rb_tree_node.h",
            f"{ZetaDir}/seq_cntr.h",
        },
        None
    )

    builder.Add(
        f"{ZetaDir}/seg_staging_vector_temp.c",
        {
            f"{File}",
            f"{ZetaDir}/circular_array.h",
            f"{ZetaDir}/debugger.h",
            f"{ZetaDir}/dynamic_hash_table.h",
            f"{ZetaDir}/mem_check_utils.h",
            f"{ZetaDir}/pool_allocator.h",
            f"{ZetaDir}/rbtree.h",
            f"{ZetaDir}/seg_staging_vector_temp.h",
            f"{ZetaDir}/seg_utils.h",
            f"{ZetaDir}/utils.h",
        },
        None
    )

    builder.Add(
        f"{ZetaDir}/seg_vector.h",
        {
            f"{File}",
            f"{ZetaDir}/seg_staging_vector_temp.h",
        },
        None
    )

    builder.Add(
        f"{ZetaDir}/seg_vector.c",
        {
            f"{File}",
            f"{ZetaDir}/seg_staging_vector_temp.c",
        },
        None
    )

    builder.Add(
        f"{ZetaBuildDir}/seg_vector.o",
        {
            f"{File}",
            f"{ZetaDir}/seg_vector.c",
        },
        lambda: compiler.c_to_obj(
            f"{ZetaBuildDir}/seg_vector.o",
            f"{ZetaDir}/seg_vector.c",
        )
    )

    builder.Add(
        f"{ZetaDir}/seg_utils.h",
        {
            f"{File}",
            f"{ZetaDir}/circular_array.h",
        },
        None
    )

    builder.Add(
        f"{ZetaDir}/seg_utils.c",
        {
            f"{File}",
            f"{ZetaDir}/seg_utils.h",
            f"{ZetaDir}/debugger.h",
            f"{ZetaDir}/utils.h",
        },
        None
    )

    builder.Add(
        f"{ZetaBuildDir}/seg_utils.o",
        {
            f"{File}",
            f"{ZetaDir}/seg_utils.c",
        },
        lambda: compiler.c_to_obj(
            f"{ZetaBuildDir}/seg_utils.o",
            f"{ZetaDir}/seg_utils.c",
        )
    )

    builder.Add(
        f"{ZetaDir}/seq_cntr.h",
        {
            f"{File}",
            f"{ZetaDir}/define.h",
        },
        None
    )

    builder.Add(
        f"{ZetaDir}/seq_cntr.c",
        {
            f"{File}",
            f"{ZetaDir}/seq_cntr.h",
            f"{ZetaDir}/debugger.h",
        },
        None
    )

    builder.Add(
        f"{ZetaBuildDir}/seq_cntr.o",
        {
            f"{File}",
            f"{ZetaDir}/seq_cntr.c",
        },
        lambda: compiler.c_to_obj(
            f"{ZetaBuildDir}/seq_cntr.o",
            f"{ZetaDir}/seq_cntr.c",
        )
    )

    builder.Add(
        f"{ZetaDir}/sha256.h",
        {
            f"{File}",
            f"{ZetaDir}/define.h",
        },
        None
    )

    builder.Add(
        f"{ZetaDir}/sha256.c",
        {
            f"{File}",
            f"{ZetaDir}/sha256.h",
            f"{ZetaDir}/utils.h",
        },
        None
    )

    builder.Add(
        f"{ZetaBuildDir}/sha256.o",
        {
            f"{File}",
            f"{ZetaDir}/sha256.c",
        },
        lambda: compiler.c_to_obj(
            f"{ZetaBuildDir}/sha256.o",
            f"{ZetaDir}/sha256.c",
        )
    )

    builder.Add(
        f"{ZetaDir}/Slaballocator.h",
        {
            f"{File}",
            f"{ZetaDir}/allocator.h",
            f"{ZetaDir}/mem_check_utils.h",
            f"{ZetaDir}/ord_linked_list_node.h",
        },
        None
    )

    builder.Add(
        f"{ZetaDir}/Slaballocator.c",
        {
            f"{File}",
            f"{ZetaDir}/Slaballocator.h",
            f"{ZetaDir}/debugger.h",
            f"{ZetaDir}/utils.h",
        },
        None
    )

    builder.Add(
        f"{ZetaBuildDir}/Slaballocator.o",
        {
            f"{File}",
            f"{ZetaDir}/Slaballocator.c",
        },
        lambda: compiler.c_to_obj(
            f"{ZetaBuildDir}/Slaballocator.o",
            f"{ZetaDir}/Slaballocator.c",
        )
    )

    builder.Add(
        f"{ZetaDir}/staging_vector.h",
        {
            f"{File}",
            f"{ZetaDir}/seg_staging_vector_temp.h",
        },
        None
    )

    builder.Add(
        f"{ZetaDir}/staging_vector.c",
        {
            f"{File}",
            f"{ZetaDir}/seg_staging_vector_temp.c",
        },
        None
    )

    builder.Add(
        f"{ZetaBuildDir}/staging_vector.o",
        {
            f"{File}",
            f"{ZetaDir}/staging_vector.c",
        },
        lambda: compiler.c_to_obj(
            f"{ZetaBuildDir}/staging_vector.o",
            f"{ZetaDir}/staging_vector.c",
        )
    )

    builder.Add(
        f"{ZetaDir}/tree_allocator.h",
        {
            f"{File}",
            f"{ZetaDir}/allocator.h",
            f"{ZetaDir}/mem_check_utils.h",
            f"{ZetaDir}/ord_rb_linked_list_node.h",
            f"{ZetaDir}/ord_rb_tree_node.h",
        },
        None
    )

    builder.Add(
        f"{ZetaDir}/tree_allocator.c",
        {
            f"{File}",
            f"{ZetaDir}/tree_allocator.h",
            f"{ZetaDir}/debugger.h",
            f"{ZetaDir}/rbtree.h",
            f"{ZetaDir}/utils.h",
        },
        None
    )

    builder.Add(
        f"{ZetaBuildDir}/tree_allocator.o",
        {
            f"{File}",
            f"{ZetaDir}/tree_allocator.c",
        },
        lambda: compiler.c_to_obj(
            f"{ZetaBuildDir}/tree_allocator.o",
            f"{ZetaDir}/tree_allocator.c",
        )
    )

    builder.Add(
        f"{ZetaDir}/utf8.h",
        {
            f"{File}",
            f"{ZetaDir}/define.h",
        },
        None
    )

    builder.Add(
        f"{ZetaDir}/utf8.c",
        {
            f"{File}",
            f"{ZetaDir}/utf8.h",
            f"{ZetaDir}/debugger.h",
            f"{ZetaDir}/utils.h",
        },
        None
    )

    builder.Add(
        f"{ZetaBuildDir}/utf8.o",
        {
            f"{File}",
            f"{ZetaDir}/utf8.c",
        },
        lambda: compiler.c_to_obj(
            f"{ZetaBuildDir}/utf8.o",
            f"{ZetaDir}/utf8.c",
        )
    )

    builder.Add(
        f"{ZetaDir}/utf16.h",
        {
            f"{File}",
            f"{ZetaDir}/define.h",
        },
        None
    )

    builder.Add(
        f"{ZetaDir}/utf16.c",
        {
            f"{File}",
            f"{ZetaDir}/utf16.h",
            f"{ZetaDir}/debugger.h",
            f"{ZetaDir}/utils.h",
        },
        None
    )

    builder.Add(
        f"{ZetaBuildDir}/utf16.o",
        {
            f"{File}",
            f"{ZetaDir}/utf16.c",
        },
        lambda: compiler.c_to_obj(
            f"{ZetaBuildDir}/utf16.o",
            f"{ZetaDir}/utf16.c",
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
            f"{ZetaDir}/debugger.h",
        },
        None
    )

    builder.Add(
        f"{ZetaBuildDir}/utils.o",
        {
            f"{File}",
            f"{ZetaDir}/utils.c",
        },
        lambda: compiler.c_to_obj(
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

    mode = ModeEnum[args.mode.upper()]

    ZetaBuildDir = None

    if mode == ModeEnum.DEBUG:
        ZetaBuildDir = f"{ZetaDevDir}/ZetaDebugBuild"

    if mode == ModeEnum.RELEASE:
        ZetaBuildDir = f"{ZetaDevDir}/ZetaReleaseBuild"

    builder = Builder()

    AddDeps(builder, ZetaBuildDir, args.verbose, mode)
    builder.Add("all", builder.GetUnits(), None)

    builder.Check()

    target = f"{ZetaBuildDir}/{args.target}"

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
        print(termcolor.colored(f"Running: ", "yellow") + f"{target}")
        os.system(target)


if __name__ == "__main__":
    main()
