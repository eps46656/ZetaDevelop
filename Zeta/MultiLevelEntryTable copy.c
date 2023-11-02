#include "MultiLevelEntryTable.h"

void Zeta_MultiLevelEntryTable_Init(void *mlet_) {
    Zeta_MultiLevelEntryTable *mlet = mlet_;
    ZETA_DEBUG_ASSERT(mlet != NULL);

    for (diff_t i = 0; i < Zeta_MultiLevelEntryTable_max_level; ++i) {
        mlet->branch_nums[i] = 0;
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

    void **n = &mlet->root;

    for (diff_t level_i = 0; level_i < level; ++level_i) {
        if (*n == NULL) {
            diff_t size = sizeof(void *) * mlet->branch_nums[level_i];
            void **page = Allocate(size);
            memset(page, 0, size);
            *n = page;
        }

        n = (void **)*n + idxes[level_i];
    }

    return n;
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

    void **n = &mlet->root;
    void **pages[Zeta_MultiLevelEntryTable_max_level];

    for (diff_t level_i = 0; level_i < level; ++level_i) {
        if (*n == NULL) { return; }
        pages[level_i] = *n;
        n = (void **)*n + idxes[level_i];
    }

    for (diff_t level_i = level - 1; 0 <= level_i; --level_i) {
        ((void **)pages[level_i])[idxes[level_i]] = NULL;
        diff_t branch_num = mlet->branch_nums[level_i];

        for (diff_t idx = 0; idx < branch_num; ++idx) {
            void *subpage = ((void **)pages[level_i])[idx];
            if (subpage != NULL) { return; }
        }

        Deallocate(pages[level_i]);
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
            break;
        }
    }

    if (idx == branch_num) { return 0; }

    dst_idxes[0] = idx;
    return 1;
}

_Bool Zeta_MultiLevelEntryTable_GetFirstNotNullIdx(void *mlet_,
                                                   diff_t *dst_idxes) {
    Zeta_MultiLevelEntryTable *mlet = mlet_;
    ZETA_DEBUG_ASSERT(mlet != NULL);

    ZETA_DEBUG_ASSERT(dst_idxes != NULL);

    diff_t level = mlet->level;
    ZETA_DEBUG_ASSERT(1 <= level &&
                      level <= Zeta_MultiLevelEntryTable_max_level);

    if (mlet->root == NULL) { return 0; }

    return FindFirstNotNull_(dst_idxes, level, mlet->branch_nums, mlet->root);
}

_Bool Zeta_MultiLevelEntryTable_GetNextNotNullIdx(void *mlet_, diff_t *idxes) {
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

    _Bool b = 0;

    for (diff_t level_i = level - 1; 0 <= level_i; --level_i) {
        ++idxes[level_i];

        if (idxes[level_i] < mlet->branch_nums[level_i]) {
            b = 1;
            break;
        }

        idxes[level_i] = 0;
    }

    if (b == 0) { return 0; }

    void *n = mlet->root;
    void *pages[Zeta_MultiLevelEntryTable_max_level];

    for (diff_t level_i = 0; level_i < level; ++level_i) {
        pages[level_i] = n;
        if (n != NULL) { n = ((void **)n)[idxes[level_i]]; }
    }

    if (n != NULL) { return 1; }

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
                return 1;
            }
        }
    }

    return 0;
}

static _Bool Clear_(Zeta_MultiLevelEntryTable *mlet, diff_t level_i,
                    void *page) {  // return if exists any entry
    diff_t level = mlet->level;
    if (level_i == level) { return 0; }

    diff_t branch_num = mlet->branch_nums[level_i];

    _Bool has_entries = 0;

    for (diff_t i = 0; i < branch_num; ++i) {
        void *subpage = ((void **)page)[i];
        if (subpage == NULL) { continue; }

        if (Clear_(mlet, level_i + 1, subpage)) {
            has_entries = 1;
        } else {
            ((void **)page)[i] = NULL;
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
