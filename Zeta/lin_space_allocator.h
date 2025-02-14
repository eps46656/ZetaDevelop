#pragma once

#include "allocator.h"
#include "assoc_cntr.h"
#include "bin_tree_node.h"
#include "mem_check_utils.h"

ZETA_ExternC_Beg;

#define ZETA_LinSpaceAllocator_AllocationStrategy_BestFit (0)
#define ZETA_LinSpaceAllocator_AllocationStrategy_TwiceBestFit (1)

ZETA_DeclareStruct(Zeta_LinSpaceAllocator_Node);
ZETA_DeclareStruct(Zeta_LinSpaceAllocator);

struct Zeta_LinSpaceAllocator_Node {
    Zeta_ShortRelCnt3RBTreeNode gn;
    Zeta_ShortRelRBTreeNode sn;
};

struct Zeta_LinSpaceAllocator {
    size_t beg;
    size_t end;

    int strategy;

    Zeta_LinSpaceAllocator_Node* gt_root;
    Zeta_LinSpaceAllocator_Node* st_root;

    Zeta_Allocator node_allocator;
};

/**
 * @brief Initialize the allocator managed space [beg, end).
 *
 * @param lsa The target allocator.
 */
void Zeta_LinSpaceAllocator_Init(void* lsa);

/**
 * @brief Allocate a continuous space at least \p cnt units.
 *
 * @param lsa The target allocator.
 * @param cnt
 *
 * @return The begin of allocated space.
 */
size_t Zeta_LinSpaceAllocator_Allocate(void* lsa, size_t cnt);

void Zeta_LinSpaceAllocator_Deallocate(void* lsa, size_t idx);

size_t Zeta_LinSpaceAllocator_GetVacantSizeL(void* lsa);

size_t Zeta_LinSpaceAllocator_GetVacantSizeR(void* lsa);

void Zeta_LinSpaceAllocator_ExtendL(void* lsa, size_t cnt);

void Zeta_LinSpaceAllocator_ExtendR(void* lsa, size_t cnt);

bool_t Zeta_LinSpaceAllocator_ShrinkL(void* lsa, size_t cnt);

bool_t Zeta_LinSpaceAllocator_ShrinkR(void* lsa, size_t cnt);

void Zeta_LinSpaceAllocator_Check(void* lsa);

void Zeta_LinSpaceAllocator_Sanitize(void* lsa, Zeta_MemRecorder* dst_ns);

ZETA_ExternC_End;
