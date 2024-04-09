#include "MultiLevelVector.h"

static void CheckIdxes_(void* mlv_, size_t* idxes) {
    Zeta_MultiLevelVector* mlv = mlv_;
    ZETA_DebugAssert(mlv != NULL);

    ZETA_DebugAssert(idxes != NULL);

    int level = mlv->level;
    ZETA_DebugAssert(0 < level);
    ZETA_DebugAssert(level <= ZETA_MultiLevelVector_max_level);

    for (int level_i = 0; level_i < level; ++level_i) {
        size_t branch_num = mlv->branch_nums[level_i];
        size_t idx = idxes[level_i];

        ZETA_DebugAssert(idx < branch_num);
    }
}

void Zeta_MultiLevelVector_Init(void* mlv_) {
    Zeta_MultiLevelVector* mlv = mlv_;
    ZETA_DebugAssert(mlv != NULL);

    int level = mlv->level;

    if (level <= 0) {
        level = 6;
        ZETA_StaticAssert(6 <= ZETA_MultiLevelVector_max_level);

        mlv->level = level;

        mlv->branch_nums[0] = 64;
        mlv->branch_nums[1] = 64;
        mlv->branch_nums[2] = 128;
        mlv->branch_nums[3] = 128;
        mlv->branch_nums[4] = 256;
        mlv->branch_nums[5] = 256;
    }

    ZETA_DebugAssert(0 < level);
    ZETA_DebugAssert(level < ZETA_MultiLevelVector_max_level);

    for (int level_i = 0; level_i < level; ++level_i) {
        size_t branch_num = mlv->branch_nums[level_i];

        ZETA_DebugAssert(0 < branch_num);
        ZETA_DebugAssert(branch_num <= ZETA_MultiLevelVector_branch_num_max);
    }

    mlv->root = NULL;

    ZETA_DebugAssert(mlv->allocator != NULL);
    ZETA_DebugAssert(mlv->allocator->GetAlign != NULL);
    ZETA_DebugAssert(mlv->allocator->GetAlign(mlv->allocator->context) %
                         alignof(void*) ==
                     0);
}

size_t Zeta_MultiLevelVector_GetCapacity(void* mlv_) {
    Zeta_MultiLevelVector* mlv = mlv_;
    ZETA_DebugAssert(mlv != NULL);

    size_t ret = 1;

    int level = mlv->level;
    ZETA_DebugAssert(0 < level);
    ZETA_DebugAssert(level <= ZETA_MultiLevelVector_max_level);

    for (int level_i = 0; level_i < level; ++level_i) {
        size_t branch_num = mlv->branch_nums[level_i];

        ZETA_DebugAssert(ret <= ZETA_GetRangeMax(size_t) / branch_num);

        ret *= branch_num;
    }

    return ret;
}

void** Zeta_MultiLevelVector_Access(void* mlv_, size_t* idxes) {
    CheckIdxes_(mlv_, idxes);

    Zeta_MultiLevelVector* mlv = mlv_;
    int level = mlv->level;

    void** n = &mlv->root;

    for (int level_i = 0; level_i < level; ++level_i) {
        if (*n == NULL) { return NULL; }
        n = (void**)*n + idxes[level_i];
    }

    return n;
}

void** Zeta_MultiLevelVector_FindFirstNotNull(void* mlv_, size_t* idxes) {
    Zeta_MultiLevelVector* mlv = mlv_;
    ZETA_DebugAssert(mlv != NULL);

    int level = mlv->level;

    for (int level_i = 0; level_i < level; ++level_i) { idxes[level_i] = 0; }

    return Zeta_MultiLevelVector_FindNextNotNull(mlv, idxes, TRUE);
}

void** Zeta_MultiLevelVector_FindLastNotNull(void* mlv_, size_t* idxes) {
    Zeta_MultiLevelVector* mlv = mlv_;
    ZETA_DebugAssert(mlv != NULL);

    int level = mlv->level;

    for (int level_i = 0; level_i < level; ++level_i) {
        idxes[level_i] = mlv->branch_nums[level_i] - 1;
    }

    return Zeta_MultiLevelVector_FindPrevNotNull(mlv, idxes, TRUE);
}

