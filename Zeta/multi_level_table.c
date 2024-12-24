#include "multi_level_table.h"

#include "debugger.h"

#if ZETA_EnableDebug

#define CheckCntr_(cntr) Zeta_MultiLevelTable_Check(cntr)

#define CheckIdxes_(cntr, idxes) Zeta_MultiLevelTable_CheckIdxes(cntr, idxes)

#else

#define CheckCntr_(cntr)

#define CheckIdxes_(cntr, idxes)

#endif

#define TestHot_(hot, idx) (((hot) >> (idx)) % 2 != 0)

static int FindPrevHot_(unsigned long long hot, size_t idx) {
    ZETA_CheckAssert(idx == (size_t)(-1) || idx <= ZETA_ULLONG_WIDTH - 1);

    if (idx == (size_t)(-1)) { return -1; }

    hot = hot << (ZETA_ULLONG_WIDTH - 1 - idx) >> (ZETA_ULLONG_WIDTH - 1 - idx);

    return hot == 0 ? -1 : ZETA_ULLONG_WIDTH - 1 - __builtin_clzll(hot);
}

static int FindNextHot_(unsigned long long hot, size_t idx) {
    ZETA_CheckAssert(idx <= ZETA_ULLONG_WIDTH);

    if (idx == ZETA_ULLONG_WIDTH) { return -1; }

    hot = hot >> idx << idx;

    return hot == 0 ? -1 : __builtin_ctzll(hot);
}

void Zeta_MultiLevelTable_Init(void* mlt_) {
    Zeta_MultiLevelTable* mlt = mlt_;
    ZETA_DebugAssert(mlt != NULL);

    int level = mlt->level;
    ZETA_DebugAssert(0 < level);
    ZETA_DebugAssert(level <= ZETA_MultiLevelTable_max_level);

    unsigned short const* branch_nums = mlt->branch_nums;

    for (int level_i = 0; level_i < level; ++level_i) {
        size_t branch_num = branch_nums[level_i];

        ZETA_DebugAssert(ZETA_MultiLevelTable_min_branch_num <= branch_num);
        ZETA_DebugAssert(branch_num <= ZETA_MultiLevelTable_max_branch_num);
    }

    mlt->size = 0;

    mlt->root = NULL;

    ZETA_DebugAssert(mlt->node_allocator != NULL);
    ZETA_DebugAssert(mlt->node_allocator->GetAlign != NULL);
    ZETA_DebugAssert(
        mlt->node_allocator->GetAlign(mlt->node_allocator->context) %
            alignof(Zeta_MultiLevelTable_Node) ==
        0);
}

void Zeta_MultiLevelTable_Deinit(void* mlt) {
    Zeta_MultiLevelTable_EraseAll(mlt);
}

size_t Zeta_MultiLevelTable_GetSize(void* mlt_) {
    Zeta_MultiLevelTable* mlt = mlt_;
    CheckCntr_(mlt);

    return mlt->size;
}

size_t Zeta_MultiLevelTable_GetCapacity(void* mlt_) {
    Zeta_MultiLevelTable* mlt = mlt_;
    CheckCntr_(mlt);

    size_t ret = 1;

    int level = mlt->level;
    unsigned short const* branch_nums = mlt->branch_nums;

    for (int level_i = 0; level_i < level; ++level_i) {
        if (__builtin_umulll_overflow(ret, branch_nums[level_i], &ret)) {
            return ZETA_SIZE_MAX;
        }
    }

    return ret;
}

void** Zeta_MultiLevelTable_Access(void* mlt_, size_t* idxes) {
    Zeta_MultiLevelTable* mlt = mlt_;
    CheckCntr_(mlt);

    int level = mlt->level;

    Zeta_MultiLevelTable_Node* node = mlt->root;

    if (node == NULL) { return NULL; }

    for (int level_i = level - 1; 0 < level_i; --level_i) {
        size_t cur_idx = idxes[level_i];
        if (!TestHot_(node->hot, cur_idx)) { return NULL; }
        node = node->ptrs[cur_idx];
    }

    size_t last_idx = idxes[0];

    return TestHot_(node->hot, last_idx) ? node->ptrs + last_idx : NULL;
}

