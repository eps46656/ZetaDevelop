#include "SlabAllocator.h"
#include "utils.h"

ZETA_StaticAssert(alignof(Zeta_SlabAllocator_SlabHead) % alignof(uintptr_t) ==
                  0);

#define ZETA_SlabSize (stride * num + sizeof(Zeta_SlabAllocator_SlabHead))

#define GetSlabHeadFromSlabUnit_(unit)                                         \
    ((Zeta_SlabAllocator_SlabHead*)((unsigned char*)(unit) +                   \
                                    stride * (num - *((unsigned char*)(unit) + \
                                                      width))))

#define GetSlabFromSlabHead_(slab_head) \
    ((void*)((unsigned char*)(slab_head)-stride * num))

#define ZETA_GetFirstChunkFromSlab(slab) ((unsigned char*)(slab))

static void* PushVacantSlab_(Zeta_SlabAllocator* sa, void* vacant_slab_n) {
    //

    if (sa->vacant) }

static void ReleaseSlab_(Zeta_SlabAllocator* sa, void* slab_n) {
    size_t width = sa->width;
    size_t stride = width + sizeof(unsigned char);
    size_t num = sa->num;

    Zeta_SlabAllocator_SlabHead* slab_head =
        ZETA_GetStructFromMember(Zeta_SlabAllocator_SlabHead, n, slab_n);
    void* slab = GetSlabFromSlabHead_(slab_head);

    Zeta_OrdLinkedListNode_Extract(slab_n);

    unsigned char* chunk = ZETA_GetFirstChunkFromSlab(slab);

    for (size_t i = 0; i < num; ++i, chunk += stride) {
        Zeta_OrdLinkedListNode_Extract(chunk);
    }

    sa->vacant_units_num -= num;

    ZETA_DebugAssert(sa->allocator != NULL);
    ZETA_DebugAssert(sa->allocator->Deallocate != NULL);

    sa->allocator->Deallocate(sa->allocator->context, slab);
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

    size_t stride = width + sizeof(unsigned char);

    if (stride < sizeof(Zeta_OrdLinkedListNode)) {
        stride = sizeof(Zeta_OrdLinkedListNode);
    }

    stride = Zeta_FindNextConMod(stride, 0, align);

    sa->width = stride - sizeof(unsigned char);

    Zeta_Allocator* allocator = sa->allocator;
    ZETA_DebugAssert(allocator != NULL);
    ZETA_DebugAssert(allocator->GetAlign != NULL);
    ZETA_DebugAssert(allocator->GetAlign(allocator->context) % align == 0);
    ZETA_DebugAssert(allocator->Query != NULL);
    ZETA_DebugAssert(allocator->Allocate != NULL);
    ZETA_DebugAssert(allocator->Deallocate != NULL);

    size_t num = sa->num;
    ZETA_DebugAssert(0 < sa->num);
    ZETA_DebugAssert(sa->num <= ZETA_SlabAllocator_max_num);

    size_t real_slab_size = allocator->Query(allocator->context, ZETA_SlabSize);

    num = (real_slab_size - sizeof(Zeta_SlabAllocator_SlabHead)) / stride;
    if (ZETA_SlabAllocator_max_num < num) { num = ZETA_SlabAllocator_max_num; }

    sa->num = num;

    sa->vacant_units_num = 0;
    sa->occupied_units_num = 0;

    Zeta_OrdLinkedListNode_Init(&sa->vacant_slab_n_list_head);
    Zeta_OrdLinkedListNode_Init(&sa->occupied_slab_n_list_head);

    Zeta_OrdLinkedListNode_Init(&sa->hot_unit_head);
}

