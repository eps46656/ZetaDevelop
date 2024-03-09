#include "SlabAllocator.h"
#include "utils.h"

typedef struct SlabHead_ SlabHead_;

struct SlabHead_ {
    Zeta_RelLinkedListNode n;
    u8_t num;
};

typedef struct SlabHead SlabHead;

struct SlabHead {
    Zeta_RelLinkedListNode n;
    u8_t num;
};

typedef struct Slab Slab;

struct Slab {};

void* GetPtr_(uintptr_t* ptr) {
    return ZETA_UINT_TO_PTR(ZETA_PTR_TO_UINT(ptr) + *ptr);
}

void SetPtr_(uintptr_t* ptr, void* val) {
    *ptr = ZETA_PTR_TO_UINT(val) - ZETA_PTR_TO_UINT(ptr);
}

#define ZETA_GetHeadFromSlab(slab) (ZETA_PTR_TO_UINT(slab) - sizeof(Head))

#define ZETA_GetFirstChunkFromSlab(slab) \
    (ZETA_PTR_TO_UINT(slab) - sizeof(Head) - stride * num)

// static Head* GetHeadFromSlab_(Slab* slab);

void Zeta_SlabAllocator_Init(void* sa_) {
    Zeta_SlabAllocator* sa = sa_;
    ZETA_DebugAssert(sa != NULL);

    size_t align = sa->align;
    ZETA_DebugAssert(0 < align);

    align = Zeta_GetLCM(align, alignof(SlabHead_));
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

    ZETA_DebugAssert(0 < sa->num);

    Zeta_Allocator* allocator = sa->allocator;
    ZETA_DebugAssert(allocator != NULL);

    ZETA_DebugAssert(allocator->GetAlign(allocator->context) % align == 0);

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
    size_t head_size = Zeta_FindNextConMod(sizeof(SlabHead_), 0, sa->align);

    if (size == 0 || width < size) { return NULL; }

    Zeta_RelLinkedListNode* hot_unit_head = &sa->hot_unit_head;

    Zeta_RelLinkedListNode* hot_unit_n =
        Zeta_RelLinkedListNode_GetR(hot_unit_head);

    if (hot_unit_n == hot_unit_head) {
        Zeta_Allocator* allocator = sa->allocator;
        ZETA_DebugAssert(allocator != NULL);

        SlabHead_* slab_head =
            allocator->Allocate(allocator->context, head_size + stride * num);

        ZETA_DebugAssert(slab_head != NULL);

        Zeta_RelLinkedListNode_Init(&slab_head->n);
        Zeta_RelLinkedListNode_InsertL(&sa->occupied_slab_n_list_head,
                                       &slab_head->n);

        slab_head->num = num - 1;

        uintptr_t first_chunk = ZETA_PTR_TO_UINT(slab_head) + head_size;
        uintptr_t chunk = first_chunk;

        for (size_t i = 0; i < num; ++i, chunk += stride) {
            Zeta_RelLinkedListNode_Init(ZETA_UINT_TO_PTR(chunk));

            Zeta_RelLinkedListNode_InsertL(ZETA_UINT_TO_PTR(hot_unit_head),
                                           ZETA_UINT_TO_PTR(chunk));

            SetPtr_(ZETA_UINT_TO_PTR(chunk + width), slab_head);
        }

        sa->num_of_vacant_units += num;

        hot_unit_n = ZETA_UINT_TO_PTR(first_chunk);
    } else {
        uintptr_t* hot_unit_back_ptr =
            ZETA_UINT_TO_PTR(ZETA_PTR_TO_UINT(hot_unit_n) + width);

        SlabHead_* slab_head = GetPtr_(hot_unit_back_ptr);

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
    size_t num = sa->num;

    Zeta_RelLinkedListNode_Init(ptr);
    Zeta_RelLinkedListNode_InsertR(&sa->hot_unit_head, ptr);

    SlabHead_* slab_head =
        GetPtr_(ZETA_UINT_TO_PTR(ZETA_PTR_TO_UINT(ptr) + width));

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

    SlabHead_* vacant_slab_head =
        ZETA_GetStructFromMember(SlabHead_, n, vacant_slab_n);

    size_t stride = width + sizeof(uintptr_t);
    size_t head_size = Zeta_FindNextConMod(sizeof(SlabHead_), 0, sa->align);

    Zeta_RelLinkedListNode_Extract(vacant_slab_n);

    uintptr_t chunk = ZETA_PTR_TO_UINT(vacant_slab_head) + head_size;

    for (size_t i = 0; i < num; ++i, chunk += stride) {
        Zeta_RelLinkedListNode_Extract(ZETA_UINT_TO_PTR(chunk));
    }

    sa->num_of_vacant_units -= num;

    Zeta_Allocator* allocator = sa->allocator;
    ZETA_DebugAssert(allocator != NULL);

    allocator->Deallocate(allocator->context, vacant_slab_head);
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
    size_t head_size = Zeta_FindNextConMod(sizeof(SlabHead_), 0, align);

    Zeta_DebugTreeMap vacant_unit_mt;
    Zeta_DebugTreeMap_Create(&vacant_unit_mt);

    Zeta_DebugTreeMap occupied_unit_mt;
    Zeta_DebugTreeMap_Create(&occupied_unit_mt);

    for (Zeta_RelLinkedListNode* hot_unit = &sa->hot_unit_head;;) {
        hot_unit = Zeta_RelLinkedListNode_GetR(hot_unit);
        if (hot_unit == &sa->hot_unit_head) { break; }

        Zeta_DebugTreeMap_KeyValPair p = Zeta_DebugTreeMap_Insert(
            &vacant_unit_mt, ZETA_PTR_TO_UINT(hot_unit));

        ZETA_DebugAssert(p.b);
    }

    size_t check_num_of_vacant_units = 0;
    size_t check_num_of_occupied_units = 0;

    for (Zeta_RelLinkedListNode* occupied_slab_n =
             &sa->occupied_slab_n_list_head;
         ;) {
        occupied_slab_n = Zeta_RelLinkedListNode_GetR(occupied_slab_n);
        if (occupied_slab_n == &sa->occupied_slab_n_list_head) { break; }

        SlabHead_* occupied_slab_head =
            ZETA_GetStructFromMember(SlabHead_, n, occupied_slab_n);

        uintptr_t first_chunk =
            ZETA_PTR_TO_UINT(occupied_slab_head) + head_size;
        uintptr_t chunk = first_chunk;

        size_t check_num = 0;

        for (size_t i = 0; i < num; ++i, chunk += stride) {
            ZETA_DebugAssert(chunk % align == 0);

            ZETA_DebugAssert(occupied_slab_head ==
                             GetPtr_(ZETA_UINT_TO_PTR(chunk + width)));

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

        size_t slab_size = chunk - ZETA_PTR_TO_UINT(occupied_slab_head);

        Zeta_DebugTreeMap_KeyValPair p = Zeta_DebugTreeMap_Insert(
            dst_used_ptr_size_tm, ZETA_PTR_TO_UINT(occupied_slab_head));

        ZETA_DebugAssert(p.b);

        *p.val = slab_size;
    }

    for (Zeta_RelLinkedListNode* vacant_slab_n = &sa->vacant_slab_n_list_head;
         ;) {
        vacant_slab_n = Zeta_RelLinkedListNode_GetR(vacant_slab_n);
        if (vacant_slab_n == &sa->vacant_slab_n_list_head) { break; }

        SlabHead_* vacant_slab_head =
            ZETA_GetStructFromMember(SlabHead_, n, vacant_slab_n);

        uintptr_t first_chunk = ZETA_PTR_TO_UINT(vacant_slab_head) + head_size;
        uintptr_t chunk = first_chunk;

        for (size_t i = 0; i < num; ++i, chunk += stride) {
            ZETA_DebugAssert(chunk % align == 0);

            ZETA_DebugAssert(vacant_slab_head ==
                             GetPtr_(ZETA_UINT_TO_PTR(chunk + width)));

            ZETA_DebugAssert(Zeta_DebugTreeMap_Find(&vacant_unit_mt, chunk).b);
        }

        check_num_of_vacant_units += num;

        size_t slab_size = chunk - ZETA_PTR_TO_UINT(vacant_slab_head);

        Zeta_DebugTreeMap_KeyValPair p = Zeta_DebugTreeMap_Insert(
            dst_used_ptr_size_tm, ZETA_PTR_TO_UINT(vacant_slab_head));

        ZETA_DebugAssert(p.b);

        *p.val = slab_size;
    }

    ZETA_DebugAssert(check_num_of_vacant_units == sa->num_of_vacant_units);
    ZETA_DebugAssert(check_num_of_occupied_units == sa->num_of_occupied_units);

    Zeta_DebugTreeMap_Move(dst_released_ptr_size_tm, &occupied_unit_mt);

    Zeta_DebugTreeMap_Destroy(&vacant_unit_mt);
    Zeta_DebugTreeMap_Destroy(&occupied_unit_mt);
}
