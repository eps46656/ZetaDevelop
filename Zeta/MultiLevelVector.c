#include "MultiLevelVector.h"

#define mask_width (32)

static void CheckIdxes_(void* mlv_, size_t const* idxes) {
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

static int FindLSB_(Zeta_MultiLevelVector_mask_t x) {
    if (x == 0) { return -1; }

    const int table[] = { 0, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0 };

    int ret = 0;

    for (; x % 256 == 0; x /= 256) { ret += 8; }

    x %= 256;

    return ret + (x % 16 == 0 ? (4 + table[x / 16]) : table[x % 16]);
}

static int FindMSB_(Zeta_MultiLevelVector_mask_t x) {
    if (x == 0) { return -1; }

    const int table[] = { 0, 0, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3 };

    int ret = 0;

    for (; 256 <= x; x /= 256) { ret += 8; }

    return ret + (x < 16 ? table[x % 16] : (4 + table[x / 16]));
}

static bool_t Test_(Zeta_MultiLevelVector_Node* node, size_t idx) {
    int i = idx / mask_width;
    int j = idx % mask_width;

    return (node->mask[i] & ((Zeta_MultiLevelVector_mask_t)1 << j)) != 0;
}

static int FindPrev_(Zeta_MultiLevelVector_Node* node, size_t branch_num,
                     size_t idx) {
    ZETA_Unused(branch_num);

    if (idx == (size_t)(-1)) { return -1; }

    int i = idx / mask_width;
    int j = idx % mask_width;

    Zeta_MultiLevelVector_mask_t* mask = node->mask + i;

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

static int FindNext_(Zeta_MultiLevelVector_Node* node, size_t branch_num,
                     size_t idx) {
    if (idx == branch_num) { return -1; }

    int i = idx / mask_width;
    int j = idx % mask_width;

    Zeta_MultiLevelVector_mask_t* mask = node->mask + i;

    if (0 < j) {
        int k = FindLSB_(*mask >> j << j);
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

static void SetMask_(Zeta_MultiLevelVector_Node* node, size_t idx) {
    int i = idx / mask_width;
    int j = idx % mask_width;

    node->mask[i] |= (Zeta_MultiLevelVector_mask_t)1 << j;
}

static void UnsetMask_(Zeta_MultiLevelVector_Node* node, size_t idx) {
    int i = idx / mask_width;
    int j = idx % mask_width;

    node->mask[i] &= ~((Zeta_MultiLevelVector_mask_t)1 << j);
}

static bool_t IsEmpty_(Zeta_MultiLevelVector_Node* node, size_t branch_num) {
    int k = (branch_num + mask_width - 1) / mask_width;

    for (int i = 0; i < k; ++i) {
        if (node->mask[i] != 0) { return FALSE; }
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
        ZETA_DebugAssert(branch_num <= ZETA_MultiLevelVector_max_branch_num);
    }

    mlv->size = 0;

    mlv->root = NULL;

    ZETA_DebugAssert(mlv->node_allocator != NULL);
    ZETA_DebugAssert(mlv->node_allocator->GetAlign != NULL);
    ZETA_DebugAssert(
        mlv->node_allocator->GetAlign(mlv->node_allocator->context) %
            alignof(Zeta_MultiLevelVector_mask_t) ==
        0);

    ZETA_DebugAssert(mlv->table_allocator != NULL);
    ZETA_DebugAssert(mlv->table_allocator->GetAlign != NULL);
    ZETA_DebugAssert(
        mlv->table_allocator->GetAlign(mlv->table_allocator->context) %
            alignof(void*) ==
        0);
}

size_t Zeta_MultiLevelVector_GetSize(void* mlv_) {
    Zeta_MultiLevelVector* mlv = mlv_;
    ZETA_DebugAssert(mlv != NULL);

    return mlv->size;
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

void** Zeta_MultiLevelVector_Access(void* mlv_, size_t const* idxes) {
    if (ZETA_IsDebug) { CheckIdxes_(mlv_, idxes); }

    Zeta_MultiLevelVector* mlv = mlv_;
    int level = mlv->level;

    if (mlv->root == NULL) { return NULL; }

    void** node = &mlv->root;

    for (int level_i = 0; level_i < level; ++level_i) {
        if (!Test_(*node, idxes[level_i])) { return NULL; }
        node = ((Zeta_MultiLevelVector_Node*)(*node))->table + idxes[level_i];
    }

    return node;
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
    size_t* branch_nums = mlv->branch_nums;

    for (int level_i = 0; level_i < level; ++level_i) {
        idxes[level_i] = branch_nums[level_i] - 1;
    }

    return Zeta_MultiLevelVector_FindPrevNotNull(mlv, idxes, TRUE);
}

void** Zeta_MultiLevelVector_FindPrevNotNull(void* mlv_, size_t* idxes,
                                             bool_t included) {
    if (ZETA_IsDebug) { CheckIdxes_(mlv_, idxes); }

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
    if (root == NULL) { goto FAIL_RET; }

    void* nodes[ZETA_MultiLevelVector_max_level];

    int level_i = 0;
    void* node = root;

    for (;; ++level_i) {
        nodes[level_i] = node;
        if (!Test_(node, idxes[level_i])) { break; }

        void** nxt_node =
            ((Zeta_MultiLevelVector_Node*)node)->table + idxes[level_i];

        if (level_i == level - 1) { return nxt_node; }

        node = *nxt_node;
    }

    for (; 0 <= level_i; --level_i) {
        node = nodes[level_i];

        int found_idx =
            FindPrev_(node, branch_nums[level_i], idxes[level_i] - 1);

        if (0 <= found_idx) {
            idxes[level_i] = found_idx;
            break;
        }
    }

    if (level_i == -1) { goto FAIL_RET; }

    while (level_i < level - 1) {
        node = ((Zeta_MultiLevelVector_Node*)node)->table[idxes[level_i]];
        ++level_i;
        nodes[level_i] = node;
        idxes[level_i] =
            FindPrev_(node, branch_nums[level_i], branch_nums[level_i] - 1);
    }

    return ((Zeta_MultiLevelVector_Node*)nodes[level - 1])->table +
           idxes[level - 1];

FAIL_RET:;

    for (int level_i = 0; level_i < level; ++level_i) {
        idxes[level_i] = branch_nums[level_i] - 1;
    }

    return NULL;
}

void** Zeta_MultiLevelVector_FindNextNotNull(void* mlv_, size_t* idxes,
                                             bool_t included) {
    if (ZETA_IsDebug) { CheckIdxes_(mlv_, idxes); }

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
    if (root == NULL) { goto FAIL_RET; }

    void* nodes[ZETA_MultiLevelVector_max_level];

    int level_i = 0;
    void* node = root;

    for (;; ++level_i) {
        nodes[level_i] = node;
        if (!Test_(node, idxes[level_i])) { break; }

        void** nxt_node =
            ((Zeta_MultiLevelVector_Node*)node)->table + idxes[level_i];

        if (level_i == level - 1) { return nxt_node; }

        node = *nxt_node;
    }

    for (; 0 <= level_i; --level_i) {
        node = nodes[level_i];

        int found_idx =
            FindNext_(node, branch_nums[level_i], idxes[level_i] + 1);

        if (0 <= found_idx) {
            idxes[level_i] = found_idx;
            break;
        }
    }

    if (level_i == -1) { goto FAIL_RET; }

    while (level_i < level - 1) {
        node = ((Zeta_MultiLevelVector_Node*)node)->table[idxes[level_i]];
        ++level_i;
        nodes[level_i] = node;
        idxes[level_i] = FindNext_(node, branch_nums[level_i], 0);
    }

    return ((Zeta_MultiLevelVector_Node*)nodes[level - 1])->table +
           idxes[level - 1];

FAIL_RET:;

    for (int level_i = 0; level_i < level; ++level_i) { idxes[level_i] = 0; }

    return NULL;
}

static Zeta_MultiLevelVector_Node* AllocatePage_(
    size_t branch_num, void* node_allocator_context,
    void* (*AllocateNode)(void* context, size_t size),
    void* table_allocator_context,
    void* (*AllocateTable)(void* context, size_t size)) {
    size_t masks_num = (branch_num + mask_width - 1) / mask_width;

    Zeta_MultiLevelVector_Node* node =
        AllocateNode(node_allocator_context,
                     offsetof(Zeta_MultiLevelVector_Node, mask) +
                         sizeof(Zeta_MultiLevelVector_mask_t) * masks_num);
    ZETA_DebugAssert(node != NULL);

    ((Zeta_MultiLevelVector_Node*)node)->table =
        AllocateTable(table_allocator_context, sizeof(void*) * branch_num);
    ZETA_DebugAssert(((Zeta_MultiLevelVector_Node*)node)->table != NULL);

    for (size_t i = 0; i < masks_num; ++i) { node->mask[i] = 0; }

    return node;
}

void** Zeta_MultiLevelVector_Insert(void* mlv_, size_t* idxes) {
    if (ZETA_IsDebug) { CheckIdxes_(mlv_, idxes); }

    Zeta_MultiLevelVector* mlv = mlv_;
    int level = mlv->level;
    size_t* branch_nums = mlv->branch_nums;

    void* node_allocator_context = mlv->node_allocator->context;
    void* table_allocator_context = mlv->table_allocator->context;

    void* (*AllocateNode)(void* context, size_t size) =
        mlv->node_allocator->Allocate;
    ZETA_DebugAssert(AllocateNode != NULL);

    void* (*AllocateTable)(void* context, size_t size) =
        mlv->table_allocator->Allocate;
    ZETA_DebugAssert(AllocateTable != NULL);

    if (mlv->root == NULL) {
        mlv->root =
            AllocatePage_(branch_nums[0], node_allocator_context, AllocateNode,
                          table_allocator_context, AllocateTable);
    }

    Zeta_MultiLevelVector_Node* node = mlv->root;

    for (int level_i = 0; level_i < level - 1; ++level_i) {
        if (Test_(node, idxes[level_i])) {
            node = ((Zeta_MultiLevelVector_Node*)node)->table[idxes[level_i]];
            continue;
        }

        SetMask_(node, idxes[level_i]);

        node = ((Zeta_MultiLevelVector_Node*)node)->table[idxes[level_i]] =
            AllocatePage_(branch_nums[level_i + 1], node_allocator_context,
                          AllocateNode, table_allocator_context, AllocateTable);
    }

    void** ret = ((Zeta_MultiLevelVector_Node*)node)->table + idxes[level - 1];

    if (Test_(node, idxes[level - 1])) { return ret; }

    ++mlv->size;

    SetMask_(node, idxes[level - 1]);
    *ret = NULL;

    return ret;
}

void Zeta_MultiLevelVector_Erase(void* mlv_, size_t* idxes) {
    if (ZETA_IsDebug) { CheckIdxes_(mlv_, idxes); }

    Zeta_MultiLevelVector* mlv = mlv_;
    int level = mlv->level;
    size_t* branch_nums = mlv->branch_nums;

    Zeta_Allocator* node_allocator = mlv->node_allocator;
    ZETA_DebugAssert(node_allocator != NULL);

    Zeta_Allocator* table_allocator = mlv->table_allocator;
    ZETA_DebugAssert(table_allocator != NULL);

    void* node_allocator_context = node_allocator->context;
    void* table_allocator_context = table_allocator->context;

    void (*DeallocateNode)(void* context, void* ptr) =
        node_allocator->Deallocate;
    ZETA_DebugAssert(DeallocateNode != NULL);

    void (*DeallocateTable)(void* context, void* ptr) =
        table_allocator->Deallocate;
    ZETA_DebugAssert(DeallocateTable != NULL);

    void* node = mlv->root;
    if (node == NULL) { return; }

    void* nodes[ZETA_MultiLevelVector_max_level];

    for (int level_i = 0; level_i < level; ++level_i) {
        nodes[level_i] = node;
        if (!Test_(node, idxes[level_i])) { return; }
        node = ((Zeta_MultiLevelVector_Node*)node)->table[idxes[level_i]];
    }

    --mlv->size;

    for (int level_i = level - 1; 0 <= level_i; --level_i) {
        void* node = nodes[level_i];

        UnsetMask_(node, idxes[level_i]);

        if (!IsEmpty_(node, branch_nums[level_i])) { return; }

        DeallocateTable(table_allocator_context,
                        ((Zeta_MultiLevelVector_Node*)node)->table);
        DeallocateNode(node_allocator_context, node);
    }

    mlv->root = NULL;
}

static void EraseAll_(int level, size_t const* branch_nums, void* node,
                      void* node_allocator_context,
                      void* table_allocator_context,
                      void (*DeallocateNode)(void* context, void* ptr),
                      void (*DeallocateTable)(void* context, void* ptr)) {
    if (1 < level) {
        size_t branch_num = branch_nums[0];

        for (size_t idx = 0; idx < branch_num; ++idx) {
            if (Test_(node, idx)) {
                EraseAll_(level - 1, branch_nums + 1,
                          ((Zeta_MultiLevelVector_Node*)node)->table[idx],
                          node_allocator_context, table_allocator_context,
                          DeallocateNode, DeallocateTable);
            }
        }
    }

    DeallocateTable(table_allocator_context,
                    ((Zeta_MultiLevelVector_Node*)node)->table);
    DeallocateNode(node_allocator_context, node);
}

void Zeta_MultiLevelVector_EraseAll(void* mlv_) {
    Zeta_MultiLevelVector* mlv = mlv_;
    ZETA_DebugAssert(mlv != NULL);

    int level = mlv->level;

    void* n = mlv->root;
    if (n == NULL) { return; }

    Zeta_Allocator* node_allocator = mlv->node_allocator;
    ZETA_DebugAssert(node_allocator != NULL);

    Zeta_Allocator* table_allocator = mlv->table_allocator;
    ZETA_DebugAssert(table_allocator != NULL);

    void (*DeallocateNode)(void* context, void* ptr) =
        node_allocator->Deallocate;
    ZETA_DebugAssert(DeallocateNode != NULL);

    void (*DeallocateTable)(void* context, void* ptr) =
        table_allocator->Deallocate;
    ZETA_DebugAssert(DeallocateTable != NULL);

    EraseAll_(level, mlv->branch_nums, n, node_allocator->context,
              table_allocator->context, DeallocateNode, DeallocateTable);

    mlv->size = 0;
    mlv->root = NULL;
}

static void GetAllPages_(Zeta_DebugTreeMap* dst_node,
                         Zeta_DebugTreeMap* dst_table, int level,
                         size_t const* branch_nums, void* node) {
    size_t branch_num = branch_nums[0];
    size_t masks_num = (branch_num + mask_width - 1) / mask_width;

    *Zeta_DebugTreeMap_Insert(dst_node, ZETA_GetAddrFromPtr(node)).val =
        ZETA_GetAddrFromPtr(
            &((Zeta_MultiLevelVector_Node*)(NULL))->mask[masks_num]) -
        ZETA_GetAddrFromPtr(NULL);
    *Zeta_DebugTreeMap_Insert(
         dst_table,
         ZETA_GetAddrFromPtr(((Zeta_MultiLevelVector_Node*)node)->table))
         .val = sizeof(void*) * branch_num;

    if (level == 1) { return; }

    size_t cnt = 0;

    for (size_t idx = 0; idx < branch_num; ++idx) {
        if (Test_(node, idx)) {
            ++cnt;
            GetAllPages_(dst_node, dst_table, level - 1, branch_nums + 1,
                         ((Zeta_MultiLevelVector_Node*)node)->table[idx]);
        }
    }

    ZETA_DebugAssert(0 < cnt);
}

void Zeta_MultiLevelVector_GetAllPages(void* mlv_, Zeta_DebugTreeMap* dst_node,
                                       Zeta_DebugTreeMap* dst_table) {
    Zeta_MultiLevelVector* mlv = mlv_;
    ZETA_DebugAssert(mlv != NULL);

    ZETA_DebugAssert(dst_node != NULL);
    ZETA_DebugAssert(dst_table != NULL);

    if (mlv->root != NULL) {
        GetAllPages_(dst_node, dst_table, mlv->level, mlv->branch_nums,
                     mlv->root);
    }
}
