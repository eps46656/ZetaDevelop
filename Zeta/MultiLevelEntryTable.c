#include "MultiLevelEntryTable.h"

static void CheckIdxes_(void *mlet_, diff_t *idxes) {
    Zeta_MultiLevelEntryTable *mlet = mlet_;
    ZETA_DEBUG_ASSERT(mlet != NULL);

    ZETA_DEBUG_ASSERT(idxes != NULL);

    diff_t level = mlet->level;
    ZETA_DEBUG_ASSERT(1 <= level);
    ZETA_DEBUG_ASSERT(level <= Zeta_MultiLevelEntryTable_max_level);

    for (diff_t level_i = 0; level_i < level; ++level_i) {
        diff_t branch_num = mlet->branch_nums[level_i];
        diff_t idx = idxes[level_i];

        ZETA_DEBUG_ASSERT(2 <= branch_num);
        ZETA_DEBUG_ASSERT(branch_num <=
                          Zeta_MultiLevelEntryTable_max_branch_num);

        ZETA_DEBUG_ASSERT(0 <= idx)
        ZETA_DEBUG_ASSERT(idx < branch_num);
    }
}

void Zeta_MultiLevelEntryTable_Init(void *mlet_) {
    Zeta_MultiLevelEntryTable *mlet = mlet_;
    ZETA_DEBUG_ASSERT(mlet != NULL);

    for (diff_t level_i = 0; level_i < Zeta_MultiLevelEntryTable_max_level;
         ++level_i) {
        mlet->branch_nums[level_i] = 0;
    }

    mlet->level = 0;
    mlet->root = NULL;
    mlet->allocator = NULL;
}

diff_t Zeta_MultiLevelEntryTable_GetCapacity(void *mlet_) {
    Zeta_MultiLevelEntryTable *mlet = mlet_;
    ZETA_DEBUG_ASSERT(mlet != NULL);

    diff_t ret = 1;

    diff_t level = mlet->level;
    ZETA_DEBUG_ASSERT(1 <= level);
    ZETA_DEBUG_ASSERT(level <= Zeta_MultiLevelEntryTable_max_level);

    for (diff_t level_i = 0; level_i < level; ++level_i) {
        diff_t branch_num = mlet->branch_nums[level_i];

        ZETA_DEBUG_ASSERT(2 <= branch_num);
        ZETA_DEBUG_ASSERT(branch_num <=
                          Zeta_MultiLevelEntryTable_max_branch_num);

        ret *= branch_num;
    }

    return ret;
}

void **Zeta_MultiLevelEntryTable_Access(void *mlet_, diff_t *idxes) {
    CheckIdxes_(mlet_, idxes);

    Zeta_MultiLevelEntryTable *mlet = mlet_;
    diff_t level = mlet->level;

    void **n = &mlet->root;

    for (diff_t level_i = 0; level_i < level; ++level_i) {
        if (*n == NULL) { return NULL; }
        n = (void **)*n + idxes[level_i];
    }

    return n;
}

void *AllocatePage_(diff_t branch_num, void *allocator_context,
                    void *(*Allocate)(void *context, diff_t size)) {
    void **ret = Allocate(allocator_context, sizeof(void *) * branch_num);
    for (diff_t idx = 0; idx < branch_num; ++idx) { ret[idx] = NULL; }
    return ret;
}

void **Zeta_MultiLevelEntryTable_Insert(void *mlet_, diff_t *idxes) {
    CheckIdxes_(mlet_, idxes);

    Zeta_MultiLevelEntryTable *mlet = mlet_;
    diff_t level = mlet->level;

    Zeta_Allocator *allocator = mlet->allocator;
    ZETA_DEBUG_ASSERT(allocator != NULL);

    void *allocator_context = allocator->context;

    void *(*Allocate)(void *context, diff_t size) = allocator->Allocate;
    ZETA_DEBUG_ASSERT(Allocate != NULL);

    if (mlet->root == NULL) {
        mlet->root =
            AllocatePage_(mlet->branch_nums[0], allocator_context, Allocate);
    }

    void *page = mlet->root;

    for (diff_t level_i = 0; level_i < level - 1; ++level_i) {
        void **subpage = (void **)page + idxes[level_i];

        if (*subpage == NULL) {
            *subpage = AllocatePage_(mlet->branch_nums[level_i + 1],
                                     allocator_context, Allocate);
        }

        page = *subpage;
    }

    return (void **)page + idxes[level - 1];
}

