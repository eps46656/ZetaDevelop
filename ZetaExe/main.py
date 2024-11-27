import dataclasses
import pathlib
import typing

from typeguard import typechecked

from Builder import Builder
from LLVMCompiler import LLVMCompiler, LLVMCompilerConfig
from utils import ModeEnum, Target

FILE = pathlib.Path(__file__).absolute()
DIR = FILE.parents[0]


@dataclasses.dataclass
class Config:
    verbose: bool

    build_dir: object

    target: Target

    mode: ModeEnum

    enable_debug: bool

    enable_asan: bool

    c_include_dirs: typing.Iterable[object]
    cpp_include_dirs: typing.Iterable[object]

    zeta_dir: object
    zeta_build_dir: object


@typechecked
def AddDeps(builder: Builder, config: Config):
    compiler = LLVMCompiler(LLVMCompilerConfig(
        verbose=config.verbose,

        target=config.target,

        mode=config.mode,

        base_dir=DIR.parents[0],

        build_dir=config.build_dir,

        c_include_dirs={
            f"{config.zeta_dir}",
        },

        cpp_include_dirs={
            f"{config.zeta_dir}",
        },

        enable_debug=config.enable_debug,

        enable_asan=config.enable_asan,
    ))

    zeta_dir = pathlib.Path(config.zeta_dir)
    zeta_exe_dir = DIR

    zeta_build_dir = pathlib.Path(config.zeta_build_dir)
    zeta_exe_build_dir = pathlib.Path(config.build_dir)

    # --------------------------------------------------------------------------

    builder.Add(f"{FILE}", {}, None)

    builder.Add(
        f"{zeta_exe_dir}/assoc_cntr_utils.h",
        {
            f"{FILE}",
            f"{zeta_dir}/assoc_cntr.h",
            f"{zeta_dir}/debugger.h",
        },
        None
    )

    builder.Add(
        f"{zeta_exe_dir}/cpp_std_allocator.h",
        {
            f"{zeta_dir}/define.h",
        },
        None
    )

    builder.Add(
        f"{zeta_exe_dir}/ChainingMLTScheduler.h",
        {
            f"{FILE}",
            f"{zeta_dir}/MultiLevelTable.h",
            f"{zeta_dir}/ord_linked_list_node.h",
            f"{zeta_exe_dir}/zeta_pool_allocator.h",
        },
        None
    )

    builder.Add(
        f"{zeta_exe_dir}/ChainingStdRBTScheduler.h",
        {
            f"{FILE}",
            f"{zeta_dir}/ord_linked_list_node.h",
            f"{zeta_exe_dir}/cpp_std_allocator.h",
        },
        None
    )

    builder.Add(
        f"{zeta_exe_dir}/cas_alloc_utils.h",
        {
            f"{FILE}",
            f"{zeta_dir}/cascade_allocator.h",
            f"{zeta_dir}/memory.h",
            f"{zeta_exe_dir}/std_allocator.h",
        },
        None
    )

    builder.Add(
        f"{zeta_exe_dir}/circular_array_utils.h",
        {
            f"{FILE}",
            f"{zeta_dir}/circular_array.h",
            f"{zeta_exe_dir}/seq_cntr_utils.h",
        },
        None
    )

    builder.Add(
        f"{zeta_exe_dir}/debug_deque_utils.h",
        {
            f"{FILE}",
            f"{zeta_dir}/debug_deque.h",
            f"{zeta_exe_dir}/seq_cntr_utils.h",
        },
        None
    )

    builder.Add(
        f"{zeta_exe_dir}/debug_hash_table_utils.h",
        {
            f"{FILE}",
            f"{zeta_dir}/debug_hash_table.h",
            f"{zeta_dir}/debugger.h",
            f"{zeta_exe_dir}/assoc_cntr_utils.h",
        },
        None
    )

    builder.Add(
        f"{zeta_exe_dir}/dynamic_hash_table_utils.h",
        {
            f"{FILE}",
            f"{zeta_dir}/dynamic_hash_table.h",
            f"{zeta_exe_dir}/std_allocator.h",
        },
        None
    )

    builder.Add(
        f"{zeta_exe_dir}/dynamic_vector_utils.h",
        {
            f"{FILE}",
            f"{zeta_dir}/dynamic_vector.h",
            f"{zeta_dir}/mem_check_utils.h",
            f"{zeta_exe_dir}/seq_cntr_utils.h",
            f"{zeta_exe_dir}/std_allocator.h",
        },
        None
    )

    builder.Add(
        f"{zeta_exe_dir}/file_block_vector.h",
        {
            f"{zeta_dir}/block_vector.h",
        },
        None
    )

    builder.Add(
        f"{zeta_exe_dir}/PriorityQueue.h",
        {
            f"{FILE}",
            f"{zeta_dir}/debugger.h",
            f"{zeta_dir}/Groupbin_heap.h",
        },
        None
    )

    builder.Add(
        f"{zeta_exe_dir}/random.h",
        {
            f"{FILE}",
            f"{zeta_dir}/define.h",
        },
        None
    )

    builder.Add(
        f"{zeta_exe_dir}/staging_vector_utils.h",
        {
            f"{FILE}",
            f"{zeta_dir}/staging_vector.h",
            f"{zeta_exe_dir}/seq_cntr_utils.h",
            f"{zeta_exe_dir}/std_allocator.h",
        },
        None
    )

    builder.Add(
        f"{zeta_exe_dir}/std_allocator.h",
        {
            f"{zeta_dir}/allocator.h",
            f"{zeta_dir}/debug_hash_table.h",
            f"{zeta_exe_dir}/test_head.h",
        },
        None
    )

    builder.Add(
        f"{zeta_exe_dir}/test_head.h",
        {
            f"{zeta_dir}/define.h",
        },
        None
    )

    builder.Add(
        f"{zeta_exe_dir}/test_1.c",
        {
            f"{FILE}",
            f"{zeta_dir}/bin_tree.h",
            f"{zeta_dir}/ord_cnt_rb_tree_node.h",
            f"{zeta_dir}/rbtree.h",
            f"{zeta_dir}/random.h",
            f"{zeta_dir}/utils.h",
            f"{zeta_exe_dir}/test_head.h",
        },
        None
    )

    builder.Add(
        f"{zeta_exe_build_dir}/test_1.o",
        {
            f"{FILE}",
            f"{zeta_exe_dir}/test_1.c",
        },
        lambda: compiler.c_to_obj(
            f"{zeta_exe_build_dir}/test_1.o",
            f"{zeta_exe_dir}/test_1.c",
        )
    )

    builder.Add(
        f"{zeta_exe_dir}/test_binheap.cpp",
        {
            f"{FILE}",
            f"{zeta_dir}/bin_heap.h",
            f"{zeta_dir}/ord_cnt_rb_tree_node.h"
        },
        None
    )

    builder.Add(
        f"{zeta_exe_build_dir}/test_binheap.o",
        {
            f"{FILE}",
            f"{zeta_exe_dir}/test_binheap.cpp",
        },
        lambda: compiler.cpp_to_obj(
            f"{zeta_exe_build_dir}/test_binheap.o",
            f"{zeta_exe_dir}/test_binheap.cpp",
        )
    )

    builder.Add(
        f"{zeta_exe_dir}/test_gplist.c",
        {
            f"{FILE}",
            f"{zeta_dir}/bin_tree.h",
            f"{zeta_dir}/ord_cnt_rb_tree_node.h",
            f"{zeta_dir}/rbtree.h",
            f"{zeta_dir}/utils.h",
            f"{zeta_exe_dir}/test_head.h",
        },
        None
    )

    builder.Add(
        f"{zeta_exe_dir}/OkMap.h",
        {
            f"{FILE}",
        },
        None
    )

    builder.Add(
        f"{zeta_exe_dir}/OkMap.cpp",
        {
            f"{FILE}",
            f"{zeta_exe_dir}/OkMap.h",
        },
        None
    )

    builder.Add(
        f"{zeta_exe_dir}/gplist.c",
        {
            f"{FILE}",
            f"{zeta_dir}/bin_tree.h",
            f"{zeta_dir}/ord_cnt_rb_tree_node.h",
            f"{zeta_dir}/rbtree.h",
            f"{zeta_dir}/utils.h",
        },
        None
    )

    builder.Add(
        f"{zeta_exe_dir}/LeftistHeap.h",
        {
            f"{FILE}",
        },
        None
    )

    builder.Add(
        f"{zeta_exe_dir}/NaiveStdRBTScheduler.h",
        {
            f"{FILE}",
            f"{zeta_exe_dir}/cpp_std_allocator.h",
        },
        None
    )

    builder.Add(
        f"{zeta_exe_dir}/seg_vector_utils.h",
        {
            f"{FILE}",
            f"{zeta_dir}/seg_vector.h",
            f"{zeta_exe_dir}/seq_cntr_utils.h",
            f"{zeta_exe_dir}/std_allocator.h",
        },
        None
    )

    builder.Add(
        f"{zeta_exe_dir}/seq_cntr_utils.h",
        {
            f"{FILE}",
        },
        None
    )

    builder.Add(
        f"{zeta_exe_dir}/test_hashtable.c",
        {
            f"{FILE}",
            f"{zeta_dir}/utils.h",
            f"{zeta_exe_dir}/test_head.h",
        },
        None
    )

    builder.Add(
        f"{zeta_exe_dir}/test_fs.cpp",
        {
            f"{FILE}",
        },
        None
    )

    builder.Add(
        f"{zeta_exe_dir}/test_2.c",
        {
            f"{FILE}",
        },
        None
    )

    builder.Add(
        f"{zeta_exe_build_dir}/test_2.o",
        {
            f"{FILE}",
            f"{zeta_exe_dir}/test_2.c"
        },
        lambda: compiler.c_to_obj(
            f"{zeta_exe_build_dir}/test_2.o",
            f"{zeta_exe_dir}/test_2.c",
        )
    )

    builder.Add(
        f"{zeta_exe_build_dir}/test_2.exe",
        {
            f"{FILE}",
            f"{zeta_build_dir}/debug_str_pipe.o",
            f"{zeta_build_dir}/debugger.o",
            f"{zeta_build_dir}/logger.o",
            f"{zeta_build_dir}/io.o",
            f"{zeta_build_dir}/utils.o",
            f"{zeta_exe_build_dir}/test_2.o"
        },
        lambda: compiler.to_exe(
            f"{zeta_exe_build_dir}/test_2.exe",
            {
                f"{zeta_build_dir}/debug_str_pipe.o",
                f"{zeta_build_dir}/debugger.o",
                f"{zeta_build_dir}/logger.o",
                f"{zeta_build_dir}/io.o",
                f"{zeta_build_dir}/utils.o",
                f"{zeta_exe_build_dir}/test_2.o",
            }
        )
    )

    builder.Add(
        f"{zeta_exe_dir}/test_3.c",
        {
            f"{FILE}",

            f"{zeta_dir}/utils.h"
        },
        None
    )

    builder.Add(
        f"{zeta_exe_build_dir}/test_3.o",
        {
            f"{FILE}",
            f"{zeta_exe_dir}/test_3.c"
        },
        lambda: compiler.c_to_obj(
            f"{zeta_exe_build_dir}/test_3.o",
            f"{zeta_exe_dir}/test_3.c",
        )
    )

    builder.Add(
        f"{zeta_exe_build_dir}/test_3.exe",
        {
            f"{FILE}",
            f"{zeta_exe_build_dir}/test_3.o"
        },
        lambda: compiler.to_exe(
            f"{zeta_exe_build_dir}/test_3.exe",
            {
                f"{zeta_exe_build_dir}/test_3.o",
            }
        )
    )

    builder.Add(
        f"{zeta_exe_dir}/test_datetime.cpp",
        {
            f"{FILE}",
            f"{zeta_dir}/datetime.h",
        },
        None
    )

    builder.Add(
        f"{zeta_exe_build_dir}/test_datetime.o",
        {
            f"{FILE}",
            f"{zeta_exe_dir}/test_datetime.cpp",
        },
        lambda: compiler.cpp_to_obj(
            f"{zeta_exe_build_dir}/test_datetime.o",
            f"{zeta_exe_dir}/test_datetime.cpp",
        )
    )

    builder.Add(
        f"{zeta_exe_build_dir}/test_datetime.exe",
        {
            f"{FILE}",

            f"{zeta_build_dir}/datetime.o",

            f"{zeta_exe_build_dir}/test_datetime.o",
        },
        lambda: compiler.to_exe(
            f"{zeta_exe_build_dir}/test_datetime.exe",
            {
                f"{zeta_build_dir}/datetime.o",

                f"{zeta_exe_build_dir}/test_datetime.o",
            }
        )
    )

    builder.Add(
        f"{zeta_exe_dir}/test_dht.cpp",
        {
            f"{FILE}",

            f"{zeta_dir}/debug_deque.h",
            f"{zeta_dir}/debugger.h",
            f"{zeta_dir}/mem_check_utils.h",
            f"{zeta_exe_dir}/assoc_cntr_utils.h",
            f"{zeta_exe_dir}/debug_hash_table_utils.h",
            f"{zeta_exe_dir}/dynamic_hash_table_utils.h",
            f"{zeta_exe_dir}/pod_value.h",
            f"{zeta_exe_dir}/random.h",
            f"{zeta_exe_dir}/std_allocator.h",
            f"{zeta_exe_dir}/timer.h",
        },
        None
    )

    builder.Add(
        f"{zeta_exe_build_dir}/test_dht.o",
        {
            f"{FILE}",
            f"{zeta_exe_dir}/test_dht.cpp",
        },
        lambda: compiler.cpp_to_obj(
            f"{zeta_exe_build_dir}/test_dht.o",
            f"{zeta_exe_dir}/test_dht.cpp",
        )
    )

    builder.Add(
        f"{zeta_exe_build_dir}/test_dht.exe",
        {
            f"{FILE}",
            f"{zeta_build_dir}/assoc_cntr.o",
            f"{zeta_build_dir}/bin_tree.o",
            f"{zeta_build_dir}/debug_hash_table.o",
            f"{zeta_build_dir}/debug_str_pipe.o",
            f"{zeta_build_dir}/debugger.o",
            f"{zeta_build_dir}/dynamic_hash_table.o",
            f"{zeta_build_dir}/generic_hash_table.o",
            f"{zeta_build_dir}/io.o",
            f"{zeta_build_dir}/logger.o",
            f"{zeta_build_dir}/mem_check_utils.o",
            f"{zeta_build_dir}/ord_linked_list_node.o",
            f"{zeta_build_dir}/ord_rb_tree_node.o",
            f"{zeta_build_dir}/rbtree.o",
            f"{zeta_build_dir}/utils.o",
            f"{zeta_exe_build_dir}/timer.o",
            f"{zeta_exe_build_dir}/test_dht.o",
        },
        lambda: compiler.to_exe(
            f"{zeta_exe_build_dir}/test_dht.exe",

            {
                f"{zeta_build_dir}/assoc_cntr.o",
                f"{zeta_build_dir}/bin_tree.o",
                f"{zeta_build_dir}/debug_hash_table.o",
                f"{zeta_build_dir}/debug_str_pipe.o",
                f"{zeta_build_dir}/debugger.o",
                f"{zeta_build_dir}/dynamic_hash_table.o",
                f"{zeta_build_dir}/generic_hash_table.o",
                f"{zeta_build_dir}/io.o",
                f"{zeta_build_dir}/logger.o",
                f"{zeta_build_dir}/mem_check_utils.o",
                f"{zeta_build_dir}/ord_linked_list_node.o",
                f"{zeta_build_dir}/ord_rb_tree_node.o",
                f"{zeta_build_dir}/rbtree.o",
                f"{zeta_build_dir}/utils.o",
                f"{zeta_exe_build_dir}/timer.o",
                f"{zeta_exe_build_dir}/test_dht.o",
            }
        )
    )

    builder.Add(
        f"{zeta_exe_dir}/test_dv.cpp",
        {
            f"{FILE}",
            f"{zeta_dir}/dynamic_vector.h",
            f"{zeta_dir}/utils.h",
        },
        None
    )

    builder.Add(
        f"{zeta_exe_dir}/test_elf.cpp",
        {
            f"{FILE}",
        },
        None
    )

    builder.Add(
        f"{zeta_exe_dir}/test_fat32.cpp",
        {
            f"{FILE}",
            f"{zeta_dir}/disk_info.h",
            f"{zeta_dir}/disk_part_gpt.h",
            f"{zeta_dir}/disk_part_mbt.h",
        },
        None
    )

    builder.Add(
        f"{zeta_exe_dir}/test_jump.cpp",
        {
            f"{FILE}",
            f"{zeta_dir}/jump.h",
        },
        None
    )

    builder.Add(
        f"{zeta_exe_build_dir}/test_jump.o",
        {
            f"{FILE}",
            f"{zeta_exe_dir}/test_jump.cpp",
        },
        lambda: compiler.cpp_to_obj(
            f"{zeta_exe_build_dir}/test_jump.o",
            f"{zeta_exe_dir}/test_jump.cpp",
        )
    )

    builder.Add(
        f"{zeta_exe_build_dir}/test_jump.exe",
        {
            f"{FILE}",

            f"{zeta_build_dir}/debug_str_pipe.o",
            f"{zeta_build_dir}/debugger.o",
            f"{zeta_build_dir}/io.o",
            f"{zeta_build_dir}/jump.o",
            f"{zeta_build_dir}/logger.o",

            f"{zeta_exe_build_dir}/test_jump.o",
        },
        lambda: compiler.to_exe(
            f"{zeta_exe_build_dir}/test_jump.exe",
            {
                f"{zeta_build_dir}/debug_str_pipe.o",
                f"{zeta_build_dir}/debugger.o",
                f"{zeta_build_dir}/io.o",
                f"{zeta_build_dir}/jump.o",
                f"{zeta_build_dir}/logger.o",

                f"{zeta_exe_build_dir}/test_jump.o",
            }
        )
    )

    builder.Add(
        f"{zeta_exe_dir}/test_tree_alloc.cpp",
        {
            f"{FILE}",
            f"{zeta_dir}/debug_hash_table.h",
            f"{zeta_dir}/debugger.h",
            f"{zeta_dir}/tree_allocator.h",
        },
        None
    )

    builder.Add(
        f"{zeta_exe_build_dir}/test_tree_alloc.o",
        {
            f"{FILE}",
            f"{zeta_exe_dir}/test_tree_alloc.cpp",
        },
        lambda: compiler.cpp_to_obj(
            f"{zeta_exe_build_dir}/test_tree_alloc.o",
            f"{zeta_exe_dir}/test_tree_alloc.cpp",
        )
    )

    builder.Add(
        f"{zeta_exe_build_dir}/test_tree_alloc.exe",
        {
            f"{FILE}",

            f"{zeta_build_dir}/allocator.o",
            f"{zeta_build_dir}/assoc_cntr.o",
            f"{zeta_build_dir}/bin_tree.o",
            f"{zeta_build_dir}/debug_hash_table.o",
            f"{zeta_build_dir}/debug_hash_table.o",
            f"{zeta_build_dir}/debug_str_pipe.o",
            f"{zeta_build_dir}/debugger.o",
            f"{zeta_build_dir}/io.o",
            f"{zeta_build_dir}/logger.o",
            f"{zeta_build_dir}/mem_check_utils.o",
            f"{zeta_build_dir}/ord_rb_linked_list_node.o",
            f"{zeta_build_dir}/ord_rb_tree_node.o",
            f"{zeta_build_dir}/rbtree.o",
            f"{zeta_build_dir}/tree_allocator.o",
            f"{zeta_build_dir}/utils.o",

            f"{zeta_exe_build_dir}/test_tree_alloc.o",
        },
        lambda: compiler.to_exe(
            f"{zeta_exe_build_dir}/test_tree_alloc.exe",
            {
                f"{zeta_build_dir}/allocator.o",
                f"{zeta_build_dir}/assoc_cntr.o",
                f"{zeta_build_dir}/bin_tree.o",
                f"{zeta_build_dir}/debug_hash_table.o",
                f"{zeta_build_dir}/debug_hash_table.o",
                f"{zeta_build_dir}/debug_str_pipe.o",
                f"{zeta_build_dir}/debugger.o",
                f"{zeta_build_dir}/io.o",
                f"{zeta_build_dir}/logger.o",
                f"{zeta_build_dir}/mem_check_utils.o",
                f"{zeta_build_dir}/ord_rb_linked_list_node.o",
                f"{zeta_build_dir}/ord_rb_tree_node.o",
                f"{zeta_build_dir}/rbtree.o",
                f"{zeta_build_dir}/tree_allocator.o",
                f"{zeta_build_dir}/utils.o",

                f"{zeta_exe_build_dir}/test_tree_alloc.o",
            }
        )
    )

    builder.Add(
        f"{zeta_exe_dir}/test_usb.cpp",
        {
            f"{FILE}",
            f"{zeta_dir}/disk_info.h",
            f"{zeta_dir}/disk_part_gpt.h",
            f"{zeta_dir}/disk_part_mbt.h",
            f"{zeta_dir}/define.h"
        },
        None
    )

    builder.Add(
        f"{zeta_exe_dir}/test_utf8.c",
        {
            f"{FILE}",

            f"{zeta_dir}/debugger.h",
            f"{zeta_dir}/utf8.h",
        },
        None
    )

    builder.Add(
        f"{zeta_exe_build_dir}/test_utf8.o",
        {
            f"{FILE}",

            f"{zeta_exe_dir}/test_utf8.c",

            f"{zeta_dir}/utf8.h",
        },

        lambda: compiler.cpp_to_obj(
            f"{zeta_exe_build_dir}/test_utf8.o",
            f"{zeta_exe_dir}/test_utf8.c",
        )
    )

    builder.Add(
        f"{zeta_exe_build_dir}/test_utf8.exe",
        {
            f"{FILE}",

            f"{zeta_build_dir}/debugger.o",
            f"{zeta_build_dir}/debug_str_pipe.o",
            f"{zeta_build_dir}/logger.o",
            f"{zeta_build_dir}/io.o",
            f"{zeta_build_dir}/utf8.o",
            f"{zeta_build_dir}/utils.o",

            f"{zeta_exe_build_dir}/test_utf8.o",
        },
        lambda: compiler.to_exe(
            f"{zeta_exe_build_dir}/test_utf8.exe",
            {
                f"{zeta_build_dir}/debugger.o",
                f"{zeta_build_dir}/debug_str_pipe.o",
                f"{zeta_build_dir}/logger.o",
                f"{zeta_build_dir}/io.o",
                f"{zeta_build_dir}/utf8.o",
                f"{zeta_build_dir}/utils.o",

                f"{zeta_exe_build_dir}/test_utf8.o",
            }
        )
    )

    builder.Add(
        f"{zeta_exe_dir}/test_utf16.c",
        {
            f"{FILE}",

            f"{zeta_dir}/debugger.h",
            f"{zeta_dir}/utf16.h",
        },
        None
    )

    builder.Add(
        f"{zeta_exe_build_dir}/test_utf16.o",
        {
            f"{FILE}",

            f"{zeta_exe_dir}/test_utf16.c",

            f"{zeta_dir}/utf16.h",
        },

        lambda: compiler.cpp_to_obj(
            f"{zeta_exe_build_dir}/test_utf16.o",
            f"{zeta_exe_dir}/test_utf16.c",
        )
    )

    builder.Add(
        f"{zeta_exe_build_dir}/test_utf16.exe",
        {
            f"{FILE}",

            f"{zeta_build_dir}/debugger.o",
            f"{zeta_build_dir}/debug_str_pipe.o",
            f"{zeta_build_dir}/logger.o",
            f"{zeta_build_dir}/io.o",
            f"{zeta_build_dir}/utf16.o",
            f"{zeta_build_dir}/utils.o",

            f"{zeta_exe_build_dir}/test_utf16.o",
        },
        lambda: compiler.to_exe(
            f"{zeta_exe_build_dir}/test_utf16.exe",
            {
                f"{zeta_build_dir}/debugger.o",
                f"{zeta_build_dir}/debug_str_pipe.o",
                f"{zeta_build_dir}/logger.o",
                f"{zeta_build_dir}/io.o",
                f"{zeta_build_dir}/utf16.o",
                f"{zeta_build_dir}/utils.o",

                f"{zeta_exe_build_dir}/test_utf16.o",
            }
        )
    )

    builder.Add(
        f"{zeta_exe_dir}/test_segvec.cpp",
        {
            f"{FILE}",
            f"{zeta_dir}/seg_vector.h",
            f"{zeta_exe_dir}/std_allocator.h"
        },
        None
    )

    builder.Add(
        f"{zeta_exe_build_dir}/test_segvec.o",
        {
            f"{FILE}",
            f"{zeta_exe_dir}/test_segvec.cpp",
        },
        lambda: compiler.cpp_to_obj(
            f"{zeta_exe_build_dir}/test_segvec.o",
            f"{zeta_exe_dir}/test_segvec.cpp",
        )
    )

    builder.Add(
        f"{zeta_exe_build_dir}/test_segvec.exe",
        {
            f"{FILE}",

            f"{zeta_build_dir}/seg_vector.o",
            f"{zeta_build_dir}/bin_tree.o",
            f"{zeta_build_dir}/circular_array.o",
            f"{zeta_build_dir}/debug_deque.o",
            f"{zeta_build_dir}/debug_hash_table.o",
            f"{zeta_build_dir}/rbtree.o",
            f"{zeta_build_dir}/ord_cnt_rb_tree_node.o",
            f"{zeta_build_dir}/utils.o",

            f"{zeta_exe_build_dir}/test_segvec.o",
        },
        lambda: compiler.to_exe(
            f"{zeta_exe_build_dir}/test_segvec.exe",
            {
                f"{zeta_build_dir}/seg_vector.o",
                f"{zeta_build_dir}/bin_tree.o",
                f"{zeta_build_dir}/circular_array.o",
                f"{zeta_build_dir}/debug_deque.o",
                f"{zeta_build_dir}/debug_hash_table.o",
                f"{zeta_build_dir}/rbtree.o",
                f"{zeta_build_dir}/ord_cnt_rb_tree_node.o",
                f"{zeta_build_dir}/utils.o",

                f"{zeta_exe_build_dir}/test_segvec.o",
            }
        )
    )

    builder.Add(
        f"{zeta_exe_dir}/test_segvec2.cpp",
        {
            f"{FILE}",
            f"{zeta_dir}/seg_vector.h",
            f"{zeta_exe_dir}/std_allocator.h"
        },
        None
    )

    builder.Add(
        f"{zeta_exe_build_dir}/test_segvec2.o",
        {
            f"{FILE}",
            f"{zeta_exe_dir}/test_segvec2.cpp",
        },
        lambda: compiler.cpp_to_obj(
            f"{zeta_exe_build_dir}/test_segvec2.o",
            f"{zeta_exe_dir}/test_segvec2.cpp",
        )
    )

    builder.Add(
        f"{zeta_exe_build_dir}/test_segvec2.exe",
        {
            f"{FILE}",

            f"{zeta_build_dir}/bin_tree.o",
            f"{zeta_build_dir}/circular_array.o",
            f"{zeta_build_dir}/debug_deque.o",
            f"{zeta_build_dir}/debug_hash_table.o",
            f"{zeta_build_dir}/ord_cnt_rb_tree_node.o",
            f"{zeta_build_dir}/rbtree.o",
            f"{zeta_build_dir}/seg_vector.o",
            f"{zeta_build_dir}/utils.o",

            f"{zeta_exe_build_dir}/test_segvec2.o",
        },
        lambda: compiler.to_exe(
            f"{zeta_exe_build_dir}/test_segvec2.exe",
            {
                f"{zeta_build_dir}/bin_tree.o",
                f"{zeta_build_dir}/circular_array.o",
                f"{zeta_build_dir}/debug_deque.o",
                f"{zeta_build_dir}/debug_hash_table.o",
                f"{zeta_build_dir}/ord_cnt_rb_tree_node.o",
                f"{zeta_build_dir}/rbtree.o",
                f"{zeta_build_dir}/seg_vector.o",
                f"{zeta_build_dir}/utils.o",

                f"{zeta_exe_build_dir}/test_segvec2.o",
            }
        )
    )

    builder.Add(
        f"{zeta_exe_dir}/test_segvec_speed.cpp",
        {
            f"{FILE}",
            f"{zeta_dir}/seg_vector.h",
            f"{zeta_exe_dir}/std_allocator.h"
        },
        None
    )

    builder.Add(
        f"{zeta_exe_build_dir}/test_segvec_speed.o",
        {
            f"{FILE}",
            f"{zeta_exe_dir}/test_segvec_speed.cpp",
        },
        lambda: compiler.cpp_to_obj(
            f"{zeta_exe_build_dir}/test_segvec_speed.o",
            f"{zeta_exe_dir}/test_segvec_speed.cpp",
        )
    )

    builder.Add(
        f"{zeta_exe_build_dir}/test_segvec_speed.exe",
        {
            f"{FILE}",

            f"{zeta_build_dir}/seg_vector.o",
            f"{zeta_build_dir}/bin_tree.o",
            f"{zeta_build_dir}/circular_array.o",
            f"{zeta_build_dir}/debug_deque.o",
            f"{zeta_build_dir}/debug_hash_table.o",
            f"{zeta_build_dir}/rbtree.o",
            f"{zeta_build_dir}/ord_cnt_rb_tree_node.o",
            f"{zeta_build_dir}/utils.o",

            f"{zeta_exe_build_dir}/test_segvec_speed.o",
        },
        lambda: compiler.to_exe(
            f"{zeta_exe_build_dir}/test_segvec_speed.exe",
            {
                f"{zeta_build_dir}/seg_vector.o",
                f"{zeta_build_dir}/bin_tree.o",
                f"{zeta_build_dir}/circular_array.o",
                f"{zeta_build_dir}/debug_deque.o",
                f"{zeta_build_dir}/debug_hash_table.o",
                f"{zeta_build_dir}/rbtree.o",
                f"{zeta_build_dir}/ord_cnt_rb_tree_node.o",
                f"{zeta_build_dir}/utils.o",

                f"{zeta_exe_build_dir}/test_segvec_speed.o",
            }
        )
    )

    builder.Add(
        f"{zeta_exe_dir}/test_segvec2_speed.cpp",
        {
            f"{FILE}",
            f"{zeta_dir}/seg_vector.h",
            f"{zeta_exe_dir}/std_allocator.h"
        },
        None
    )

    builder.Add(
        f"{zeta_exe_build_dir}/test_segvec2_speed.o",
        {
            f"{FILE}",
            f"{zeta_exe_dir}/test_segvec2_speed.cpp",
        },
        lambda: compiler.cpp_to_obj(
            f"{zeta_exe_build_dir}/test_segvec2_speed.o",
            f"{zeta_exe_dir}/test_segvec2_speed.cpp",
        )
    )

    builder.Add(
        f"{zeta_exe_build_dir}/test_segvec2_speed.exe",
        {
            f"{FILE}",

            f"{zeta_build_dir}/seg_vector.o",
            f"{zeta_build_dir}/bin_tree.o",
            f"{zeta_build_dir}/circular_array.o",
            f"{zeta_build_dir}/debug_hash_table.o",
            f"{zeta_build_dir}/rbtree.o",
            f"{zeta_build_dir}/ord_cnt_rb_tree_node.o",
            f"{zeta_build_dir}/utils.o",

            f"{zeta_exe_build_dir}/test_segvec2_speed.o",
        },
        lambda: compiler.to_exe(
            f"{zeta_exe_build_dir}/test_segvec2_speed.exe",
            {
                f"{zeta_build_dir}/seg_vector.o",
                f"{zeta_build_dir}/bin_tree.o",
                f"{zeta_build_dir}/circular_array.o",
                f"{zeta_build_dir}/debug_hash_table.o",
                f"{zeta_build_dir}/rbtree.o",
                f"{zeta_build_dir}/ord_cnt_rb_tree_node.o",
                f"{zeta_build_dir}/utils.o",

                f"{zeta_exe_build_dir}/test_segvec2_speed.o",
            }
        )
    )

    builder.Add(
        f"{zeta_exe_dir}/test_seqcntr.cpp",
        {
            f"{FILE}",
            f"{zeta_dir}/debug_deque.h",
            f"{zeta_dir}/circular_array.h",
            f"{zeta_dir}/seg_vector.h",
            f"{zeta_exe_dir}/std_allocator.h"
        },
        None
    )

    builder.Add(
        f"{zeta_exe_build_dir}/test_seqcntr.o",
        {
            f"{FILE}",
            f"{zeta_exe_dir}/test_seqcntr.cpp",
        },
        lambda: compiler.cpp_to_obj(
            f"{zeta_exe_build_dir}/test_seqcntr.o",
            f"{zeta_exe_dir}/test_seqcntr.cpp",
        )
    )

    builder.Add(
        f"{zeta_exe_build_dir}/test_seqcntr.exe",
        {
            f"{FILE}",

            f"{zeta_build_dir}/bin_tree.o",
            f"{zeta_build_dir}/circular_array.o",
            f"{zeta_build_dir}/debug_deque.o",
            f"{zeta_build_dir}/debug_hash_table.o",
            f"{zeta_build_dir}/rbtree.o",
            f"{zeta_build_dir}/ord_cnt_rb_tree_node.o",
            f"{zeta_build_dir}/seg_vector.o",
            f"{zeta_build_dir}/utils.o",

            f"{zeta_exe_build_dir}/test_seqcntr.o",
        },
        lambda: compiler.to_exe(
            f"{zeta_exe_build_dir}/test_seqcntr.exe",
            {
                f"{zeta_build_dir}/bin_tree.o",
                f"{zeta_build_dir}/circular_array.o",
                f"{zeta_build_dir}/debug_deque.o",
                f"{zeta_build_dir}/debug_hash_table.o",
                f"{zeta_build_dir}/rbtree.o",
                f"{zeta_build_dir}/ord_cnt_rb_tree_node.o",
                f"{zeta_build_dir}/seg_vector.o",
                f"{zeta_build_dir}/utils.o",

                f"{zeta_exe_build_dir}/test_seqcntr.o",
            }
        )
    )

    builder.Add(
        f"{zeta_exe_dir}/test_stagevec.cpp",
        {
            f"{FILE}",
            f"{zeta_dir}/debug_deque.h",
            f"{zeta_dir}/debugger.h",
            f"{zeta_dir}/logger.h",
            f"{zeta_exe_dir}/cas_alloc_utils.h",
            f"{zeta_exe_dir}/circular_array_utils.h",
            f"{zeta_exe_dir}/debug_deque_utils.h",
            f"{zeta_exe_dir}/dynamic_vector_utils.h",
            f"{zeta_exe_dir}/pod_value.h",
            f"{zeta_exe_dir}/random.h",
            f"{zeta_exe_dir}/seg_vector_utils.h",
            f"{zeta_exe_dir}/seq_cntr_utils.h",
            f"{zeta_exe_dir}/staging_vector_utils.h",
            f"{zeta_exe_dir}/timer.h",
        },
        None
    )

    builder.Add(
        f"{zeta_exe_build_dir}/test_stagevec.o",
        {
            f"{FILE}",
            f"{zeta_exe_dir}/test_stagevec.cpp",
        },
        lambda: compiler.cpp_to_obj(
            f"{zeta_exe_build_dir}/test_stagevec.o",
            f"{zeta_exe_dir}/test_stagevec.cpp",
        )
    )

    builder.Add(
        f"{zeta_exe_build_dir}/test_stagevec.s",
        {
            f"{FILE}",
            f"{zeta_exe_build_dir}/test_stagevec.o",
        },
        lambda: compiler.ll_to_asm(
            f"{zeta_exe_build_dir}/test_stagevec.s",
            f"{zeta_exe_build_dir}/test_stagevec.o",
        )
    )

    builder.Add(
        f"{zeta_exe_build_dir}/test_stagevec.exe",
        {
            f"{FILE}",
            f"{zeta_build_dir}/algorithm.o",
            f"{zeta_build_dir}/allocator.o",
            f"{zeta_build_dir}/assoc_cntr.o",
            f"{zeta_build_dir}/bin_tree.o",
            f"{zeta_build_dir}/cascade_allocator.o",
            f"{zeta_build_dir}/circular_array.o",
            f"{zeta_build_dir}/debug_deque.o",
            f"{zeta_build_dir}/debug_hash_table.o",
            f"{zeta_build_dir}/debug_str_pipe.o",
            f"{zeta_build_dir}/debugger.o",
            f"{zeta_build_dir}/dummy_vector.o",
            f"{zeta_build_dir}/dynamic_hash_table.o",
            f"{zeta_build_dir}/dynamic_vector.o",
            f"{zeta_build_dir}/generic_hash_table.o",
            f"{zeta_build_dir}/io.o",
            f"{zeta_build_dir}/logger.o",
            f"{zeta_build_dir}/mem_check_utils.o",
            f"{zeta_build_dir}/memory.o",
            f"{zeta_build_dir}/ord_cnt_3rb_tree_node.o",
            f"{zeta_build_dir}/ord_cnt_rb_tree_node.o",
            f"{zeta_build_dir}/ord_linked_list_node.o",
            f"{zeta_build_dir}/ord_rb_tree_node.o",
            f"{zeta_build_dir}/pool_allocator.o",
            f"{zeta_build_dir}/rbtree.o",
            f"{zeta_build_dir}/seg_utils.o",
            f"{zeta_build_dir}/seg_vector.o",
            f"{zeta_build_dir}/seq_cntr.o",
            f"{zeta_build_dir}/staging_vector.o",
            f"{zeta_build_dir}/utils.o",
            f"{zeta_exe_build_dir}/test_stagevec.o",
            f"{zeta_exe_build_dir}/timer.o",
        },
        lambda: compiler.to_exe(
            f"{zeta_exe_build_dir}/test_stagevec.exe",

            {
                f"{zeta_build_dir}/algorithm.o",
                f"{zeta_build_dir}/allocator.o",
                f"{zeta_build_dir}/assoc_cntr.o",
                f"{zeta_build_dir}/bin_tree.o",
                f"{zeta_build_dir}/cascade_allocator.o",
                f"{zeta_build_dir}/circular_array.o",
                f"{zeta_build_dir}/debug_deque.o",
                f"{zeta_build_dir}/debug_hash_table.o",
                f"{zeta_build_dir}/debug_str_pipe.o",
                f"{zeta_build_dir}/debugger.o",
                f"{zeta_build_dir}/dummy_vector.o",
                f"{zeta_build_dir}/dynamic_hash_table.o",
                f"{zeta_build_dir}/dynamic_vector.o",
                f"{zeta_build_dir}/generic_hash_table.o",
                f"{zeta_build_dir}/io.o",
                f"{zeta_build_dir}/logger.o",
                f"{zeta_build_dir}/mem_check_utils.o",
                f"{zeta_build_dir}/memory.o",
                f"{zeta_build_dir}/ord_cnt_3rb_tree_node.o",
                f"{zeta_build_dir}/ord_cnt_rb_tree_node.o",
                f"{zeta_build_dir}/ord_linked_list_node.o",
                f"{zeta_build_dir}/ord_rb_tree_node.o",
                f"{zeta_build_dir}/pool_allocator.o",
                f"{zeta_build_dir}/rbtree.o",
                f"{zeta_build_dir}/seg_utils.o",
                f"{zeta_build_dir}/seg_vector.o",
                f"{zeta_build_dir}/seq_cntr.o",
                f"{zeta_build_dir}/staging_vector.o",
                f"{zeta_build_dir}/utils.o",
                f"{zeta_exe_build_dir}/test_stagevec.o",
                f"{zeta_exe_build_dir}/timer.o",
            }
        )
    )

    builder.Add(
        f"{zeta_exe_dir}/test_stagevec_speed.cpp",
        {
            f"{FILE}",
            f"{zeta_dir}/circular_array.h",
            f"{zeta_dir}/debug_deque.h",
            f"{zeta_dir}/debugger.h",
            f"{zeta_dir}/logger.h",
            f"{zeta_dir}/seg_vector.h",
            f"{zeta_dir}/staging_vector.h",
            f"{zeta_exe_dir}/std_allocator.h",
        },
        None
    )

    builder.Add(
        f"{zeta_exe_build_dir}/test_stagevec_speed.o",
        {
            f"{FILE}",
            f"{zeta_exe_dir}/test_stagevec_speed.cpp",
        },
        lambda: compiler.cpp_to_obj(
            f"{zeta_exe_build_dir}/test_stagevec_speed.o",
            f"{zeta_exe_dir}/test_stagevec_speed.cpp",
        )
    )

    builder.Add(
        f"{zeta_exe_build_dir}/test_stagevec_speed.exe",
        {
            f"{FILE}",
            f"{zeta_build_dir}/bin_tree.o",
            f"{zeta_build_dir}/circular_array.o",
            f"{zeta_build_dir}/debug_deque.o",
            f"{zeta_build_dir}/debug_hash_table.o",
            f"{zeta_build_dir}/debug_str_pipe.o",
            f"{zeta_build_dir}/debugger.o",
            f"{zeta_build_dir}/dummy_vector.o",
            f"{zeta_build_dir}/io.o",
            f"{zeta_build_dir}/logger.o",
            f"{zeta_build_dir}/ord_cnt_3rb_tree_node.o",
            f"{zeta_build_dir}/ord_cnt_rb_tree_node.o",
            f"{zeta_build_dir}/rbtree.o",
            f"{zeta_build_dir}/seg_utils.o",
            f"{zeta_build_dir}/seq_cntr.o",
            f"{zeta_build_dir}/staging_vector.o",
            f"{zeta_build_dir}/utils.o",
            f"{zeta_exe_build_dir}/test_stagevec_speed.o",
        },
        lambda: compiler.to_exe(
            f"{zeta_exe_build_dir}/test_stagevec_speed.exe",

            {
                f"{zeta_build_dir}/bin_tree.o",
                f"{zeta_build_dir}/circular_array.o",
                f"{zeta_build_dir}/debug_deque.o",
                f"{zeta_build_dir}/debug_hash_table.o",
                f"{zeta_build_dir}/debug_str_pipe.o",
                f"{zeta_build_dir}/debugger.o",
                f"{zeta_build_dir}/dummy_vector.o",
                f"{zeta_build_dir}/io.o",
                f"{zeta_build_dir}/logger.o",
                f"{zeta_build_dir}/ord_cnt_3rb_tree_node.o",
                f"{zeta_build_dir}/ord_cnt_rb_tree_node.o",
                f"{zeta_build_dir}/rbtree.o",
                f"{zeta_build_dir}/seg_utils.o",
                f"{zeta_build_dir}/seq_cntr.o",
                f"{zeta_build_dir}/staging_vector.o",
                f"{zeta_build_dir}/utils.o",
                f"{zeta_exe_build_dir}/test_stagevec_speed.o",
            }
        )
    )

    builder.Add(
        f"{zeta_exe_dir}/test_stdio.c",
        {
            f"{FILE}",
            f"{zeta_dir}/define.h",
            f"{zeta_dir}/pipe.h",
        },
        None
    )

    builder.Add(
        f"{zeta_exe_dir}/FileIO.s",
        {
            f"{FILE}",
        },
        None
    )

    builder.Add(
        f"{zeta_exe_dir}/test_io.c",
        {
            f"{FILE}",
        },
        None
    )

    builder.Add(
        f"{zeta_exe_dir}/test_kmp.cpp",
        {
            f"{FILE}",

            f"{zeta_dir}/define.h"
        },
        None
    )

    builder.Add(
        f"{zeta_exe_build_dir}/test_kmp.o",
        {
            f"{FILE}",
            f"{zeta_exe_dir}/test_kmp.cpp"
        },
        lambda: compiler.cpp_to_obj(
            f"{zeta_exe_build_dir}/test_kmp.o",
            f"{zeta_exe_dir}/test_kmp.cpp",
        )
    )

    builder.Add(
        f"{zeta_exe_build_dir}/test_kmp.exe",
        {
            f"{FILE}",
            f"{zeta_exe_build_dir}/test_kmp.o"
        },
        lambda: compiler.to_exe(
            f"{zeta_exe_build_dir}/test_kmp.exe",
            {
                f"{zeta_exe_build_dir}/test_kmp.o",
            }
        )
    )

    builder.Add(
        f"{zeta_exe_dir}/test_LeftistHeap.cpp",
        {
            f"{FILE}",
            f"{zeta_exe_dir}/LeftistHeap.h",
        },
        None
    )

    builder.Add(
        f"{zeta_exe_build_dir}/test_LeftistHeap.o",
        {
            f"{FILE}",
            f"{zeta_exe_dir}/test_LeftistHeap.cpp",
        },
        lambda: compiler.cpp_to_obj(
            f"{zeta_exe_build_dir}/test_LeftistHeap.o",
            f"{zeta_exe_dir}/test_LeftistHeap.cpp",
        )
    )

    builder.Add(
        f"{zeta_exe_build_dir}/test_LeftistHeap.exe",
        {
            f"{FILE}",
            f"{zeta_exe_build_dir}/test_LeftistHeap.o",
        },
        lambda: compiler.to_exe(
            f"{zeta_exe_build_dir}/test_LeftistHeap.exe",
            {
                f"{zeta_exe_build_dir}/test_LeftistHeap.o",
            }
        )
    )

    builder.Add(
        f"{zeta_exe_dir}/test_log.cpp",
        {
            f"{FILE}",
            f"{zeta_dir}/utils.h",
        },
        None
    )

    builder.Add(
        f"{zeta_exe_build_dir}/test_log.o",
        {
            f"{FILE}",
            f"{zeta_exe_dir}/test_log.cpp",
        },
        lambda: compiler.cpp_to_obj(
            f"{zeta_exe_build_dir}/test_log.o",
            f"{zeta_exe_dir}/test_log.cpp",
        )
    )

    builder.Add(
        f"{zeta_exe_build_dir}/test_log.exe",
        {
            f"{FILE}",

            f"{zeta_build_dir}/debug_str_pipe.o",
            f"{zeta_build_dir}/debugger.o",
            f"{zeta_build_dir}/logger.o",
            f"{zeta_build_dir}/io.o",
            f"{zeta_build_dir}/utils.o",

            f"{zeta_exe_build_dir}/test_log.o",
        },
        lambda: compiler.to_exe(
            f"{zeta_exe_build_dir}/test_log.exe",

            {
                f"{zeta_build_dir}/debug_str_pipe.o",
                f"{zeta_build_dir}/debugger.o",
                f"{zeta_build_dir}/logger.o",
                f"{zeta_build_dir}/io.o",
                f"{zeta_build_dir}/utils.o",

                f"{zeta_exe_build_dir}/test_log.o",
            }
        )
    )

    builder.Add(
        f"{zeta_exe_dir}/test_lru.cpp",
        {
            f"{FILE}",
            f"{zeta_dir}/lru_cache_manager.h",
        },
        None
    )

    builder.Add(
        f"{zeta_exe_dir}/test_lrucm.cpp",
        {
            f"{FILE}",
            f"{zeta_dir}/dummy_block_vector.h",
            f"{zeta_dir}/dummy_cache_manager.h",
            f"{zeta_dir}/lru_cache_manager.h",
            f"{zeta_dir}/rbtree.h",
            f"{zeta_exe_dir}/file_block_vector.h",
            f"{zeta_exe_dir}/std_allocator.h",
        },
        None
    )

    builder.Add(
        f"{zeta_exe_build_dir}/test_lrucm.o",
        {
            f"{FILE}",
            f"{zeta_exe_dir}/test_lrucm.cpp",
        },
        lambda: compiler.cpp_to_obj(
            f"{zeta_exe_build_dir}/test_lrucm.o",
            f"{zeta_exe_dir}/test_lrucm.cpp",
        )
    )

    builder.Add(
        f"{zeta_exe_build_dir}/test_lrucm.exe",
        {
            f"{FILE}",

            f"{zeta_build_dir}/allocator.o",
            f"{zeta_build_dir}/bin_tree.o",
            f"{zeta_build_dir}/block_vector.o",
            f"{zeta_build_dir}/debug_hash_table.o",
            f"{zeta_build_dir}/dummy_block_vector.o",
            f"{zeta_build_dir}/dummy_cache_manager.o",
            f"{zeta_build_dir}/lru_cache_manager.o",
            f"{zeta_build_dir}/ord_linked_list_node.o",
            f"{zeta_build_dir}/ord_rb_linked_list_node.o",
            f"{zeta_build_dir}/ord_rb_tree_node.o",
            f"{zeta_build_dir}/rbtree.o",
            f"{zeta_build_dir}/utils.o",

            f"{zeta_exe_build_dir}/test_lrucm.o",
        },
        lambda: compiler.to_exe(
            f"{zeta_exe_build_dir}/test_lrucm.exe",
            {
                f"{zeta_build_dir}/allocator.o",
                f"{zeta_build_dir}/bin_tree.o",
                f"{zeta_build_dir}/block_vector.o",
                f"{zeta_build_dir}/debug_hash_table.o",
                f"{zeta_build_dir}/dummy_block_vector.o",
                f"{zeta_build_dir}/dummy_cache_manager.o",
                f"{zeta_build_dir}/lru_cache_manager.o",
                f"{zeta_build_dir}/ord_linked_list_node.o",
                f"{zeta_build_dir}/ord_rb_linked_list_node.o",
                f"{zeta_build_dir}/ord_rb_tree_node.o",
                f"{zeta_build_dir}/rbtree.o",
                f"{zeta_build_dir}/utils.o",

                f"{zeta_exe_build_dir}/test_lrucm.o",
            }
        )
    )

    builder.Add(
        f"{zeta_exe_dir}/test_lsb.cpp",
        {
            f"{FILE}",
            f"{zeta_dir}/utils.h",
        },
        None
    )

    builder.Add(
        f"{zeta_exe_dir}/test_sort.cpp",
        {
            f"{FILE}",
            f"{zeta_dir}/algorithm.h",
            f"{zeta_dir}/cascade_allocator.h",
            f"{zeta_dir}/debugger.h",
            f"{zeta_dir}/memory.h",
            f"{zeta_exe_dir}/cas_alloc_utils.h",
            f"{zeta_exe_dir}/timer.h",
        },
        None
    )

    builder.Add(
        f"{zeta_exe_build_dir}/test_sort.o",
        {
            f"{FILE}",
            f"{zeta_exe_dir}/test_sort.cpp",
        },
        lambda: compiler.cpp_to_obj(
            f"{zeta_exe_build_dir}/test_sort.o",
            f"{zeta_exe_dir}/test_sort.cpp",
        )
    )

    builder.Add(
        f"{zeta_exe_build_dir}/test_sort.exe",
        {
            f"{FILE}",

            f"{zeta_build_dir}/algorithm.o",
            f"{zeta_build_dir}/allocator.o",
            f"{zeta_build_dir}/cascade_allocator.o",
            f"{zeta_build_dir}/debug_str_pipe.o",
            f"{zeta_build_dir}/debugger.o",
            f"{zeta_build_dir}/io.o",
            f"{zeta_build_dir}/mem_check_utils.o",
            f"{zeta_build_dir}/memory.o",
            f"{zeta_build_dir}/ord_linked_list_node.o",
            f"{zeta_build_dir}/utils.o",

            f"{zeta_exe_build_dir}/timer.o",

            f"{zeta_exe_build_dir}/test_sort.o",
        },
        lambda: compiler.to_exe(
            f"{zeta_exe_build_dir}/test_sort.exe",
            {
                f"{zeta_build_dir}/algorithm.o",
                f"{zeta_build_dir}/allocator.o",
                f"{zeta_build_dir}/cascade_allocator.o",
                f"{zeta_build_dir}/debug_str_pipe.o",
                f"{zeta_build_dir}/debugger.o",
                f"{zeta_build_dir}/io.o",
                f"{zeta_build_dir}/mem_check_utils.o",
                f"{zeta_build_dir}/memory.o",
                f"{zeta_build_dir}/ord_linked_list_node.o",
                f"{zeta_build_dir}/utils.o",

                f"{zeta_exe_build_dir}/timer.o",

                f"{zeta_exe_build_dir}/test_sort.o",
            }
        )
    )

    builder.Add(
        f"{zeta_exe_dir}/DyVec.c",
        {
            f"{FILE}",
            f"{zeta_dir}/dynamic_vector.h",
        },
        None
    )

    builder.Add(
        f"{zeta_exe_dir}/test_slaballoc.cpp",
        {
            f"{FILE}",
            f"{zeta_dir}/debug_hash_table.h",
            f"{zeta_dir}/Slaballocator.h",
        },
        None
    )

    builder.Add(
        f"{zeta_exe_build_dir}/test_slaballoc.o",
        {
            f"{FILE}",
            f"{zeta_exe_dir}/test_slaballoc.cpp",
        },
        lambda: compiler.cpp_to_obj(
            f"{zeta_exe_build_dir}/test_slaballoc.o",
            f"{zeta_exe_dir}/test_slaballoc.cpp",
        )
    )

    builder.Add(
        f"{zeta_exe_build_dir}/test_slaballoc.exe",
        {
            f"{FILE}",

            f"{zeta_build_dir}/allocator.o",
            f"{zeta_build_dir}/debug_hash_table.o",
            f"{zeta_build_dir}/ord_linked_list_node.o",
            f"{zeta_build_dir}/ord_rb_linked_list_node.o",
            f"{zeta_build_dir}/Slaballocator.o",
            f"{zeta_build_dir}/utils.o",

            f"{zeta_exe_build_dir}/test_slaballoc.o",
        },
        lambda: compiler.to_exe(
            f"{zeta_exe_build_dir}/test_slaballoc.exe",
            {
                f"{zeta_build_dir}/allocator.o",
                f"{zeta_build_dir}/debug_hash_table.o",
                f"{zeta_build_dir}/ord_linked_list_node.o",
                f"{zeta_build_dir}/ord_rb_linked_list_node.o",
                f"{zeta_build_dir}/Slaballocator.o",
                f"{zeta_build_dir}/utils.o",

                f"{zeta_exe_build_dir}/test_slaballoc.o",
            }
        )
    )

    builder.Add(
        f"{zeta_exe_dir}/test_lin_space_allocator.cpp",
        {
            f"{FILE}",
            f"{zeta_dir}/lin_space_allocator.h",
            f"{zeta_exe_dir}/random.h",
            f"{zeta_exe_dir}/std_allocator.h",
        },
        None
    )

    builder.Add(
        f"{zeta_exe_build_dir}/test_lin_space_allocator.o",
        {
            f"{FILE}",
            f"{zeta_exe_dir}/std_allocator.h",
            f"{zeta_exe_dir}/test_lin_space_allocator.cpp",
        },
        lambda: compiler.cpp_to_obj(
            f"{zeta_exe_build_dir}/test_lin_space_allocator.o",
            f"{zeta_exe_dir}/test_lin_space_allocator.cpp",
        )
    )

    builder.Add(
        f"{zeta_exe_build_dir}/test_lin_space_allocator.exe",
        {
            f"{FILE}",

            f"{zeta_build_dir}/allocator.o",
            f"{zeta_build_dir}/bin_tree.o",
            f"{zeta_build_dir}/debug_str_pipe.o",
            f"{zeta_build_dir}/debugger.o",
            f"{zeta_build_dir}/io.o",
            f"{zeta_build_dir}/lin_space_allocator.o",
            f"{zeta_build_dir}/logger.o",
            f"{zeta_build_dir}/mem_check_utils.o",
            f"{zeta_build_dir}/rbtree.o",
            f"{zeta_build_dir}/utils.o",

            f"{zeta_exe_build_dir}/test_lin_space_allocator.o",
        },
        lambda: compiler.to_exe(
            f"{zeta_exe_build_dir}/test_lin_space_allocator.exe",

            {
                f"{zeta_build_dir}/allocator.o",
                f"{zeta_build_dir}/bin_tree.o",
                f"{zeta_build_dir}/debug_str_pipe.o",
                f"{zeta_build_dir}/debugger.o",
                f"{zeta_build_dir}/io.o",
                f"{zeta_build_dir}/lin_space_allocator.o",
                f"{zeta_build_dir}/logger.o",
                f"{zeta_build_dir}/mem_check_utils.o",
                f"{zeta_build_dir}/rbtree.o",
                f"{zeta_build_dir}/utils.o",

                f"{zeta_exe_build_dir}/test_lin_space_allocator.o",
            }
        )
    )

    builder.Add(
        f"{zeta_exe_dir}/test_mlv.cpp",
        {
            f"{FILE}",
            f"{zeta_dir}/MultiLevelTable.h",
            f"{zeta_exe_dir}/std_allocator.h",
        },
        None
    )

    builder.Add(
        f"{zeta_exe_build_dir}/test_mlv.o",
        {
            f"{FILE}",
            f"{zeta_dir}/MultiLevelTable.h",
            f"{zeta_exe_dir}/std_allocator.h",
            f"{zeta_exe_dir}/test_mlv.cpp",
        },
        lambda: compiler.cpp_to_obj(
            f"{zeta_exe_build_dir}/test_mlv.o",
            f"{zeta_exe_dir}/test_mlv.cpp",
        )
    )

    builder.Add(
        f"{zeta_exe_build_dir}/test_mlv.exe",
        {
            f"{FILE}",

            f"{zeta_build_dir}/allocator.o",
            f"{zeta_build_dir}/debug_hash_table.o",
            f"{zeta_build_dir}/MultiLevelTable.o",
            f"{zeta_build_dir}/utils.o",

            f"{zeta_exe_build_dir}/test_mlv.o",
        },
        lambda: compiler.to_exe(
            f"{zeta_exe_build_dir}/test_mlv.exe",

            {
                f"{zeta_build_dir}/allocator.o",
                f"{zeta_build_dir}/debug_hash_table.o",
                f"{zeta_build_dir}/MultiLevelTable.o",
                f"{zeta_build_dir}/utils.o",

                f"{zeta_exe_build_dir}/test_mlv.o",
            }
        )
    )

    builder.Add(
        f"{zeta_exe_dir}/test_mlt.cpp",
        {
            f"{FILE}",
            f"{zeta_dir}/MultiLevelTable.h",
            f"{zeta_exe_dir}/cpp_std_allocator.h",
            f"{zeta_exe_dir}/zeta_pool_allocator.h",
        },
        None
    )

    builder.Add(
        f"{zeta_exe_build_dir}/test_mlt.o",
        {
            f"{FILE}",
            f"{zeta_dir}/MultiLevelTable.h",
            f"{zeta_exe_dir}/std_allocator.h",
            f"{zeta_exe_dir}/test_mlt.cpp",
        },
        lambda: compiler.cpp_to_obj(
            f"{zeta_exe_build_dir}/test_mlt.o",
            f"{zeta_exe_dir}/test_mlt.cpp",
        )
    )

    builder.Add(
        f"{zeta_exe_build_dir}/test_mlt.exe",
        {
            f"{FILE}",

            f"{zeta_build_dir}/allocator.o",
            f"{zeta_build_dir}/debug_hash_table.o",
            f"{zeta_build_dir}/MultiLevelTable.o",
            f"{zeta_build_dir}/ord_linked_list_node.o",
            f"{zeta_build_dir}/utils.o",

            f"{zeta_exe_build_dir}/test_mlt.o",
        },
        lambda: compiler.to_exe(
            f"{zeta_exe_build_dir}/test_mlt.exe",

            {
                f"{zeta_build_dir}/allocator.o",
                f"{zeta_build_dir}/debug_hash_table.o",
                f"{zeta_build_dir}/MultiLevelTable.o",
                f"{zeta_build_dir}/ord_linked_list_node.o",
                f"{zeta_build_dir}/utils.o",

                f"{zeta_exe_build_dir}/test_mlt.o",
            }
        )
    )

    builder.Add(
        f"{zeta_exe_dir}/test_OkMap.cpp",
        {
            f"{FILE}",
            f"{zeta_exe_dir}/OkMap.h",
        },
        None
    )

    builder.Add(
        f"{zeta_exe_dir}/test_o1sh.cpp",
        {
            f"{FILE}",
            f"{zeta_dir}/MultiLevelTable.h",
            f"{zeta_exe_dir}/std_allocator.h",
        },
        None
    )

    builder.Add(
        f"{zeta_exe_dir}/test_pipe.c",
        {
            f"{FILE}",
            f"{zeta_dir}/debug_str_pipe.h",
            f"{zeta_dir}/debugger.h",
        },
        None
    )

    builder.Add(
        f"{zeta_exe_build_dir}/test_pipe.o",
        {
            f"{FILE}",
            f"{zeta_exe_dir}/test_pipe.c",
        },
        lambda: compiler.c_to_obj(
            f"{zeta_exe_build_dir}/test_pipe.o",
            f"{zeta_exe_dir}/test_pipe.c",
        )
    )

    builder.Add(
        f"{zeta_exe_build_dir}/test_pipe.exe",
        {
            f"{FILE}",
            f"{zeta_build_dir}/debug_str_pipe.o",
            f"{zeta_build_dir}/debugger.o",
            f"{zeta_build_dir}/io.o",
            f"{zeta_build_dir}/logger.o",
            f"{zeta_build_dir}/utils.o",
            f"{zeta_exe_build_dir}/test_pipe.o",
        },
        lambda: compiler.to_exe(
            f"{zeta_exe_build_dir}/test_pipe.exe",
            {
                f"{zeta_build_dir}/debug_str_pipe.o",
                f"{zeta_build_dir}/debugger.o",
                f"{zeta_build_dir}/io.o",
                f"{zeta_build_dir}/logger.o",
                f"{zeta_build_dir}/utils.o",
                f"{zeta_exe_build_dir}/test_pipe.o",
            }
        )
    )

    builder.Add(
        f"{zeta_exe_dir}/test_qsort.cpp",
        {
            f"{FILE}",
            f"{zeta_dir}/algorithm.h",
        },
        None
    )

    builder.Add(
        f"{zeta_exe_build_dir}/test_qsort.o",
        {
            f"{FILE}",
            f"{zeta_exe_dir}/test_qsort.cpp",
        },
        lambda: compiler.cpp_to_obj(
            f"{zeta_exe_build_dir}/test_qsort.o",
            f"{zeta_exe_dir}/test_qsort.cpp",
        )
    )

    builder.Add(
        f"{zeta_exe_build_dir}/test_qsort.exe",
        {
            f"{FILE}",
            f"{zeta_build_dir}/algorithm.o",
            f"{zeta_build_dir}/debugger.o",
            f"{zeta_build_dir}/debug_str_pipe.o",
            f"{zeta_build_dir}/io.o",
            f"{zeta_build_dir}/utils.o",
            f"{zeta_exe_build_dir}/test_qsort.o",
        },
        lambda: compiler.to_exe(
            f"{zeta_exe_build_dir}/test_qsort.exe",

            {
                f"{zeta_build_dir}/algorithm.o",
                f"{zeta_build_dir}/debugger.o",
                f"{zeta_build_dir}/debug_str_pipe.o",
                f"{zeta_build_dir}/io.o",
                f"{zeta_build_dir}/utils.o",
                f"{zeta_exe_build_dir}/test_qsort.o",
            }
        )
    )

    builder.Add(
        f"{zeta_exe_dir}/test_scheduler.cpp",
        {
            f"{FILE}",
            f"{zeta_exe_dir}/ChainingMLTScheduler.h",
            f"{zeta_exe_dir}/ChainingStdRBTScheduler.h",
            f"{zeta_exe_dir}/NaiveStdRBTScheduler.h",
        },
        None
    )

    builder.Add(
        f"{zeta_exe_build_dir}/test_scheduler.o",
        {
            f"{FILE}",
            f"{zeta_dir}/MultiLevelTable.h",
            f"{zeta_exe_dir}/std_allocator.h",
            f"{zeta_exe_dir}/test_scheduler.cpp",
        },
        lambda: compiler.cpp_to_obj(
            f"{zeta_exe_build_dir}/test_scheduler.o",
            f"{zeta_exe_dir}/test_scheduler.cpp",
        )
    )

    builder.Add(
        f"{zeta_exe_build_dir}/test_scheduler.exe",
        {
            f"{FILE}",

            f"{zeta_build_dir}/allocator.o",
            f"{zeta_build_dir}/debug_hash_table.o",
            f"{zeta_build_dir}/MultiLevelTable.o",
            f"{zeta_build_dir}/ord_linked_list_node.o",
            f"{zeta_build_dir}/utils.o",

            f"{zeta_exe_build_dir}/test_scheduler.o",
        },
        lambda: compiler.to_exe(
            f"{zeta_exe_build_dir}/test_scheduler.exe",

            {
                f"{zeta_build_dir}/allocator.o",
                f"{zeta_build_dir}/debug_hash_table.o",
                f"{zeta_build_dir}/MultiLevelTable.o",
                f"{zeta_build_dir}/ord_linked_list_node.o",
                f"{zeta_build_dir}/utils.o",

                f"{zeta_exe_build_dir}/test_scheduler.o",
            }
        )
    )

    builder.Add(
        f"{zeta_exe_dir}/switch.s",
        {
            f"{FILE}",
        },
        None
    )

    builder.Add(
        f"{zeta_exe_dir}/test_cas_alloc.cpp",
        {
            f"{FILE}",
            f"{zeta_dir}/cascade_allocator.h",
            f"{zeta_dir}/debug_hash_table.h",
            f"{zeta_dir}/debugger.h",
            f"{zeta_dir}/mem_check_utils.h",
        },
        None
    )

    builder.Add(
        f"{zeta_exe_build_dir}/test_cas_alloc.o",
        {
            f"{FILE}",
            f"{zeta_exe_dir}/test_cas_alloc.cpp",
        },
        lambda: compiler.cpp_to_obj(
            f"{zeta_exe_build_dir}/test_cas_alloc.o",
            f"{zeta_exe_dir}/test_cas_alloc.cpp",
        )
    )

    builder.Add(
        f"{zeta_exe_build_dir}/test_cas_alloc.exe",
        {
            f"{FILE}",

            f"{zeta_build_dir}/allocator.o",
            f"{zeta_build_dir}/cascade_allocator.o",
            f"{zeta_build_dir}/debug_str_pipe.o",
            f"{zeta_build_dir}/debugger.o",
            f"{zeta_build_dir}/io.o",
            f"{zeta_build_dir}/logger.o",
            f"{zeta_build_dir}/mem_check_utils.o",
            f"{zeta_build_dir}/ord_linked_list_node.o",
            f"{zeta_build_dir}/utils.o",

            f"{zeta_exe_build_dir}/test_cas_alloc.o",
        },
        lambda: compiler.to_exe(
            f"{zeta_exe_build_dir}/test_cas_alloc.exe",

            {
                f"{zeta_build_dir}/allocator.o",
                f"{zeta_build_dir}/cascade_allocator.o",
                f"{zeta_build_dir}/debug_str_pipe.o",
                f"{zeta_build_dir}/debugger.o",
                f"{zeta_build_dir}/io.o",
                f"{zeta_build_dir}/logger.o",
                f"{zeta_build_dir}/mem_check_utils.o",
                f"{zeta_build_dir}/ord_linked_list_node.o",
                f"{zeta_build_dir}/utils.o",

                f"{zeta_exe_build_dir}/test_cas_alloc.o",
            }
        )
    )

    builder.Add(
        f"{zeta_exe_dir}/test_cntrbt.cpp",
        {
            f"{FILE}",
            f"{zeta_dir}/debugger.h",
            f"{zeta_dir}/rbtree.h",
            f"{zeta_dir}/ord_cnt_3rb_tree_node.h",
            f"{zeta_dir}/ord_cnt_rb_tree_node.h",
            f"{zeta_dir}/utils.h",
        },
        None
    )

    builder.Add(
        f"{zeta_exe_build_dir}/test_cntrbt.o",
        {
            f"{FILE}",
            f"{zeta_exe_dir}/test_cntrbt.cpp",
        },
        lambda: compiler.cpp_to_obj(
            f"{zeta_exe_build_dir}/test_cntrbt.o",
            f"{zeta_exe_dir}/test_cntrbt.cpp",
        )
    )

    builder.Add(
        f"{zeta_exe_build_dir}/test_cntrbt.exe",
        {
            f"{FILE}",

            f"{zeta_build_dir}/bin_tree.o",
            f"{zeta_build_dir}/debug_str_pipe.o",
            f"{zeta_build_dir}/debugger.o",
            f"{zeta_build_dir}/io.o",
            f"{zeta_build_dir}/logger.o",
            f"{zeta_build_dir}/mem_check_utils.o",
            f"{zeta_build_dir}/ord_cnt_3rb_tree_node.o",
            f"{zeta_build_dir}/ord_cnt_rb_tree_node.o",
            f"{zeta_build_dir}/rbtree.o",
            f"{zeta_build_dir}/utils.o",

            f"{zeta_exe_build_dir}/test_cntrbt.o",
        },
        lambda: compiler.to_exe(
            f"{zeta_exe_build_dir}/test_cntrbt.exe",

            {
                f"{zeta_build_dir}/bin_tree.o",
                f"{zeta_build_dir}/debug_str_pipe.o",
                f"{zeta_build_dir}/debugger.o",
                f"{zeta_build_dir}/io.o",
                f"{zeta_build_dir}/logger.o",
                f"{zeta_build_dir}/mem_check_utils.o",
                f"{zeta_build_dir}/ord_cnt_3rb_tree_node.o",
                f"{zeta_build_dir}/ord_cnt_rb_tree_node.o",
                f"{zeta_build_dir}/rbtree.o",
                f"{zeta_build_dir}/utils.o",

                f"{zeta_exe_build_dir}/test_cntrbt.o",
            }
        )
    )

    builder.Add(
        f"{zeta_exe_dir}/test_context.c",
        {
            f"{FILE}",
            f"{zeta_dir}/define.h",
        },
        None
    )

    builder.Add(
        f"{zeta_exe_dir}/cpuid.s",
        {
            f"{FILE}",
        },
        None
    )

    builder.Add(
        f"{zeta_exe_dir}/test_cpuid.c",
        {
            f"{FILE}",
            f"{zeta_dir}/define.h",
        },
        None
    )

    builder.Add(
        f"{zeta_exe_dir}/test_crc.c",
        {
            f"{FILE}",
            f"{zeta_dir}/crc.h",
        },
        None
    )

    builder.Add(
        f"{zeta_exe_dir}/test_sha256.c",
        {
            f"{FILE}",
            f"{zeta_dir}/sha256.h",
        },
        None
    )

    builder.Add(
        f"{zeta_exe_dir}/timer.h",
        {
            f"{FILE}",
        },
        None
    )

    builder.Add(
        f"{zeta_exe_dir}/timer.cpp",
        {
            f"{FILE}",
            f"{zeta_exe_dir}/timer.h",
        },
        None
    )

    builder.Add(
        f"{zeta_exe_build_dir}/timer.o",
        {
            f"{FILE}",
            f"{zeta_exe_dir}/timer.h",
        },
        lambda: compiler.cpp_to_obj(
            f"{zeta_exe_build_dir}/timer.o",
            f"{zeta_exe_dir}/timer.cpp",
        )
    )

    builder.Add(
        f"{zeta_exe_dir}/pod_value.h",
        {
            f"{FILE}",
        },
        None
    )

    builder.Add(
        f"{zeta_exe_dir}/pod_value.cpp",
        {
            f"{FILE}",
            f"{zeta_exe_dir}/pod_value.h",
        },
        None
    )

    builder.Add(
        f"{zeta_exe_build_dir}/pod_value.o",
        {
            f"{FILE}",
            f"{zeta_exe_dir}/pod_value.h",
        },
        lambda: compiler.cpp_to_obj(
            f"{zeta_exe_build_dir}/pod_value.o",
            f"{zeta_exe_dir}/pod_value.cpp",
        )
    )

    builder.Add(
        f"{zeta_exe_dir}/zeta_pool_allocator.h",
        {
            f"{zeta_dir}/allocator.h",
        },
        None
    )
