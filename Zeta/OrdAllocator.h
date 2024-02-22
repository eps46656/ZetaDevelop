#pragma once

#include "Allocator.h"
#include "RelLinkedListNode.h"
#include "RelRBTreeNode.h"

ZETA_extern_c_beg;

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

#define ZETA_OrdAllocator_max_num_of_slab_level 64

typedef struct Zeta_OrdAllocator Zeta_OrdAllocator;

struct Zeta_OrdAllocator {
    /**
     * The begining of managed memory block.
     */
    void* mem;

    /**
     * The alignment of each alloated memory block, should always be a factory
     * of alignedof(void *).
     */
    size_t align;

    /**
     * The size(byte) of managed memory block.
     */
    size_t size;

    void* data_beg;
    void* data_end;

    Zeta_RelRBTreeNode* sn_root;

    /**
     * The number of slab level, should not less than 0 or larger than
     * Zeta_OrdAllocator_max_num_of_slab_level - 1. Fill 0 means not using slab.
     * Fill a negative number to invoke auto configiration when initializing.
     */
    int num_of_slab_level;

    /**
     * The width(byte) of units for each slab level. If a negative value is
     * filled in num_of_slab_level. This field will be auto configured when
     * initializing.
     */
    int slab_widths[ZETA_OrdAllocator_max_num_of_slab_level];

    /**
     * The number of units in a slab block for each slab level. If a negative
     * value is filled in num_of_slab_level. This field will be auto
     * configured when initializing.
     */
    int slab_sizes[ZETA_OrdAllocator_max_num_of_slab_level];

    /**
     * The minimum number of units should be reserved before returning the slab
     * for each level. If a negative value is filled in num_of_slab_level. This
     * field will be auto configured when initializing.
     */
    int slab_resvs[ZETA_OrdAllocator_max_num_of_slab_level];

    /**
     * The list heads of slab list for each slab level.
     */
    Zeta_RelLinkedListNode* slab_list_heads;
};

/**
 * @brief Initialize the ord allocator
 *
 * @param ord_allocator The target ord allocator.
 */
void Zeta_OrdAllocator_Init(void* ord_allocator);

/**
 * @brief Return the actual size of allocated memory if request size of memory
 * with Allocate.
 *
 * @param ord_allocator The target ord allocator.
 * @param size The size(byte) of allocated memory should at least occupied at
 * least size of bytes.
 */
size_t Zeta_OrdAllocator_Query(void* ord_allocator, size_t size);

/**
 * @brief Allocate a continuous memory from managed memory block, then return
 * its pointer. If allocation failed, return NULL.
 *
 * @param ord_allocator The target ord allocator.
 * @param size The size(byte) of allocated memory should at least occupied at
 * least size of bytes.
 */
void* Zeta_OrdAllocator_Allocate(void* ord_allocator, size_t size);

/**
 * @brief Deallocate the target memory block. The memory block should be
 * returned by Zeta_OrdAllocator_Allocate and has not be call in this function.
 *
 * @param ord_allocator The ord allocator who allocated the target memory
 * block.
 * @param ptr The target memory block.
 */
void Zeta_OrdAllocator_Deallocate(void* ord_allocator, void* ptr);

void Zeta_OrdAllocator_Check(void* ord_allocator, bool_t print_state,
                             void* dst_ptr_size_tm);

void Zeta_OrdAllocator_ToAllocator(void* ord_allocator, Zeta_Allocator* dst);

ZETA_extern_c_end;