void** Zeta_MultiLevelTable_FindFirst(void* mlt_, size_t* dst_idxes) {
    Zeta_MultiLevelTable* mlt = mlt_;
    CheckCntr_(mlt);

    int level = mlt->level;

    size_t idxes[ZETA_MultiLevelTable_max_level];

    for (int level_i = 0; level_i < level; ++level_i) { idxes[level_i] = 0; }

    void** ret = Zeta_MultiLevelTable_FindNext(mlt, idxes, TRUE);

    if (dst_idxes != NULL) {
        for (int level_i = 0; level_i < level; ++level_i) {
            dst_idxes[level_i] = idxes[level_i];
        }
    }

    return ret;
}

void** Zeta_MultiLevelTable_FindLast(void* mlt_, size_t* dst_idxes) {
    Zeta_MultiLevelTable* mlt = mlt_;
    CheckCntr_(mlt);

    int level = mlt->level;
    unsigned short const* branch_nums = mlt->branch_nums;

    size_t idxes[ZETA_MultiLevelTable_max_level];

    for (int level_i = 0; level_i < level; ++level_i) {
        idxes[level_i] = branch_nums[level_i] - 1;
    }

    void** ret = Zeta_MultiLevelTable_FindPrev(mlt, idxes, TRUE);

    if (dst_idxes != NULL) {
        for (int level_i = 0; level_i < level; ++level_i) {
            dst_idxes[level_i] = idxes[level_i];
        }
    }

    return ret;
}

void** Zeta_MultiLevelTable_FindPrev(void* mlt_, size_t* idxes,
                                     bool_t included) {
    Zeta_MultiLevelTable* mlt = mlt_;
    CheckIdxes_(mlt, idxes);

    int level = mlt->level;
    unsigned short const* branch_nums = mlt->branch_nums;

    if (!included) {
        for (int level_i = 0; level_i < level; ++level_i) {
            if (0 < idxes[level_i]) {
                --idxes[level_i];
                goto L1;
            }

            idxes[level_i] = branch_nums[level_i] - 1;
        }

        return NULL;
    }

L1:;

    Zeta_MultiLevelTable_Node* root = mlt->root;

    if (root == NULL) {
        for (int level_i = 0; level_i < level; ++level_i) {
            idxes[level_i] = branch_nums[level_i] - 1;
        }

        return NULL;
    }

    Zeta_MultiLevelTable_Node* nodes[ZETA_MultiLevelTable_max_level];

    int level_i = level - 1;
    Zeta_MultiLevelTable_Node* node = root;

    for (;; --level_i) {
        nodes[level_i] = node;
        if (!TestHot_(node->hot, idxes[level_i])) { break; }

        void** nxt_node = node->ptrs + idxes[level_i];
        if (level_i == 0) { return nxt_node; }

        node = *nxt_node;
    }

    for (; level_i < level; ++level_i) {
        node = nodes[level_i];

        int found_idx = FindPrevHot_(node->hot, idxes[level_i] - 1);

        if (0 <= found_idx) {
            idxes[level_i] = found_idx;
            break;
        }
    }

    if (level_i == level) {
        for (int level_i = 0; level_i < level; ++level_i) {
            idxes[level_i] = branch_nums[level_i] - 1;
        }

        return NULL;
    }

    while (0 < level_i) {
        node = node->ptrs[idxes[level_i]];
        --level_i;
        nodes[level_i] = node;
        idxes[level_i] = FindPrevHot_(node->hot, branch_nums[level_i] - 1);
    }

    return nodes[0]->ptrs + idxes[0];
}

