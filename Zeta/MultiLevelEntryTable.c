#include "MultiLevelEntryTable.h"

void Zeta_MultiLevelEntryTable_Init(void *mlet_) {
    Zeta_MultiLevelEntryTable *mlet = mlet_;
    ZETA_DEBUG_ASSERT(mlet != NULL);

    for (diff_t level_i = 0; level_i < Zeta_MultiLevelEntryTable_max_level;
         ++level_i) {
        mlet->branch_nums[level_i] = 0;
    }

    mlet->level = 0;
    mlet->root = NULL;
    mlet->Allocate = NULL;
    mlet->Deallocate = NULL;
}

void **Zeta_MultiLevelEntryTable_Access(void *mlet_, diff_t *idxes) {
    Zeta_MultiLevelEntryTable *mlet = mlet_;
    ZETA_DEBUG_ASSERT(mlet != NULL);

    ZETA_DEBUG_ASSERT(idxes != NULL);

    diff_t level = mlet->level;
    ZETA_DEBUG_ASSERT(1 <= level &&
                      level <= Zeta_MultiLevelEntryTable_max_level);

    for (diff_t level_i = 0; level_i < level; ++level_i) {
        diff_t branch_num = mlet->branch_nums[level_i];
        diff_t idx = idxes[level_i];

        ZETA_DEBUG_ASSERT(2 <= branch_num &&
                          branch_num <=
                              Zeta_MultiLevelEntryTable_max_branch_num);
        ZETA_DEBUG_ASSERT(0 <= idx && idx < branch_num);
    }

    void **n = &mlet->root;

    for (diff_t level_i = 0; level_i < level; ++level_i) {
        if (*n == NULL) { return NULL; }
        n = (void **)*n + idxes[level_i];
    }

    return n;
}

void *AllocatePage_(diff_t branch_num, void *(*Allocate)(diff_t size)) {
    void **ret = Allocate(sizeof(void *) * branch_num);
    for (diff_t idx = 0; idx < branch_num; ++idx) { ret[idx] = NULL; }
    return ret;
}

void **Zeta_MultiLevelEntryTable_Insert(void *mlet_, diff_t *idxes) {
    Zeta_MultiLevelEntryTable *mlet = mlet_;
    ZETA_DEBUG_ASSERT(mlet != NULL);

    ZETA_DEBUG_ASSERT(idxes != NULL);

    diff_t level = mlet->level;
    ZETA_DEBUG_ASSERT(1 <= level &&
                      level <= Zeta_MultiLevelEntryTable_max_level);

    for (diff_t level_i = 0; level_i < level; ++level_i) {
        diff_t branch_num = mlet->branch_nums[level_i];
        diff_t idx = idxes[level_i];

        ZETA_DEBUG_ASSERT(2 <= branch_num &&
                          branch_num <=
                              Zeta_MultiLevelEntryTable_max_branch_num);
        ZETA_DEBUG_ASSERT(0 <= idx && idx < branch_num);
    }

    void *(*Allocate)(diff_t size) = mlet->Allocate;
    ZETA_DEBUG_ASSERT(Allocate != NULL);

    if (mlet->root == 0) {
        mlet->root = AllocatePage_(mlet->branch_nums[0], Allocate);
    }

    void *page = mlet->root;

    for (diff_t level_i = 0; level_i < level - 1; ++level_i) {
        void **subpage = (void **)page + idxes[level_i];

        if (*subpage == NULL) {
            *subpage = AllocatePage_(mlet->branch_nums[level_i + 1], Allocate);
        }

        page = *subpage;
    }

    return (void **)page + idxes[level - 1];
}

void Zeta_MultiLevelEntryTable_Erase(void *mlet_, diff_t *idxes) {
    Zeta_MultiLevelEntryTable *mlet = mlet_;
    ZETA_DEBUG_ASSERT(mlet != NULL);

    ZETA_DEBUG_ASSERT(idxes != NULL);

    diff_t level = mlet->level;
    ZETA_DEBUG_ASSERT(1 <= level &&
                      level <= Zeta_MultiLevelEntryTable_max_level);

    for (diff_t level_i = 0; level_i < level; ++level_i) {
        diff_t branch_num = mlet->branch_nums[level_i];
        diff_t idx = idxes[level_i];

        ZETA_DEBUG_ASSERT(2 <= branch_num &&
                          branch_num <=
                              Zeta_MultiLevelEntryTable_max_branch_num);
        ZETA_DEBUG_ASSERT(0 <= idx && idx < branch_num);
    }

    void (*Deallocate)(void *page) = mlet->Deallocate;
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

        Deallocate(page);
    }

    mlet->root = NULL;
}

