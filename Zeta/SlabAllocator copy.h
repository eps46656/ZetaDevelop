#pragma once

#include "Allocator.h"
#include "LinkedListNode.h"

#define ZETA_SlabAllocator_max_num_of_types ZETA_maxof(u8_t)

/*

SlabLayout {
    Zeta_SlabAllocator_SlabHead {
        int div_t;
        Zeta_LinkedListNode* n;
        void * ptr;
    }

    byte_t paddings[];

    unit {
        void* ptr;
        void* data[size];
    } [N]
}

occ_s =
sizeof(Zeta_SlabAllocator_SlabHead) +
N * (sizeof(void*) + size)

<= page_size

size % align(void*) == 0

for page_size == 4096

    N   size
    1	4064
    2	2024
    3	1344
    4	1008
    6	664
    8	496
    12	328
    16	240
    24	160
    32	112
    48	72
    64	48

*/

typedef struct Zeta_SlabAllocator_Unit Zeta_SlabAllocator_Unit;

struct Zeta_SlabAllocator_Unit {
    void* ptr;
    char data[1];
};

typedef struct Zeta_SlabAllocator_Slab Zeta_SlabAllocator_Slab;

struct Zeta_SlabAllocator_Slab {
    u8_t type_i;
    Zeta_LinkedListNode n;
    void* ptr;

    Zeta_SlabAllocator_Unit units[1];
};

typedef struct Zeta_SlabAllocator Zeta_SlabAllocator;

struct Zeta_SlabAllocator {
    int num_of_types;
    int sizes[Zeta_SlabAllocator_max_num_of_types];
    int nums[Zeta_SlabAllocator_max_num_of_types];

    Zeta_LinkedListNode slab_list_heads[Zeta_SlabAllocator_max_num_of_types];

    Zeta_Allocator* allocator;
};

void Zeta_SlabAllocator_Init(void* sa);

int Zeta_SlabAllocator_GetMaxSize(size_t page_size, int num);

int Zeta_SlabAllocator_GetMaxNum(size_t page_size, int size);

size_t Zeta_SlabAllocator_GetPageSize(int size, int num);

void Zeta_SlabAllocator_Entrust(void* sa, int num_of_types, int const* sizes,
                                int const* nums, Zeta_Allocator* allocator);

size_t Zeta_SlabAllocator_Query(void* sa, size_t size);

void* Zeta_SlabAllocator_Allocate(void* sa, size_t size);

void Zeta_SlabAllocator_Deallocate(void* sa, void* ptr);

void Zeta_SlabAllocator_DeployAllocator(void* sa, Zeta_Allocator* dst);
