#pragma once

#include "Allocator.h"
#include "DebugTreeMap.h"
#include "OrdLinkedListNode.h"

ZETA_ExternC_Beg;

/*

To allocate small size memory unit efficiently.

Single SlabAllocator only provides single width of unit.

*/

#define ZETA_SlabAllocator_max_num ZETA_GetRangeMax(unsigned char)

typedef struct Zeta_SlabAllocator_SlabHead Zeta_SlabAllocator_SlabHead;

struct Zeta_SlabAllocator_SlabHead {
    Zeta_OrdLinkedListNode n;
    unsigned char vacant_units_num;
};

typedef struct Zeta_SlabAllocator Zeta_SlabAllocator;

struct Zeta_SlabAllocator {
    size_t align;
    size_t width;
    size_t num;

    size_t buffer_units_num;

    size_t vacant_units_num;
    size_t occupied_units_num;

    Zeta_OrdLinkedListNode* hot_unit_head;

    Zeta_OrdLinkedListNode* vacant_slab_n_head;
    Zeta_OrdLinkedListNode* occupied_slab_n_head;

    Zeta_Allocator* allocator;
};

void Zeta_SlabAllocator_Init(void* sa);

void Zeta_SlabAllocator_Deinit(void* sa);

size_t Zeta_SlabAllocator_GetAlign(void* sa);

size_t Zeta_SlabAllocator_Query(void* sa, size_t size);

void* Zeta_SlabAllocator_Allocate(void* sa, size_t size);

void Zeta_SlabAllocator_Deallocate(void* sa, void* ptr);

void Zeta_SlabAllocator_DeployAllocator(void* sa, Zeta_Allocator* dst);

void Zeta_SlabAllocator_Check(void* sa, Zeta_DebugTreeMap* dst_used_ptr_size_tm,
                              Zeta_DebugTreeMap* dst_released_ptr_size_tm);

ZETA_ExternC_End;
