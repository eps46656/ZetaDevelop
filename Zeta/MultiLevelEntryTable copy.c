#include "MultiLevelEntryTable.h"

static void CheckIdxes_(void* mlet_, size_t* idxes) {
    Zeta_MultiLevelEntryTable* mlet = mlet_;
    ZETA_DebugAssert(mlet != NULL);

    ZETA_DebugAssert(idxes != NULL);

    int level = mlet->level;
    ZETA_DebugAssert(1 <= level);
    ZETA_DebugAssert(level <= ZETA_MultiLevelEntryTable_max_level);

    for (int level_i = 0; level_i < level; ++level_i) {
        size_t branch_num = mlet->branch_nums[level_i];
        size_t idx = idxes[level_i];

        ZETA_DebugAssert(2 <= branch_num);
        ZETA_DebugAssert(branch_num <=
                         ZETA_MultiLevelEntryTable_max_branch_num);

        ZETA_DebugAssert(0 <= idx) ZETA_DebugAssert(idx < branch_num);
    }
}

void Zeta_MultiLevelEntryTable_Init(void* mlet_) {
    Zeta_MultiLevelEntryTable* mlet = mlet_;
    ZETA_DebugAssert(mlet != NULL);

    int level = mlet->level;

    if (level <= 0) {
        level = 6;

        mlet->level = level;

        mlet->branch_nums[0] = 64;
        mlet->branch_nums[1] = 64;
        mlet->branch_nums[2] = 128;
        mlet->branch_nums[3] = 128;
        mlet->branch_nums[4] = 256;
        mlet->branch_nums[5] = 256;
    }

    ZETA_DebugAssert(1 <= level);
    ZETA_DebugAssert(level < ZETA_MultiLevelEntryTable_max_level);

    for (int level_i = 0; level_i < level; ++level_i) {
        size_t branch_num = mlet->branch_nums[level_i];

        ZETA_DebugAssert(2 <= branch_num);
        ZETA_DebugAssert(branch_num <=
                         ZETA_MultiLevelEntryTable_max_branch_num);
    }

    mlet->root = NULL;

    ZETA_DebugAssert(mlet->allocator != NULL);
}

size_t Zeta_MultiLevelEntryTable_GetCapacity(void* mlet_) {
    Zeta_MultiLevelEntryTable* mlet = mlet_;
    ZETA_DebugAssert(mlet != NULL);

    size_t ret = 1;

    int level = mlet->level;
    ZETA_DebugAssert(1 <= level);
    ZETA_DebugAssert(level <= ZETA_MultiLevelEntryTable_max_level);

    for (int level_i = 0; level_i < level; ++level_i) {
        size_t branch_num = mlet->branch_nums[level_i];

        ZETA_DebugAssert(2 <= branch_num);
        ZETA_DebugAssert(branch_num <=
                         ZETA_MultiLevelEntryTable_max_branch_num);

        ZETA_DebugAssert(ret <= ZETA_maxof(size_t) / branch_num);

        ret *= branch_num;
    }

    return ret;
}

void** Zeta_MultiLevelEntryTable_Access(void* mlet_, size_t* idxes) {
    CheckIdxes_(mlet_, idxes);

    Zeta_MultiLevelEntryTable* mlet = mlet_;
    int level = mlet->level;

    void** n = &mlet->root;

    for (int level_i = 0; level_i < level; ++level_i) {
        if (*n == NULL) { return NULL; }
        n = (void**)*n + idxes[level_i];
    }

    return n;
}

