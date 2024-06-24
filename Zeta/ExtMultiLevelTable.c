#include "ExtMultiLevelTable.h"

typedef u64_t uN_t;

ZETA_StaticAssert(ZETA_ExtMultiLevelTable_max_branch_num <= ZETA_WidthOf(uN_t));

ZETA_StaticAssert(alignof(unsigned long long) % alignof(void*) == 0);

static void CheckIdxes_(void* mlt_, size_t const* idxes) {
    Zeta_ExtMultiLevelTable* mlt = mlt_;
    ZETA_DebugAssert(mlt != NULL);

    ZETA_DebugAssert(idxes != NULL);

    int level = mlt->level;
    ZETA_DebugAssert(0 < level);
    ZETA_DebugAssert(level <= ZETA_ExtMultiLevelTable_max_level);

    for (int level_i = 0; level_i < level; ++level_i) {
        size_t branch_num = mlt->branch_nums[level_i];
        size_t idx = idxes[level_i];

        ZETA_DebugAssert(0 < branch_num);
        ZETA_DebugAssert(branch_num <= ZETA_ExtMultiLevelTable_max_branch_num);

        ZETA_DebugAssert(idx < branch_num);
    }
}

#define TestHot_(hot, idx)                                  \
    (((uN_t)(hot[idx / ZETA_WidthOf(unsigned long long)]) & \
      ((uN_t)1 << (idx % ZETA_WidthOf(unsigned long long)))) != 0)

static int FindPrev_(unsigned long long hot, size_t branch_num, size_t idx) {
    if (idx == (size_t)(-1)) { return -1; }

    if (idx < branch_num - 1) {
        hot = (uN_t)hot & (((uN_t)2 << idx) - (uN_t)1);
        if (hot == 0) { return -1; }
    }

    return ZETA_WidthOf(unsigned long long) - 1 - __builtin_clzll(hot);
}

static int FindNext_(unsigned long long hot, size_t branch_num, size_t idx) {
    if (idx == branch_num) { return -1; }

    if (0 < idx) {
        hot = (uN_t)hot >> idx << idx;
        if (hot == 0) { return -1; }
    }

    return __builtin_ctzll(hot);
}

static size_t GetNodeSize_(size_t branch_num) {
    return sizeof(unsigned long long) *
               ((branch_num + ZETA_WidthOf(unsigned long long) - 1) /
                ZETA_WidthOf(unsigned long long)) +
           sizeof(void*) * branch_num;
}

static void** GetPtrs_(void* node, size_t branch_num) {
    return (void**)((unsigned long long*)node +
                    (branch_num + ZETA_WidthOf(unsigned long long) - 1) /
                        ZETA_WidthOf(unsigned long long));
}

void Zeta_ExtMultiLevelTable_Init(void* mlt_) {
    Zeta_ExtMultiLevelTable* mlt = mlt_;
    ZETA_DebugAssert(mlt != NULL);

    int level = mlt->level;
    ZETA_DebugAssert(0 < level);
    ZETA_DebugAssert(level <= ZETA_ExtMultiLevelTable_max_level);

    for (int level_i = 0; level_i < level; ++level_i) {
        size_t branch_num = mlt->branch_nums[level_i];

        ZETA_DebugAssert(0 < branch_num);
        ZETA_DebugAssert(branch_num <= ZETA_ExtMultiLevelTable_max_branch_num);
    }

    mlt->size = 0;

    mlt->root = NULL;

    ZETA_DebugAssert(mlt->node_allocator != NULL);
    ZETA_DebugAssert(mlt->node_allocator->GetAlign != NULL);
    ZETA_DebugAssert(
        mlt->node_allocator->GetAlign(mlt->node_allocator->context) %
            alignof(Zeta_ExtMultiLevelTable_Node) ==
        0);
}

void Zeta_ExtMultiLevelTable_Deinit(void* mlt) {
    Zeta_ExtMultiLevelTable_EraseAll(mlt);
}

size_t Zeta_ExtMultiLevelTable_GetSize(void* mlt_) {
    Zeta_ExtMultiLevelTable* mlt = mlt_;
    ZETA_DebugAssert(mlt != NULL);

    return mlt->size;
}

