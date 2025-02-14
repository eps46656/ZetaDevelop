#include "slab_allocator.h"

#include "debugger.h"
#include "utils.h"

ZETA_StaticAssert(alignof(Zeta_SlabAllocator_SlabHead) % alignof(uintptr_t) ==
                  0);

#define ZETA_SlabSize \
    (stride * units_per_slab + sizeof(Zeta_SlabAllocator_SlabHead))

#define SlabUnitToSlabHead_(unit)                         \
    ((Zeta_SlabAllocator_SlabHead*)((unit) +              \
                                    stride *              \
                                        (units_per_slab - \
                                         *(unsigned char*)((unit) + width))))

#define SlabHeadToSlab_(slab_head) \
    ((void*)((void*)(slab_head) - stride * units_per_slab))

#define ZETA_GetFirstChunkFromSlab(slab) ((void*)(slab))

static void LListPushL_(Zeta_OrdLListNode** head, Zeta_OrdLListNode* n) {
    if (*head != NULL) { Zeta_LList_OrdLListNode_InsertL(*head, n); }
    *head = n;
}

static Zeta_OrdLListNode* LListPopL_(Zeta_OrdLListNode** head) {
    Zeta_OrdLListNode* n = *head;
    Zeta_OrdLListNode* nr = Zeta_OrdLListNode_GetR(n);

    if (nr == n) {
        *head = NULL;
    } else {
        *head = nr;
        Zeta_LList_OrdLListNode_Extract(n);
    }

    return n;
}

static Zeta_OrdLListNode* LListPopR_(Zeta_OrdLListNode** head) {
    Zeta_OrdLListNode* n = *head;
    Zeta_OrdLListNode* nl = Zeta_OrdLListNode_GetL(n);

    if (nl == n) {
        *head = NULL;
    } else {
        Zeta_LList_OrdLListNode_Extract(nl);
    }

    return nl;
}

static void LListExtract_(Zeta_OrdLListNode** head, Zeta_OrdLListNode* n) {
    if (*head == n) {
        LListPopL_(head);
    } else {
        Zeta_LList_OrdLListNode_Extract(n);
    }
}

static bool_t ReleaseLastSlab_(Zeta_SlabAllocator* sa) {
    if (sa->vacant_slab_list == NULL) { return FALSE; }

    Zeta_OrdLListNode* vacant_slab_n = LListPopR_(&sa->vacant_slab_list);

    size_t width = sa->width;
    size_t stride = width + sizeof(unsigned char);
    size_t units_per_slab = sa->units_per_slab;

    Zeta_SlabAllocator_SlabHead* vacant_slab_head =
        ZETA_MemberToStruct(Zeta_SlabAllocator_SlabHead, n, vacant_slab_n);
    void* vacant_slab = SlabHeadToSlab_(vacant_slab_head);

    void* chunk = ZETA_GetFirstChunkFromSlab(vacant_slab);

    for (size_t i = 0; i < units_per_slab; ++i, chunk += stride) {
        LListExtract_(&sa->hot_slab_units_list, (void*)chunk);
    }

    sa->vacant_units_cnt -= units_per_slab;

    ZETA_Allocator_Deallocate(sa->origin_allocator, vacant_slab);

    return TRUE;
}

void Zeta_SlabAllocator_Init(void* sa_) {
    Zeta_SlabAllocator* sa = sa_;
    ZETA_DebugAssert(sa != NULL);

    size_t align = sa->align;
    ZETA_DebugAssert(0 < align);

    sa->align = align = Zeta_LCM(align, alignof(uintptr_t));

    size_t width = sa->width;
    ZETA_DebugAssert(0 < width);

    size_t stride = ZETA_IntRoundUp(
        ZETA_GetMinOf(sizeof(Zeta_OrdLListNode), width + sizeof(unsigned char)),
        align);

    sa->width = stride - sizeof(unsigned char);

    ZETA_Allocator_Check(sa->origin_allocator, align);

    size_t units_per_slab = sa->units_per_slab;
    ZETA_DebugAssert(0 < sa->units_per_slab);
    ZETA_DebugAssert(sa->units_per_slab <=
                     ZETA_SlabAllocator_max_units_per_slab);

    size_t real_slab_size =
        ZETA_Allocator_Query(sa->origin_allocator, ZETA_SlabSize);

    sa->units_per_slab = units_per_slab = ZETA_GetMinOf(
        ZETA_SlabAllocator_max_units_per_slab,
        (real_slab_size - sizeof(Zeta_SlabAllocator_SlabHead)) / stride);

    sa->vacant_units_cnt = 0;
    sa->occupied_units_cnt = 0;

    sa->hot_slab_units_list = NULL;

    sa->vacant_slab_list = NULL;
    sa->occupied_slab_list = NULL;
}