void Zeta_SlabAllocator_Deinit(void* sa_) {
    Zeta_SlabAllocator* sa = sa_;
    ZETA_DebugAssert(sa != NULL);

    ZETA_DebugAssert(sa->occupied_units_num == 0);

    void* vacant_slab_n_list_head = &sa->vacant_slab_n_list_head;

    for (;;) {
        void* vacant_slab_n =
            Zeta_OrdLinkedListNode_GetR(vacant_slab_n_list_head);

        if (vacant_slab_n == vacant_slab_n_list_head) { break; }

        ReleaseSlab_(sa, vacant_slab_n);
    }
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
    size_t num = sa->num;

    if (size == 0 || width < size) { return NULL; }

    void* hot_unit_head = &sa->hot_unit_head;

    void* hot_unit_n = Zeta_OrdLinkedListNode_GetR(hot_unit_head);

    if (hot_unit_n != hot_unit_head) {
        Zeta_SlabAllocator_SlabHead* slab_head =
            GetSlabHeadFromSlabUnit_(hot_unit_n);

        Zeta_OrdLinkedListNode_Extract(&slab_head->n);
        Zeta_OrdLinkedListNode_InsertL(&sa->occupied_slab_n_list_head,
                                       &slab_head->n);

        --slab_head->vacant_units_num;

        Zeta_OrdLinkedListNode_Extract(hot_unit_n);

        --sa->vacant_units_num;
        ++sa->occupied_units_num;

        return hot_unit_n;
    }

    ZETA_DebugAssert(sa->allocator != NULL);
    ZETA_DebugAssert(sa->allocator->Allocate != NULL);

    void* slab = sa->allocator->Allocate(sa->allocator->context, ZETA_SlabSize);
    ZETA_DebugAssert(slab != NULL);
    ZETA_DebugAssert(ZETA_GetAddrFromPtr(slab) % sa->align == 0);

    Zeta_SlabAllocator_SlabHead* slab_head =
        (void*)((unsigned char*)slab + stride * num);

    Zeta_OrdLinkedListNode_Init(&slab_head->n);
    Zeta_OrdLinkedListNode_InsertL(&sa->occupied_slab_n_list_head,
                                   &slab_head->n);

    slab_head->vacant_units_num = num - 1;

    unsigned char* first_chunk = ZETA_GetFirstChunkFromSlab(slab);
    *(first_chunk + width) = 0;

    unsigned char* chunk = first_chunk + stride;

    for (size_t i = 1; i < num; ++i, chunk += stride) {
        Zeta_OrdLinkedListNode_Init(chunk);
        Zeta_OrdLinkedListNode_InsertL(hot_unit_head, chunk);

        *(chunk + width) = i;
    }

    sa->vacant_units_num += num - 1;
    ++sa->occupied_units_num;

    return first_chunk;
}

void Zeta_SlabAllocator_Deallocate(void* sa_, void* ptr) {
    Zeta_SlabAllocator* sa = sa_;
    ZETA_DebugAssert(sa != NULL);

    if (ptr == NULL) { return; }

    size_t width = sa->width;
    size_t stride = width + sizeof(unsigned char);
    size_t num = sa->num;

    Zeta_OrdLinkedListNode_Init(ptr);
    Zeta_OrdLinkedListNode_InsertR(&sa->hot_unit_head, ptr);

    Zeta_SlabAllocator_SlabHead* slab_head = GetSlabHeadFromSlabUnit_(ptr);

    ++slab_head->vacant_units_num;

    ++sa->vacant_units_num;
    --sa->occupied_units_num;

    if (slab_head->vacant_units_num == num) {
        Zeta_OrdLinkedListNode_Extract(&slab_head->n);
        Zeta_OrdLinkedListNode_InsertL(&sa->vacant_slab_n_list_head,
                                       &slab_head->n);
    }

    if (sa->vacant_units_num - sa->num < sa->buffer_units_num) { return; }

    void* vacant_slab_n_list_head = &sa->vacant_slab_n_list_head;

    void* vacant_slab_n = Zeta_OrdLinkedListNode_GetR(vacant_slab_n_list_head);

    if (vacant_slab_n != vacant_slab_n_list_head) {
        ReleaseSlab_(sa, vacant_slab_n);
    }
}

