#include "SlabAllocator.h"

void Zeta_SlabAllocator_Init(void* sa_) {
    Zeta_SlabAllocator* sa = sa_;

    sa->page_size = 0;
    sa->num_of_divs = 0;

    sa->allocator = NULL;
}

static diff_t GetSize_(diff_t page_size, diff_t div) {
    size_t unit_size = (page_size - sizeof(Zeta_SlabAllocator_Slab) +
                        sizeof(Zeta_SlabAllocator_Unit)) /
                       div;

    return unit_size < sizeof(void*)
               ? 0
               : (unit_size - sizeof(void*)) / alignof(void*) * alignof(void*);
}

diff_t Zeta_SlabAllocator_GetSize(void* sa_, diff_t div_i) {
    Zeta_SlabAllocator* sa = sa_;
    ZETA_DEBUG_ASSERT(sa != NULL);

    ZETA_DEBUG_ASSERT(0 <= div_i);
    ZETA_DEBUG_ASSERT(div_i < sa->num_of_divs);

    return GetSize_(sa->page_size, sa->divs[div_i]);
}

void Zeta_SlabAllocator_Entrust(void* sa_, diff_t page_size, diff_t num_of_divs,
                                const diff_t* divs, Zeta_Allocator* allocator) {
    Zeta_SlabAllocator* sa = sa_;

    ZETA_DEBUG_ASSERT(sa != NULL);

    ZETA_DEBUG_ASSERT(0 <= page_size);
    ZETA_DEBUG_ASSERT(page_size <= Zeta_SlabAllocator_max_page_size);

    ZETA_DEBUG_ASSERT(0 < num_of_divs);
    ZETA_DEBUG_ASSERT(num_of_divs <= Zeta_SlabAllocator_max_num_of_divs);

    for (diff_t i = 0; i < num_of_divs; ++i) { ZETA_DEBUG_ASSERT(0 < divs[i]); }

    ZETA_DEBUG_ASSERT(allocator != NULL);

    sa->page_size = page_size;

    sa->num_of_divs = num_of_divs;

    for (diff_t i = 0; i < num_of_divs; ++i) { sa->divs[i] = divs[i]; }

    for (diff_t i = 0; i < Zeta_SlabAllocator_max_num_of_divs; ++i) {
        Zeta_DoublyLinkedNode_Init(sa->slab_list_heads + i);
    }

    sa->allocator = allocator;
}

void* Zeta_SlabAllocator_Allocate(void* sa_, diff_t size) {
    Zeta_SlabAllocator* sa = sa_;

    ZETA_DEBUG_ASSERT(sa != NULL);
    ZETA_DEBUG_ASSERT(sa->allocator != NULL);
    ZETA_DEBUG_ASSERT(sa->allocator->Allocate != NULL);

    diff_t div_i;

    {
        diff_t div_lb = -1;
        diff_t div_rb = sa->num_of_divs - 1;

        while (div_lb < div_rb) {
            diff_t div_mb = (div_lb + div_rb + 1) / 2;

            diff_t s = GetSize_(sa->page_size, sa->divs[div_mb]);

            if (s < size) {
                div_rb = div_mb - 1;
            } else {
                div_lb = div_mb;
            }
        }

        div_i = div_lb;
    }

    if (div_i == -1) { return NULL; }

    diff_t div = sa->divs[div_i];

    ZETA_PRINT_VAR("%lld", div);
    ZETA_PRINT_VAR("%lld", GetSize_(sa->page_size, div));

    if (GetSize_(sa->page_size, div) <= 0) { return NULL; }

    void* slab_list_head = sa->slab_list_heads + div_i;
    void* slab_k_n = Zeta_DoublyLinkedNode_GetR(slab_list_head);

    Zeta_SlabAllocator_Slab* slab_k;

    _Bool alloc_new_slab = 0;

    if (slab_list_head == slab_k_n) {
        alloc_new_slab = 1;
    } else {
        slab_k = ZETA_FROM_MEM(Zeta_SlabAllocator_Slab, n, slab_k_n);
        if (slab_k->ptr == NULL) { alloc_new_slab = 1; }
    }

    if (alloc_new_slab) {
        slab_k = sa->allocator->Allocate(sa->allocator->context, sa->page_size);
        if (slab_k == NULL) { return NULL; }

        slab_k_n = &slab_k->n;

        slab_k->div_i = div_i;
        Zeta_DoublyLinkedNode_Init(slab_k_n);
        Zeta_DoublyLinkedNode_InsertR(slab_list_head, slab_k_n);

        slab_k->ptr = slab_k->units;

        for (int unit_i = 0; unit_i < size - 1; ++unit_i) {
            Zeta_SlabAllocator_Unit* unit = slab_k->units + unit_i;
            Zeta_SlabAllocator_Unit* nxt_unit = slab_k->units + unit_i + 1;
            unit->ptr = nxt_unit;
        }

        slab_k->units[size - 1].ptr = NULL;
    }

    Zeta_SlabAllocator_Unit* ret_unit = slab_k->ptr;
    Zeta_SlabAllocator_Unit* ret_nxt_unit = ret_unit->ptr;

    ret_unit->ptr = slab_k;
    slab_k->ptr = ret_nxt_unit;

    if (slab_k->ptr == NULL) {
        Zeta_DoublyLinkedNode_Extract(slab_k_n);
        Zeta_DoublyLinkedNode_InsertL(slab_list_head, slab_k_n);
    }

    return ret_unit->data;
}

void Zeta_SlabAllocator_Deallocate(void* sa_, void* ptr) {
    Zeta_SlabAllocator* sa = sa_;

    ZETA_DEBUG_ASSERT(sa != NULL);
    ZETA_DEBUG_ASSERT(sa->allocator != NULL);
    ZETA_DEBUG_ASSERT(sa->allocator->Deallocate != NULL);

    if (ptr == NULL) { return; }

    Zeta_SlabAllocator_Unit* unit =
        ZETA_FROM_MEM(Zeta_SlabAllocator_Unit, data[0], ptr);

    Zeta_SlabAllocator_Slab* slab_k = unit->ptr;

    unit->ptr = slab_k->ptr;
    slab_k->ptr = unit;

    diff_t div_i = slab_k->div_i;

    Zeta_DoublyLinkedNode* slab_list_head = sa->slab_list_heads + div_i;

    Zeta_DoublyLinkedNode_Extract(&slab_k->n);

    diff_t div = sa->divs[div_i];

    for (diff_t i = 0; i < div; ++i) {
        if (slab_k->units[i].ptr == slab_k) {
            Zeta_DoublyLinkedNode_InsertL(slab_list_head, &slab_k->n);
            return;
        }
    }

    sa->allocator->Deallocate(sa->allocator->context, slab_k);
}

void Zeta_SlabAllocator_DeployAllocator(void* sa_, Zeta_Allocator* dst) {
    Zeta_SlabAllocator* sa = sa_;

    ZETA_DEBUG_ASSERT(sa != NULL);
    ZETA_DEBUG_ASSERT(dst != NULL);

    dst->context = sa;

    dst->Allocate = Zeta_SlabAllocator_Allocate;

    dst->Deallocate = Zeta_SlabAllocator_Deallocate;
}
