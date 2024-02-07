#include "DynamicVector.h"

#include "utils.h"

void Zeta_DynamicVector_Init(void* dv_) {
    Zeta_DynamicVector* dv = dv_;
    ZETA_DebugAssert(dv != NULL);

    ZETA_DebugAssert(1 <= dv->width);
    ZETA_DebugAssert(1 <= dv->cluster_capacity);

    dv->offset = 0;
    dv->size = 0;

    Zeta_MultiLevelEntryTable_Init(&dv->mlet);

    ZETA_DebugAssert(dv->allocator != NULL);
}

void Zeta_DynamicVector_StdConfig(void* dv_, size_t width,
                                  size_t cluster_capacity,
                                  Zeta_Allocator* allocator) {
    Zeta_DynamicVector* dv = dv_;
    ZETA_DebugAssert(dv != NULL);

    ZETA_DebugAssert(1 <= width);
    ZETA_DebugAssert(1 <= cluster_capacity);

    dv->width = width;
    dv->cluster_capacity = cluster_capacity;

    dv->offset = 0;
    dv->size = 0;

    dv->mlet.level = 4;

    dv->mlet.branch_nums[0] = 256;
    dv->mlet.branch_nums[1] = 256;
    dv->mlet.branch_nums[3] = 512;
    dv->mlet.branch_nums[4] = 512;

    dv->mlet.root = NULL;

    dv->mlet.allocator = allocator;

    dv->allocator = allocator;
}

size_t Zeta_DynamicVector_GetWidth(void* dv_) {
    Zeta_DynamicVector* dv = dv_;
    ZETA_DebugAssert(dv != NULL);
    return dv->width;
}

size_t Zeta_DynamicVector_GetSize(void* dv_) {
    Zeta_DynamicVector* dv = dv_;
    ZETA_DebugAssert(dv != NULL);
    return dv->size;
}

static void GetIdxes_(Zeta_DynamicVector* dv, size_t* idxes, size_t idx) {
    const int level = dv->mlet.level;

    for (int level_i = level - 1; 0 <= level_i; --level_i) {
        size_t branch_num = dv->mlet.branch_nums[level_i];
        idxes[level_i] = idx % branch_num;
        idx /= branch_num;
    }
}

void* Access_(Zeta_DynamicVector* dv_, size_t idx) {
    Zeta_DynamicVector* dv = dv_;
    ZETA_DebugAssert(dv != NULL);

    ZETA_DebugAssert(1 <= dv->width);
    ZETA_DebugAssert(1 <= dv->cluster_capacity);

    ZETA_DebugAssert(0 <= idx);
    ZETA_DebugAssert(idx < dv->size);

    size_t mlet_capacity = Zeta_MultiLevelEntryTable_GetCapacity(&dv->mlet);
    ZETA_DebugAssert(dv->cluster_capacity <=
                     ZETA_maxof(size_t) / mlet_capacity);

    size_t capacity = dv->cluster_capacity * mlet_capacity;
    ZETA_DebugAssert(capacity <= ZETA_max_mod_under_size_t);

    size_t real_idx = (dv->offset + idx) % capacity;
    size_t cluster_i = real_idx / dv->cluster_capacity;
    size_t cluster_j = real_idx % dv->cluster_capacity;

    size_t idxes[ZETA_MultiLevelEntryTable_max_level];
    GetIdxes_(dv, idxes, cluster_i);

    void** cluster = Zeta_MultiLevelEntryTable_Access(&dv->mlet, idxes);

    return ZETA_OffsetPtr(*cluster, dv->width * cluster_j);
}

void* Zeta_DynamicVector_Access(void* dv, size_t idx) {
    return Access_(dv, idx);
}

