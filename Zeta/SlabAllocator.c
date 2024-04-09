#include "SlabAllocator.h"

#include "utils.h"

typedef struct SlabHead SlabHead;

struct SlabHead {
    Zeta_OrdLinkedListNode n;
    unsigned char num;
};

ZETA_StaticAssert(alignof(SlabHead) % alignof(uintptr_t) == 0);

#define ZETA_SlabSize (stride * num + sizeof(SlabHead))

#define GetSlabHeadFromSlabUnit_(unit)    \
    ((SlabHead*)((unsigned char*)(unit) + \
                 stride * (num - *((unsigned char*)(unit) + width))))

#define GetSlabFromSlabHead_(slab_head) \
    ((void*)((unsigned char*)(slab_head)-stride * num))

#define ZETA_GetFirstChunkFromSlab(slab) ((unsigned char*)(slab))

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

    sa->width = width = stride - sizeof(unsigned char);

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

    Zeta_OrdLinkedListNode_Init(&sa->vacant_slab_n_list_head);
    Zeta_OrdLinkedListNode_Init(&sa->occupied_slab_n_list_head);

    Zeta_OrdLinkedListNode_Init(&sa->hot_unit_head);
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
    size_t stride = width + sizeof(SlabHead*);

    if (size == 0 || width < size) { return NULL; }

    Zeta_OrdLinkedListNode* hot_unit_head = &sa->hot_unit_head;

    Zeta_OrdLinkedListNode* hot_unit_n =
        Zeta_OrdLinkedListNode_GetR(hot_unit_head);

    if (hot_unit_n == hot_unit_head) {
        Zeta_Allocator* allocator = sa->allocator;
        ZETA_DebugAssert(allocator != NULL);

        void* slab = allocator->Allocate(allocator->context, ZETA_SlabSize);

        ZETA_DebugAssert(slab != NULL);

        SlabHead* slab_head = (SlabHead*)((unsigned char*)slab + stride * num);

        Zeta_OrdLinkedListNode_Init(&slab_head->n);
        Zeta_OrdLinkedListNode_InsertL(&sa->occupied_slab_n_list_head,
                                       &slab_head->n);

        slab_head->num = num - 1;

        unsigned char* first_chunk = ZETA_GetFirstChunkFromSlab(slab);
        unsigned char* chunk = first_chunk;

        for (size_t i = 0; i < num; ++i, chunk += stride) {
            Zeta_OrdLinkedListNode_Init(chunk);
            Zeta_OrdLinkedListNode_InsertL(hot_unit_head, chunk);

            *(chunk + width) = i;
        }

        sa->num_of_vacant_units += num;

        hot_unit_n = (Zeta_OrdLinkedListNode*)first_chunk;
    } else {
        SlabHead* slab_head = GetSlabHeadFromSlabUnit_(hot_unit_n);

        Zeta_OrdLinkedListNode_Extract(&slab_head->n);
        Zeta_OrdLinkedListNode_InsertL(&sa->occupied_slab_n_list_head,
                                       &slab_head->n);

        --slab_head->num;
    }

    Zeta_OrdLinkedListNode_Extract(hot_unit_n);

    --sa->num_of_vacant_units;
    ++sa->num_of_occupied_units;

    return hot_unit_n;
}

