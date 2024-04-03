#include "SlabAllocator.h"

#include "utils.h"

typedef struct Slab Slab;

struct Slab {};

typedef struct SlabHead SlabHead;

struct SlabHead {
    Zeta_RelLinkedListNode n;
    u8_t num;
};

ZETA_StaticAssert(alignof(SlabHead) % alignof(uintptr_t) == 0);

#define ZETA_SlabSize (stride * num + sizeof(SlabHead))

#define ZETA_GetSlabHeadFromSlab(slab) \
    (ZETA_GetPtrFromAddr(ZETA_GetAddrFromPtr(slab) + stride * num))

#define ZETA_GetSlabFromSlabHead(slab_head) \
    (ZETA_GetPtrFromAddr(ZETA_GetAddrFromPtr(slab_head) - stride * num))

#define ZETA_GetFirstChunkFromSlab(slab) (ZETA_GetAddrFromPtr(slab))

void* GetPtr_(uintptr_t* ptr) {
    return ZETA_GetPtrFromAddr(ZETA_GetAddrFromPtr(ptr) + *ptr);
}

void SetPtr_(uintptr_t* ptr, void* val) {
    *ptr = ZETA_GetAddrFromPtr(val) - ZETA_GetAddrFromPtr(ptr);
}

void Zeta_SlabAllocator_Init(void* sa_) {
    Zeta_SlabAllocator* sa = sa_;
    ZETA_DebugAssert(sa != NULL);

    size_t align = sa->align;
    ZETA_DebugAssert(0 < align);

    align = Zeta_GetLCM(align, alignof(uintptr_t));

    sa->align = align;

    size_t width = sa->width;
    ZETA_DebugAssert(0 < width);

    if (width < sizeof(Zeta_RelLinkedListNode)) {
        width = sizeof(Zeta_RelLinkedListNode);
    }

    width = Zeta_FindNextConMod(width + sizeof(uintptr_t), 0, align) -
            sizeof(uintptr_t);

    sa->width = width;

    size_t stride = width + sizeof(uintptr_t);

    Zeta_Allocator* allocator = sa->allocator;
    ZETA_DebugAssert(allocator != NULL);

    ZETA_DebugAssert(allocator->GetAlign(allocator->context) % align == 0);

    size_t num = sa->num;
    ZETA_DebugAssert(0 < sa->num);
    ZETA_DebugAssert(sa->num <= ZETA_SlabAllocator_max_num);

    size_t real_slab_size = allocator->Query(allocator->context, ZETA_SlabSize);

    num = (real_slab_size - sizeof(SlabHead)) / stride;
    if (ZETA_SlabAllocator_max_num < num) { num = ZETA_SlabAllocator_max_num; }

    sa->num = num;

    sa->num_of_vacant_units = 0;
    sa->num_of_occupied_units = 0;

    Zeta_RelLinkedListNode_Init(&sa->vacant_slab_n_list_head);
    Zeta_RelLinkedListNode_Init(&sa->occupied_slab_n_list_head);

    Zeta_RelLinkedListNode_Init(&sa->hot_unit_head);
}

size_t Zeta_SlabAllocator_GetAlign(void* sa_) {
    Zeta_SlabAllocator* sa = sa_;
    ZETA_DebugAssert(sa != NULL);

    return sa->align;
}

size_t Zeta_SlabAllocator_Query(void* sa_, size_t size) {
    Zeta_SlabAllocator* sa = sa_;
    ZETA_DebugAssert(sa != NULL);

    return 0 < size && size <= sa->width ? sa->width : 0;
}