size_t Zeta_ExtMultiLevelTable_GetCapacity(void* mlt_) {
    Zeta_ExtMultiLevelTable* mlt = mlt_;
    ZETA_DebugAssert(mlt != NULL);

    size_t ret = 1;

    int level = mlt->level;
    ZETA_DebugAssert(0 < level);
    ZETA_DebugAssert(level <= ZETA_ExtMultiLevelTable_max_level);

    for (int level_i = 0; level_i < level; ++level_i) {
        size_t branch_num = mlt->branch_nums[level_i];

        ZETA_DebugAssert(ret <= ZETA_RangeMaxOf(size_t) / branch_num);

        ret *= branch_num;
    }

    return ret;
}

void** Zeta_ExtMultiLevelTable_Access(void* mlt_, size_t const* idxes) {
    if (ZETA_IsDebug) { CheckIdxes_(mlt_, idxes); }

    Zeta_ExtMultiLevelTable* mlt = mlt_;
    int level = mlt->level;

    if (mlt->root == NULL) { return NULL; }

    Zeta_ExtMultiLevelTable_Node* node = mlt->root;

    for (int level_i = 0; level_i < level - 1; ++level_i) {
        if (!TestHot_(node->hot, idxes[level_i])) { return NULL; }
        node = node->ptrs[idxes[level_i]];
    }

    return TestHot_(node->hot, idxes[level - 1]) ? node->ptrs + idxes[level - 1]
                                                 : NULL;
}

void** Zeta_ExtMultiLevelTable_FindFirst(void* mlt_, size_t* idxes) {
    Zeta_ExtMultiLevelTable* mlt = mlt_;
    ZETA_DebugAssert(mlt != NULL);

    int level = mlt->level;

    for (int level_i = 0; level_i < level; ++level_i) { idxes[level_i] = 0; }

    return Zeta_ExtMultiLevelTable_FindNext(mlt, idxes, TRUE);
}

void** Zeta_ExtMultiLevelTable_FindLast(void* mlt_, size_t* idxes) {
    Zeta_ExtMultiLevelTable* mlt = mlt_;
    ZETA_DebugAssert(mlt != NULL);

    int level = mlt->level;
    size_t* branch_nums = mlt->branch_nums;

    for (int level_i = 0; level_i < level; ++level_i) {
        idxes[level_i] = branch_nums[level_i] - 1;
    }

    return Zeta_ExtMultiLevelTable_FindPrev(mlt, idxes, TRUE);
}

void** Zeta_ExtMultiLevelTable_FindPrev(void* mlt_, size_t* idxes,
                                        bool_t included) {
    if (ZETA_IsDebug) { CheckIdxes_(mlt_, idxes); }

    Zeta_ExtMultiLevelTable* mlt = mlt_;
    int level = mlt->level;
    size_t* branch_nums = mlt->branch_nums;

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

    Zeta_ExtMultiLevelTable_Node* root = mlt->root;

    if (root == NULL) {
        for (int level_i = 0; level_i < level; ++level_i) {
            idxes[level_i] = branch_nums[level_i] - 1;
        }

        return NULL;
    }

    Zeta_ExtMultiLevelTable_Node* nodes[ZETA_ExtMultiLevelTable_max_level];

    int level_i = 0;
    Zeta_ExtMultiLevelTable_Node* node = root;

    for (;; ++level_i) {
        nodes[level_i] = node;
        if (!TestHot_(node->hot, idxes[level_i])) { break; }

        void** nxt_node = node->ptrs + idxes[level_i];
        if (level_i == level - 1) { return nxt_node; }

        node = *nxt_node;
    }

    for (; 0 <= level_i; --level_i) {
        node = nodes[level_i];

        int found_idx =
            FindPrev_(node->hot, branch_nums[level_i], idxes[level_i] - 1);

        if (0 <= found_idx) {
            idxes[level_i] = found_idx;
            break;
        }
    }

    if (level_i == -1) {
        for (int level_i = 0; level_i < level; ++level_i) {
            idxes[level_i] = branch_nums[level_i] - 1;
        }

        return NULL;
    }

    while (level_i < level - 1) {
        node = node->ptrs[idxes[level_i]];
        ++level_i;
        nodes[level_i] = node;
        idxes[level_i] = FindPrev_(node->hot, branch_nums[level_i],
                                   branch_nums[level_i] - 1);
    }

    return (nodes[level - 1])->ptrs + idxes[level - 1];
}