static _Bool FindFirstNotNull_(diff_t *dst_idxes, diff_t level,
                               diff_t *branch_nums, void *page) {
    if (level == 0) { return 1; }

    diff_t idx = 0;
    diff_t branch_num = branch_nums[0];

    for (; idx < branch_num; ++idx) {
        void *subpage = ((void **)page)[idx];
        if (subpage == NULL) { continue; }

        if (FindFirstNotNull_(dst_idxes + 1, level - 1, branch_nums + 1,
                              subpage)) {
            dst_idxes[0] = idx;
            return 1;
        }
    }

    return 0;
}

void **Zeta_MultiLevelEntryTable_GetNextNotNullIdx(void *mlet_, diff_t *idxes,
                                                   diff_t step) {
    Zeta_MultiLevelEntryTable *mlet = mlet_;
    ZETA_DEBUG_ASSERT(mlet != NULL);

    ZETA_DEBUG_ASSERT(idxes != NULL);
    ZETA_DEBUG_ASSERT(0 <= step && step <= 1);

    diff_t level = mlet->level;
    ZETA_DEBUG_ASSERT(1 <= level &&
                      level <= Zeta_MultiLevelEntryTable_max_level);

    for (diff_t level_i = 0; level_i < level; ++level_i) {
        diff_t branch_num = mlet->branch_nums[level_i];
        diff_t idx = idxes[level_i];

        ZETA_DEBUG_ASSERT(2 <= branch_num &&
                          branch_num <=
                              Zeta_MultiLevelEntryTable_max_branch_num);
        ZETA_DEBUG_ASSERT(0 <= idx && idx < branch_num);
    }

    if (step == 1) {
        _Bool b = 0;

        for (diff_t level_i = level - 1; 0 <= level_i; --level_i) {
            ++idxes[level_i];

            if (idxes[level_i] < mlet->branch_nums[level_i]) {
                b = 1;
                break;
            }

            idxes[level_i] = 0;
        }

        if (b == 0) { return NULL; }
    }

    void *n = mlet->root;
    void *pages[Zeta_MultiLevelEntryTable_max_level];

    for (diff_t level_i = 0; level_i < level; ++level_i) {
        pages[level_i] = n;
        if (n != NULL) { n = ((void **)n)[idxes[level_i]]; }
    }

    if (n != NULL) { return n; }

    for (diff_t level_i = level - 1; 0 <= level_i; --level_i) {
        void *page = pages[level_i];
        if (page == NULL) { continue; }

        diff_t branch_num = mlet->branch_nums[level_i];

        for (diff_t idx = idxes[level_i] + 1; idx < branch_num; ++idx) {
            void *subpage = ((void **)page)[idx];
            if (subpage == NULL) { continue; }

            diff_t level_j = level_i + 1;

            if (FindFirstNotNull_(idxes + level_j, level - level_j,
                                  mlet->branch_nums + level_j, subpage)) {
                idxes[level_i] = idx;
                return (void **)pages[level - 1] + idxes[level - 1];
            }
        }
    }

    return NULL;
}

static _Bool Clear_(Zeta_MultiLevelEntryTable *mlet, diff_t level_i,
                    void *page) {  // return if exists any entry
    diff_t level = mlet->level;
    if (level_i == level) { return 0; }

    diff_t branch_num = mlet->branch_nums[level_i];

    _Bool has_entries = 0;

    for (diff_t idx = 0; idx < branch_num; ++idx) {
        void *subpage = ((void **)page)[idx];
        if (subpage == NULL) { continue; }

        if (Clear_(mlet, level_i + 1, subpage)) {
            has_entries = 1;
        } else {
            ((void **)page)[idx] = NULL;
        }
    }

    if (has_entries) { return 1; }

    mlet->Deallocate(page);
    return 0;
}

void Zeta_MultiLevelEntryTable_Clear(void *mlet_) {
    Zeta_MultiLevelEntryTable *mlet = mlet_;
    ZETA_DEBUG_ASSERT(mlet != NULL);

    diff_t level = mlet->level;
    ZETA_DEBUG_ASSERT(1 <= level &&
                      level <= Zeta_MultiLevelEntryTable_max_level);

    for (diff_t level_i = 0; level_i < level; ++level_i) {
        diff_t branch_num = mlet->branch_nums[level_i];
        ZETA_DEBUG_ASSERT(2 <= branch_num &&
                          branch_num <=
                              Zeta_MultiLevelEntryTable_max_branch_num);
    }

    ZETA_DEBUG_ASSERT(mlet->Deallocate != 0);

    void *n = mlet->root;
    if (n == NULL) { return; }

    if (!Clear_(mlet, 0, n)) { mlet->root = NULL; }
}
