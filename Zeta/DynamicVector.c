#include "DynamicVector.h"

void Zeta_DynamicVector_Init(void* dv_) {
    Zeta_DynamicVector* dv = dv_;
    ZETA_DEBUG_ASSERT(dv != NULL);

    dv->width = 0;
    dv->cluster_size = 0;
}

void Zeta_DynamicVector_StdConfig(void* dv_, diff_t width, diff_t cluster_size,
                                  Zeta_Allocator* allocator) {
    Zeta_DynamicVector* dv = dv_;
    ZETA_DEBUG_ASSERT(dv != NULL);

    ZETA_DEBUG_ASSERT(0 <= width);
    ZETA_DEBUG_ASSERT(width <= cluster_size);

    dv->width = width;
    dv->cluster_size = cluster_size;

    dv->offset = 0;
    dv->size = 0;

    dv->mlet.level = 4;

    dv->mlet.branch_nums[0] = 512;
    dv->mlet.branch_nums[1] = 512;
    dv->mlet.branch_nums[3] = 256;
    dv->mlet.branch_nums[4] = 256;

    dv->mlet.root = NULL;

    dv->mlet.allocator = allocator;

    dv->allocator = allocator;
}

diff_t Zeta_DynamicVector_GetSize(void* dv_) {
    Zeta_DynamicVector* dv = dv_;
    ZETA_DEBUG_ASSERT(dv != NULL);
    return dv->size;
}

static void GetIdxes_(Zeta_DynamicVector* dv, diff_t* idxes, diff_t idx) {
    const int level = dv->mlet.level;

    for (int level_i = level - 1; 0 <= level_i; --level_i) {
        diff_t branch_num = dv->mlet.branch_nums[level_i];
        idxes[level_i] = idx % branch_num;
        idx /= branch_num;
    }
}

void* Zeta_DynamicVector_Access(void* dv_, diff_t idx) {
    Zeta_DynamicVector* dv = dv_;
    ZETA_DEBUG_ASSERT(dv != NULL);

    ZETA_DEBUG_ASSERT(0 <= idx);
    ZETA_DEBUG_ASSERT(idx < dv->size);

    diff_t cluster_capacity = dv->cluster_size / dv->width;
    diff_t mlet_capacity = Zeta_MultiLevelEntryTable_GetCapacity(&dv->mlet);
    diff_t capacity = cluster_capacity * mlet_capacity;

    diff_t real_idx = (dv->offset + idx) % capacity;

    diff_t cluster_i = real_idx / mlet_capacity;
    diff_t cluster_j = real_idx % mlet_capacity;

    diff_t idxes[Zeta_MultiLevelEntryTable_max_level];
    GetIdxes_(dv, idxes, cluster_i);

    void** cluster = Zeta_MultiLevelEntryTable_Access(&dv->mlet, idxes);

    return ZETA_PTR_OFFSET(*cluster, dv->width * cluster_j);
}

void** Zeta_DynamicVector_PushL(void* dv_) {
    Zeta_DynamicVector* dv = dv_;
    ZETA_DEBUG_ASSERT(dv != NULL);

    Zeta_Allocator* allocator = dv->allocator;
    ZETA_DEBUG_ASSERT(allocator != NULL);

    void* allocator_context = allocator->context;

    void* (*Allocate)(void* context, diff_t size) = allocator->Allocate;
    ZETA_DEBUG_ASSERT(Allocate != NULL);

    diff_t cluster_capacity = dv->cluster_size / dv->width;
    diff_t mlet_capacity = Zeta_MultiLevelEntryTable_GetCapacity(&dv->mlet);
    diff_t capacity = cluster_capacity * mlet_capacity;

    if (dv->size == capacity) { return NULL; }

    --dv->offset;
    ++dv->size;

    diff_t real_idx = (dv->offset + 0) % capacity;

    diff_t cluster_i = real_idx / mlet_capacity;
    diff_t cluster_j = real_idx % mlet_capacity;

    diff_t idxes[Zeta_MultiLevelEntryTable_max_level];
    GetIdxes_(dv, idxes, cluster_i);

    void** cluster = Zeta_MultiLevelEntryTable_Insert(&dv->mlet, idxes);

    if (*cluster == NULL) {
        *cluster = Allocate(allocator_context, dv->cluster_size);
    }

    return ZETA_PTR_OFFSET(*cluster, dv->width * cluster_j);
}