static void** FindPrevNotNull_(int level, size_t* branch_nums, size_t* idxes,
                               void* page) {
    size_t branch_num = branch_nums[0];

    if (level == 1) {
        for (size_t idx = idxes[0] + 1; 1 <= --idx;) {
            if (((void**)page)[idx] != NULL) {
                idxes[0] = idx;
                return (void**)page + idx;
            }
        }

        idxes[0] = branch_num - 1;
        return NULL;
    }

    for (size_t idx = idxes[0] + 1; 1 <= --idx;) {
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

static void** FindNextNotNull_(int level, size_t* branch_nums, size_t* idxes,
                               void* page) {
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

void** Zeta_MultiLevelEntryTable_FindPrevNotNull(void* mlet_, size_t* idxes,
                                                 bool_t included) {
    CheckIdxes_(mlet_, idxes);

    Zeta_MultiLevelEntryTable* mlet = mlet_;
    int level = mlet->level;

    if (!included) {
        for (int level_i = level - 1; 0 <= level_i; --level_i) {
            if (1 <= idxes[level_i]) {
                --idxes[level_i];
                goto L1;
            }

            idxes[level_i] = mlet->branch_nums[level_i] - 1;
        }

        return NULL;
    }

L1:;

    void* page = mlet->root;

    if (page != NULL) {
        return FindPrevNotNull_(level, mlet->branch_nums, idxes, page);
    }

    for (int level_i = 0; level_i < level; ++level_i) { idxes[level_i] = 0; }

    return NULL;
}

void** Zeta_MultiLevelEntryTable_FindNextNotNull(void* mlet_, size_t* idxes,
                                                 bool_t included) {
    CheckIdxes_(mlet_, idxes);

    Zeta_MultiLevelEntryTable* mlet = mlet_;
    int level = mlet->level;

    if (!included) {
        for (int level_i = level - 1; 0 <= level_i; --level_i) {
            ++idxes[level_i];
            if (idxes[level_i] < mlet->branch_nums[level_i]) { goto L1; }
            idxes[level_i] = 0;
        }

        return NULL;
    }

L1:;

    void* page = mlet->root;

    if (page != NULL) {
        return FindNextNotNull_(level, mlet->branch_nums, idxes, page);
    }

    for (int level_i = 0; level_i < level; ++level_i) { idxes[level_i] = 0; }

    return NULL;
}

void* AllocatePage_(size_t branch_num, void* allocator_context,
                    void* (*Allocate)(void* context, size_t size)) {
    void** ret = Allocate(allocator_context, sizeof(void*) * branch_num);

    for (size_t idx = 0; idx < branch_num; ++idx) { ret[idx] = NULL; }

    return ret;
}

void** Zeta_MultiLevelEntryTable_Insert(void* mlet_, size_t* idxes) {
    CheckIdxes_(mlet_, idxes);

    Zeta_MultiLevelEntryTable* mlet = mlet_;
    int level = mlet->level;

    Zeta_Allocator* allocator = mlet->allocator;
    ZETA_DebugAssert(allocator != NULL);

    void* allocator_context = allocator->context;

    void* (*Allocate)(void* context, size_t size) = allocator->Allocate;
    ZETA_DebugAssert(Allocate != NULL);

    if (mlet->root == NULL) {
        mlet->root =
            AllocatePage_(mlet->branch_nums[0], allocator_context, Allocate);
    }

    void* page = mlet->root;

    for (int level_i = 0; level_i < level - 1; ++level_i) {
        void** subpage = (void**)page + idxes[level_i];

        if (*subpage == NULL) {
            *subpage = AllocatePage_(mlet->branch_nums[level_i + 1],
                                     allocator_context, Allocate);
        }

        page = *subpage;
    }

    return (void**)page + idxes[level - 1];
}

void Zeta_MultiLevelEntryTable_Erase(void* mlet_, size_t* idxes) {
    CheckIdxes_(mlet_, idxes);

    Zeta_MultiLevelEntryTable* mlet = mlet_;
    int level = mlet->level;

    Zeta_Allocator* allocator = mlet->allocator;
    ZETA_DebugAssert(allocator != NULL);

    void* allocator_context = allocator->context;

    void (*Deallocate)(void* context, void* ptr) = allocator->Deallocate;
    ZETA_DebugAssert(Deallocate != NULL);

    void* n = mlet->root;
    void* pages[ZETA_MultiLevelEntryTable_max_level];

    for (int level_i = 0; level_i < level; ++level_i) {
        if (n == NULL) { return; }
        pages[level_i] = n;
        n = ((void**)n)[idxes[level_i]];
    }

    for (int level_i = level - 1; 0 <= level_i; --level_i) {
        void* page = pages[level_i];
        ((void**)page)[idxes[level_i]] = NULL;

        size_t branch_num = mlet->branch_nums[level_i];

        for (size_t idx = 0; idx < branch_num; ++idx) {
            if (((void**)page)[idx] != NULL) { return; }
        }

        Deallocate(allocator_context, page);
    }

    mlet->root = NULL;

    return;
}

static void EraseAll_(int level, size_t* branch_nums, void* page,
                      void* allocator_context,
                      void (*Deallocate)(void* context, void* ptr),
                      void* callback_context,
                      void (*Callback)(void* context, void* ptr)) {
    size_t branch_num = branch_nums[0];

    if (level == 1) {
        if (Callback == NULL) { return; }

        for (size_t idx = 0; idx < branch_num; ++idx) {
            void* ptr = ((void**)page)[idx];
            if (ptr != NULL) { Callback(callback_context, ptr); }
        }

        return;
    }

    for (size_t idx = 0; idx < branch_num; ++idx) {
        void* subpage = ((void**)page)[idx];

        if (subpage != NULL) {
            EraseAll_(level + 1, branch_nums + 1, subpage, allocator_context,
                      Deallocate, callback_context, Callback);
        }
    }

    Deallocate(allocator_context, page);
}

void Zeta_MultiLevelEntryTable_EraseAll(void* mlet_, void* callback_context,
                                        void (*Callback)(void* context,
                                                         void* ptr)) {
    Zeta_MultiLevelEntryTable* mlet = mlet_;
    ZETA_DebugAssert(mlet != NULL);

    int level = mlet->level;
    ZETA_DebugAssert(1 <= level);
    ZETA_DebugAssert(level <= ZETA_MultiLevelEntryTable_max_level);

    for (int level_i = 0; level_i < level; ++level_i) {
        size_t branch_num = mlet->branch_nums[level_i];
        ZETA_DebugAssert(2 <= branch_num);
        ZETA_DebugAssert(branch_num <=
                         ZETA_MultiLevelEntryTable_max_branch_num);
    }

    void* n = mlet->root;
    if (n == NULL) { return; }

    Zeta_Allocator* allocator = mlet->allocator;
    ZETA_DebugAssert(allocator != NULL);

    void (*Deallocate)(void* context, void* ptr) = allocator->Deallocate;
    ZETA_DebugAssert(Deallocate != NULL);

    EraseAll_(level, mlet->branch_nums, n, allocator->context, Deallocate,
              callback_context, Callback);

    mlet->root = NULL;
}

static bool_t Clear_(
    int level, size_t* branch_nums, void* page, void* allocator_context,
    void (*Deallocate)(void* context,
                       void* ptr)) {  // return if exists any entry
    if (level == 0) { return 0; }

    size_t branch_num = branch_nums[0];

    bool_t has_entries = 0;

    for (size_t idx = 0; idx < branch_num; ++idx) {
        void* subpage = ((void**)page)[idx];
        if (subpage == NULL) { continue; }

        if (Clear_(level + 1, branch_nums + 1, subpage, allocator_context,
                   Deallocate)) {
            has_entries = 1;
        } else {
            ((void**)page)[idx] = NULL;
        }
    }

    if (has_entries) { return 1; }

    Deallocate(allocator_context, page);
    return 0;
}

void Zeta_MultiLevelEntryTable_Clear(void* mlet_) {
    Zeta_MultiLevelEntryTable* mlet = mlet_;
    ZETA_DebugAssert(mlet != NULL);

    int level = mlet->level;
    ZETA_DebugAssert(1 <= level);
    ZETA_DebugAssert(level <= ZETA_MultiLevelEntryTable_max_level);

    for (int level_i = 0; level_i < level; ++level_i) {
        size_t branch_num = mlet->branch_nums[level_i];
        ZETA_DebugAssert(2 <= branch_num);
        ZETA_DebugAssert(branch_num <=
                         ZETA_MultiLevelEntryTable_max_branch_num);
    }

    void* n = mlet->root;
    if (n == NULL) { return; }

    Zeta_Allocator* allocator = mlet->allocator;
    ZETA_DebugAssert(allocator != NULL);

    void (*Deallocate)(void* context, void* ptr) = allocator->Deallocate;
    ZETA_DebugAssert(Deallocate != NULL);

    if (!Clear_(level, mlet->branch_nums, n, allocator->context, Deallocate)) {
        mlet->root = NULL;
    }
}

static void GetAllPages_(Zeta_DebugTreeMap* dst, void* page, int level,
                         const size_t* branch_nums) {
    Zeta_DebugTreeMap_Insert(dst, (size_t)(uintptr_t)page);

    if (level == 0) { return; }

    for (size_t i = 0; i < branch_nums[0]; ++i) {
        void* subpage = ((void**)page)[i];

        if (subpage != NULL) {
            GetAllPages_(dst, subpage, level - 1, branch_nums + 1);
        }
    }
}

void Zeta_MultiLevelEntryTable_GetAllPages(void* mlet_,
                                           Zeta_DebugTreeMap* dst) {
    Zeta_MultiLevelEntryTable* mlet = mlet_;
    ZETA_DebugAssert(mlet != NULL);

    ZETA_DebugAssert(dst != NULL);

    if (mlet->root != NULL) {
        GetAllPages_(dst, mlet->root, mlet->level - 1, mlet->branch_nums);
    }
}
