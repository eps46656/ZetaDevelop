#pragma once

#include "Allocator.h"
#include "MemCheck.h"
#include "OrdLinkedListNode.h"

ZETA_ExternC_Beg;

ZETA_DeclareStruct(Zeta_SlabAllocator);
ZETA_DeclareStruct(Zeta_SlabAllocator_SlabHead);

#define ZETA_SlabAllocator_max_units_per_slab ZETA_RangeMaxOf(unsigned char)

struct Zeta_SlabAllocator {
    size_t align;
    size_t width;
    size_t units_per_slab;

    size_t buffer_units_cnt;

    size_t vacant_units_cnt;
    size_t occupied_units_cnt;

    Zeta_OrdLinkedListNode* hot_slab_units_list;

    Zeta_OrdLinkedListNode* vacant_slab_list;
    Zeta_OrdLinkedListNode* occupied_slab_list;

    Zeta_Allocator* allocator;
};

struct Zeta_SlabAllocator_SlabHead {
    Zeta_OrdLinkedListNode n;
    unsigned char vacant_units_cnt;
};

void Zeta_SlabAllocator_Init(void* sa);

void Zeta_SlabAllocator_Deinit(void* sa);

size_t Zeta_SlabAllocator_GetAlign(void* sa);

size_t Zeta_SlabAllocator_Query(void* sa, size_t size);

void* Zeta_SlabAllocator_Allocate(void* sa, size_t size);

void Zeta_SlabAllocator_Deallocate(void* sa, void* ptr);

bool_t Zeta_SlabAllocator_ReleaseBuffer(void* sa);

void Zeta_SlabAllocator_Check(void* sa, Zeta_MemRecorder* dst_used_records,
                              Zeta_MemRecorder* dst_released_records);

void Zeta_SlabAllocator_DeployAllocator(void* sa, Zeta_Allocator* dst);

ZETA_ExternC_End;
