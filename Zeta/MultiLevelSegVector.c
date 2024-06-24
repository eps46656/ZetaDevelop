#include "MultiLevelSegVector.h"

#include "utils.h"

static void CheckMLSV_(void* mlsv_) {
    Zeta_MultiLevelSegVector* mlsv = mlsv_;
    ZETA_DebugAssert(mlsv != NULL);

    size_t width = mlsv->width;
    size_t stride = mlsv->stride;
    size_t seg_capacity = mlsv->seg_capacity;

    ZETA_DebugAssert(0 < width);
    ZETA_DebugAssert(width <= stride);
    ZETA_DebugAssert(seg_capacity <= Zeta_MultiLevelSegVector_max_seg_capacity);

    ZETA_DebugAssert(mlsv->node_allocator != NULL);
    ZETA_DebugAssert(mlsv->node_allocator->Allocate != NULL);
    ZETA_DebugAssert(mlsv->node_allocator->Deallocate != NULL);

    ZETA_DebugAssert(mlsv->data_allocator != NULL);
    ZETA_DebugAssert(mlsv->data_allocator->Allocate != NULL);
    ZETA_DebugAssert(mlsv->data_allocator->Deallocate != NULL);
}

void Zeta_MultiLevelSegVector_Init(void* mlsv_) {
    Zeta_MultiLevelSegVector* mlsv = mlsv_;
    ZETA_DebugAssert(mlsv != NULL);

    ZETA_DebugAssert(0 < mlsv->width);

    ZETA_DebugAssert(0 < mlsv->seg_capacity);
    ZETA_DebugAssert(mlsv->seg_capacity <=
                     Zeta_MultiLevelSegVector_max_seg_capacity);

    mlsv->offset = 0;
    mlsv->size = 0;

    Zeta_MultiLevelTable_Init(&mlsv->mlt);
}

size_t Zeta_MultiLevelSegVector_GetWidth(void* mlsv_) {
    Zeta_MultiLevelSegVector* mlsv = mlsv_;
    ZETA_DebugAssert(mlsv != NULL);
    return mlsv->width;
}

size_t Zeta_MultiLevelSegVector_GetSize(void* mlsv_) {
    Zeta_MultiLevelSegVector* mlsv = mlsv_;
    ZETA_DebugAssert(mlsv != NULL);
    return mlsv->size;
}

static void GetIdxes_(Zeta_MultiLevelSegVector* mlsv, size_t* idxes,
                      size_t idx) {
    int level = mlsv->mlt.level;

    for (int level_i = level - 1; 0 <= level_i; --level_i) {
        size_t branch_num = mlsv->mlt.branch_nums[level_i];
        idxes[level_i] = idx % branch_num;
        idx /= branch_num;
    }
}

void* Access_(Zeta_MultiLevelSegVector* mlsv_, size_t idx) {
    Zeta_MultiLevelSegVector* mlsv = mlsv_;
    ZETA_DebugAssert(mlsv != NULL);

    ZETA_DebugAssert(0 < mlsv->width);
    ZETA_DebugAssert(0 < mlsv->seg_capacity);

    ZETA_DebugAssert(idx < mlsv->size);

    size_t mlv_capacity = Zeta_MultiLevelTable_GetCapacity(&mlsv->mlt);
    ZETA_DebugAssert(mlsv->seg_capacity <=
                     ZETA_RangeMaxOf(size_t) / mlv_capacity);

    size_t capacity = mlsv->seg_capacity * mlv_capacity;
    ZETA_DebugAssert(capacity <= ZETA_GetMaxMod(size_t));

    size_t real_idx = (mlsv->offset + idx) % capacity;
    size_t seg_i = real_idx / mlsv->seg_capacity;
    size_t seg_j = real_idx % mlsv->seg_capacity;

    size_t idxes[ZETA_MultiLevelTable_max_level];
    GetIdxes_(mlsv, idxes, seg_i);

    void** seg = Zeta_MultiLevelTable_Access(&mlsv->mlt, idxes);

    return ZETA_UINT_TO_Ptr(ZETA_PtrToAddr(*seg) + mlsv->width * seg_j);
}

void* Zeta_MultiLevelSegVector_Access(void* mlsv, size_t idx) {
    return Access_(mlsv, idx);
}