void Zeta_MultiLevelEntryTable_Erase(void *mlet_, diff_t *idxes) {
    CheckIdxes_(mlet_, idxes);

    Zeta_MultiLevelEntryTable *mlet = mlet_;
    diff_t level = mlet->level;

    Zeta_Allocator *allocator = mlet->allocator;
    ZETA_DEBUG_ASSERT(allocator != NULL);

    void *allocator_context = allocator->context;

    void (*Deallocate)(void *context, void *ptr) = allocator->Deallocate;
    ZETA_DEBUG_ASSERT(Deallocate != NULL);

    void *n = mlet->root;
    void *pages[Zeta_MultiLevelEntryTable_max_level];

    for (diff_t level_i = 0; level_i < level; ++level_i) {
        if (n == NULL) { return; }
        pages[level_i] = n;
        n = ((void **)n)[idxes[level_i]];
    }

    for (diff_t level_i = level - 1; 0 <= level_i; --level_i) {
        void *page = pages[level_i];
        ((void **)page)[idxes[level_i]] = NULL;

        diff_t branch_num = mlet->branch_nums[level_i];

        for (diff_t idx = 0; idx < branch_num; ++idx) {
            if (((void **)page)[idx] != NULL) { return; }
        }

        Deallocate(allocator_context, page);
    }

    mlet->root = NULL;
}

static void **FindPrevNotNull_(diff_t level, diff_t *branch_nums, diff_t *idxes,
                               void *page) {
    diff_t branch_num = branch_nums[0];

    if (level == 1) {
        for (diff_t idx = idxes[0]; 0 <= idx; --idx) {
            if (((void **)page)[idx] != NULL) {
                idxes[0] = idx;
                return (void **)page + idx;
            }
        }

        idxes[0] = branch_num - 1;
        return NULL;
    }

    for (diff_t idx = idxes[0]; 0 <= idx; --idx) {
        void *subpage = ((void **)page)[idx];
        if (subpage == NULL) { continue; }

        void **ret =
            FindPrevNotNull_(level - 1, branch_nums + 1, idxes + 1, subpage);

        if (ret != NULL) {
            idxes[0] = idx;
            return ret;
        }
    }

    idxes[0] = branch_num - 1;
    return NULL;
}

static void **FindNextNotNull_(diff_t level, diff_t *branch_nums, diff_t *idxes,
                               void *page) {
    diff_t branch_num = branch_nums[0];

    if (level == 1) {
        for (diff_t idx = idxes[0]; idx < branch_num; ++idx) {
            if (((void **)page)[idx] != NULL) {
                idxes[0] = idx;
                return (void **)page + idx;
            }
        }

        idxes[0] = 0;
        return NULL;
    }

    for (diff_t idx = idxes[0]; idx < branch_num; ++idx) {
        void *subpage = ((void **)page)[idx];
        if (subpage == NULL) { continue; }

        void **ret =
            FindNextNotNull_(level - 1, branch_nums + 1, idxes + 1, subpage);

        if (ret != NULL) {
            idxes[0] = idx;
            return ret;
        }
    }

    idxes[0] = 0;
    return NULL;
}

void **Zeta_MultiLevelEntryTable_FindNextNotNull(void *mlet_, diff_t *idxes,
                                                 _Bool included) {
    CheckIdxes_(mlet_, idxes);

    Zeta_MultiLevelEntryTable *mlet = mlet_;
    diff_t level = mlet->level;

    if (!included) {
        for (diff_t level_i = level - 1; 0 <= level_i; --level_i) {
            ++idxes[level_i];
            if (idxes[level_i] < mlet->branch_nums[level_i]) { goto L1; }
            idxes[level_i] = 0;
        }

        return NULL;
    }

L1:;

    void *page = mlet->root;

    if (page != NULL) {
        return FindNextNotNull_(level, mlet->branch_nums, idxes, page);
    }

    for (diff_t level_i = 0; level_i < level; ++level_i) { idxes[level_i] = 0; }

    return NULL;
}

void **Zeta_MultiLevelEntryTable_FindPrevNotNull(void *mlet_, diff_t *idxes,
                                                 _Bool included) {
    CheckIdxes_(mlet_, idxes);

    Zeta_MultiLevelEntryTable *mlet = mlet_;
    diff_t level = mlet->level;

    if (!included) {
        for (diff_t level_i = level - 1; 0 <= level_i; --level_i) {
            --idxes[level_i];
            if (0 <= idxes[level_i]) { goto L1; }
            idxes[level_i] = mlet->branch_nums[level_i] - 1;
        }

        return NULL;
    }

L1:;

    void *page = mlet->root;

    if (page != NULL) {
        return FindPrevNotNull_(level, mlet->branch_nums, idxes, page);
    }

    for (diff_t level_i = 0; level_i < level; ++level_i) { idxes[level_i] = 0; }

    return NULL;
}

