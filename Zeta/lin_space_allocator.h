#pragma once

#include "allocator.h"
#include "assoc_cntr.h"
#include "mem_check_utils.h"

ZETA_ExternC_Beg;

#define ZETA_LinSpaceAllocator_AllocationStrategy_BestFit (0)
#define ZETA_LinSpaceAllocator_AllocationStrategy_TwiceBestFit (1)

ZETA_DeclareStruct(Zeta_LinSpaceAllocator_Node);
ZETA_DeclareStruct(Zeta_LinSpaceAllocator);

struct Zeta_LinSpaceAllocator_Node {
    int gpc;  // gt_coolor
    int glc;  // state_color: VacantColor, OccupiedColor
    int gr;

    int spc;  // st_color
    int sl;
    int sr;

    unsigned acc_size;
} __attribute__((aligned(2)));

struct Zeta_LinSpaceAllocator {
    size_t beg;
    size_t end;

    int strategy;

    Zeta_LinSpaceAllocator_Node* gt_root;
    Zeta_LinSpaceAllocator_Node* st_root;

    Zeta_Allocator* node_allocator;
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

void Zeta_LinSpaceAllocator_ExtendL(void* lsa, size_t cnt);

void Zeta_LinSpaceAllocator_ExtendR(void* lsa, size_t cnt);

bool_t Zeta_LinSpaceAllocator_ShrinkL(void* lsa, size_t cnt);

bool_t Zeta_LinSpaceAllocator_ShrinkR(void* lsa, size_t cnt);

void Zeta_LinSpaceAllocator_Check(void* lsa);

void Zeta_LinSpaceAllocator_Sanitize(void* lsa, Zeta_MemRecorder* dst_ns);

ZETA_ExternC_End;