void** Zeta_ExtMultiLevelTable_FindNext(void* mlt_, size_t* idxes,
                                        bool_t included) {
    if (ZETA_IsDebug) { CheckIdxes_(mlt_, idxes); }

    Zeta_ExtMultiLevelTable* mlt = mlt_;
    int level = mlt->level;
    size_t* branch_nums = mlt->branch_nums;

    if (!included) {
        for (int level_i = level - 1; 0 <= level_i; --level_i) {
            ++idxes[level_i];
            if (idxes[level_i] < branch_nums[level_i]) { goto L1; }
            idxes[level_i] = 0;
        }

        return NULL;
    }

L1:;

    Zeta_ExtMultiLevelTable_Node* root = mlt->root;

    if (root == NULL) {
        for (int level_i = 0; level_i < level; ++level_i) {
            idxes[level_i] = 0;
        }

        return NULL;
    }

    Zeta_ExtMultiLevelTable_Node* nodes[ZETA_ExtMultiLevelTable_max_level];

    int level_i = 0;
    Zeta_ExtMultiLevelTable_Node* node = root;

    for (;; ++level_i) {
        nodes[level_i] = node;
        if (!TestHot_(node->hot, idxes[level_i])) { break; }

        void** nxt_node = node->ptrs + idxes[level_i];
        if (level_i == level - 1) { return nxt_node; }

        node = *nxt_node;
    }

    for (; 0 <= level_i; --level_i) {
        node = nodes[level_i];

        int found_idx =
            FindNext_(node->hot, branch_nums[level_i], idxes[level_i] + 1);

        if (0 <= found_idx) {
            idxes[level_i] = found_idx;
            break;
        }
    }

    if (level_i == -1) {
        for (int level_i = 0; level_i < level; ++level_i) {
            idxes[level_i] = 0;
        }

        return NULL;
    }

    while (level_i < level - 1) {
        node = node->ptrs[idxes[level_i]];
        ++level_i;
        nodes[level_i] = node;
        idxes[level_i] = FindNext_(node->hot, branch_nums[level_i], 0);
    }

    return nodes[level - 1]->ptrs + idxes[level - 1];
}

static Zeta_ExtMultiLevelTable_Node* AllocateNode_(
    size_t branch_num, void* node_allocator_context,
    void* (*AllocateNode)(void* context, size_t size)) {
    void* node_ =
        AllocateNode(node_allocator_context,
                     offsetof(Zeta_ExtMultiLevelTable_Node, ptrs[branch_num]));

    ZETA_DebugAssert(node_ != NULL);
    ZETA_DebugAssert(
        ZETA_PtrToAddr(node_) % alignof(Zeta_ExtMultiLevelTable_Node) == 0);

    Zeta_ExtMultiLevelTable_Node* node = node_;
    node->hot = 0;

    return node;
}

void** Zeta_ExtMultiLevelTable_Insert(void* mlt_, size_t* idxes) {
    if (ZETA_IsDebug) { CheckIdxes_(mlt_, idxes); }

    Zeta_ExtMultiLevelTable* mlt = mlt_;
    int level = mlt->level;
    size_t* branch_nums = mlt->branch_nums;

    void* node_allocator_context = mlt->node_allocator->context;

    void* (*AllocateNode)(void* context, size_t size) =
        mlt->node_allocator->Allocate;
    ZETA_DebugAssert(AllocateNode != NULL);

    if (mlt->root == NULL) {
        mlt->root =
            AllocateNode_(branch_nums[0], node_allocator_context, AllocateNode);
    }

    Zeta_ExtMultiLevelTable_Node* node = mlt->root;

    /*
    for (int level_i = 0; level_i < level - 1; ++level_i) {
        if (TestHot_(node->hot, idxes[level_i])) {
            node = node->ptrs[idxes[level_i]];
            continue;
        }

        node->hot = (uN_t)node->hot | ((uN_t)(1) << idxes[level_i]);

        node = node->ptrs[idxes[level_i]] = AllocateNode_(
            branch_nums[level_i + 1], node_allocator_context, AllocateNode);
    }
    */

    int level_i = 0;

    for (; level_i < level - 1 && TestHot_(node->hot, idxes[level_i]);
         ++level_i) {
        node = node->ptrs[idxes[level_i]];
    }

    for (; level_i < level - 1; ++level_i) {
        node->hot = (uN_t)node->hot | ((uN_t)(1) << idxes[level_i]);

        node = node->ptrs[idxes[level_i]] = AllocateNode_(
            branch_nums[level_i + 1], node_allocator_context, AllocateNode);
    }

    void** ret = node->ptrs + idxes[level - 1];

    if (TestHot_(node->hot, idxes[level - 1])) { return ret; }

    ++mlt->size;

    node->hot = (uN_t)node->hot | ((uN_t)(1) << idxes[level - 1]);
    *ret = NULL;

    return ret;
}