static void** FindPrevNotNull_(int level, size_t const* branch_nums,
                               size_t* idxes, void* page) {
    size_t branch_num = branch_nums[0];

    if (level == 1) {
        for (size_t idx = idxes[0] + 1; 0 < idx--;) {
            if (((void**)page)[idx] != NULL) {
                idxes[0] = idx;
                return (void**)page + idx;
            }
        }

        idxes[0] = branch_num - 1;

        return NULL;
    }

    if (((void**)page)[idxes[0]] == NULL) {
        for (int level_i = 1; level_i < level; ++level_i) {
            idxes[level_i] = branch_nums[level_i] - 1;
        }

        if (idxes[0] == 0) {
            idxes[0] = branch_nums[0] - 1;
            return NULL;
        }

        --idxes[0];
    }

    for (size_t idx = idxes[0] + 1; 0 < idx--;) {
        void* subpage = ((void**)page)[idx];
        if (subpage == NULL) { continue; }

        void** ret =
            FindPrevNotNull_(level - 1, branch_nums + 1, idxes + 1, subpage);

        if (ret != NULL) {
            idxes[0] = idx;
            return ret;
        }
    }

    idxes[0] = branch_num - 1;

    return NULL;
}

static void** FindNextNotNull_(int level, size_t const* branch_nums,
                               size_t* idxes, void* page) {
    size_t branch_num = branch_nums[0];

    if (level == 1) {
        for (size_t idx = idxes[0]; idx < branch_num; ++idx) {
            if (((void**)page)[idx] != NULL) {
                idxes[0] = idx;
                return (void**)page + idx;
            }
        }

        idxes[0] = 0;

        return NULL;
    }

    if (((void**)page)[idxes[0]] == NULL) {
        for (int level_i = 1; level_i < level; ++level_i) {
            idxes[level_i] = 0;
        }

        if (idxes[0] == branch_nums[0] - 1) {
            idxes[0] = 0;
            return NULL;
        }

        ++idxes[0];
    }

    for (size_t idx = idxes[0]; idx < branch_num; ++idx) {
        void* subpage = ((void**)page)[idx];
        if (subpage == NULL) { continue; }

        void** ret =
            FindNextNotNull_(level - 1, branch_nums + 1, idxes + 1, subpage);

        if (ret != NULL) {
            idxes[0] = idx;
            return ret;
        }
    }

    idxes[0] = 0;

    return NULL;
}

void** Zeta_MultiLevelVector_FindPrevNotNull(void* mlv_, size_t* idxes,
                                             bool_t included) {
    CheckIdxes_(mlv_, idxes);

    Zeta_MultiLevelVector* mlv = mlv_;
    int level = mlv->level;

    if (!included) {
        for (int level_i = level - 1; 0 <= level_i; --level_i) {
            if (0 < idxes[level_i]) {
                --idxes[level_i];
                goto L1;
            }

            idxes[level_i] = mlv->branch_nums[level_i] - 1;
        }

        return NULL;
    }

L1:;

    void* page = mlv->root;

    if (page != NULL) {
        return FindPrevNotNull_(level, mlv->branch_nums, idxes, page);
    }

    for (int level_i = 0; level_i < level; ++level_i) { idxes[level_i] = 0; }

    return NULL;
}

void** Zeta_MultiLevelVector_FindNextNotNull(void* mlv_, size_t* idxes,
                                             bool_t included) {
    CheckIdxes_(mlv_, idxes);

    Zeta_MultiLevelVector* mlv = mlv_;
    int level = mlv->level;

    if (!included) {
        for (int level_i = level - 1; 0 <= level_i; --level_i) {
            ++idxes[level_i];
            if (idxes[level_i] < mlv->branch_nums[level_i]) { goto L1; }
            idxes[level_i] = 0;
        }

        return NULL;
    }

L1:;

    void* page = mlv->root;

    if (page != NULL) {
        return FindNextNotNull_(level, mlv->branch_nums, idxes, page);
    }

    for (int level_i = 0; level_i < level; ++level_i) { idxes[level_i] = 0; }

    return NULL;
}