void* Zeta_MultiLevelSegVector_Insert(void* mlsv_, size_t idx) {
    Zeta_MultiLevelSegVector* mlsv = mlsv_;
    ZETA_DebugAssert(mlsv != NULL);

    ZETA_DebugAssert(0 < mlsv->width);
    ZETA_DebugAssert(0 < mlsv->seg_capacity);

    Zeta_Allocator* allocator = mlsv->allocator;
    ZETA_DebugAssert(allocator != NULL);

    void* allocator_context = allocator->context;

    void* (*Allocate)(void* context, size_t size) = allocator->Allocate;
    ZETA_DebugAssert(Allocate != NULL);

    size_t mlv_capacity = Zeta_MultiLevelTable_GetCapacity(&mlsv->mlt);
    ZETA_DebugAssert(mlsv->seg_capacity <=
                     ZETA_RangeMaxOf(size_t) / mlv_capacity);

    size_t capacity = mlsv->seg_capacity * mlv_capacity;
    ZETA_DebugAssert(capacity <= ZETA_GetMaxMod(size_t));

    ZETA_DebugAssert(mlsv->offset < capacity);
    ZETA_DebugAssert(mlsv->size < capacity);

    ZETA_DebugAssert(idx <= mlsv->size);

    size_t hole_idx;

    if (idx < mlsv->size - idx) {  // l move
        hole_idx = 0;
        mlsv->offset = (mlsv->offset + capacity - 1) % capacity;
        ++mlsv->size;
    } else {  // r move
        hole_idx = mlsv->size;
        ++mlsv->size;
    }

    size_t real_idx = (mlsv->offset + hole_idx) % capacity;
    size_t seg_i = real_idx / mlsv->seg_capacity;

    size_t idxes[ZETA_MultiLevelTable_max_level];
    GetIdxes_(mlsv, idxes, seg_i);

    void** seg = Zeta_MultiLevelTable_Insert(&mlsv->mlt, idxes);

    if (*seg == NULL) {
        *seg = Allocate(allocator_context, mlsv->width * mlsv->seg_capacity);
    }

    for (; hole_idx < idx; ++hole_idx) {
        Zeta_MemCopy(Access_(mlsv, hole_idx), Access_(mlsv, hole_idx + 1),
                     mlsv->width);
    }

    for (; idx < hole_idx; --hole_idx) {
        Zeta_MemCopy(Access_(mlsv, hole_idx), Access_(mlsv, hole_idx - 1),
                     mlsv->width);
    }

    return Access_(mlsv, idx);
}

void Zeta_MultiLevelSegVector_Erase(void* mlsv_, size_t idx) {
    Zeta_MultiLevelSegVector* mlsv = mlsv_;
    ZETA_DebugAssert(mlsv != NULL);

    ZETA_DebugAssert(0 < mlsv->width);
    ZETA_DebugAssert(0 < mlsv->seg_capacity);

    Zeta_Allocator* allocator = mlsv->allocator;
    ZETA_DebugAssert(allocator != NULL);

    void* allocator_context = allocator->context;

    void (*Deallocate)(void* context, void* ptr) = allocator->Deallocate;
    ZETA_DebugAssert(Deallocate != NULL);

    size_t mlv_capacity = Zeta_MultiLevelTable_GetCapacity(&mlsv->mlt);
    ZETA_DebugAssert(mlsv->seg_capacity <=
                     ZETA_RangeMaxOf(size_t) / mlv_capacity);

    size_t capacity = mlsv->seg_capacity * mlv_capacity;
    ZETA_DebugAssert(capacity <= ZETA_GetMaxMod(size_t));

    ZETA_DebugAssert(mlsv->offset < capacity);
    ZETA_DebugAssert(mlsv->size < capacity);

    ZETA_DebugAssert(idx < mlsv->size);

    if (idx < mlsv->size - 1 - idx) {  // l move
        for (; 0 < idx; --idx) {
            Zeta_MemCopy(Access_(mlsv, idx), Access_(mlsv, idx - 1),
                         mlsv->width);
        }

        size_t real_idx = (mlsv->offset + 0) % capacity;
        size_t seg_i = real_idx / mlsv->seg_capacity;
        size_t seg_j = real_idx % mlsv->seg_capacity;

        if (seg_j == mlsv->seg_capacity - 1 || mlsv->size == 1) {
            size_t idxes[ZETA_MultiLevelTable_max_level];
            GetIdxes_(mlsv, idxes, seg_i);

            Deallocate(allocator_context,
                       *Zeta_MultiLevelTable_Access(&mlsv->mlt, idxes));

            Zeta_MultiLevelTable_Erase(&mlsv->mlt, idxes);
        }

        mlsv->offset = (mlsv->offset + 1) % capacity;
        --mlsv->size;
    } else {  // r move
        for (; idx < mlsv->size - 1; ++idx) {
            Zeta_MemCopy(Access_(mlsv, idx), Access_(mlsv, idx + 1),
                         mlsv->width);
        }

        size_t real_idx = (mlsv->offset + mlsv->size - 1) % capacity;
        size_t seg_i = real_idx / mlsv->seg_capacity;
        size_t seg_j = real_idx % mlsv->seg_capacity;

        if (seg_j == 0 || mlsv->size == 1) {
            size_t idxes[ZETA_MultiLevelTable_max_level];
            GetIdxes_(mlsv, idxes, seg_i);

            Deallocate(allocator_context,
                       *Zeta_MultiLevelTable_Access(&mlsv->mlt, idxes));

            Zeta_MultiLevelTable_Erase(&mlsv->mlt, idxes);
        }

        --mlsv->size;
    }

    if (mlsv->size == 0) { mlsv->offset = 0; }
}

