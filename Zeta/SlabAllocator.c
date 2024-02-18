#include "SlabAllocator.h"

void Zeta_SlabAllocator_Init(void* sa_) {
    Zeta_SlabAllocator* sa = sa_;

    sa->num_of_types = 0;

    sa->allocator = NULL;
}

int Zeta_SlabAllocator_GetMaxSize(size_t page_size, int num) {
    ZETA_DebugAssert(0 < num);

    size_t header_size =
        sizeof(Zeta_SlabAllocator_Slab) - sizeof(Zeta_SlabAllocator_Unit);

    size_t unit_size =
        (page_size - header_size) / num / alignof(void*) * alignof(void*);

    if (unit_size <= sizeof(void*)) { return 0; }

    return unit_size - sizeof(void*);
}

int Zeta_SlabAllocator_GetMaxNum(size_t page_size, int size) {
    ZETA_DebugAssert(0 < size);

    size = (size + alignof(void*) - 1) / alignof(void*) * alignof(void*);

    size_t header_size =
        sizeof(Zeta_SlabAllocator_Slab) - sizeof(Zeta_SlabAllocator_Unit);

    size_t unit_size = sizeof(void*) + size;

    if (page_size <= header_size) { return 0; }

    return (page_size - header_size) / unit_size;
}

size_t Zeta_SlabAllocator_GetPageSize(int size, int num) {
    ZETA_DebugAssert(0 < size);
    ZETA_DebugAssert(0 < num);

    size = (size + alignof(void*) - 1) / alignof(void*) * alignof(void*);

    return sizeof(Zeta_SlabAllocator_Slab) - sizeof(Zeta_SlabAllocator_Unit) +
           (sizeof(void*) + size) * num;
}

void Zeta_SlabAllocator_Entrust(void* sa_, int num_of_types, const int* sizes,
                                const int* nums, Zeta_Allocator* allocator) {
    Zeta_SlabAllocator* sa = sa_;

    ZETA_DebugAssert(sa != NULL);

    ZETA_DebugAssert(0 < num_of_types);
    ZETA_DebugAssert(num_of_types <= Zeta_SlabAllocator_max_num_of_types);

    for (int i = 0; i < num_of_types; ++i) {
        ZETA_DebugAssert(0 < sizes[i]);
        ZETA_DebugAssert(0 < nums[i]);
    }

    for (int i = 1; i < num_of_types; ++i) {
        ZETA_DebugAssert(sizes[i - 1] < sizes[i]);
    }

    ZETA_DebugAssert(allocator != NULL);

    sa->num_of_types = num_of_types;

    for (int i = 0; i < num_of_types; ++i) {
        sa->sizes[i] = sizes[i];
        sa->nums[i] = nums[i];
    }

    for (int i = 0; i < Zeta_SlabAllocator_max_num_of_types; ++i) {
        Zeta_LinkedListNode_Init(sa->slab_list_heads + i);
    }

    sa->allocator = allocator;
}

static int GetTypeIdx_(int num_of_types, const int* sizes, size_t size) {
    int type_lb = 0;
    int type_rb = num_of_types;

    while (type_lb < type_rb) {
        int type_mb = (type_lb + type_rb) / 2;

        if ((size_t)sizes[type_mb] < size) {
            type_lb = type_mb + 1;
        } else {
            type_rb = type_mb;
        }
    }

    return type_lb;
}

size_t Zeta_SlabAllocator_Query(void* sa_, size_t size) {
    Zeta_SlabAllocator* sa = sa_;
    ZETA_DebugAssert(sa != NULL);

    if (size == 0) { return 0; }

    int type_i = GetTypeIdx_(sa->num_of_types, sa->sizes, size);
    if (type_i == sa->num_of_types) { return 0; }

    return sa->sizes[type_i];
}

void* Zeta_SlabAllocator_Allocate(void* sa_, size_t size) {
    Zeta_SlabAllocator* sa = sa_;

    ZETA_DebugAssert(sa != NULL);
    ZETA_DebugAssert(sa->allocator != NULL);
    ZETA_DebugAssert(sa->allocator->Allocate != NULL);

    if (size == 0) { return NULL; }

    int type_i = GetTypeIdx_(sa->num_of_types, sa->sizes, size);

    ZETA_PrintVar("%d", type_i);

    if (type_i == sa->num_of_types) { return NULL; }

    void* slab_list_head = sa->slab_list_heads + type_i;
    void* slab_k_n = Zeta_LinkedListNode_GetR(slab_list_head);

    Zeta_SlabAllocator_Slab* slab_k;

    int alloc_new_slab = 0;

    if (slab_list_head == slab_k_n) {
        alloc_new_slab = 1;
    } else {
        slab_k = ZETA_GetStructFromMember(Zeta_SlabAllocator_Slab, n, slab_k_n);
        if (slab_k->ptr == NULL) { alloc_new_slab = 1; }
    }

    if (alloc_new_slab != 0) {
        slab_k = sa->allocator->Allocate(
            sa->allocator->context, Zeta_SlabAllocator_GetPageSize(
                                        sa->sizes[type_i], sa->nums[type_i]));

        if (slab_k == NULL) { return NULL; }

        slab_k_n = &slab_k->n;

        slab_k->type_i = type_i;
        Zeta_LinkedListNode_Init(slab_k_n);
        Zeta_LinkedListNode_InsertR(slab_list_head, slab_k_n);

        slab_k->ptr = slab_k->units;

        int num = sa->nums[type_i];

        for (int unit_i = 0; unit_i < num - 1; ++unit_i) {
            Zeta_SlabAllocator_Unit* unit = slab_k->units + unit_i;
            Zeta_SlabAllocator_Unit* nxt_unit = slab_k->units + unit_i + 1;
            unit->ptr = nxt_unit;
        }

        slab_k->units[num - 1].ptr = NULL;
    }

    Zeta_SlabAllocator_Unit* ret_unit = slab_k->ptr;
    Zeta_SlabAllocator_Unit* ret_nxt_unit = ret_unit->ptr;

    ret_unit->ptr = slab_k;
    slab_k->ptr = ret_nxt_unit;

    if (slab_k->ptr == NULL) {
        Zeta_LinkedListNode_Extract(slab_k_n);
        Zeta_LinkedListNode_InsertL(slab_list_head, slab_k_n);
    }

    return ret_unit->data;
}

void Zeta_SlabAllocator_Deallocate(void* sa_, void* ptr) {
    Zeta_SlabAllocator* sa = sa_;

    ZETA_DebugAssert(sa != NULL);
    ZETA_DebugAssert(sa->allocator != NULL);
    ZETA_DebugAssert(sa->allocator->Deallocate != NULL);

    if (ptr == NULL) { return; }

    Zeta_SlabAllocator_Unit* unit =
        ZETA_GetStructFromMember(Zeta_SlabAllocator_Unit, data[0], ptr);

    Zeta_SlabAllocator_Slab* slab_k = unit->ptr;

    unit->ptr = slab_k->ptr;
    slab_k->ptr = unit;

    int type_i = slab_k->type_i;

    Zeta_LinkedListNode* slab_list_head = sa->slab_list_heads + type_i;

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