void* AllocatePage_(size_t branch_num, void* allocator_context,
                    void* (*Allocate)(void* context, size_t size)) {
    void** ret = Allocate(allocator_context, sizeof(void*) * branch_num);
    ZETA_DebugAssert(ret != NULL);

    for (size_t idx = 0; idx < branch_num; ++idx) { ret[idx] = NULL; }

    return ret;
}

void** Zeta_MultiLevelVector_Insert(void* mlv_, size_t* idxes) {
    CheckIdxes_(mlv_, idxes);

    Zeta_MultiLevelVector* mlv = mlv_;
    int level = mlv->level;

    Zeta_Allocator* allocator = mlv->allocator;
    ZETA_DebugAssert(allocator != NULL);

    void* allocator_context = allocator->context;

    void* (*Allocate)(void* context, size_t size) = allocator->Allocate;
    ZETA_DebugAssert(Allocate != NULL);

    if (mlv->root == NULL) {
        mlv->root =
            AllocatePage_(mlv->branch_nums[0], allocator_context, Allocate);
    }

    void* page = mlv->root;

    for (int level_i = 0; level_i < level - 1; ++level_i) {
        void** subpage = (void**)page + idxes[level_i];

        if (*subpage == NULL) {
            *subpage = AllocatePage_(mlv->branch_nums[level_i + 1],
                                     allocator_context, Allocate);
        }

        page = *subpage;
    }

    return (void**)page + idxes[level - 1];
}

void Zeta_MultiLevelVector_Erase(void* mlv_, size_t* idxes) {
    CheckIdxes_(mlv_, idxes);

    Zeta_MultiLevelVector* mlv = mlv_;
    int level = mlv->level;

    Zeta_Allocator* allocator = mlv->allocator;
    ZETA_DebugAssert(allocator != NULL);

    void* allocator_context = allocator->context;

    void (*Deallocate)(void* context, void* ptr) = allocator->Deallocate;
    ZETA_DebugAssert(Deallocate != NULL);

    void* n = mlv->root;
    void* pages[ZETA_MultiLevelVector_max_level];

    for (int level_i = 0; level_i < level; ++level_i) {
        if (n == NULL) { return; }
        pages[level_i] = n;
        n = ((void**)n)[idxes[level_i]];
    }

    for (int level_i = level - 1; 0 <= level_i; --level_i) {
        void* page = pages[level_i];
        ((void**)page)[idxes[level_i]] = NULL;

        size_t branch_num = mlv->branch_nums[level_i];

        for (size_t idx = 0; idx < branch_num; ++idx) {
            if (((void**)page)[idx] != NULL) { return; }
        }

        Deallocate(allocator_context, page);
    }

    mlv->root = NULL;
}

static void EraseAll_(int level, size_t const* branch_nums, void* page,
                      void* allocator_context,
                      void (*Deallocate)(void* context, void* ptr)) {
    size_t branch_num = branch_nums[0];

    if (level == 1) {
        Deallocate(allocator_context, page);
        return;
    }

    for (size_t idx = 0; idx < branch_num; ++idx) {
        void* subpage = ((void**)page)[idx];

        if (subpage != NULL) {
            EraseAll_(level - 1, branch_nums + 1, subpage, allocator_context,
                      Deallocate);
        }
    }

    Deallocate(allocator_context, page);
}

