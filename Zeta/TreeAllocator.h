#pragma once

#include "Allocator.h"
#include "DebugHashTable.h"
#include "MemCheck.h"
#include "OrdRBLinkedListNode.h"
#include "OrdRBTreeNode.h"

ZETA_ExternC_Beg;

ZETA_DeclareStruct(Zeta_TreeAllocator);

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

struct Zeta_TreeAllocator {
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

    /**
     * The least size(byte) of allocated memory block.
     */
    size_t least_size;

    void* data_beg;
    void* data_end;

    Zeta_OrdRBLinkedListNode hn_head;

    Zeta_OrdRBTreeNode* sn_root;
};

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

/**
 * @brief Initialize the tree allocator
 *
 * @param ta The target tree allocator.
 */
void Zeta_TreeAllocator_Init(void* ta);

/**
 * @brief Deinitialize the tree allocator
 *
 * @param ta The target tree allocator.
 */
void Zeta_TreeAllocator_Deinit(void* ta);

size_t Zeta_TreeAllocator_GetAlign(void* ta);

/**
 * @brief Return the actual size of allocated memory if request size of memory
 * with Allocate.
 *
 * @param ta The target tree allocator.
 * @param size The size(byte) of allocated memory should at least occupied at
 * least size of bytes.
 */
size_t Zeta_TreeAllocator_Query(void* ta, size_t size);

/**
 * @brief Allocate a continuous memory from managed memory block, then return
 * its pointer. If allocation failed, return NULL.
 *
 * @param ta The target tree allocator.
 * @param size The size(byte) of allocated memory should at least occupied at
 * least size of bytes.
 */
void* Zeta_TreeAllocator_Allocate(void* ta, size_t size);

/**
 * @brief Deallocate the target memory block. The memory block should be
 * returned by Zeta_TreeAllocator_Allocate and has not be call in this function.
 *
 * @param ta The tree allocator who allocated the target memory
 * block.
 * @param ptr The target memory block.
 */
void Zeta_TreeAllocator_Deallocate(void* ta, void* ptr);

void Zeta_TreeAllocator_Check(void* ta, bool_t print_state,
                              Zeta_MemRecorder* dst_recorder);

void Zeta_TreeAllocator_DeployAllocator(void* ta, Zeta_Allocator* dst);

ZETA_ExternC_End;