void* Zeta_DynamicVector_Insert(void* dv_, size_t idx) {
    Zeta_DynamicVector* dv = dv_;
    ZETA_DebugAssert(dv != NULL);

    ZETA_DebugAssert(1 <= dv->width);
    ZETA_DebugAssert(1 <= dv->cluster_capacity);

    Zeta_Allocator* allocator = dv->allocator;
    ZETA_DebugAssert(allocator != NULL);

    void* allocator_context = allocator->context;

    void* (*Allocate)(void* context, size_t size) = allocator->Allocate;
    ZETA_DebugAssert(Allocate != NULL);

    size_t mlet_capacity = Zeta_MultiLevelEntryTable_GetCapacity(&dv->mlet);
    ZETA_DebugAssert(dv->cluster_capacity <=
                     ZETA_maxof(size_t) / mlet_capacity);

    size_t capacity = dv->cluster_capacity * mlet_capacity;
    ZETA_DebugAssert(capacity <= ZETA_max_mod_under_size_t);

    ZETA_DebugAssert(0 <= dv->offset);
    ZETA_DebugAssert(dv->offset < capacity);
    ZETA_DebugAssert(dv->size < capacity);

    ZETA_DebugAssert(0 <= idx);
    ZETA_DebugAssert(idx <= dv->size);

    size_t hole_idx;

    if (idx < dv->size - idx) {  // l move
        hole_idx = 0;
        dv->offset = (dv->offset + capacity - 1) % capacity;
        ++dv->size;
    } else {  // r move
        hole_idx = dv->size;
        ++dv->size;
    }

    size_t real_idx = (dv->offset + hole_idx) % capacity;
    size_t cluster_i = real_idx / dv->cluster_capacity;

    size_t idxes[ZETA_MultiLevelEntryTable_max_level];
    GetIdxes_(dv, idxes, cluster_i);

    void** cluster = Zeta_MultiLevelEntryTable_Insert(&dv->mlet, idxes);

    if (*cluster == NULL) {
        *cluster =
            Allocate(allocator_context, dv->width * dv->cluster_capacity);
    }

    for (; hole_idx < idx; ++hole_idx) {
        Zeta_MemCopy(dv->width, Access_(dv, hole_idx),
                     Access_(dv, hole_idx + 1));
    }

    for (; idx < hole_idx; --hole_idx) {
        Zeta_MemCopy(dv->width, Access_(dv, hole_idx),
                     Access_(dv, hole_idx - 1));
    }

    return Access_(dv, idx);
}

void Zeta_DynamicVector_Erase(void* dv_, size_t idx) {
    Zeta_DynamicVector* dv = dv_;
    ZETA_DebugAssert(dv != NULL);

    ZETA_DebugAssert(1 <= dv->width);
    ZETA_DebugAssert(1 <= dv->cluster_capacity);

    Zeta_Allocator* allocator = dv->allocator;
    ZETA_DebugAssert(allocator != NULL);

    void* allocator_context = allocator->context;

    void (*Deallocate)(void* context, void* ptr) = allocator->Deallocate;
    ZETA_DebugAssert(Deallocate != NULL);

    size_t mlet_capacity = Zeta_MultiLevelEntryTable_GetCapacity(&dv->mlet);
    ZETA_DebugAssert(dv->cluster_capacity <=
                     ZETA_maxof(size_t) / mlet_capacity);

    size_t capacity = dv->cluster_capacity * mlet_capacity;
    ZETA_DebugAssert(capacity <= ZETA_max_mod_under_size_t);

    ZETA_DebugAssert(0 <= dv->offset);
    ZETA_DebugAssert(dv->offset < capacity);
    ZETA_DebugAssert(dv->size < capacity);

    ZETA_DebugAssert(0 <= idx);
    ZETA_DebugAssert(idx < dv->size);

    if (idx < dv->size - 1 - idx) {  // l move
        for (; 0 < idx; --idx) {
            Zeta_MemCopy(dv->width, Access_(dv, idx), Access_(dv, idx - 1));
        }

        size_t real_idx = (dv->offset + 0) % capacity;
        size_t cluster_i = real_idx / dv->cluster_capacity;
        size_t cluster_j = real_idx % dv->cluster_capacity;

        if (cluster_j == dv->cluster_capacity - 1 || dv->size == 1) {
            size_t idxes[ZETA_MultiLevelEntryTable_max_level];
            GetIdxes_(dv, idxes, cluster_i);

            Deallocate(allocator_context,
                       *Zeta_MultiLevelEntryTable_Access(&dv->mlet, idxes));

            Zeta_MultiLevelEntryTable_Erase(&dv->mlet, idxes);
        }

        dv->offset = (dv->offset + 1) % capacity;
        --dv->size;
    } else {  // r move
        for (; idx < dv->size - 1; ++idx) {
            Zeta_MemCopy(dv->width, Access_(dv, idx), Access_(dv, idx + 1));
        }

        size_t real_idx = (dv->offset + dv->size - 1) % capacity;
        size_t cluster_i = real_idx / dv->cluster_capacity;
        size_t cluster_j = real_idx % dv->cluster_capacity;

        if (cluster_j == 0 || dv->size == 1) {
            size_t idxes[ZETA_MultiLevelEntryTable_max_level];
            GetIdxes_(dv, idxes, cluster_i);

            Deallocate(allocator_context,
                       *Zeta_MultiLevelEntryTable_Access(&dv->mlet, idxes));

            Zeta_MultiLevelEntryTable_Erase(&dv->mlet, idxes);
        }

        --dv->size;
    }

    if (dv->size == 0) { dv->offset = 0; }
}