void Zeta_MultiLevelVector_EraseAll(void* mlv_) {
    Zeta_MultiLevelVector* mlv = mlv_;
    ZETA_DebugAssert(mlv != NULL);

    int level = mlv->level;

    void* n = mlv->root;
    if (n == NULL) { return; }

    Zeta_Allocator* allocator = mlv->allocator;
    ZETA_DebugAssert(allocator != NULL);

    void (*Deallocate)(void* context, void* ptr) = allocator->Deallocate;
    ZETA_DebugAssert(Deallocate != NULL);

    EraseAll_(level, mlv->branch_nums, n, allocator->context, Deallocate);

    mlv->root = NULL;
}

static bool_t IsClean_(int level, size_t const* branch_nums, void* page) {
    size_t branch_num = branch_nums[0];

    if (level == 1) {
        for (size_t idx = 0; idx < branch_num; ++idx) {
            if (((void**)page)[idx] != NULL) { return TRUE; }
        }

        return FALSE;
    }

    bool_t has_subpage = FALSE;

    for (size_t idx = 0; idx < branch_num; ++idx) {
        void* subpage = ((void**)page)[idx];
        if (subpage == NULL) { continue; }

        has_subpage = TRUE;

        if (!IsClean_(level - 1, branch_nums + 1, subpage)) { return FALSE; }
    }

    return has_subpage;
}

bool_t Zeta_MultiLevelVector_IsClean(void* mlv_) {
    Zeta_MultiLevelVector* mlv = mlv_;
    ZETA_DebugAssert(mlv != NULL);

    void* n = mlv->root;

    return n == NULL || IsClean_(mlv->level, mlv->branch_nums, n);
}

static bool_t Clear_(int level, size_t const* branch_nums, void* page,
                     void* allocator_context,
                     void (*Deallocate)(void* context, void* ptr)) {
    size_t branch_num = branch_nums[0];

    if (level == 1) {
        for (size_t idx = 0; idx < branch_num; ++idx) {
            if (((void**)page)[idx] != NULL) { return TRUE; }
        }

        return FALSE;
    }

    bool_t has_entries = FALSE;

    for (size_t idx = 0; idx < branch_num; ++idx) {
        void* subpage = ((void**)page)[idx];
        if (subpage == NULL) { continue; }

        if (Clear_(level - 1, branch_nums + 1, subpage, allocator_context,
                   Deallocate)) {
            has_entries = TRUE;
        } else {
            ((void**)page)[idx] = NULL;
        }
    }

    if (has_entries) { return TRUE; }

    Deallocate(allocator_context, page);

    return FALSE;
}

void Zeta_MultiLevelVector_Clear(void* mlv_) {
    Zeta_MultiLevelVector* mlv = mlv_;
    ZETA_DebugAssert(mlv != NULL);

    int level = mlv->level;

    void* n = mlv->root;
    if (n == NULL) { return; }

    Zeta_Allocator* allocator = mlv->allocator;
    ZETA_DebugAssert(allocator != NULL);

    void (*Deallocate)(void* context, void* ptr) = allocator->Deallocate;
    ZETA_DebugAssert(Deallocate != NULL);

    if (!Clear_(level, mlv->branch_nums, n, allocator->context, Deallocate)) {
        mlv->root = NULL;
    }
}

static void GetAllPages_(Zeta_DebugTreeMap* dst, int level,
                         size_t const* branch_nums, void* page) {
    Zeta_DebugTreeMap_Insert(dst, (size_t)(uintptr_t)page);

    if (level == 0) { return; }

    for (size_t i = 0; i < branch_nums[0]; ++i) {
        void* subpage = ((void**)page)[i];

        if (subpage != NULL) {
            GetAllPages_(dst, level - 1, branch_nums + 1, subpage);
        }
    }
}

void Zeta_MultiLevelVector_GetAllPages(void* mlv_, Zeta_DebugTreeMap* dst) {
    Zeta_MultiLevelVector* mlv = mlv_;
    ZETA_DebugAssert(mlv != NULL);

    ZETA_DebugAssert(dst != NULL);

    if (mlv->root != NULL) {
        GetAllPages_(dst, mlv->level - 1, mlv->branch_nums, mlv->root);
    }
}