void** Zeta_DynamicVector_PushR(void* dv_) {
    Zeta_DynamicVector* dv = dv_;
    ZETA_DEBUG_ASSERT(dv != NULL);

    Zeta_Allocator* allocator = dv->allocator;
    ZETA_DEBUG_ASSERT(allocator != NULL);

    void* allocator_context = allocator->context;

    void* (*Allocate)(void* context, diff_t size) = allocator->Allocate;
    ZETA_DEBUG_ASSERT(Allocate != NULL);

    diff_t cluster_capacity = dv->cluster_size / dv->width;
    diff_t mlet_capacity = Zeta_MultiLevelEntryTable_GetCapacity(&dv->mlet);
    diff_t capacity = cluster_capacity * mlet_capacity;

    if (dv->size == capacity) { return NULL; }

    ++dv->size;

    diff_t real_idx = (dv->offset + dv->size - 1) % capacity;

    diff_t cluster_i = real_idx / mlet_capacity;
    diff_t cluster_j = real_idx % mlet_capacity;

    diff_t idxes[Zeta_MultiLevelEntryTable_max_level];
    GetIdxes_(dv, idxes, cluster_i);

    void** cluster = Zeta_MultiLevelEntryTable_Insert(&dv->mlet, idxes);

    if (*cluster == NULL) {
        *cluster = Allocate(allocator_context, dv->cluster_size);
    }

    return ZETA_PTR_OFFSET(*cluster, dv->width * cluster_j);
}

void Zeta_DynamicVector_PopL(void* dv_, void* destry_context,
                             void (*Destroy)(void* context, void* ptr)) {
    Zeta_DynamicVector* dv = dv_;
    ZETA_DEBUG_ASSERT(dv != NULL);

    ZETA_DEBUG_ASSERT(0 < dv->size);

    Zeta_Allocator* allocator = dv->allocator;
    ZETA_DEBUG_ASSERT(allocator != NULL);

    void* allocator_context = allocator->context;

    void (*Deallocate)(void* context, void* ptr) = allocator->Deallocate;
    ZETA_DEBUG_ASSERT(Deallocate != NULL);

    diff_t cluster_capacity = dv->cluster_size / dv->width;
    diff_t mlet_capacity = Zeta_MultiLevelEntryTable_GetCapacity(&dv->mlet);
    diff_t capacity = cluster_capacity * mlet_capacity;

    diff_t real_idx = (dv->offset + 0) % capacity;

    --dv->size;

    if (dv->size == 0) {
        dv->offset = 0;
    } else {
        dv->offset = (dv->offset + 1) % capacity;
    }

    diff_t cluster_i = real_idx / mlet_capacity;
    diff_t cluster_j = real_idx % mlet_capacity;

    diff_t idxes[Zeta_MultiLevelEntryTable_max_level];
    GetIdxes_(dv, idxes, cluster_i);

    void** cluster = Zeta_MultiLevelEntryTable_Access(&dv->mlet, idxes);

    if (Destroy != NULL) {
        Destroy(destry_context,
                ZETA_PTR_OFFSET(*cluster, dv->width * cluster_j));
    }

    if (cluster_j == cluster_capacity - 1) {
        Deallocate(allocator_context, *cluster);
        Zeta_MultiLevelEntryTable_Erase(&dv->mlet, idxes);
    }
}

void Zeta_DynamicVector_PopR(void* dv_, void* destry_context,
                             void (*Destroy)(void* context, void* ptr)) {
    Zeta_DynamicVector* dv = dv_;
    ZETA_DEBUG_ASSERT(dv != NULL);

    ZETA_DEBUG_ASSERT(0 < dv->size);

    Zeta_Allocator* allocator = dv->allocator;
    ZETA_DEBUG_ASSERT(allocator != NULL);

    void* allocator_context = allocator->context;

    void (*Deallocate)(void* context, void* ptr) = allocator->Deallocate;
    ZETA_DEBUG_ASSERT(Deallocate != NULL);

    diff_t cluster_capacity = dv->cluster_size / dv->width;
    diff_t mlet_capacity = Zeta_MultiLevelEntryTable_GetCapacity(&dv->mlet);
    diff_t capacity = cluster_capacity * mlet_capacity;

    diff_t real_idx = (dv->offset + dv->size - 1) % capacity;

    --dv->size;
    if (dv->size == 0) { dv->offset = 0; }

    diff_t cluster_i = real_idx / mlet_capacity;
    diff_t cluster_j = real_idx % mlet_capacity;

    diff_t idxes[Zeta_MultiLevelEntryTable_max_level];
    GetIdxes_(dv, idxes, cluster_i);

    void** cluster = Zeta_MultiLevelEntryTable_Access(&dv->mlet, idxes);

    if (Destroy != NULL) {
        void* ptr = ZETA_PTR_OFFSET(*cluster, dv->width * cluster_j);
        Destroy(destry_context, ptr);
    }

    if (cluster_j == 0) {
        Deallocate(allocator_context, *cluster);
        Zeta_MultiLevelEntryTable_Erase(&dv->mlet, idxes);
    }
}

void Zeta_DynamicVector_ToVector(void* dv_, Zeta_Vector* dst) {
    Zeta_DynamicVector* dv = dv_;
    ZETA_DEBUG_ASSERT(dv != NULL);

    ZETA_DEBUG_ASSERT(dst != NULL);

    dst->context = dv;
    dst->GetSize = Zeta_DynamicVector_GetSize;
    dst->Access = Zeta_DynamicVector_Access;
}
