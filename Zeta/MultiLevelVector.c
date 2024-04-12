#include "MultiLevelVector.h"

typedef u64_t mask_t;

ZETA_StaticAssert(alignof(void*) % alignof(mask_t) == 0);

#define mask_width (64)

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

static int FindLSB_(mask_t x) {
    if (x == 0) { return -1; }

    const int table[] = { 0, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0 };

    int ret = 0;

    for (; x % 256 == 0; x /= 256) { ret += 8; }

    x %= 256;

    return ret + (x < 16 ? table[x % 16] : (4 + table[x / 16]));
}

static int FindMSB_(mask_t x) {
    if (x == 0) { return -1; }

    const int table[] = { 0, 0, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3 };

    int ret = 0;

    for (; 256 <= x; x /= 256) { ret += 8; }

    return ret + (x < 16 ? table[x % 16] : (4 + table[x / 16]));
}

static bool_t Test_(void* page, size_t branch_num, size_t idx) {
    mask_t mask = *((mask_t*)((void**)page + branch_num) + idx / mask_width);
    return (mask & ((mask_t)1 << (idx % mask_width))) != 0;
}

static int FindPrev_(void* page, size_t branch_num, size_t idx) {
    if (idx == (size_t)(-1)) { return -1; }

    int i = idx / mask_width;
    int j = idx % mask_width;

    mask_t* mask = (mask_t*)((void**)page + branch_num) + i;

    if (j < mask_width - 1) {
        int j_ = mask_width - j - 1;
        int k = FindMSB_(*mask << j_ >> j_);
        if (0 <= k) { return mask_width * i + k; }

        --mask;
        --i;
    }

    for (; 0 <= i; --i, --mask) {
        int k = FindMSB_(*mask);
        if (0 <= k) { return mask_width * i + k; }
    }

    return -1;
}

static int FindNext_(void* page, size_t branch_num, size_t idx) {
    if (idx == branch_num) { return -1; }

    int i = idx / mask_width;
    int j = idx % mask_width;

    mask_t* mask = (mask_t*)((void**)page + branch_num) + i;

    if (0 < j) {
        int k = FindLSB_(*mask >> j);
        if (0 <= k) { return mask_width * i + k; }

        ++mask;
        ++i;
    }

    for (int i_end = (branch_num - 1) / mask_width; i <= i_end; ++i, ++mask) {
        int k = FindLSB_(*mask);
        if (0 <= k) { return mask_width * i + k; }
    }

    return -1;
}

static void SetMask_(void* page, size_t branch_num, size_t idx) {
    int i = idx / mask_width;
    int j = idx % mask_width;

    mask_t* mask = (mask_t*)((void**)page + branch_num) + i;

    *mask |= (mask_t)1 << j;
}

static void UnsetMask_(void* page, size_t branch_num, size_t idx) {
    int i = idx / mask_width;
    int j = idx % mask_width;

    mask_t* mask = (mask_t*)((void**)page + branch_num) + i;

    *mask &= ~((mask_t)1 << j);
}

