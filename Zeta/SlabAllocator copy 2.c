#include "SlabAllocator.h"

#include "utils.h"

#define TYPE_EMPTY (0)
#define TYPE_LOW (1)
#define TYPE_ACTIVE (2)
#define TYPE_FULL (10)

typedef struct Slab_ Slab_;

struct Slab_ {
    Zeta_RelLinkedListNode n;
    u8_t type;
    u8_t num;
    u8_t unit_i;
};

void Zeta_SlabAllocator_Init(void* sa_) {
    Zeta_SlabAllocator* sa = sa_;
    ZETA_DebugAssert(sa != NULL);

    size_t align = sa->align;
    ZETA_DebugAssert(0 < align);

    align = Zeta_GetLCM(align, alignof(Slab_));
    align = Zeta_GetLCM(align, alignof(u8_t));
    sa->align = align;

    size_t width = sa->width;
    ZETA_DebugAssert(0 < width);

    width = Zeta_FindNextConMod(width + sizeof(u8_t), 0, align) - sizeof(u8_t);
    sa->width = width;

    ZETA_DebugAssert(0 < sa->num);

    Zeta_RelLinkedListNode_Init(&sa->slab_list_head);

    sa->allocator = NULL;
}

size_t Zeta_SlabAllocator_GetAlign(void* sa) {
    Zeta_SlabAllocator* sa = sa_;
    ZETA_DebugAssert(sa != NULL);

    return sa->align;
}

size_t Zeta_SlabAllocator_Query(void* sa_, size_t size) {
    Zeta_SlabAllocator* sa = sa_;
    ZETA_DebugAssert(sa != NULL);

    return size == 0 ? 0 : sa->width;
}

void* Zeta_SlabAllocator_Allocate(void* sa_, size_t size) {
    Zeta_SlabAllocator* sa = sa_;
    ZETA_DebugAssert(sa != NULL);

    ZETA_DebugAssert(sa->allocator != NULL);
    ZETA_DebugAssert(sa->allocator->Allocate != NULL);

    size_t width = sa->width;
    size_t num = sa->num;

    if (size == 0 || width < size) { return NULL; }

    void* slab_list_head = &sa->slab_list_head;
    void* slab_n = Zeta_LinkedListNode_GetR(slab_list_head);

    Slab_* slab;

    bool_t alloc_new_slab = FALSE;

    if (slab_list_head == slab_n) {
        alloc_new_slab = TRUE;
    } else {
        slab = ZETA_GetStructFromMember(Slab_, n, slab_n);
        if (slab->num == 0) { alloc_new_slab = TRUE; }
    }

    if (alloc_new_slab) {
        slab = sa->allocator->Allocate(
            sa->allocator->context,
            sizeof(Slab_) + (width + sizeof(u8_t)) * num);

        if (slab == NULL) { return NULL; }

        slab_n = &slab->n;
        Zeta_LinkedListNode_Init(slab_n);
        Zeta_LinkedListNode_InsertR(slab_list_head, slab_n);

        slab->num = num;
        slab->unit_i = 0;

        uintptr_t nxt_unit_i = ZETA_PTR_TO_UINT(slab) + sizeof(Slab_) + width;

        for (int unit_i = 0; unit_i < num; ++unit_i) {
            (u8_t*)ZETA_UINT_TO_PTR(nxt_unit_i) = unit_i + 1;
            nxt_unit_i += width + sizeof(u8_t);
        }
    }

    --slab->num;

    u8_t unit_i = slab->unit_i;
    u8_t* nxt_unit_i =
        ZETA_UINT_TO_PTR(ZETA_PTR_TO_UINT(slab) + sizeof(Slab_) + width +
                         (width + sizeof(u8_t)) * unit_i);

    slab->unit_i = *nxt_unit_i;
    *nxt_unit_i = unit_i;

    if (slab->num == 0) {
        Zeta_LinkedListNode_Extract(slab_n);
        Zeta_LinkedListNode_InsertL(slab_list_head, slab_n);
    }

    return ZETA_UINT_TO_PTR(ZETA_PTR_TO_UINT(nxt_unit_i) - width);
}

void Zeta_SlabAllocator_Deallocate(void* sa_, void* ptr) {
    Zeta_SlabAllocator* sa = sa_;

    ZETA_DebugAssert(sa != NULL);
    ZETA_DebugAssert(sa->allocator != NULL);
    ZETA_DebugAssert(sa->allocator->Deallocate != NULL);

    if (ptr == NULL) { return; }

    size_t width = sa->width;
    size_t num = sa->num;

    u8_t* nxt_unit_i = ZETA_UINT_TO_PTR(ZETA_PTR_TO_UINT(ptr) + width);

    Slab_* slab =
        ZETA_UINT_TO_PTR(ZETA_PTR_TO_UINT(nxt_unit_i) - sizeof(Slab_) - width -
                         (width + sizeof(u8_t)) * (*nxt_unit_i));

    ++slab->num;

    ZETA_Swap(slab->unit_i, *nxt_unit_i);

    Zeta_LinkedListNode_Extract(&slab_k->n);

    int num = sa->nums[type_i];

    for (int i = 0; i < num; ++i) {
        if (slab_k->units[i].ptr == slab_k) {
            Zeta_LinkedListNode_InsertL(slab_list_head, &slab_k->n);
            return;
        }
    }

    sa->allocator->Deallocate(sa->allocator->context, slab_k);
}

void Zeta_SlabAllocator_DeployAllocator(void* sa_, Zeta_Allocator* dst) {
    Zeta_SlabAllocator* sa = sa_;

    ZETA_DebugAssert(sa != NULL);
    ZETA_DebugAssert(dst != NULL);

    dst->context = sa;

    dst->Allocate = Zeta_SlabAllocator_Allocate;

    dst->Deallocate = Zeta_SlabAllocator_Deallocate;
}