void Zeta_SlabAllocator_Deinit(void* sa_) {
    Zeta_SlabAllocator* sa = sa_;
    ZETA_DebugAssert(sa != NULL);

    ZETA_DebugAssert(sa->occupied_units_cnt == 0);

    while (ReleaseLastSlab_(sa)) {}
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
    size_t stride = width + sizeof(unsigned char);
    size_t units_per_slab = sa->units_per_slab;

    if (size == 0 || width < size) { return NULL; }

    if (sa->hot_slab_units_list != NULL) {
        Zeta_SlabAllocator_SlabHead* slab_head =
            SlabUnitToSlabHead_(sa->hot_slab_units_list);

        if (slab_head->vacant_units_cnt == units_per_slab) {
            LListExtract_(&sa->vacant_slab_list, &slab_head->n);
            LListPushL_(&sa->occupied_slab_list, &slab_head->n);
        }

        --slab_head->vacant_units_cnt;

        --sa->vacant_units_cnt;
        ++sa->occupied_units_cnt;

        return LListPopL_(&sa->hot_slab_units_list);
    }

    void* slab = ZETA_Allocator_SafeAllocate(sa->origin_allocator, sa->align,
                                             ZETA_SlabSize);

    Zeta_SlabAllocator_SlabHead* slab_head = slab + stride * units_per_slab;

    Zeta_OrdLListNode_Init(&slab_head->n);
    LListPushL_(&sa->occupied_slab_list, &slab_head->n);

    slab_head->vacant_units_cnt = units_per_slab - 1;

    sa->vacant_units_cnt += units_per_slab - 1;
    ++sa->occupied_units_cnt;

    unsigned char* first_chunk = ZETA_GetFirstChunkFromSlab(slab);
    *(first_chunk + width) = 0;

    unsigned char* chunk = first_chunk + stride * (units_per_slab - 1);

    for (size_t i = units_per_slab - 1; 0 < i; --i, chunk -= stride) {
        Zeta_OrdLListNode_Init(chunk);
        LListPushL_(&sa->hot_slab_units_list, (void*)chunk);

        *(chunk + width) = i;
    }

    return first_chunk;
}

void Zeta_SlabAllocator_Deallocate(void* sa_, void* ptr) {
    Zeta_SlabAllocator* sa = sa_;
    ZETA_DebugAssert(sa != NULL);

    if (ptr == NULL) { return; }

    size_t width = sa->width;
    size_t stride = width + sizeof(unsigned char);
    size_t units_per_slab = sa->units_per_slab;

    Zeta_OrdLListNode_Init(ptr);
    LListPushL_(&sa->hot_slab_units_list, ptr);

    Zeta_SlabAllocator_SlabHead* slab_head = SlabUnitToSlabHead_(ptr);

    ++slab_head->vacant_units_cnt;

    ++sa->vacant_units_cnt;
    --sa->occupied_units_cnt;

    if (slab_head->vacant_units_cnt == units_per_slab) {
        LListExtract_(&sa->occupied_slab_list, &slab_head->n);
        LListPushL_(&sa->vacant_slab_list, &slab_head->n);
    }

    if (sa->buffer_units_cnt <= sa->vacant_units_cnt - sa->units_per_slab) {
        ReleaseLastSlab_(sa);
    }
}

bool_t Zeta_SlabAllocator_ReleaseBuffer(void* sa_) {
    Zeta_SlabAllocator* sa = sa_;
    ZETA_DebugAssert(sa != NULL);

    return ReleaseLastSlab_(sa);
}