void Zeta_SlabAllocator_DeployAllocator(void* sa_, Zeta_Allocator* dst) {
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

    size_t width = sa->width;
    size_t stride = width + sizeof(unsigned char);
    size_t num = sa->num;

    Zeta_DebugTreeMap vacant_unit_mt;
    Zeta_DebugTreeMap_Create(&vacant_unit_mt);

    Zeta_DebugTreeMap occupied_unit_mt;
    Zeta_DebugTreeMap_Create(&occupied_unit_mt);

    for (Zeta_OrdLinkedListNode* hot_unit = &sa->hot_unit_head;;) {
        hot_unit = Zeta_OrdLinkedListNode_GetR(hot_unit);
        if (hot_unit == &sa->hot_unit_head) { break; }

        ZETA_DebugAssert(Zeta_DebugTreeMap_Insert(&vacant_unit_mt,
                                                  ZETA_GetAddrFromPtr(hot_unit))
                             .b);
    }

    size_t check_vacant_units_num = 0;
    size_t check_occupied_units_num = 0;

    for (void* occupied_slab_n = &sa->occupied_slab_n_list_head;;) {
        occupied_slab_n = Zeta_OrdLinkedListNode_GetR(occupied_slab_n);
        if (occupied_slab_n == &sa->occupied_slab_n_list_head) { break; }

        Zeta_SlabAllocator_SlabHead* occupied_slab_head =
            ZETA_GetStructFromMember(Zeta_SlabAllocator_SlabHead, n,
                                     occupied_slab_n);

        void* occupied_slab = GetSlabFromSlabHead_(occupied_slab_head);

        unsigned char* first_chunk = ZETA_GetFirstChunkFromSlab(occupied_slab);
        unsigned char* chunk = first_chunk;

        size_t check_vacant_units_num = 0;

        for (size_t i = 0; i < num; ++i, chunk += stride) {
            ZETA_DebugAssert(i == *(chunk + width));

            if (Zeta_DebugTreeMap_Find(&vacant_unit_mt,
                                       ZETA_GetAddrFromPtr(chunk))
                    .b) {
                ++check_vacant_units_num;
            } else {
                Zeta_DebugTreeMap_KeyValPair p = Zeta_DebugTreeMap_Insert(
                    &occupied_unit_mt, ZETA_GetAddrFromPtr(chunk));

                ZETA_DebugAssert(p.b);

                *p.val = width;

                ++check_occupied_units_num;
            }
        }

        ZETA_DebugAssert(check_vacant_units_num ==
                         occupied_slab_head->vacant_units_num);

        check_vacant_units_num += check_vacant_units_num;

        size_t slab_size = ZETA_GetAddrFromPtr(chunk) +
                           sizeof(Zeta_SlabAllocator_SlabHead) -
                           ZETA_GetAddrFromPtr(occupied_slab);

        Zeta_DebugTreeMap_KeyValPair p = Zeta_DebugTreeMap_Insert(
            dst_used_ptr_size_tm, ZETA_GetAddrFromPtr(occupied_slab_head));

        ZETA_DebugAssert(p.b);

        *p.val = slab_size;
    }

    for (void* vacant_slab_n = &sa->vacant_slab_n_list_head;;) {
        vacant_slab_n = Zeta_OrdLinkedListNode_GetR(vacant_slab_n);
        if (vacant_slab_n == &sa->vacant_slab_n_list_head) { break; }

        Zeta_SlabAllocator_SlabHead* vacant_slab_head =
            ZETA_GetStructFromMember(Zeta_SlabAllocator_SlabHead, n,
                                     vacant_slab_n);

        void* vacant_slab = GetSlabFromSlabHead_(vacant_slab_head);

        unsigned char* first_chunk = ZETA_GetFirstChunkFromSlab(vacant_slab);
        unsigned char* chunk = first_chunk;

        for (size_t i = 0; i < num; ++i, chunk += stride) {
            ZETA_DebugAssert(i == *(chunk + width));
            ZETA_DebugAssert(Zeta_DebugTreeMap_Find(&vacant_unit_mt,
                                                    ZETA_GetAddrFromPtr(chunk))
                                 .b);
        }

        check_vacant_units_num += num;

        size_t slab_size = ZETA_GetAddrFromPtr(chunk) +
                           sizeof(Zeta_SlabAllocator_SlabHead) -
                           ZETA_GetAddrFromPtr(vacant_slab);

        Zeta_DebugTreeMap_KeyValPair p = Zeta_DebugTreeMap_Insert(
            dst_used_ptr_size_tm, ZETA_GetAddrFromPtr(vacant_slab_head));

        ZETA_DebugAssert(p.b);

        *p.val = slab_size;
    }

    ZETA_DebugAssert(check_vacant_units_num == sa->vacant_units_num);
    ZETA_DebugAssert(check_occupied_units_num == sa->occupied_units_num);

    Zeta_DebugTreeMap_Move(dst_released_ptr_size_tm, &occupied_unit_mt);

    Zeta_DebugTreeMap_Destroy(&vacant_unit_mt);
    Zeta_DebugTreeMap_Destroy(&occupied_unit_mt);
}