void* Zeta_DynamicVector_PushL(void* dv_) {
    Zeta_DynamicVector* dv = dv_;
    ZETA_DebugAssert(dv != NULL);

    return Zeta_DynamicVector_Insert(dv, 0);
}

void* Zeta_DynamicVector_PushR(void* dv_) {
    Zeta_DynamicVector* dv = dv_;
    ZETA_DebugAssert(dv != NULL);

    return Zeta_DynamicVector_Insert(dv, dv->size);
}

void Zeta_DynamicVector_PopL(void* dv_) {
    Zeta_DynamicVector* dv = dv_;
    ZETA_DebugAssert(dv != NULL);

    Zeta_DynamicVector_Erase(dv, 0);
}

void Zeta_DynamicVector_PopR(void* dv_) {
    Zeta_DynamicVector* dv = dv_;
    ZETA_DebugAssert(dv != NULL);

    Zeta_DynamicVector_Erase(dv, dv->size - 1);
}

void Zeta_DynamicVector_EraseAll(void* dv_) {
    Zeta_DynamicVector* dv = dv_;
    ZETA_DebugAssert(dv != NULL);

    size_t mlet_capacity = Zeta_MultiLevelEntryTable_GetCapacity(&dv->mlet);
    ZETA_DebugAssert(dv->cluster_capacity <=
                     ZETA_maxof(size_t) / mlet_capacity);

    size_t capacity = dv->cluster_capacity * mlet_capacity;
    ZETA_DebugAssert(capacity <= ZETA_max_mod_under_size_t);

    while (0 < dv->size) {
        size_t real_idx = (dv->offset + 0) % capacity;
        if (real_idx % dv->cluster_capacity == 0) { break; }

        Zeta_DynamicVector_Erase(dv, 0);
    }

    while (0 < dv->size) {
        size_t real_idx = (dv->offset + dv->size) % capacity;
        if (real_idx % dv->cluster_capacity == 0) { break; }

        Zeta_DynamicVector_Erase(dv, dv->size - 1);
    }

    if (dv->size == 0) {
        dv->offset = 0;
        return;
    }

    size_t idxes[ZETA_MultiLevelEntryTable_max_level];

    size_t cluster_i_beg = dv->offset / dv->cluster_capacity;
    size_t cluster_i_end =
        (dv->offset + dv->size) % capacity % dv->cluster_capacity;

    for (size_t i = cluster_i_beg; i < cluster_i_end; ++i) {
        GetIdxes_(dv, idxes, i);
        Zeta_MultiLevelEntryTable_Erase(&dv->mlet, idxes);
    }

    dv->offset = 0;
    dv->size = 0;
}

void Zeta_DynamicVector_ToVector(void* dv_, Zeta_Vector* dst) {
    Zeta_DynamicVector* dv = dv_;
    ZETA_DebugAssert(dv != NULL);

    ZETA_DebugAssert(dst != NULL);

    dst->context = dv;
    dst->GetWidth = Zeta_DynamicVector_GetWidth;
    dst->GetSize = Zeta_DynamicVector_GetSize;
    dst->Access = Zeta_DynamicVector_Access;
}