void** Zeta_MultiLevelTable_FindNext(void* mlt_, size_t* idxes,
                                     bool_t included) {
    Zeta_MultiLevelTable* mlt = mlt_;
    CheckIdxes_(mlt, idxes);

    int level = mlt->level;
    unsigned short const* branch_nums = mlt->branch_nums;

    if (!included) {
        for (int level_i = 0; level_i < level; ++level_i) {
            if (idxes[level_i] < branch_nums[level_i] - 1) {
                ++idxes[level_i];
                goto L1;
            }

            idxes[level_i] = 0;
        }

        return NULL;
    }

L1:;

    Zeta_MultiLevelTable_Node* root = mlt->root;

    if (root == NULL) {
        for (int level_i = 0; level_i < level; ++level_i) {
            idxes[level_i] = 0;
        }

        return NULL;
    }

    Zeta_MultiLevelTable_Node* nodes[ZETA_MultiLevelTable_max_level];

    int level_i = level - 1;
    Zeta_MultiLevelTable_Node* node = root;

    for (;; --level_i) {
        nodes[level_i] = node;
        if (!TestHot_(node->hot, idxes[level_i])) { break; }

        void** nxt_node = node->ptrs + idxes[level_i];
        if (level_i == 0) { return nxt_node; }

        node = *nxt_node;
    }

    for (; level_i < level; ++level_i) {
        node = nodes[level_i];

        int found_idx = FindNextHot_(node->hot, idxes[level_i] + 1);

        if (0 <= found_idx) {
            idxes[level_i] = found_idx;
            break;
        }
    }

    if (level_i == level) {
        for (int level_i = 0; level_i < level; ++level_i) {
            idxes[level_i] = 0;
        }

        return NULL;
    }

    while (0 < level_i) {
        node = node->ptrs[idxes[level_i]];
        --level_i;
        nodes[level_i] = node;
        idxes[level_i] = FindNextHot_(node->hot, 0);
    }

    return nodes[0]->ptrs + idxes[0];
}

static Zeta_MultiLevelTable_Node* AllocateNode_(
    size_t branch_num, Zeta_Allocator* node_allocator) {
    Zeta_MultiLevelTable_Node* node = ZETA_Allocator_SafeAllocate(
        node_allocator, alignof(Zeta_MultiLevelTable_Node),
        offsetof(Zeta_MultiLevelTable_Node, ptrs[branch_num]));

    node->hot = 0;

    return node;
}

void** Zeta_MultiLevelTable_Insert(void* mlt_, size_t* idxes) {
    Zeta_MultiLevelTable* mlt = mlt_;
    CheckIdxes_(mlt, idxes);

    int level = mlt->level;
    unsigned short const* branch_nums = mlt->branch_nums;

    if (mlt->root == NULL) {
        mlt->root = AllocateNode_(branch_nums[0], mlt->node_allocator);
    }

    Zeta_MultiLevelTable_Node* node = mlt->root;

    int level_i = level - 1;

    for (; 0 < level_i && TestHot_(node->hot, idxes[level_i]); --level_i) {
        node = node->ptrs[idxes[level_i]];
    }

    for (; 0 < level_i; --level_i) {
        node->hot = node->hot + (1ULL << idxes[level_i]);

        node = node->ptrs[idxes[level_i]] =
            AllocateNode_(branch_nums[level_i + 1], mlt->node_allocator);
    }

    void** ret = node->ptrs + idxes[0];

    if (TestHot_(node->hot, idxes[0])) { return ret; }

    ++mlt->size;

    node->hot = node->hot + (1ULL << idxes[0]);
    *ret = NULL;

    return ret;
}

void* Zeta_MultiLevelTable_Erase(void* mlt_, size_t* idxes) {
    Zeta_MultiLevelTable* mlt = mlt_;
    CheckIdxes_(mlt, idxes);

    int level = mlt->level;

    Zeta_MultiLevelTable_Node* node = mlt->root;
    if (node == NULL) { return NULL; }

    Zeta_MultiLevelTable_Node* nodes[ZETA_MultiLevelTable_max_level];

    for (int level_i = level - 1;; --level_i) {
        nodes[level_i] = node;
        if (level_i == 0) { break; }
        if (!TestHot_(node->hot, idxes[level_i])) { return NULL; }
        node = node->ptrs[idxes[level_i]];
    }

    if (!TestHot_(node->hot, idxes[0])) { return NULL; }

    void* ret = node->ptrs[idxes[0]];

    --mlt->size;

    for (int level_i = 0; level_i < level; ++level_i) {
        node = nodes[level_i];

        node->hot = node->hot - (1ULL << idxes[level_i]);

        if (node->hot != 0) { return ret; }

        ZETA_Allocator_Deallocate(mlt->node_allocator, node);
    }

    mlt->root = NULL;

    return ret;
}