void Zeta_SlabAllocator_Check(void* sa_, Zeta_MemRecorder* dst_used_records,
                              Zeta_MemRecorder* dst_released_records) {
    Zeta_SlabAllocator* sa = sa_;
    ZETA_DebugAssert(sa != NULL);

    ZETA_DebugAssert(dst_used_records != NULL);
    ZETA_DebugAssert(dst_released_records != NULL);

#if ZETA_EnableDebug
    size_t width = sa->width;
    size_t stride = width + sizeof(unsigned char);
    size_t units_per_slab = sa->units_per_slab;

    Zeta_MemRecorder* vacant_unit = Zeta_MemRecorder_Create();
    Zeta_MemRecorder* occupied_unit = Zeta_MemRecorder_Create();

    if (sa->hot_slab_units_list != NULL) {
        Zeta_OrdLListNode* hot_unit = sa->hot_slab_units_list;

        for (;;) {
            Zeta_MemRecorder_Record(vacant_unit, hot_unit, 0);

            hot_unit = Zeta_OrdLListNode_GetR(hot_unit);
            if (hot_unit == sa->hot_slab_units_list) { break; }
        }
    }

    size_t check_vacant_units_cnt = 0;
    size_t check_occupied_units_cnt = 0;

    if (sa->occupied_slab_list != NULL) {
        void* occupied_slab_n = sa->occupied_slab_list;

        for (;;) {
            Zeta_SlabAllocator_SlabHead* occupied_slab_head =
                ZETA_MemberToStruct(Zeta_SlabAllocator_SlabHead, n,
                                    occupied_slab_n);

            void* occupied_slab = SlabHeadToSlab_(occupied_slab_head);

            unsigned char* first_chunk =
                ZETA_GetFirstChunkFromSlab(occupied_slab);
            unsigned char* chunk = first_chunk;

            size_t check_slab_vacant_units_cnt = 0;

            for (size_t i = 0; i < units_per_slab; ++i, chunk += stride) {
                ZETA_DebugAssert(i == *(chunk + width));

                if (Zeta_MemRecorder_GetRecordSize(vacant_unit, chunk) ==
                    ZETA_SIZE_MAX) {
                    Zeta_MemRecorder_Record(occupied_unit, chunk, width);
                    Zeta_MemRecorder_Record(dst_released_records, chunk, width);
                    ++check_occupied_units_cnt;
                } else {
                    ++check_slab_vacant_units_cnt;
                }
            }

            ZETA_DebugAssert(check_slab_vacant_units_cnt ==
                             occupied_slab_head->vacant_units_cnt);

            check_vacant_units_cnt += check_slab_vacant_units_cnt;

            Zeta_MemRecorder_Record(dst_used_records, occupied_slab_head,
                                    ZETA_PtrToAddr(chunk) +
                                        sizeof(Zeta_SlabAllocator_SlabHead) -
                                        ZETA_PtrToAddr(occupied_slab));

            occupied_slab_n = Zeta_OrdLListNode_GetR(occupied_slab_n);
            if (occupied_slab_n == sa->occupied_slab_list) { break; }
        }
    }

    if (sa->vacant_slab_list != NULL) {
        void* vacant_slab_n = sa->vacant_slab_list;

        for (;;) {
            Zeta_SlabAllocator_SlabHead* vacant_slab_head = ZETA_MemberToStruct(
                Zeta_SlabAllocator_SlabHead, n, vacant_slab_n);

            void* vacant_slab = SlabHeadToSlab_(vacant_slab_head);

            unsigned char* first_chunk =
                ZETA_GetFirstChunkFromSlab(vacant_slab);
            unsigned char* chunk = first_chunk;

            for (size_t i = 0; i < units_per_slab; ++i, chunk += stride) {
                ZETA_DebugAssert(i == *(chunk + width));
                ZETA_DebugAssert(Zeta_MemRecorder_GetRecordSize(
                                     vacant_unit, chunk) != ZETA_SIZE_MAX);
            }

            check_vacant_units_cnt += units_per_slab;

            Zeta_MemRecorder_Record(dst_used_records, vacant_slab_head,
                                    ZETA_PtrToAddr(chunk) +
                                        sizeof(Zeta_SlabAllocator_SlabHead) -
                                        ZETA_PtrToAddr(vacant_slab));

            vacant_slab_n = Zeta_OrdLListNode_GetR(vacant_slab_n);
            if (vacant_slab_n == sa->vacant_slab_list) { break; }
        }
    }

    ZETA_DebugAssert(check_vacant_units_cnt == sa->vacant_units_cnt);
    ZETA_DebugAssert(check_occupied_units_cnt == sa->occupied_units_cnt);

    Zeta_MemRecorder_Destroy(vacant_unit);
    Zeta_MemRecorder_Destroy(occupied_unit);
#endif
}

Zeta_Allocator_VTable const zeta_slab_allocator = {
    .GetAlign = Zeta_SlabAllocator_GetAlign,

    .Query = Zeta_SlabAllocator_Query,

    .Allocate = Zeta_SlabAllocator_Allocate,

    .Deallocate = Zeta_SlabAllocator_Deallocate,
};