static bool_t IsEmpty_(void* page, size_t branch_num) {
    unsigned int k = (branch_num + mask_width - 1) / mask_width;

    mask_t* mask = (mask_t*)((void**)page + branch_num);

    for (unsigned int i = 0; i < k; ++i) {
        if (mask[i] != 0) { return FALSE; }
    }

    return TRUE;
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

void** Zeta_MultiLevelVector_FindPrevNotNull(void* mlv_, size_t* idxes,
                                             bool_t included) {
    CheckIdxes_(mlv_, idxes);

    Zeta_MultiLevelVector* mlv = mlv_;
    int level = mlv->level;
    size_t* branch_nums = mlv->branch_nums;

    if (!included) {
        for (int level_i = level - 1; 0 <= level_i; --level_i) {
            if (0 < idxes[level_i]) {
                --idxes[level_i];
                goto L1;
            }

            idxes[level_i] = branch_nums[level_i] - 1;
        }

        return NULL;
    }

L1:;

    void* root = mlv->root;

    if (root == NULL) {
        for (int level_i = 0; level_i < level; ++level_i) {
            idxes[level_i] = 0;
        }

        return NULL;
    }

    void* pages[ZETA_MultiLevelVector_max_level];

    int level_i = 0;
    void* page = root;

    for (; level_i < level; ++level_i) {
        pages[level_i] = page;
        if (!Test_(page, branch_nums[level_i], idxes[level_i])) { break; }
        page = ((void**)page)[idxes[level_i]];
    }

    if (level_i == level) { return page; }

    for (; 0 <= level_i; --level_i) {
        page = pages[level_i];

        int found_idx =
            FindPrev_(page, branch_nums[level_i], idxes[level_i] - 1);

        if (found_idx != -1) {
            idxes[level_i] = found_idx;
            page = ((void**)page)[found_idx];
            ++level_i;
            break;
        }
    }

    if (level_i == -1) {
        for (level_i = 0; level_i < level; ++level_i) {
            idxes[level_i] = branch_nums[level_i] - 1;
        }

        return NULL;
    }

    for (; level_i < level; ++level_i) {
        int found_idx =
            FindPrev_(page, branch_nums[level_i], branch_nums[level_i] - 1);
        ZETA_DebugAssert(found_idx != -1);

        idxes[level_i] = found_idx;
        page = ((void**)page)[found_idx];
    }

    return page;
}

void** Zeta_MultiLevelVector_FindNextNotNull(void* mlv_, size_t* idxes,
                                             bool_t included) {
    CheckIdxes_(mlv_, idxes);

    Zeta_MultiLevelVector* mlv = mlv_;
    int level = mlv->level;
    size_t* branch_nums = mlv->branch_nums;

    if (!included) {
        for (int level_i = level - 1; 0 <= level_i; --level_i) {
            ++idxes[level_i];
            if (idxes[level_i] < branch_nums[level_i]) { goto L1; }
            idxes[level_i] = 0;
        }

        return NULL;
    }

L1:;

    void* root = mlv->root;

    if (root == NULL) {
        for (int level_i = 0; level_i < level; ++level_i) {
            idxes[level_i] = 0;
        }

        return NULL;
    }

    void* pages[ZETA_MultiLevelVector_max_level];

    int level_i = 0;
    void* page = root;

    for (; level_i < level; ++level_i) {
        pages[level_i] = page;
        if (!Test_(page, branch_nums[level_i], idxes[level_i])) { break; }
        page = ((void**)page)[idxes[level_i]];
    }

    if (level_i == level) { return page; }

    for (; 0 <= level_i; --level_i) {
        page = pages[level_i];

        int found_idx =
            FindNext_(page, branch_nums[level_i], idxes[level_i] + 1);

        if (found_idx != -1) {
            idxes[level_i] = found_idx;
            page = ((void**)page)[found_idx];
            ++level_i;
            break;
        }
    }

    if (level_i == -1) {
        for (level_i = 0; level_i < level; ++level_i) { idxes[level_i] = 0; }
        return NULL;
    }

    for (; level_i < level; ++level_i) {
        int found_idx = FindNext_(page, branch_nums[level_i], 0);
        ZETA_DebugAssert(found_idx != -1);

        idxes[level_i] = found_idx;
        page = ((void**)page)[found_idx];
    }

    return page;
}

void* AllocatePage_(size_t branch_num, void* allocator_context,
                    void* (*Allocate)(void* context, size_t size)) {
    size_t k = (branch_num + mask_width - 1) / mask_width;

    void** ret = Allocate(allocator_context,
                          sizeof(void*) * branch_num + sizeof(mask_t) * k);
    ZETA_DebugAssert(ret != NULL);

    for (size_t idx = 0; idx < branch_num; ++idx) { ret[idx] = NULL; }

    mask_t* mask = (mask_t*)(ret + branch_num);

    for (size_t i = 0; i < k; ++i) { mask[i] = 0; }

    return ret;
}

void** Zeta_MultiLevelVector_Insert(void* mlv_, size_t* idxes) {
    CheckIdxes_(mlv_, idxes);

    Zeta_MultiLevelVector* mlv = mlv_;
    int level = mlv->level;
    size_t* branch_nums = mlv->branch_nums;

    Zeta_Allocator* allocator = mlv->allocator;
    ZETA_DebugAssert(allocator != NULL);

    void* allocator_context = allocator->context;

    void* (*Allocate)(void* context, size_t size) = allocator->Allocate;
    ZETA_DebugAssert(Allocate != NULL);

    if (mlv->root == NULL) {
        mlv->root = AllocatePage_(branch_nums[0], allocator_context, Allocate);
    }

    void* page = mlv->root;

    for (int level_i = 0; level_i < level - 1; ++level_i) {
        SetMask_(page, branch_nums[level_i], idxes[level_i]);

        void** subpage = (void**)page + idxes[level_i];

        if (*subpage == NULL) {
            *subpage = AllocatePage_(mlv->branch_nums[level_i + 1],
                                     allocator_context, Allocate);
        }

        page = *subpage;
    }

    SetMask_(page, branch_nums[level - 1], idxes[level - 1]);

    return (void**)page + idxes[level - 1];
}

void Zeta_MultiLevelVector_Erase(void* mlv_, size_t* idxes) {
    CheckIdxes_(mlv_, idxes);

    Zeta_MultiLevelVector* mlv = mlv_;
    int level = mlv->level;
    size_t* branch_nums = mlv->branch_nums;

    Zeta_Allocator* allocator = mlv->allocator;
    ZETA_DebugAssert(allocator != NULL);

    void* allocator_context = allocator->context;

    void (*Deallocate)(void* context, void* ptr) = allocator->Deallocate;
    ZETA_DebugAssert(Deallocate != NULL);

    void* page = mlv->root;
    if (page == NULL) { return; }

    void* pages[ZETA_MultiLevelVector_max_level];

    for (int level_i = 0; level_i < level; ++level_i) {
        pages[level_i] = page;
        if (!Test_(page, branch_nums[level_i], idxes[level_i])) { return; }
        page = ((void**)page)[idxes[level_i]];
    }

    for (int level_i = level - 1; 0 <= level_i; --level_i) {
        void* page = pages[level_i];
        UnsetMask_(page, branch_nums[level_i], idxes[level_i]);
        if (!IsEmpty_(page, branch_nums[level_i])) { return; }
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
