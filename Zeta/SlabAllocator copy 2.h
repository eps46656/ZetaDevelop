#pragma once

#include "Allocator.h"
#include "RelLinkedListNode.h"

/*

To allocate small size memory unit efficiently.

Single SlabAllocator only provides single width of unit.

*/

#define ZETA_SlabAllocator_max_of_num ZETA_maxof(u8_t)

typedef struct Zeta_SlabAllocator Zeta_SlabAllocator;

struct Zeta_SlabAllocator {
    size_t align;
    size_t width;
    size_t num;

    Zeta_RelLinkedListNode slab_list_head;

    Zeta_Allocator* allocator;
};

void Zeta_SlabAllocator_Init(void* sa);

size_t Zeta_SlabAllocator_GetAlign(void* sa);

size_t Zeta_SlabAllocator_Query(void* sa, size_t size);

void* Zeta_SlabAllocator_Allocate(void* sa, size_t size);

void Zeta_SlabAllocator_Deallocate(void* sa, void* ptr);

void Zeta_SlabAllocator_DeployAllocator(void* sa, Zeta_Allocator* dst);
