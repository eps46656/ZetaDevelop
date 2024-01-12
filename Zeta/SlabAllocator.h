#pragma once

#include "Allocator.h"
#include "DoublyLinkedNode.h"

#define Zeta_SlabAllocator_max_page_size (1 * 1024 * 1024 * 1024)

#define Zeta_SlabAllocator_max_num_of_divs 32

/*

SlabLayout {
    Zeta_SlabAllocator_SlabHead {
        Zeta_DoublyLinkedNode* slab_n;
        void * unit;
    }

    unit {
        void* nxt_unit;
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

ZETA_DECL_STRUCT(Zeta_SlabAllocator_Unit) {
    void* ptr;
    char data[1];
};

ZETA_DECL_STRUCT(Zeta_SlabAllocator_Slab) {
    diff_t div_i;
    Zeta_DoublyLinkedNode n;
    void* ptr;

    Zeta_SlabAllocator_Unit units[1];
};

ZETA_DECL_STRUCT(Zeta_SlabAllocator) {
    diff_t page_size;

    diff_t num_of_divs;
    diff_t divs[Zeta_SlabAllocator_max_num_of_divs];

    Zeta_DoublyLinkedNode slab_list_heads[Zeta_SlabAllocator_max_num_of_divs];

    Zeta_Allocator* allocator;
};

void Zeta_SlabAllocator_Init(void* sa);

diff_t Zeta_SlabAllocator_GetSize(void* sa, diff_t div_i);

void Zeta_SlabAllocator_Entrust(void* sa, diff_t page_size, diff_t num_of_divs,
                                const diff_t* divs, Zeta_Allocator* allocator);

void* Zeta_SlabAllocator_Allocate(void* sa, diff_t size);

void Zeta_SlabAllocator_Deallocate(void* sa, void* ptr);

void Zeta_SlabAllocator_DeployAllocator(void* sa, Zeta_Allocator* dst);