void* Zeta_SlabAllocator_Allocate(void* sa_, size_t size) {
    Zeta_SlabAllocator* sa = sa_;
    ZETA_DebugAssert(sa != NULL);

    size_t width = sa->width;
    size_t num = sa->num;
    size_t stride = width + sizeof(uintptr_t);

    if (size == 0 || width < size) { return NULL; }

    Zeta_RelLinkedListNode* hot_unit_head = &sa->hot_unit_head;

    Zeta_RelLinkedListNode* hot_unit_n =
        Zeta_RelLinkedListNode_GetR(hot_unit_head);

    if (hot_unit_n == hot_unit_head) {
        Zeta_Allocator* allocator = sa->allocator;
        ZETA_DebugAssert(allocator != NULL);

        Slab* slab = allocator->Allocate(allocator->context, ZETA_SlabSize);

        ZETA_DebugAssert(slab != NULL);

        SlabHead* slab_head = ZETA_GetSlabHeadFromSlab(slab);

        Zeta_RelLinkedListNode_Init(&slab_head->n);
        Zeta_RelLinkedListNode_InsertL(&sa->occupied_slab_n_list_head,
                                       &slab_head->n);

        slab_head->num = num - 1;

        uintptr_t first_chunk = ZETA_GetFirstChunkFromSlab(slab);
        uintptr_t chunk = first_chunk;

        for (size_t i = 0; i < num; ++i, chunk += stride) {
            Zeta_RelLinkedListNode_Init(ZETA_GetPtrFromAddr(chunk));

            Zeta_RelLinkedListNode_InsertL(ZETA_GetPtrFromAddr(hot_unit_head),
                                           ZETA_GetPtrFromAddr(chunk));

            SetPtr_(ZETA_GetPtrFromAddr(chunk + width), slab);
        }

        sa->num_of_vacant_units += num;

        hot_unit_n = ZETA_GetPtrFromAddr(first_chunk);
    } else {
        uintptr_t* hot_unit_back_ptr =
            ZETA_GetPtrFromAddr(ZETA_GetAddrFromPtr(hot_unit_n) + width);

        Slab* slab = GetPtr_(hot_unit_back_ptr);

        SlabHead* slab_head = ZETA_GetSlabHeadFromSlab(slab);

        Zeta_RelLinkedListNode_Extract(&slab_head->n);
        Zeta_RelLinkedListNode_InsertL(&sa->occupied_slab_n_list_head,
                                       &slab_head->n);

        --slab_head->num;
    }

    Zeta_RelLinkedListNode_Extract(hot_unit_n);

    --sa->num_of_vacant_units;
    ++sa->num_of_occupied_units;

    return hot_unit_n;
}

void Zeta_SlabAllocator_Deallocate(void* sa_, void* ptr) {
    Zeta_SlabAllocator* sa = sa_;
    ZETA_DebugAssert(sa != NULL);

    if (ptr == NULL) { return; }

    size_t width = sa->width;
    size_t stride = width + sizeof(uintptr_t);
    size_t num = sa->num;

    Zeta_RelLinkedListNode_Init(ptr);
    Zeta_RelLinkedListNode_InsertR(&sa->hot_unit_head, ptr);

    Slab* slab = GetPtr_(ZETA_GetPtrFromAddr(ZETA_GetAddrFromPtr(ptr) + width));

    SlabHead* slab_head = ZETA_GetSlabHeadFromSlab(slab);

    ++slab_head->num;

    ++sa->num_of_vacant_units;
    --sa->num_of_occupied_units;

    if (slab_head->num == num) {
        Zeta_RelLinkedListNode_Extract(&slab_head->n);
        Zeta_RelLinkedListNode_InsertL(&sa->vacant_slab_n_list_head,
                                       &slab_head->n);
    }

    if (4 * sa->num_of_vacant_units < sa->num_of_occupied_units) { return; }

    Zeta_RelLinkedListNode* vacant_slab_n_list_head =
        &sa->vacant_slab_n_list_head;

    Zeta_RelLinkedListNode* vacant_slab_n =
        Zeta_RelLinkedListNode_GetR(vacant_slab_n_list_head);

    if (vacant_slab_n == vacant_slab_n_list_head) { return; }

    SlabHead* vacant_slab_head =
        ZETA_GetStructFromMember(SlabHead, n, vacant_slab_n);

    Slab* vacant_slab = ZETA_GetSlabFromSlabHead(vacant_slab_head);

    Zeta_RelLinkedListNode_Extract(vacant_slab_n);

    uintptr_t chunk = ZETA_GetFirstChunkFromSlab(vacant_slab);

    for (size_t i = 0; i < num; ++i, chunk += stride) {
        Zeta_RelLinkedListNode_Extract(ZETA_GetPtrFromAddr(chunk));
    }

    sa->num_of_vacant_units -= num;

    Zeta_Allocator* allocator = sa->allocator;
    ZETA_DebugAssert(allocator != NULL);

    allocator->Deallocate(allocator->context, vacant_slab);
}

void Zeta_SlabAllocator_ToAllocator(void* sa_, Zeta_Allocator* dst) {
    Zeta_SlabAllocator* sa = sa_;
    ZETA_DebugAssert(sa != NULL);

    ZETA_DebugAssert(dst != NULL);

    dst->context = sa;
    dst->GetAlign = Zeta_SlabAllocator_GetAlign;
    dst->Query = Zeta_SlabAllocator_Query;
    dst->Allocate = Zeta_SlabAllocator_Allocate;
    dst->Deallocate = Zeta_SlabAllocator_Deallocate;
}

