#pragma once

#include "Allocator.h"
#include "DebugHashMap.h"
#include "OrdLinkedListNode.h"

ZETA_ExternC_Beg;

/*

To allocate small size memory unit efficiently.

Single SlabAllocator only provides single width of unit.

*/

ZETA_DeclareStruct(Zeta_SlabAllocator);
ZETA_DeclareStruct(Zeta_SlabAllocator_SlabHead);

#define ZETA_SlabAllocator_max_num ZETA_RangeMaxOf(unsigned char)

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

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

struct Zeta_SlabAllocator_SlabHead {
    Zeta_OrdLinkedListNode n;
    unsigned char vacant_units_num;
};

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

void Zeta_SlabAllocator_Init(void* sa);

void Zeta_SlabAllocator_Deinit(void* sa);

size_t Zeta_SlabAllocator_GetAlign(void* sa);

size_t Zeta_SlabAllocator_Query(void* sa, size_t size);

void* Zeta_SlabAllocator_Allocate(void* sa, size_t size);

void Zeta_SlabAllocator_Deallocate(void* sa, void* ptr);

bool_t Zeta_SlabAllocator_ReleaseBuffer(void* sa);

void Zeta_SlabAllocator_DeployAllocator(void* sa, Zeta_Allocator* dst);

void Zeta_SlabAllocator_Check(void* sa, Zeta_DebugHashMap* dst_used_records_tm,
                              Zeta_DebugHashMap* dst_released_records);

ZETA_ExternC_End;