void Zeta_ExtMultiLevelTable_Erase(void* mlt_, size_t* idxes) {
    if (ZETA_IsDebug) { CheckIdxes_(mlt_, idxes); }

    Zeta_ExtMultiLevelTable* mlt = mlt_;
    int level = mlt->level;

    Zeta_Allocator* node_allocator = mlt->node_allocator;
    ZETA_DebugAssert(node_allocator != NULL);

    void* node_allocator_context = node_allocator->context;

    void (*DeallocateNode)(void* context, void* ptr) =
        node_allocator->Deallocate;
    ZETA_DebugAssert(DeallocateNode != NULL);

    Zeta_ExtMultiLevelTable_Node* node = mlt->root;
    if (node == NULL) { return; }

    Zeta_ExtMultiLevelTable_Node* nodes[ZETA_ExtMultiLevelTable_max_level];

    for (int level_i = 0; level_i < level; ++level_i) {
        nodes[level_i] = node;
        if (!TestHot_(node->hot, idxes[level_i])) { return; }
        node = node->ptrs[idxes[level_i]];
    }

    --mlt->size;

    for (int level_i = level - 1; 0 <= level_i; --level_i) {
        Zeta_ExtMultiLevelTable_Node* node = nodes[level_i];

        node->hot = (uN_t)node->hot ^ ((uN_t)(1) << idxes[level_i]);

        if (node->hot != 0) { return; }

        DeallocateNode(node_allocator_context, node);
    }

    mlt->root = NULL;
}

static void EraseAll_(int level, size_t const* branch_nums,
                      Zeta_ExtMultiLevelTable_Node* node,
                      void* node_allocator_context,
                      void (*DeallocateNode)(void* context, void* ptr)) {
    if (1 < level) {
        size_t branch_num = branch_nums[0];

        for (size_t idx = 0; idx < branch_num; ++idx) {
            if (TestHot_(node->hot, idx)) {
                EraseAll_(level - 1, branch_nums + 1, node->ptrs[idx],
                          node_allocator_context, DeallocateNode);
            }
        }
    }

    DeallocateNode(node_allocator_context, node);
}

void Zeta_ExtMultiLevelTable_EraseAll(void* mlt_) {
    Zeta_ExtMultiLevelTable* mlt = mlt_;
    ZETA_DebugAssert(mlt != NULL);

    int level = mlt->level;

    Zeta_ExtMultiLevelTable_Node* n = mlt->root;
    if (n == NULL) { return; }

    Zeta_Allocator* node_allocator = mlt->node_allocator;
    ZETA_DebugAssert(node_allocator != NULL);

    void (*DeallocateNode)(void* context, void* ptr) =
        node_allocator->Deallocate;
    ZETA_DebugAssert(DeallocateNode != NULL);

    EraseAll_(level, mlt->branch_nums, n, node_allocator->context,
              DeallocateNode);

    mlt->size = 0;
    mlt->root = NULL;
}

static size_t Check_(Zeta_DebugHashMap* dst_node, int level,
                     size_t const* branch_nums,
                     Zeta_ExtMultiLevelTable_Node* node) {
    ZETA_DebugAssert(node->hot != 0);

    size_t branch_num = branch_nums[0];

    *Zeta_DebugHashMap_Insert(dst_node, ZETA_PtrToAddr(node)).val =
        offsetof(Zeta_ExtMultiLevelTable_Node, ptrs[branch_num]);

    if (level == 1) {
        size_t size = 0;

        for (size_t idx = 0; idx < branch_num; ++idx) {
            if (TestHot_(node->hot, idx)) { ++size; }
        }

        return size;
    }

    size_t size = 0;

    for (size_t idx = 0; idx < branch_num; ++idx) {
        if (TestHot_(node->hot, idx)) {
            size +=
                Check_(dst_node, level - 1, branch_nums + 1, node->ptrs[idx]);
        }
    }

    return size;
}

void Zeta_ExtMultiLevelTable_Check(void* mlt_, Zeta_DebugHashMap* dst_node) {
    Zeta_ExtMultiLevelTable* mlt = mlt_;
    ZETA_DebugAssert(mlt != NULL);

    ZETA_DebugAssert(dst_node != NULL);

    if (mlt->root == NULL) {
        ZETA_DebugAssert(mlt->size == 0);
    } else {
        size_t size = Check_(dst_node, mlt->level, mlt->branch_nums, mlt->root);
        ZETA_DebugAssert(mlt->size == size);
    }
}