void Zeta_SlabAllocator_Check(void* sa_,
                              Zeta_DebugTreeMap* dst_used_ptr_size_tm,
                              Zeta_DebugTreeMap* dst_released_ptr_size_tm) {
    Zeta_SlabAllocator* sa = sa_;
    ZETA_DebugAssert(sa != NULL);

    ZETA_DebugAssert(dst_used_ptr_size_tm != NULL);
    ZETA_DebugAssert(dst_released_ptr_size_tm != NULL);

    size_t align = sa->align;
    size_t width = sa->width;
    size_t stride = width + sizeof(uintptr_t);
    size_t num = sa->num;

    Zeta_DebugTreeMap vacant_unit_mt;
    Zeta_DebugTreeMap_Create(&vacant_unit_mt);

    Zeta_DebugTreeMap occupied_unit_mt;
    Zeta_DebugTreeMap_Create(&occupied_unit_mt);

    for (Zeta_RelLinkedListNode* hot_unit = &sa->hot_unit_head;;) {
        hot_unit = Zeta_RelLinkedListNode_GetR(hot_unit);
        if (hot_unit == &sa->hot_unit_head) { break; }

        Zeta_DebugTreeMap_KeyValPair p = Zeta_DebugTreeMap_Insert(
            &vacant_unit_mt, ZETA_GetAddrFromPtr(hot_unit));

        ZETA_DebugAssert(p.b);
    }

    size_t check_num_of_vacant_units = 0;
    size_t check_num_of_occupied_units = 0;

    for (Zeta_RelLinkedListNode* occupied_slab_n =
             &sa->occupied_slab_n_list_head;
         ;) {
        occupied_slab_n = Zeta_RelLinkedListNode_GetR(occupied_slab_n);
        if (occupied_slab_n == &sa->occupied_slab_n_list_head) { break; }

        SlabHead* occupied_slab_head =
            ZETA_GetStructFromMember(SlabHead, n, occupied_slab_n);

        Slab* occupied_slab = ZETA_GetSlabFromSlabHead(occupied_slab_head);

        uintptr_t first_chunk = ZETA_GetFirstChunkFromSlab(occupied_slab);
        uintptr_t chunk = first_chunk;

        size_t check_num = 0;

        for (size_t i = 0; i < num; ++i, chunk += stride) {
            ZETA_DebugAssert(chunk % align == 0);

            ZETA_DebugAssert(occupied_slab ==
                             GetPtr_(ZETA_GetPtrFromAddr(chunk + width)));

            if (Zeta_DebugTreeMap_Find(&vacant_unit_mt, chunk).b) {
                ++check_num;
            } else {
                Zeta_DebugTreeMap_KeyValPair p =
                    Zeta_DebugTreeMap_Insert(&occupied_unit_mt, chunk);

                ZETA_DebugAssert(p.b);

                *p.val = width;

                ++check_num_of_occupied_units;
            }
        }

        ZETA_DebugAssert(check_num == occupied_slab_head->num);

        check_num_of_vacant_units += check_num;

        size_t slab_size = chunk - ZETA_GetAddrFromPtr(occupied_slab_head);

        Zeta_DebugTreeMap_KeyValPair p = Zeta_DebugTreeMap_Insert(
            dst_used_ptr_size_tm, ZETA_GetAddrFromPtr(occupied_slab_head));

        ZETA_DebugAssert(p.b);

        *p.val = slab_size;
    }

    for (Zeta_RelLinkedListNode* vacant_slab_n = &sa->vacant_slab_n_list_head;
         ;) {
        vacant_slab_n = Zeta_RelLinkedListNode_GetR(vacant_slab_n);
        if (vacant_slab_n == &sa->vacant_slab_n_list_head) { break; }

        SlabHead* vacant_slab_head =
            ZETA_GetStructFromMember(SlabHead, n, vacant_slab_n);

        Slab* vacant_slab = ZETA_GetSlabFromSlabHead(vacant_slab_head);

        uintptr_t first_chunk = ZETA_GetFirstChunkFromSlab(vacant_slab);
        uintptr_t chunk = first_chunk;

        for (size_t i = 0; i < num; ++i, chunk += stride) {
            ZETA_DebugAssert(chunk % align == 0);

            ZETA_DebugAssert(vacant_slab ==
                             GetPtr_(ZETA_GetPtrFromAddr(chunk + width)));

            ZETA_DebugAssert(Zeta_DebugTreeMap_Find(&vacant_unit_mt, chunk).b);
        }

        check_num_of_vacant_units += num;

        size_t slab_size = chunk - ZETA_GetAddrFromPtr(vacant_slab_head);

        Zeta_DebugTreeMap_KeyValPair p = Zeta_DebugTreeMap_Insert(
            dst_used_ptr_size_tm, ZETA_GetAddrFromPtr(vacant_slab_head));

        ZETA_DebugAssert(p.b);

        *p.val = slab_size;
    }

    ZETA_DebugAssert(check_num_of_vacant_units == sa->num_of_vacant_units);
    ZETA_DebugAssert(check_num_of_occupied_units == sa->num_of_occupied_units);

    Zeta_DebugTreeMap_Move(dst_released_ptr_size_tm, &occupied_unit_mt);

    Zeta_DebugTreeMap_Destroy(&vacant_unit_mt);
    Zeta_DebugTreeMap_Destroy(&occupied_unit_mt);
}
