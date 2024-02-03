#pragma once

#include "Allocator.h"
#include "OrdDoublyLinkedNode.h"
#include "OrdRBTreeNode.h"

/*

// for vacumm
head i {
    state // indicate vacumm
    ln for pre nxt block
    rbn for size
}
back ptr to head
data i



// for occupied
head j {
    state // indicate occupied
    ln for pre nxt block
}
back ptr to head
data j


// for slab
head j {
    state // indicate slab
    ln for pre nxt block
    ptr to slab unit
}
{
    back ptr to head
    data
}[K]





head + back ptr + data = block

head
    contains meta data

back ptr


state =
    slab 0      0
    slab 1      1
    slab 2
    ...
    slab z

    vacumm      -1
    occupied    -2

*/

#define Zeta_OrdAllocator_max_num_of_slab_level 64

typedef struct Zeta_OrdAllocator Zeta_OrdAllocator;

struct Zeta_OrdAllocator {
    void* ptr;

    /**
     * The alignment of each alloated memory block, should always be a factory
     * of alignedof(void *).
     */
    size_t align;

    /**
     * The size(byte) of managed memory block.
     */
    size_t size;

    /**
     * The number of slab level, should not less than 1 or larger than
     * Zeta_OrdAllocator_max_num_of_slab_level.
     */
    int num_of_slab_level;

    /**
     * The width(byte) of units for each slab level.
     */
    int slab_widths[Zeta_OrdAllocator_max_num_of_slab_level];

    /**
     * The number of units in a slab block for each slab level.
     */
    int slab_sizes[Zeta_OrdAllocator_max_num_of_slab_level];

    Zeta_OrdRBTreeNode* sn_root;

    /**
     * The list heads of slab list for each slab level.
     */
    Zeta_OrdDoublyLinkedNode
        slab_list_heads[Zeta_OrdAllocator_max_num_of_slab_level];
};

EXTERN_C void Zeta_OrdAllocator_Init(void* ord_allocator);

EXTERN_C void* Zeta_OrdAllocator_Allocate(void* ord_allocator, size_t size);

/**
 * @brief Deallocate the target memory block. The memory block should be
 * returned by Zeta_OrdAllocator_Allocate and has not be call in this function.
 *
 * @param ord_allocator The ord allocator who allocated the target memory
 * block.
 * @param ptr The target memory block.
 */
EXTERN_C void Zeta_OrdAllocator_Deallocate(void* ord_allocator, void* ptr);

EXTERN_C void Zeta_OrdAllocator_Check(void* ord_allocator, bool_t print_state,
                                      void* dst_ptr_size_ht);

EXTERN_C void Zeta_OrdAllocator_ToAllocator(void* ord_allocator,
                                            Zeta_Allocator* dst);