static void EraseAll_(int level, unsigned short const* branch_nums,
                      Zeta_MultiLevelTable_Node* node,
                      Zeta_Allocator* node_allocator) {
    if (level == 0) { ZETA_Allocator_Deallocate(node_allocator, node); }

    size_t branch_num = branch_nums[level];

    for (size_t idx = 0; idx < branch_num; ++idx) {
        if (TestHot_(node->hot, idx)) {
            EraseAll_(level - 1, branch_nums, node->ptrs[idx], node_allocator);
        }
    }

    ZETA_Allocator_Deallocate(node_allocator, node);
}

void Zeta_MultiLevelTable_EraseAll(void* mlt_) {
    Zeta_MultiLevelTable* mlt = mlt_;
    CheckCntr_(mlt);

    int level = mlt->level;

    Zeta_MultiLevelTable_Node* n = mlt->root;
    if (n == NULL) { return; }

    EraseAll_(level - 1, mlt->branch_nums, n, mlt->node_allocator);

    mlt->size = 0;
    mlt->root = NULL;
}

void Zeta_MultiLevelTable_Check(void* mlt_) {
    Zeta_MultiLevelTable* mlt = mlt_;
    ZETA_DebugAssert(mlt != NULL);

    int level = mlt->level;
    ZETA_DebugAssert(0 < level);
    ZETA_DebugAssert(level <= ZETA_MultiLevelTable_max_level);

    unsigned short const* branch_nums = mlt->branch_nums;

    for (int level_i = 0; level_i < level; ++level_i) {
        size_t branch_num = branch_nums[level_i];

        ZETA_DebugAssert(ZETA_MultiLevelTable_min_branch_num <= branch_num);
        ZETA_DebugAssert(branch_num <= ZETA_MultiLevelTable_max_branch_num);
    }
}

void Zeta_MultiLevelTable_CheckIdxes(void* mlt_, size_t const* idxes) {
    Zeta_MultiLevelTable* mlt = mlt_;
    ZETA_DebugAssert(mlt != NULL);

    ZETA_DebugAssert(idxes != NULL);

    int level = mlt->level;
    unsigned short const* branch_nums = mlt->branch_nums;

    for (int level_i = 0; level_i < level; ++level_i) {
        ZETA_DebugAssert(idxes[level_i] < branch_nums[level_i]);
    }
}

static size_t Sanitize_(Zeta_MemRecorder* dst_node, int level,
                        unsigned short const* branch_nums,
                        Zeta_MultiLevelTable_Node* node) {
    ZETA_DebugAssert(node->hot != 0);

    size_t branch_num = branch_nums[level];

    if (dst_node != NULL) {
        Zeta_MemRecorder_Record(
            dst_node, node,
            offsetof(Zeta_MultiLevelTable_Node, ptrs[branch_num]));
    }

    if (level == 1) {
        size_t size = 0;

        for (size_t idx = 0; idx < branch_num; ++idx) {
            if (TestHot_(node->hot, idx)) { ++size; }
        }

        return size;
    }

    size_t size = 0;

    for (size_t idx = 0; idx < branch_num;) {
        idx = FindNextHot_(node->hot, idx);

        if (idx == (size_t)(-1)) { break; }

        ZETA_DebugAssert(TestHot_(node->hot, idx));

        size += Sanitize_(dst_node, level - 1, branch_nums, node->ptrs[idx]);

        ++idx;
    }

    return size;
}

void Zeta_MultiLevelTable_Sanitize(void* mlt_, Zeta_MemRecorder* dst_node) {
    Zeta_MultiLevelTable* mlt = mlt_;
    CheckCntr_(mlt);

    if (mlt->root == NULL) {
        ZETA_DebugAssert(mlt->size == 0);
    } else {
        size_t size =
            Sanitize_(dst_node, mlt->level - 1, mlt->branch_nums, mlt->root);
        ZETA_DebugAssert(mlt->size == size);
    }
}
