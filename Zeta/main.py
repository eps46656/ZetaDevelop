import dataclasses
import pathlib

from typeguard import typechecked

from Builder import Builder
from LLVMCompiler import LLVMCompiler, LLVMCompilerConfig
from utils import ModeEnum, Target

FILE = pathlib.Path(__file__).absolute()
DIR = FILE.parents[0]


@dataclasses.dataclass
class Config:
    verbose: bool

    build_dir: pathlib.Path

    target: Target

    mode: ModeEnum

    enable_argu_debug: bool
    enable_integrity_debug: bool
    enable_deep_debug: bool

    enable_asan: bool

    c_include_dirs: list[pathlib.Path]
    cpp_include_dirs: list[pathlib.Path]


@typechecked
def AddDeps(builder: Builder, config: Config):
    compiler = LLVMCompiler(LLVMCompilerConfig(
        verbose=config.verbose,

        build_dir=config.build_dir,

        target=config.target,

        mode=config.mode,

        base_dir=DIR.parents[0],

        c_include_dirs=[],
        cpp_include_dirs=[],

        enable_argu_debug=config.enable_argu_debug,
        enable_integrity_debug=config.enable_integrity_debug,
        enable_deep_debug=config.enable_deep_debug,

        enable_asan=config.enable_asan,
    ))

    zeta_dir = DIR
    zeta_build_dir = config.build_dir

    # --------------------------------------------------------------------------

    builder.Add(f"{FILE}", {}, None)

    builder.Add(
        f"{zeta_dir}/algorithm.h",
        {
            f"{FILE}",
            f"{zeta_dir}/utils.h",
        },
        None
    )

    builder.Add(
        f"{zeta_dir}/algorithm.c",
        {
            f"{FILE}",
            f"{zeta_dir}/algorithm.h",
            f"{zeta_dir}/debugger.h",
        },
        None
    )

    builder.Add(
        f"{zeta_build_dir}/algorithm.o",
        {
            f"{FILE}",
            f"{zeta_dir}/algorithm.c",
        },
        lambda: compiler.c_to_obj(
            f"{zeta_build_dir}/algorithm.o",
            f"{zeta_dir}/algorithm.c",
        )
    )

    builder.Add(
        f"{zeta_dir}/allocator.h",
        {
            f"{FILE}",
            f"{zeta_dir}/mem_check_utils.h",
        },
        None
    )

    builder.Add(
        f"{zeta_dir}/allocator.c",
        {
            f"{FILE}",
            f"{zeta_dir}/allocator.h",
            f"{zeta_dir}/debugger.h",
        },
        None
    )

    builder.Add(
        f"{zeta_build_dir}/allocator.o",
        {
            f"{FILE}",
            f"{zeta_dir}/allocator.c",
        },
        lambda: compiler.c_to_obj(
            f"{zeta_build_dir}/allocator.o",
            f"{zeta_dir}/allocator.c",
        )
    )

    builder.Add(
        f"{zeta_dir}/assoc_cntr.h",
        {
            f"{FILE}",
            f"{zeta_dir}/define.h",
        },
        None
    )

    builder.Add(
        f"{zeta_dir}/assoc_cntr.c",
        {
            f"{FILE}",
            f"{zeta_dir}/assoc_cntr.h",
            f"{zeta_dir}/debugger.h",
        },
        None
    )

    builder.Add(
        f"{zeta_build_dir}/assoc_cntr.o",
        {
            f"{FILE}",
            f"{zeta_dir}/assoc_cntr.c",
        },
        lambda: compiler.c_to_obj(
            f"{zeta_build_dir}/assoc_cntr.o",
            f"{zeta_dir}/assoc_cntr.c",
        )
    )

    builder.Add(
        f"{zeta_dir}/bin_heap.h",
        {
            f"{FILE}",
            f"{zeta_dir}/define.h",
        },
        None
    )

    builder.Add(
        f"{zeta_dir}/bin_heap.c",
        {
            f"{FILE}",
            f"{zeta_dir}/bin_heap.h",
            f"{zeta_dir}/debugger.h",
            f"{zeta_dir}/utils.h",
        },
        None
    )

    builder.Add(
        f"{zeta_build_dir}/bin_heap.o",
        {
            f"{FILE}",
            f"{zeta_dir}/bin_heap.c",
        },
        lambda: compiler.c_to_obj(
            f"{zeta_build_dir}/bin_heap.o",
            f"{zeta_dir}/bin_heap.c",
        )
    )

    builder.Add(
        f"{zeta_dir}/bin_tree.h",
        {
            f"{FILE}",
            f"{zeta_dir}/define.h",
        },
        None
    )

    builder.Add(
        f"{zeta_dir}/bin_tree.c",
        {
            f"{FILE}",
            f"{zeta_dir}/bin_tree.h",
            f"{zeta_dir}/debugger.h",
            f"{zeta_dir}/utils.h",
        },
        None
    )

    builder.Add(
        f"{zeta_build_dir}/bin_tree.o",
        {
            f"{FILE}",
            f"{zeta_dir}/bin_tree.c",
        },
        lambda: compiler.c_to_obj(
            f"{zeta_build_dir}/bin_tree.o",
            f"{zeta_dir}/bin_tree.c",
        )
    )

    builder.Add(
        f"{zeta_dir}/block_vector.h",
        {
            f"{FILE}",

            f"{zeta_dir}/define.h",
        },
        None
    )

    builder.Add(
        f"{zeta_dir}/block_vector.c",
        {
            f"{FILE}",
            f"{zeta_dir}/block_vector.h",
        },
        None
    )

    builder.Add(
        f"{zeta_build_dir}/block_vector.o",
        {
            f"{FILE}",
            f"{zeta_dir}/block_vector.c",
        },
        lambda: compiler.c_to_obj(
            f"{zeta_build_dir}/block_vector.o",
            f"{zeta_dir}/block_vector.c",
        )
    )

    builder.Add(
        f"{zeta_dir}/cache_manager.h",
        {
            f"{FILE}",
            f"{zeta_dir}/define.h",
        },
        None
    )

    builder.Add(
        f"{zeta_dir}/cache_manager.c",
        {
            f"{FILE}",
            f"{zeta_dir}/cache_manager.h",
        },
        None
    )

    builder.Add(
        f"{zeta_build_dir}/cache_manager.o",
        {
            f"{FILE}",
            f"{zeta_dir}/cache_manager.c",
        },
        lambda: compiler.c_to_obj(
            f"{zeta_build_dir}/cache_manager.o",
            f"{zeta_dir}/cache_manager.c",
        )
    )

    builder.Add(
        f"{zeta_dir}/cascade_allocator.h",
        {
            f"{FILE}",
            f"{zeta_dir}/allocator.h",
        },
        None
    )

    builder.Add(
        f"{zeta_dir}/cascade_allocator.c",
        {
            f"{FILE}",
            f"{zeta_dir}/cascade_allocator.h",
            f"{zeta_dir}/debugger.h",
            f"{zeta_dir}/ord_linked_list_node.h",
            f"{zeta_dir}/utils.h",
        },
        None
    )

    builder.Add(
        f"{zeta_build_dir}/cascade_allocator.o",
        {
            f"{FILE}",
            f"{zeta_dir}/cascade_allocator.c",
        },
        lambda: compiler.c_to_obj(
            f"{zeta_build_dir}/cascade_allocator.o",
            f"{zeta_dir}/cascade_allocator.c",
        )
    )

    builder.Add(
        f"{zeta_dir}/circular_array.h",
        {
            f"{FILE}",
            f"{zeta_dir}/seq_cntr.h",
        },
        None
    )

    builder.Add(
        f"{zeta_dir}/circular_array.c",
        {
            f"{FILE}",
            f"{zeta_dir}/circular_array.h",
            f"{zeta_dir}/debugger.h",
            f"{zeta_dir}/utils.h",
        },
        None
    )

    builder.Add(
        f"{zeta_build_dir}/circular_array.o",
        {
            f"{FILE}",
            f"{zeta_dir}/circular_array.c",
        },
        lambda: compiler.c_to_obj(
            f"{zeta_build_dir}/circular_array.o",
            f"{zeta_dir}/circular_array.c",
        )
    )

    builder.Add(
        f"{zeta_dir}/crc.h",
        {
            f"{FILE}",
            f"{zeta_dir}/define.h",
        },
        None
    )

    builder.Add(
        f"{zeta_dir}/crc.c",
        {
            f"{FILE}",
            f"{zeta_dir}/crc.h",
            f"{zeta_dir}/debugger.h",
        },
        None
    )

    builder.Add(
        f"{zeta_build_dir}/crc.o",
        {
            f"{FILE}",
            f"{zeta_dir}/crc.c",
        },
        lambda: compiler.c_to_obj(
            f"{zeta_build_dir}/crc.o",
            f"{zeta_dir}/crc.c",
        )
    )

    builder.Add(
        f"{zeta_dir}/datetime.h",
        {
            f"{FILE}",
            f"{zeta_dir}/define.h",
        },
        None
    )

    builder.Add(
        f"{zeta_dir}/datetime.c",
        {
            f"{FILE}",
            f"{zeta_dir}/datetime.h",
            f"{zeta_dir}/debugger.h",
        },
        None
    )

    builder.Add(
        f"{zeta_build_dir}/datetime.o",
        {
            f"{FILE}",
            f"{zeta_dir}/datetime.c",
        },
        lambda: compiler.c_to_obj(
            f"{zeta_build_dir}/datetime.o",
            f"{zeta_dir}/datetime.c",
        )
    )

    builder.Add(
        f"{zeta_dir}/debug_deque.h",
        {
            f"{FILE}",
            f"{zeta_dir}/seq_cntr.h",
        },
        None
    )

    builder.Add(
        f"{zeta_dir}/debug_deque.cpp",
        {
            f"{FILE}",
            f"{zeta_dir}/debug_deque.h",
            f"{zeta_dir}/debugger.h",
            f"{zeta_dir}/utils.h",
        },
        None
    )

    builder.Add(
        f"{zeta_build_dir}/debug_deque.o",
        {
            f"{FILE}",
            f"{zeta_dir}/debug_deque.cpp",
        },
        lambda: compiler.cpp_to_obj(
            f"{zeta_build_dir}/debug_deque.o",
            f"{zeta_dir}/debug_deque.cpp",
        )
    )

    builder.Add(
        f"{zeta_dir}/debug_hash_table.h",
        {
            f"{FILE}",
            f"{zeta_dir}/assoc_cntr.h",
        },
        None
    )

    builder.Add(
        f"{zeta_dir}/debug_hash_table.cpp",
        {
            f"{FILE}",
            f"{zeta_dir}/debug_hash_table.h",
            f"{zeta_dir}/debugger.h",
        },
        None
    )

    builder.Add(
        f"{zeta_build_dir}/debug_hash_table.o",
        {
            f"{FILE}",
            f"{zeta_dir}/debug_hash_table.cpp",
        },
        lambda: compiler.cpp_to_obj(
            f"{zeta_build_dir}/debug_hash_table.o",
            f"{zeta_dir}/debug_hash_table.cpp",
        )
    )

    builder.Add(
        f"{zeta_dir}/debug_str_pipe.h",
        {
            f"{FILE}",
            f"{zeta_dir}/pipe.h",
        },
        None
    )

    builder.Add(
        f"{zeta_dir}/debug_str_pipe.cpp",
        {
            f"{FILE}",
            f"{zeta_dir}/debug_str_pipe.h",
            f"{zeta_dir}/debugger.h",
        },
        None
    )

    builder.Add(
        f"{zeta_build_dir}/debug_str_pipe.o",
        {
            f"{FILE}",
            f"{zeta_dir}/debug_str_pipe.cpp",
        },
        lambda: compiler.cpp_to_obj(
            f"{zeta_build_dir}/debug_str_pipe.o",
            f"{zeta_dir}/debug_str_pipe.cpp",
        )
    )

    builder.Add(
        f"{zeta_dir}/debugger.h",
        {
            f"{FILE}",
            f"{zeta_dir}/logger.h",
        },
        None
    )

    builder.Add(
        f"{zeta_dir}/debugger.c",
        {
            f"{FILE}",
            f"{zeta_dir}/debugger.h",
            f"{zeta_dir}/debug_str_pipe.h",
            f"{zeta_dir}/pipe.h",
        },
        None
    )

    builder.Add(
        f"{zeta_build_dir}/debugger.o",
        {
            f"{FILE}",
            f"{zeta_dir}/debugger.c",
        },
        lambda: compiler.c_to_obj(
            f"{zeta_build_dir}/debugger.o",
            f"{zeta_dir}/debugger.c",
        )
    )

    builder.Add(
        f"{zeta_dir}/disk_info.h",
        {
            f"{FILE}",
            f"{zeta_dir}/define.h",
        },
        None
    )

    builder.Add(
        f"{zeta_dir}/disk_info.c",
        {
            f"{FILE}",
            f"{zeta_dir}/disk_part_gpt.h",
            f"{zeta_dir}/utils.h",
        },
        None
    )

    builder.Add(
        f"{zeta_build_dir}/disk_info.o",
        {
            f"{FILE}",
            f"{zeta_dir}/disk_info.c",
        },
        lambda: compiler.c_to_obj(
            f"{zeta_build_dir}/disk_info.o",
            f"{zeta_dir}/disk_info.c",
        )
    )

    builder.Add(
        f"{zeta_dir}/disk_part_gpt.h",
        {
            f"{FILE}",
            f"{zeta_dir}/define.h",
        },
        None
    )

    builder.Add(
        f"{zeta_dir}/disk_part_gpt.c",
        {
            f"{FILE}",
            f"{zeta_dir}/disk_part_gpt.h",
            f"{zeta_dir}/utils.h",
        },
        None
    )

    builder.Add(
        f"{zeta_build_dir}/disk_part_gpt.o",
        {
            f"{FILE}",
            f"{zeta_dir}/disk_part_gpt.c",
        },
        lambda: compiler.c_to_obj(
            f"{zeta_build_dir}/disk_part_gpt.o",
            f"{zeta_dir}/disk_part_gpt.c",
        )
    )

    builder.Add(
        f"{zeta_dir}/disk_part_mbt.h",
        {
            f"{FILE}",
            f"{zeta_dir}/define.h",
        },
        None
    )

    builder.Add(
        f"{zeta_dir}/disk_part_mbt.c",
        {
            f"{FILE}",
            f"{zeta_dir}/disk_part_mbt.h",
            f"{zeta_dir}/utils.h",
        },
        None
    )

    builder.Add(
        f"{zeta_build_dir}/disk_part_mbt.o",
        {
            f"{FILE}",
            f"{zeta_dir}/disk_part_mbt.c",
        },
        lambda: compiler.c_to_obj(
            f"{zeta_build_dir}/disk_part_mbt.o",
            f"{zeta_dir}/disk_part_mbt.c",
        )
    )

    builder.Add(
        f"{zeta_dir}/dummy_block_vector.h",
        {
            f"{FILE}",
            f"{zeta_dir}/block_vector.h",
        },
        None
    )

    builder.Add(
        f"{zeta_dir}/dummy_block_vector.c",
        {
            f"{FILE}",
            f"{zeta_dir}/dummy_block_vector.h",
            f"{zeta_dir}/debugger.h",
            f"{zeta_dir}/utils.h",
        },
        None
    )

    builder.Add(
        f"{zeta_build_dir}/dummy_block_vector.o",
        {
            f"{FILE}",
            f"{zeta_dir}/dummy_block_vector.c",
        },
        lambda: compiler.c_to_obj(
            f"{zeta_build_dir}/dummy_block_vector.o",
            f"{zeta_dir}/dummy_block_vector.c",
        )
    )

    builder.Add(
        f"{zeta_dir}/dummy_cache_manager.h",
        {
            f"{FILE}",
            f"{zeta_dir}/block_vector.h",
            f"{zeta_dir}/cache_manager.h",
        },
        None
    )

    builder.Add(
        f"{zeta_dir}/dummy_cache_manager.c",
        {
            f"{FILE}",
            f"{zeta_dir}/dummy_cache_manager.h",
            f"{zeta_dir}/debugger.h",
        },
        None
    )

    builder.Add(
        f"{zeta_build_dir}/dummy_cache_manager.o",
        {
            f"{FILE}",
            f"{zeta_dir}/dummy_cache_manager.c",
        },
        lambda: compiler.c_to_obj(
            f"{zeta_build_dir}/dummy_cache_manager.o",
            f"{zeta_dir}/dummy_cache_manager.c",
        )
    )

    builder.Add(
        f"{zeta_dir}/dummy_vector.h",
        {
            f"{FILE}",
            f"{zeta_dir}/seq_cntr.h",
        },
        None
    )

    builder.Add(
        f"{zeta_dir}/dummy_vector.c",
        {
            f"{FILE}",
            f"{zeta_dir}/dummy_vector.h",
            f"{zeta_dir}/debugger.h",
        },
        None
    )

    builder.Add(
        f"{zeta_build_dir}/dummy_vector.o",
        {
            f"{FILE}",
            f"{zeta_dir}/dummy_vector.c",
        },
        lambda: compiler.c_to_obj(
            f"{zeta_build_dir}/dummy_vector.o",
            f"{zeta_dir}/dummy_vector.c",
        )
    )

    builder.Add(
        f"{zeta_dir}/define.h",
        {
            f"{FILE}",
        },
        None
    )

    builder.Add(
        f"{zeta_dir}/DiffLinkedListNode.h",
        {
            f"{FILE}",
            f"{zeta_dir}/define.h",
        },
        None
    )

    builder.Add(
        f"{zeta_dir}/DiffLinkedListNode.c",
        {
            f"{FILE}",
            f"{zeta_dir}/DiffLinkedListNode.h",
        },
        None
    )

    builder.Add(
        f"{zeta_build_dir}/DiffLinkedListNode.o",
        {
            f"{FILE}",
            f"{zeta_dir}/DiffLinkedListNode.c",
        },
        lambda: compiler.c_to_obj(
            f"{zeta_build_dir}/DiffLinkedListNode.o",
            f"{zeta_dir}/DiffLinkedListNode.c",
        )
    )

    builder.Add(
        f"{zeta_dir}/dynamic_hash_table.h",
        {
            f"{FILE}",
            f"{zeta_dir}/allocator.h",
            f"{zeta_dir}/assoc_cntr.h",
            f"{zeta_dir}/generic_hash_table.h",
            f"{zeta_dir}/mem_check_utils.h",
            f"{zeta_dir}/ord_linked_list_node.h",
        },
        None
    )

    builder.Add(
        f"{zeta_dir}/dynamic_hash_table.c",
        {
            f"{FILE}",

            f"{zeta_dir}/dynamic_hash_table.h",
            f"{zeta_dir}/debugger.h",
            f"{zeta_dir}/utils.h",
        },
        None
    )

    builder.Add(
        f"{zeta_build_dir}/dynamic_hash_table.o",
        {
            f"{FILE}",
            f"{zeta_dir}/dynamic_hash_table.c",
        },
        lambda: compiler.c_to_obj(
            f"{zeta_build_dir}/dynamic_hash_table.o",
            f"{zeta_dir}/dynamic_hash_table.c",
        )
    )

    builder.Add(
        f"{zeta_dir}/dynamic_vector.h",
        {
            f"{FILE}",
            f"{zeta_dir}/allocator.h",
            f"{zeta_dir}/mem_check_utils.h",
            f"{zeta_dir}/seq_cntr.h",
        },
        None
    )

    builder.Add(
        f"{zeta_dir}/dynamic_vector.c",
        {
            f"{FILE}",
            f"{zeta_dir}/dynamic_vector.h",
            f"{zeta_dir}/circular_array.h",
            f"{zeta_dir}/debugger.h",
            f"{zeta_dir}/utils.h",
        },
        None
    )

    builder.Add(
        f"{zeta_build_dir}/dynamic_vector.o",
        {
            f"{FILE}",
            f"{zeta_dir}/dynamic_vector.c",
        },
        lambda: compiler.c_to_obj(
            f"{zeta_build_dir}/dynamic_vector.o",
            f"{zeta_dir}/dynamic_vector.c",
        )
    )

    builder.Add(
        f"{zeta_dir}/elf_utils.h",
        {
            f"{FILE}",
            f"{zeta_dir}/define.h",
        },
        None
    )

    builder.Add(
        f"{zeta_dir}/elf_utils.c",
        {
            f"{FILE}",
            f"{zeta_dir}/elf_utils.h",
            f"{zeta_dir}/debugger.h",
            f"{zeta_dir}/utils.h",
        },
        None
    )

    builder.Add(
        f"{zeta_build_dir}/elf_utils.o",
        {
            f"{FILE}",
            f"{zeta_dir}/elf_utils.c",
        },
        lambda: compiler.c_to_obj(
            f"{zeta_build_dir}/elf_utils.o",
            f"{zeta_dir}/elf_utils.c",
        )
    )

    builder.Add(
        f"{zeta_dir}/generic_hash_table.h",
        {
            f"{FILE}",
            f"{zeta_dir}/allocator.h",
            f"{zeta_dir}/mem_check_utils.h",
            f"{zeta_dir}/ord_rb_tree_node.h",
        },
        None
    )

    builder.Add(
        f"{zeta_dir}/generic_hash_table.c",
        {
            f"{FILE}",
            f"{zeta_dir}/generic_hash_table.h",
            f"{zeta_dir}/debugger.h",
            f"{zeta_dir}/mem_check_utils.h",
            f"{zeta_dir}/rbtree.h",
            f"{zeta_dir}/utils.h",
        },
        None
    )

    builder.Add(
        f"{zeta_build_dir}/generic_hash_table.o",
        {
            f"{FILE}",
            f"{zeta_dir}/generic_hash_table.c",
        },
        lambda: compiler.c_to_obj(
            f"{zeta_build_dir}/generic_hash_table.o",
            f"{zeta_dir}/generic_hash_table.c",
        )
    )

    builder.Add(
        f"{zeta_dir}/Groupbin_heap.h",
        {
            f"{FILE}",
            f"{zeta_dir}/define.h",
        },
        None
    )

    builder.Add(
        f"{zeta_dir}/Groupbin_heap.c",
        {
            f"{FILE}",
            f"{zeta_dir}/Groupbin_heap.h",

            f"{zeta_dir}/debugger.h",
            f"{zeta_dir}/utils.h",
        },
        None
    )

    builder.Add(
        f"{zeta_build_dir}/Groupbin_heap.o",
        {
            f"{FILE}",
            f"{zeta_dir}/Groupbin_heap.c",
        },
        lambda: compiler.c_to_obj(
            f"{zeta_build_dir}/Groupbin_heap.o",
            f"{zeta_dir}/Groupbin_heap.c",
        )
    )

    builder.Add(
        f"{zeta_dir}/FileSysFAT32.h",
        {
            f"{FILE}",
            f"{zeta_dir}/define.h",
        },
        None
    )

    builder.Add(
        f"{zeta_dir}/FileSysFAT32.c",
        {
            f"{FILE}",
            f"{zeta_dir}/FileSysFAT32.h",
        },
        None
    )

    builder.Add(
        f"{zeta_build_dir}/FileSysFAT32.o",
        {
            f"{FILE}",
            f"{zeta_dir}/FileSysFAT32.c",
        },
        lambda: compiler.c_to_obj(
            f"{zeta_build_dir}/FileSysFAT32.o",
            f"{zeta_dir}/FileSysFAT32.c",
        )
    )

    builder.Add(
        f"{zeta_dir}/MultiLevelTable.h",
        {
            f"{FILE}",
            f"{zeta_dir}/define.h",
        },
        None
    )

    builder.Add(
        f"{zeta_dir}/MultiLevelTable.c",
        {
            f"{FILE}",
            f"{zeta_dir}/MultiLevelTable.h",
        },
        None
    )

    builder.Add(
        f"{zeta_build_dir}/MultiLevelTable.o",
        {
            f"{FILE}",
            f"{zeta_dir}/MultiLevelTable.c",
        },
        lambda: compiler.c_to_obj(
            f"{zeta_build_dir}/MultiLevelTable.o",
            f"{zeta_dir}/MultiLevelTable.c",
        )
    )

    builder.Add(
        f"{zeta_dir}/io.h",
        {
            f"{FILE}",
            f"{zeta_dir}/pipe.h",
        },
        None
    )

    builder.Add(
        f"{zeta_dir}/io.c",
        {
            f"{FILE}",
            f"{zeta_dir}/io.h",
            f"{zeta_dir}/debugger.h",
            f"{zeta_dir}/utils.h",
        },
        None
    )

    builder.Add(
        f"{zeta_build_dir}/io.o",
        {
            f"{FILE}",
            f"{zeta_dir}/io.c",
        },
        lambda: compiler.c_to_obj(
            f"{zeta_build_dir}/io.o",
            f"{zeta_dir}/io.c",
        )
    )

    builder.Add(
        f"{zeta_dir}/jump.h",
        {
            f"{FILE}",

            f"{zeta_dir}/define.h",
        },
        None
    )

    builder.Add(
        f"{zeta_dir}/Jump_x86_64.s",
        {
            f"{FILE}",
        },
        None
    )

    builder.Add(
        f"{zeta_build_dir}/jump.o",
        {
            f"{FILE}",
            f"{zeta_dir}/jump.h",
            f"{zeta_dir}/Jump_x86_64.s",
        },
        lambda: compiler.asm_to_obj(
            f"{zeta_build_dir}/jump.o",
            f"{zeta_dir}/Jump_x86_64.s",
        )
    )

    builder.Add(
        f"{zeta_dir}/lin_space_allocator.h",
        {
            f"{FILE}",
            f"{zeta_dir}/allocator.h",
            f"{zeta_dir}/assoc_cntr.h",
            f"{zeta_dir}/mem_check_utils.h",
        },
        None
    )

    builder.Add(
        f"{zeta_dir}/lin_space_allocator.c",
        {
            f"{FILE}",
            f"{zeta_dir}/lin_space_allocator.h",
            f"{zeta_dir}/debugger.h",
            f"{zeta_dir}/rbtree.h",
            f"{zeta_dir}/ptr_utils.h",
            f"{zeta_dir}/utils.h",
        },
        None
    )

    builder.Add(
        f"{zeta_build_dir}/lin_space_allocator.o",
        {
            f"{FILE}",
            f"{zeta_dir}/lin_space_allocator.c",
        },
        lambda: compiler.c_to_obj(
            f"{zeta_build_dir}/lin_space_allocator.o",
            f"{zeta_dir}/lin_space_allocator.c",
        )
    )

    builder.Add(
        f"{zeta_dir}/logger.h",
        {
            f"{FILE}",
            f"{zeta_dir}/io.h",
        },
        None
    )

    builder.Add(
        f"{zeta_dir}/logger.c",
        {
            f"{FILE}",
            f"{zeta_dir}/logger.h",
            f"{zeta_dir}/pipe.h",
        },
        None
    )

    builder.Add(
        f"{zeta_build_dir}/logger.o",
        {
            f"{FILE}",
            f"{zeta_dir}/logger.c",
        },
        lambda: compiler.c_to_obj(
            f"{zeta_build_dir}/logger.o",
            f"{zeta_dir}/logger.c",
        )
    )

    builder.Add(
        f"{zeta_dir}/lru_cache_manager.h",
        {
            f"{FILE}",

            f"{zeta_dir}/allocator.h",
            f"{zeta_dir}/block_vector.h",
            f"{zeta_dir}/cache_manager.h",
            f"{zeta_dir}/ord_bin_tree_node.h",
            f"{zeta_dir}/ord_linked_list_node.h",
            f"{zeta_dir}/ord_rb_tree_node.h",
        },
        None
    )

    builder.Add(
        f"{zeta_dir}/lru_cache_manager.c",
        {
            f"{FILE}",

            f"{zeta_dir}/lru_cache_manager.h",

            f"{zeta_dir}/debugger.h",
            f"{zeta_dir}/rbtree.h",
            f"{zeta_dir}/utils.h",
        },
        None
    )

    builder.Add(
        f"{zeta_build_dir}/lru_cache_manager.o",
        {
            f"{FILE}",
            f"{zeta_dir}/lru_cache_manager.c",
        },
        lambda: compiler.c_to_obj(
            f"{zeta_build_dir}/lru_cache_manager.o",
            f"{zeta_dir}/lru_cache_manager.c",
        )
    )

    builder.Add(
        f"{zeta_dir}/mem_check_utils.h",
        {
            f"{FILE}",
            f"{zeta_dir}/define.h",
        },
        None
    )

    builder.Add(
        f"{zeta_dir}/mem_check_utils.cpp",
        {
            f"{FILE}",
            f"{zeta_dir}/mem_check_utils.h",
            f"{zeta_dir}/debugger.h",
            f"{zeta_dir}/utils.h",
        },
        None
    )

    builder.Add(
        f"{zeta_build_dir}/mem_check_utils.o",
        {
            f"{FILE}",
            f"{zeta_dir}/mem_check_utils.cpp",
        },
        lambda: compiler.cpp_to_obj(
            f"{zeta_build_dir}/mem_check_utils.o",
            f"{zeta_dir}/mem_check_utils.cpp",
        )
    )

    builder.Add(
        f"{zeta_dir}/memory.h",
        {
            f"{FILE}",
            f"{zeta_dir}/allocator.h",
        },
        None
    )

    builder.Add(
        f"{zeta_dir}/memory.c",
        {
            f"{FILE}",
            f"{zeta_dir}/memory.h",
        },
        None
    )

    builder.Add(
        f"{zeta_build_dir}/memory.o",
        {
            f"{FILE}",
            f"{zeta_dir}/memory.c"
        },
        lambda: compiler.c_to_obj(
            f"{zeta_build_dir}/memory.o",
            f"{zeta_dir}/memory.c"
        )
    )

    builder.Add(
        f"{zeta_dir}/Ordallocator.h",
        {
            f"{FILE}",
            f"{zeta_dir}/allocator.h",
        },
        None
    )

    builder.Add(
        f"{zeta_dir}/null.c",
        {
            f"{FILE}",
        },
        None
    )

    builder.Add(
        f"{zeta_dir}/null.cpp",
        {
            f"{FILE}",
        },
        None
    )

    builder.Add(
        f"{zeta_dir}/ord_bin_tree_node.h",
        {
            f"{FILE}",

            f"{zeta_dir}/bin_tree.h",
        },
        None
    )

    builder.Add(
        f"{zeta_dir}/ord_bin_tree_node.c",
        {
            f"{FILE}",

            f"{zeta_dir}/ord_bin_tree_node.h",
        },
        None
    )

    builder.Add(
        f"{zeta_build_dir}/ord_bin_tree_node.o",
        {
            f"{FILE}",
            f"{zeta_dir}/ord_bin_tree_node.c",
        },
        lambda: compiler.c_to_obj(
            f"{zeta_build_dir}/ord_bin_tree_node.o",
            f"{zeta_dir}/ord_bin_tree_node.c",
        )
    )

    builder.Add(
        f"{zeta_dir}/ord_linked_list_node.h",
        {
            f"{FILE}",
            f"{zeta_dir}/define.h",
        },
        None
    )

    builder.Add(
        f"{zeta_dir}/ord_linked_list_node.c",
        {
            f"{FILE}",
            f"{zeta_dir}/ord_linked_list_node.h",
        },
        None
    )

    builder.Add(
        f"{zeta_build_dir}/ord_linked_list_node.o",
        {
            f"{FILE}",
            f"{zeta_dir}/ord_linked_list_node.c",
        },
        lambda: compiler.c_to_obj(
            f"{zeta_build_dir}/ord_linked_list_node.o",
            f"{zeta_dir}/ord_linked_list_node.c",
        )
    )

    builder.Add(
        f"{zeta_dir}/ord_rb_linked_list_node.h",
        {
            f"{FILE}",
            f"{zeta_dir}/define.h",
        },
        None
    )

    builder.Add(
        f"{zeta_dir}/ord_rb_linked_list_node.c",
        {
            f"{FILE}",
            f"{zeta_dir}/ord_rb_linked_list_node.h",
        },
        None
    )

    builder.Add(
        f"{zeta_build_dir}/ord_rb_linked_list_node.o",
        {
            f"{FILE}",
            f"{zeta_dir}/ord_rb_linked_list_node.c",
        },
        lambda: compiler.c_to_obj(
            f"{zeta_build_dir}/ord_rb_linked_list_node.o",
            f"{zeta_dir}/ord_rb_linked_list_node.c",
        )
    )

    builder.Add(
        f"{zeta_dir}/ord_rb_tree_node.h",
        {
            f"{FILE}",
            f"{zeta_dir}/bin_tree.h",
        },
        None
    )

    builder.Add(
        f"{zeta_dir}/ord_rb_tree_node.c",
        {
            f"{FILE}",
            f"{zeta_dir}/ord_rb_tree_node.h",
            f"{zeta_dir}/debugger.h",
        },
        None
    )

    builder.Add(
        f"{zeta_build_dir}/ord_rb_tree_node.o",
        {
            f"{FILE}",
            f"{zeta_dir}/ord_rb_tree_node.c",
        },
        lambda: compiler.c_to_obj(
            f"{zeta_build_dir}/ord_rb_tree_node.o",
            f"{zeta_dir}/ord_rb_tree_node.c",
        )
    )

    builder.Add(
        f"{zeta_dir}/ord_cnt_3rb_tree_node.h",
        {
            f"{FILE}",
            f"{zeta_dir}/bin_tree.h",
        },
        None
    )

    builder.Add(
        f"{zeta_dir}/ord_cnt_3rb_tree_node.c",
        {
            f"{FILE}",
            f"{zeta_dir}/ord_cnt_rb_tree_node.h",
            f"{zeta_dir}/debugger.h",
        },
        None
    )

    builder.Add(
        f"{zeta_build_dir}/ord_cnt_3rb_tree_node.o",
        {
            f"{FILE}",
            f"{zeta_dir}/ord_cnt_3rb_tree_node.c",
        },
        lambda: compiler.c_to_obj(
            f"{zeta_build_dir}/ord_cnt_3rb_tree_node.o",
            f"{zeta_dir}/ord_cnt_3rb_tree_node.c",
        )
    )

    builder.Add(
        f"{zeta_dir}/ord_cnt_rb_tree_node.h",
        {
            f"{FILE}",
            f"{zeta_dir}/bin_tree.h",
        },
        None
    )

    builder.Add(
        f"{zeta_dir}/ord_cnt_rb_tree_node.c",
        {
            f"{FILE}",
            f"{zeta_dir}/ord_cnt_rb_tree_node.h",
        },
        None
    )

    builder.Add(
        f"{zeta_build_dir}/ord_cnt_rb_tree_node.o",
        {
            f"{FILE}",
            f"{zeta_dir}/ord_cnt_rb_tree_node.c",
        },
        lambda: compiler.c_to_obj(
            f"{zeta_build_dir}/ord_cnt_rb_tree_node.o",
            f"{zeta_dir}/ord_cnt_rb_tree_node.c",
        )
    )

    builder.Add(
        f"{zeta_dir}/pipe.h",
        {
            f"{FILE}",
            f"{zeta_dir}/define.h",
        },
        None
    )

    builder.Add(
        f"{zeta_dir}/pool_allocator.h",
        {
            f"{FILE}",
            f"{zeta_dir}/allocator.h",
        },
        None
    )

    builder.Add(
        f"{zeta_dir}/pool_allocator.c",
        {
            f"{FILE}",
            f"{zeta_dir}/pool_allocator.h",
            f"{zeta_dir}/debugger.h",
        },
        None
    )

    builder.Add(
        f"{zeta_build_dir}/pool_allocator.o",
        {
            f"{FILE}",
            f"{zeta_dir}/pool_allocator.c",
        },
        lambda: compiler.c_to_obj(
            f"{zeta_build_dir}/pool_allocator.o",
            f"{zeta_dir}/pool_allocator.c",
        )
    )

    builder.Add(
        f"{zeta_dir}/QTree.h",
        {
            f"{FILE}",
            f"{zeta_dir}/define.h",
        },
        None
    )

    builder.Add(
        f"{zeta_dir}/QTree.c",
        {
            f"{FILE}",
            f"{zeta_dir}/QTree.h",
            f"{zeta_dir}/circular_array.h",
            f"{zeta_dir}/seg_utils.h",
            f"{zeta_dir}/utils.h",
        },
        None
    )

    builder.Add(
        f"{zeta_build_dir}/QTree.o",
        {
            f"{FILE}",
            f"{zeta_dir}/QTree.c",
        },
        lambda: compiler.c_to_obj(
            f"{zeta_build_dir}/QTree.o",
            f"{zeta_dir}/QTree.c",
        )
    )

    builder.Add(
        f"{zeta_dir}/random.h",
        {
            f"{FILE}",
            f"{zeta_dir}/define.h",
        },
        None
    )

    builder.Add(
        f"{zeta_dir}/random.c",
        {
            f"{FILE}",
            f"{zeta_dir}/random.h",
        },
        None
    )

    builder.Add(
        f"{zeta_build_dir}/random.o",
        {
            f"{FILE}",
            f"{zeta_dir}/random.c",
        },
        lambda: compiler.c_to_obj(
            f"{zeta_build_dir}/random.o",
            f"{zeta_dir}/random.c",
        )
    )

    builder.Add(
        f"{zeta_dir}/rbtree.h",
        {
            f"{FILE}",
            f"{zeta_dir}/bin_tree.h",
        },
        None
    )

    builder.Add(
        f"{zeta_dir}/rbtree.c",
        {
            f"{FILE}",
            f"{zeta_dir}/rbtree.h",
            f"{zeta_dir}/debugger.h",
            f"{zeta_dir}/utils.h",
        },
        None
    )

    builder.Add(
        f"{zeta_build_dir}/rbtree.o",
        {
            f"{FILE}",
            f"{zeta_dir}/rbtree.c",
        },
        lambda: compiler.c_to_obj(
            f"{zeta_build_dir}/rbtree.o",
            f"{zeta_dir}/rbtree.c",
        )
    )

    builder.Add(
        f"{zeta_dir}/ptr_utils.h",
        {
            f"{FILE}",
            f"{zeta_dir}/define.h",
        },
        None
    )

    builder.Add(
        f"{zeta_dir}/SegList.h",
        {
            f"{FILE}",
            f"{zeta_dir}/allocator.h",
            f"{zeta_dir}/debug_hash_table.h",
            f"{zeta_dir}/ord_linked_list_node.h",
        },
        None
    )

    builder.Add(
        f"{zeta_dir}/SegList.c",
        {
            f"{FILE}",
            f"{zeta_dir}/SegList.h",
        },
        None
    )

    builder.Add(
        f"{zeta_build_dir}/SegList.o",
        {
            f"{FILE}",
            f"{zeta_dir}/SegList.c",
        },
        lambda: compiler.c_to_obj(
            f"{zeta_build_dir}/SegList.o",
            f"{zeta_dir}/SegList.c",
        )
    )

    builder.Add(
        f"{zeta_dir}/seg_staging_vector_temp.h",
        {
            f"{FILE}",
            f"{zeta_dir}/allocator.h",
            f"{zeta_dir}/mem_check_utils.h",
            f"{zeta_dir}/ord_cnt_3rb_tree_node.h",
            f"{zeta_dir}/ord_cnt_rb_tree_node.h",
            f"{zeta_dir}/seq_cntr.h",
        },
        None
    )

    builder.Add(
        f"{zeta_dir}/seg_staging_vector_temp.c",
        {
            f"{FILE}",
            f"{zeta_dir}/circular_array.h",
            f"{zeta_dir}/debugger.h",
            f"{zeta_dir}/dynamic_hash_table.h",
            f"{zeta_dir}/mem_check_utils.h",
            f"{zeta_dir}/pool_allocator.h",
            f"{zeta_dir}/rbtree.h",
            f"{zeta_dir}/seg_staging_vector_temp.h",
            f"{zeta_dir}/seg_utils.h",
            f"{zeta_dir}/utils.h",
        },
        None
    )

    builder.Add(
        f"{zeta_dir}/seg_vector.h",
        {
            f"{FILE}",
            f"{zeta_dir}/seg_staging_vector_temp.h",
        },
        None
    )

    builder.Add(
        f"{zeta_dir}/seg_vector.c",
        {
            f"{FILE}",
            f"{zeta_dir}/seg_staging_vector_temp.c",
        },
        None
    )

    builder.Add(
        f"{zeta_build_dir}/seg_vector.o",
        {
            f"{FILE}",
            f"{zeta_dir}/seg_vector.c",
        },
        lambda: compiler.c_to_obj(
            f"{zeta_build_dir}/seg_vector.o",
            f"{zeta_dir}/seg_vector.c",
        )
    )

    builder.Add(
        f"{zeta_dir}/seg_utils.h",
        {
            f"{FILE}",
            f"{zeta_dir}/circular_array.h",
        },
        None
    )

    builder.Add(
        f"{zeta_dir}/seg_utils.c",
        {
            f"{FILE}",
            f"{zeta_dir}/seg_utils.h",
            f"{zeta_dir}/debugger.h",
            f"{zeta_dir}/utils.h",
        },
        None
    )

    builder.Add(
        f"{zeta_build_dir}/seg_utils.o",
        {
            f"{FILE}",
            f"{zeta_dir}/seg_utils.c",
        },
        lambda: compiler.c_to_obj(
            f"{zeta_build_dir}/seg_utils.o",
            f"{zeta_dir}/seg_utils.c",
        )
    )

    builder.Add(
        f"{zeta_dir}/seq_cntr.h",
        {
            f"{FILE}",
            f"{zeta_dir}/define.h",
        },
        None
    )

    builder.Add(
        f"{zeta_dir}/seq_cntr.c",
        {
            f"{FILE}",
            f"{zeta_dir}/seq_cntr.h",
            f"{zeta_dir}/debugger.h",
        },
        None
    )

    builder.Add(
        f"{zeta_build_dir}/seq_cntr.o",
        {
            f"{FILE}",
            f"{zeta_dir}/seq_cntr.c",
        },
        lambda: compiler.c_to_obj(
            f"{zeta_build_dir}/seq_cntr.o",
            f"{zeta_dir}/seq_cntr.c",
        )
    )

    builder.Add(
        f"{zeta_dir}/sha256.h",
        {
            f"{FILE}",
            f"{zeta_dir}/define.h",
        },
        None
    )

    builder.Add(
        f"{zeta_dir}/sha256.c",
        {
            f"{FILE}",
            f"{zeta_dir}/sha256.h",
            f"{zeta_dir}/utils.h",
        },
        None
    )

    builder.Add(
        f"{zeta_build_dir}/sha256.o",
        {
            f"{FILE}",
            f"{zeta_dir}/sha256.c",
        },
        lambda: compiler.c_to_obj(
            f"{zeta_build_dir}/sha256.o",
            f"{zeta_dir}/sha256.c",
        )
    )

    builder.Add(
        f"{zeta_dir}/Slaballocator.h",
        {
            f"{FILE}",
            f"{zeta_dir}/allocator.h",
            f"{zeta_dir}/mem_check_utils.h",
            f"{zeta_dir}/ord_linked_list_node.h",
        },
        None
    )

    builder.Add(
        f"{zeta_dir}/Slaballocator.c",
        {
            f"{FILE}",
            f"{zeta_dir}/Slaballocator.h",
            f"{zeta_dir}/debugger.h",
            f"{zeta_dir}/utils.h",
        },
        None
    )

    builder.Add(
        f"{zeta_build_dir}/Slaballocator.o",
        {
            f"{FILE}",
            f"{zeta_dir}/Slaballocator.c",
        },
        lambda: compiler.c_to_obj(
            f"{zeta_build_dir}/Slaballocator.o",
            f"{zeta_dir}/Slaballocator.c",
        )
    )

    builder.Add(
        f"{zeta_dir}/staging_vector.h",
        {
            f"{FILE}",
            f"{zeta_dir}/seg_staging_vector_temp.h",
        },
        None
    )

    builder.Add(
        f"{zeta_dir}/staging_vector.c",
        {
            f"{FILE}",
            f"{zeta_dir}/seg_staging_vector_temp.c",
        },
        None
    )

    builder.Add(
        f"{zeta_build_dir}/staging_vector.o",
        {
            f"{FILE}",
            f"{zeta_dir}/staging_vector.c",
        },
        lambda: compiler.c_to_obj(
            f"{zeta_build_dir}/staging_vector.o",
            f"{zeta_dir}/staging_vector.c",
        )
    )

    builder.Add(
        f"{zeta_dir}/tree_allocator.h",
        {
            f"{FILE}",
            f"{zeta_dir}/allocator.h",
            f"{zeta_dir}/mem_check_utils.h",
            f"{zeta_dir}/ord_rb_linked_list_node.h",
            f"{zeta_dir}/ord_rb_tree_node.h",
        },
        None
    )

    builder.Add(
        f"{zeta_dir}/tree_allocator.c",
        {
            f"{FILE}",
            f"{zeta_dir}/tree_allocator.h",
            f"{zeta_dir}/debugger.h",
            f"{zeta_dir}/rbtree.h",
            f"{zeta_dir}/utils.h",
        },
        None
    )

    builder.Add(
        f"{zeta_build_dir}/tree_allocator.o",
        {
            f"{FILE}",
            f"{zeta_dir}/tree_allocator.c",
        },
        lambda: compiler.c_to_obj(
            f"{zeta_build_dir}/tree_allocator.o",
            f"{zeta_dir}/tree_allocator.c",
        )
    )

    builder.Add(
        f"{zeta_dir}/utf8.h",
        {
            f"{FILE}",
            f"{zeta_dir}/define.h",
        },
        None
    )

    builder.Add(
        f"{zeta_dir}/utf8.c",
        {
            f"{FILE}",
            f"{zeta_dir}/utf8.h",
            f"{zeta_dir}/debugger.h",
            f"{zeta_dir}/utils.h",
        },
        None
    )

    builder.Add(
        f"{zeta_build_dir}/utf8.o",
        {
            f"{FILE}",
            f"{zeta_dir}/utf8.c",
        },
        lambda: compiler.c_to_obj(
            f"{zeta_build_dir}/utf8.o",
            f"{zeta_dir}/utf8.c",
        )
    )

    builder.Add(
        f"{zeta_dir}/utf16.h",
        {
            f"{FILE}",
            f"{zeta_dir}/define.h",
        },
        None
    )

    builder.Add(
        f"{zeta_dir}/utf16.c",
        {
            f"{FILE}",
            f"{zeta_dir}/utf16.h",
            f"{zeta_dir}/debugger.h",
            f"{zeta_dir}/utils.h",
        },
        None
    )

    builder.Add(
        f"{zeta_build_dir}/utf16.o",
        {
            f"{FILE}",
            f"{zeta_dir}/utf16.c",
        },
        lambda: compiler.c_to_obj(
            f"{zeta_build_dir}/utf16.o",
            f"{zeta_dir}/utf16.c",
        )
    )

    builder.Add(
        f"{zeta_dir}/utils.h",
        {
            f"{FILE}",
            f"{zeta_dir}/define.h",
        },
        None
    )

    builder.Add(
        f"{zeta_dir}/utils.c",
        {
            f"{FILE}",
            f"{zeta_dir}/utils.h",
            f"{zeta_dir}/debugger.h",
        },
        None
    )

    builder.Add(
        f"{zeta_build_dir}/utils.o",
        {
            f"{FILE}",
            f"{zeta_dir}/utils.c",
        },
        lambda: compiler.c_to_obj(
            f"{zeta_build_dir}/utils.o",
            f"{zeta_dir}/utils.c",
        )
    )