static void EraseAll_(diff_t level, diff_t *branch_nums, void *page,
                      void *allocator_context,
                      void (*Deallocate)(void *context, void *ptr),
                      void (*Callback)(void *ptr)) {
    if (level == 0) { return; }

    diff_t branch_num = branch_nums[0];

    for (diff_t idx = 0; idx < branch_num; ++idx) {
        void *subpage = ((void **)page)[idx];

        if (subpage != NULL) {
            EraseAll_(level + 1, branch_nums + 1, subpage, allocator_context,
                      Deallocate, Callback);
        }
    }

    Deallocate(allocator_context, page);
}

void Zeta_MultiLevelEntryTable_EraseAll(void *mlet_,
                                        void (*Callback)(void *ptr)) {
    Zeta_MultiLevelEntryTable *mlet = mlet_;
    ZETA_DEBUG_ASSERT(mlet != NULL);

    diff_t level = mlet->level;
    ZETA_DEBUG_ASSERT(1 <= level);
    ZETA_DEBUG_ASSERT(level <= Zeta_MultiLevelEntryTable_max_level);

    for (diff_t level_i = 0; level_i < level; ++level_i) {
        diff_t branch_num = mlet->branch_nums[level_i];
        ZETA_DEBUG_ASSERT(2 <= branch_num);
        ZETA_DEBUG_ASSERT(branch_num <=
                          Zeta_MultiLevelEntryTable_max_branch_num);
    }

    void *n = mlet->root;
    if (n == NULL) { return; }

    Zeta_Allocator *allocator = mlet->allocator;
    ZETA_DEBUG_ASSERT(allocator != NULL);

    void (*Deallocate)(void *context, void *ptr) = allocator->Deallocate;
    ZETA_DEBUG_ASSERT(Deallocate != NULL);

    ZETA_DEBUG_ASSERT(Callback != NULL);

    EraseAll_(level, mlet->branch_nums, n, allocator->context, Deallocate,
              Callback);

    mlet->root = NULL;
}

static _Bool Clear_(
    diff_t level, diff_t *branch_nums, void *page, void *allocator_context,
    void (*Deallocate)(void *context,
                       void *ptr)) {  // return if exists any entry
    if (level == 0) { return 0; }

    diff_t branch_num = branch_nums[0];

    _Bool has_entries = 0;

    for (diff_t idx = 0; idx < branch_num; ++idx) {
        void *subpage = ((void **)page)[idx];
        if (subpage == NULL) { continue; }

        if (Clear_(level + 1, branch_nums + 1, subpage, allocator_context,
                   Deallocate)) {
            has_entries = 1;
        } else {
            ((void **)page)[idx] = NULL;
        }
    }

    if (has_entries) { return 1; }

    Deallocate(allocator_context, page);
    return 0;
}

void Zeta_MultiLevelEntryTable_Clear(void *mlet_) {
    Zeta_MultiLevelEntryTable *mlet = mlet_;
    ZETA_DEBUG_ASSERT(mlet != NULL);

    diff_t level = mlet->level;
    ZETA_DEBUG_ASSERT(1 <= level);
    ZETA_DEBUG_ASSERT(level <= Zeta_MultiLevelEntryTable_max_level);

    for (diff_t level_i = 0; level_i < level; ++level_i) {
        diff_t branch_num = mlet->branch_nums[level_i];
        ZETA_DEBUG_ASSERT(2 <= branch_num);
        ZETA_DEBUG_ASSERT(branch_num <=
                          Zeta_MultiLevelEntryTable_max_branch_num);
    }

    void *n = mlet->root;
    if (n == NULL) { return; }

    Zeta_Allocator *allocator = mlet->allocator;
    ZETA_DEBUG_ASSERT(allocator != NULL);

    void (*Deallocate)(void *context, void *ptr) = allocator->Deallocate;
    ZETA_DEBUG_ASSERT(Deallocate != NULL);

    if (!Clear_(level, mlet->branch_nums, n, allocator->context, Deallocate)) {
        mlet->root = NULL;
    }
}