void Zeta_SlabAllocator_Deallocate(void* sa_, void* ptr) {
    Zeta_SlabAllocator* sa = sa_;
    ZETA_DebugAssert(sa != NULL);

    if (ptr == NULL) { return; }

    size_t width = sa->width;
    size_t stride = width + sizeof(SlabHead*);
    size_t num = sa->num;

    Zeta_OrdLinkedListNode_Init(ptr);
    Zeta_OrdLinkedListNode_InsertR(&sa->hot_unit_head, ptr);

    SlabHead* slab_head = GetSlabHeadFromSlabUnit_(ptr);

    ++slab_head->num;

    ++sa->num_of_vacant_units;
    --sa->num_of_occupied_units;

    if (slab_head->num == num) {
        Zeta_OrdLinkedListNode_Extract(&slab_head->n);
        Zeta_OrdLinkedListNode_InsertL(&sa->vacant_slab_n_list_head,
                                       &slab_head->n);
    }

    if (4 * sa->num_of_vacant_units < sa->num_of_occupied_units) { return; }

    Zeta_OrdLinkedListNode* vacant_slab_n_list_head =
        &sa->vacant_slab_n_list_head;

    Zeta_OrdLinkedListNode* vacant_slab_n =
        Zeta_OrdLinkedListNode_GetR(vacant_slab_n_list_head);

    if (vacant_slab_n == vacant_slab_n_list_head) { return; }

    SlabHead* vacant_slab_head =
        ZETA_GetStructFromMember(SlabHead, n, vacant_slab_n);

    void* vacant_slab = GetSlabFromSlabHead_(vacant_slab_head);

    Zeta_OrdLinkedListNode_Extract(vacant_slab_n);

    unsigned char* chunk = ZETA_GetFirstChunkFromSlab(vacant_slab);

    for (size_t i = 0; i < num; ++i, chunk += stride) {
        Zeta_OrdLinkedListNode_Extract(chunk);
    }

    sa->num_of_vacant_units -= num;

    Zeta_Allocator* allocator = sa->allocator;
    ZETA_DebugAssert(allocator != NULL);

    allocator->Deallocate(allocator->context, vacant_slab);
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
    size_t stride = width + sizeof(SlabHead*);
    size_t num = sa->num;

    Zeta_DebugTreeMap vacant_unit_mt;
    Zeta_DebugTreeMap_Create(&vacant_unit_mt);

    Zeta_DebugTreeMap occupied_unit_mt;
    Zeta_DebugTreeMap_Create(&occupied_unit_mt);

    for (Zeta_OrdLinkedListNode* hot_unit = &sa->hot_unit_head;;) {
        hot_unit = Zeta_OrdLinkedListNode_GetR(hot_unit);
        if (hot_unit == &sa->hot_unit_head) { break; }

        Zeta_DebugTreeMap_KeyValPair p = Zeta_DebugTreeMap_Insert(
            &vacant_unit_mt, ZETA_GetAddrFromPtr(hot_unit));

        ZETA_DebugAssert(p.b);
    }

    size_t check_num_of_vacant_units = 0;
    size_t check_num_of_occupied_units = 0;

    for (Zeta_OrdLinkedListNode* occupied_slab_n =
             &sa->occupied_slab_n_list_head;
         ;) {
        occupied_slab_n = Zeta_OrdLinkedListNode_GetR(occupied_slab_n);
        if (occupied_slab_n == &sa->occupied_slab_n_list_head) { break; }

        SlabHead* occupied_slab_head =
            ZETA_GetStructFromMember(SlabHead, n, occupied_slab_n);

        void* occupied_slab = GetSlabFromSlabHead_(occupied_slab_head);

        unsigned char* first_chunk = ZETA_GetFirstChunkFromSlab(occupied_slab);
        unsigned char* chunk = first_chunk;

        size_t check_num = 0;

        for (size_t i = 0; i < num; ++i, chunk += stride) {
            ZETA_DebugAssert(i == *(chunk + width));

            if (Zeta_DebugTreeMap_Find(&vacant_unit_mt,
                                       ZETA_GetAddrFromPtr(chunk))
                    .b) {
                ++check_num;
            } else {
                Zeta_DebugTreeMap_KeyValPair p = Zeta_DebugTreeMap_Insert(
                    &occupied_unit_mt, ZETA_GetAddrFromPtr(chunk));

                ZETA_DebugAssert(p.b);

                *p.val = width;

                ++check_num_of_occupied_units;
            }
        }

        ZETA_DebugAssert(check_num == occupied_slab_head->num);

        check_num_of_vacant_units += check_num;

        size_t slab_size = chunk - (unsigned char*)(occupied_slab_head);

        Zeta_DebugTreeMap_KeyValPair p = Zeta_DebugTreeMap_Insert(
            dst_used_ptr_size_tm, ZETA_GetAddrFromPtr(occupied_slab_head));

        ZETA_DebugAssert(p.b);

        *p.val = slab_size;
    }

    for (Zeta_OrdLinkedListNode* vacant_slab_n = &sa->vacant_slab_n_list_head;
         ;) {
        vacant_slab_n = Zeta_OrdLinkedListNode_GetR(vacant_slab_n);
        if (vacant_slab_n == &sa->vacant_slab_n_list_head) { break; }

        SlabHead* vacant_slab_head =
            ZETA_GetStructFromMember(SlabHead, n, vacant_slab_n);

        void* vacant_slab = GetSlabFromSlabHead_(vacant_slab_head);

        unsigned char* first_chunk = ZETA_GetFirstChunkFromSlab(vacant_slab);
        unsigned char* chunk = first_chunk;

        for (size_t i = 0; i < num; ++i, chunk += stride) {
            ZETA_DebugAssert(vacant_slab_head == *(SlabHead**)(chunk + width));
            ZETA_DebugAssert(Zeta_DebugTreeMap_Find(&vacant_unit_mt,
                                                    ZETA_GetAddrFromPtr(chunk))
                                 .b);
        }

        check_num_of_vacant_units += num;

        size_t slab_size = chunk - (unsigned char*)(vacant_slab_head);

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