void* Zeta_MultiLevelSegVector_PushL(void* mlsv_) {
    Zeta_MultiLevelSegVector* mlsv = mlsv_;
    ZETA_DebugAssert(mlsv != NULL);

    return Zeta_MultiLevelSegVector_Insert(mlsv, 0);
}

void* Zeta_MultiLevelSegVector_PushR(void* mlsv_) {
    Zeta_MultiLevelSegVector* mlsv = mlsv_;
    ZETA_DebugAssert(mlsv != NULL);

    return Zeta_MultiLevelSegVector_Insert(mlsv, mlsv->size);
}

void Zeta_MultiLevelSegVector_PopL(void* mlsv_) {
    Zeta_MultiLevelSegVector* mlsv = mlsv_;
    ZETA_DebugAssert(mlsv != NULL);

    Zeta_MultiLevelSegVector_Erase(mlsv, 0);
}

void Zeta_MultiLevelSegVector_PopR(void* mlsv_) {
    Zeta_MultiLevelSegVector* mlsv = mlsv_;
    ZETA_DebugAssert(mlsv != NULL);

    Zeta_MultiLevelSegVector_Erase(mlsv, mlsv->size - 1);
}

void Zeta_MultiLevelSegVector_EraseAll(void* mlsv_) {
    Zeta_MultiLevelSegVector* mlsv = mlsv_;
    ZETA_DebugAssert(mlsv != NULL);

    size_t mlv_capacity = Zeta_MultiLevelTable_GetCapacity(&mlsv->mlt);
    ZETA_DebugAssert(mlsv->seg_capacity <=
                     ZETA_RangeMaxOf(size_t) / mlv_capacity);

    size_t capacity = mlsv->seg_capacity * mlv_capacity;
    ZETA_DebugAssert(capacity <= ZETA_GetMaxMod(size_t));

    while (0 < mlsv->size) {
        size_t real_idx = (mlsv->offset + 0) % capacity;
        if (real_idx % mlsv->seg_capacity == 0) { break; }

        Zeta_MultiLevelSegVector_Erase(mlsv, 0);
    }

    while (0 < mlsv->size) {
        size_t real_idx = (mlsv->offset + mlsv->size) % capacity;
        if (real_idx % mlsv->seg_capacity == 0) { break; }

        Zeta_MultiLevelSegVector_Erase(mlsv, mlsv->size - 1);
    }

    if (mlsv->size == 0) {
        mlsv->offset = 0;
        return;
    }

    size_t idxes[ZETA_MultiLevelTable_max_level];

    size_t seg_i_beg = mlsv->offset / mlsv->seg_capacity;
    size_t seg_i_end =
        (mlsv->offset + mlsv->size) % capacity % mlsv->seg_capacity;

    for (size_t i = seg_i_beg; i < seg_i_end; ++i) {
        GetIdxes_(mlsv, idxes, i);
        Zeta_MultiLevelTable_Erase(&mlsv->mlt, idxes);
    }

    mlsv->offset = 0;
    mlsv->size = 0;
}

void Zeta_MultiLevelSegVector_DeplotSeqContainer(void* mlsv_,
                                                 Zeta_SeqContainer* dst) {
    Zeta_MultiLevelSegVector* mlsv = mlsv_;
    ZETA_DebugAssert(mlsv != NULL);

    ZETA_DebugAssert(dst != NULL);

    dst->context = mlsv;
    dst->GetWidth = Zeta_MultiLevelSegVector_GetWidth;
    dst->GetSize = Zeta_MultiLevelSegVector_GetSize;
    dst->Access = Zeta_MultiLevelSegVector_Access;
}
