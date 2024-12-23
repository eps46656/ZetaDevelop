#include "generic_hash_table.h"

#include "bin_tree.h"
#include "debugger.h"
#include "mem_check_utils.h"
#include "ptr_utils.h"
#include "rbtree.h"
#include "utils.h"

#if ZETA_EnableDebug

#define CheckCntr_(ght) Zeta_GenericHashTable_Check((ght))

#define CheckNode_(ght, node) Zeta_GenericHashTable_CheckNode((ght), (node))

#else

#define CheckCntr_(ght) ZETA_Unused((ght))

#define CheckNode_(ght, node) \
    {                         \
        ZETA_Unused((ght));   \
        ZETA_Unused((node));  \
    }                         \
    ZETA_StaticAssert(TRUE)

#endif

#define GetCurTable_(ght) \
    ((void**)ZETA_ColorPtr_GetPtr(&ght->cur_table, alignof(void*)))

#define GetNxtTable_(ght) ght->nxt_table

#define GetState_(ght) ZETA_ColorPtr_GetColor(&ght->cur_table, alignof(void*))

#define null_state 0
#define cur_state 1
#define trans_state 2
#define nxt_state 3

static unsigned long long GetSalt_(Zeta_GenericHashTable* ght) {
    unsigned long long ret = ght->cur_salt;
    return Zeta_SimpleRandomRotate(&ret);
}

static size_t GetNxtTableCapacity_(size_t table_capacity) {
    return ZETA_GetMaxOf(23, table_capacity * 2 + 1);
}

static void** AllocateTable_(Zeta_GenericHashTable* ght, size_t capacity) {
    void** table = (void**)ZETA_Allocator_SafeAllocate(
                       ght->table_allocator, alignof(void*),
                       sizeof(void*) * (capacity + 1)) +
                   1;

    table[-1] = table;

    return table;
}

static size_t GetBucketIdx_(unsigned long long hash_code,
                            size_t bucket_capacity) {
    return Zeta_ULLHash(hash_code, 0) % bucket_capacity;
}

static void InsertNode_(Zeta_GenericHashTable* ght, void** root,
                        Zeta_GenericHashTable_Node* m) {
    Zeta_BinTreeNodeOperator const* btn_opr = &zeta_ord_rb_tree_node_opr;

    void* n = *root;

    void* le_n = NULL;
    void* gt_n = NULL;

    while (n != NULL) {
        int cmp = ght->NodeCompare(
            ght->node_cmp_context,
            ZETA_MemberToStruct(Zeta_GenericHashTable_Node, n, n), m);

        if (cmp <= 0) {
            le_n = n;
            n = Zeta_OrdRBTreeNode_GetR(NULL, n);
        } else {
            gt_n = n;
            n = Zeta_OrdRBTreeNode_GetL(NULL, n);
        }
    }

    *root = Zeta_RBTree_Insert(btn_opr, le_n, gt_n, &m->n);
}

void Zeta_GenericHashTable_Init(void* ght_) {
    Zeta_GenericHashTable* ght = ght_;
    ZETA_DebugAssert(ght != NULL);

    ZETA_ColorPtr_Set(&ght->cur_table, alignof(void*), &ght->cur_table,
                      null_state);
    ght->nxt_table = NULL;

    ght->cur_table_cnt = 0;
    ght->nxt_table_cnt = 0;

    ght->cur_table_capacity = 0;
    ght->nxt_table_capacity = 0;

    ZETA_DebugAssert(ght->table_allocator != NULL);
    ZETA_DebugAssert(ght->table_allocator->GetAlign != NULL);
    ZETA_DebugAssert(ght->table_allocator->Allocate != NULL);
    ZETA_DebugAssert(ght->table_allocator->Deallocate != NULL);
    ZETA_DebugAssert(
        ZETA_Allocator_GetAlign(ght->table_allocator) % alignof(void*) == 0);

    ZETA_DebugAssert(ght->NodeCompare != NULL);

    ZETA_DebugAssert(ght->NodeKeyCompare != NULL);
}

void Zeta_GenericHashTable_Deinit(void* ght_) {
    Zeta_GenericHashTable* ght = ght_;
    CheckCntr_(ght);

    void** cur_table = GetCurTable_(ght);
    void** nxt_table = GetNxtTable_(ght);

    if (cur_table != &ght->cur_table) {
        ZETA_Allocator_Deallocate(ght->table_allocator, cur_table - 1);
    }

    if (nxt_table != NULL) {
        ZETA_Allocator_Deallocate(ght->table_allocator, nxt_table - 1);
    }
}

#define TryDo_                                                              \
    if (state == trans_state) {                                             \
        size_t i = (void**)nxt_table[-1] - nxt_table;                       \
        size_t j = ZETA_GetMinOf(i + 32, nxt_table_capacity);               \
                                                                            \
        for (; i < j; ++i) { nxt_table[i] = NULL; }                         \
                                                                            \
        if (j < nxt_table_capacity) {                                       \
            nxt_table[-1] = nxt_table + j;                                  \
        } else {                                                            \
            state = nxt_state;                                              \
            cur_table[-1] = cur_table;                                      \
        }                                                                   \
    } else if (state == nxt_state) {                                        \
        void** p = cur_table[-1];                                           \
                                                                            \
        size_t op_cnt = 16 * 4;                                             \
                                                                            \
        while (0 < op_cnt) {                                                \
            if (*p == NULL) {                                               \
                --op_cnt;                                                   \
                if (++p == cur_table + cur_table_capacity) { break; }       \
                continue;                                                   \
            }                                                               \
                                                                            \
            Zeta_GenericHashTable_Node* trans_node =                        \
                ZETA_MemberToStruct(Zeta_GenericHashTable_Node, n, *p);     \
                                                                            \
            *p = Zeta_RBTree_Extract(btn_opr, &trans_node->n);              \
                                                                            \
            --cur_table_cnt;                                                \
                                                                            \
            InsertNode_(                                                    \
                ght,                                                        \
                nxt_table +                                                 \
                    GetBucketIdx_(ght->NodeHash(ght->node_hash_context,     \
                                                trans_node, ght->nxt_salt), \
                                  nxt_table_capacity),                      \
                trans_node);                                                \
                                                                            \
            ++nxt_table_cnt;                                                \
                                                                            \
            op_cnt = ZETA_GetMaxOf(15, op_cnt) - 15;                        \
        }                                                                   \
                                                                            \
        if (p < cur_table + cur_table_capacity) {                           \
            cur_table[-1] = p;                                              \
        } else {                                                            \
            ZETA_Allocator_Deallocate(ght->table_allocator, cur_table - 1); \
                                                                            \
            cur_table = nxt_table;                                          \
            cur_table_cnt = nxt_table_cnt;                                  \
            cur_table_capacity = nxt_table_capacity;                        \
            ght->cur_salt = ght->nxt_salt;                                  \
                                                                            \
            nxt_table = NULL;                                               \
            nxt_table_cnt = 0;                                              \
            nxt_table_capacity = 0;                                         \
                                                                            \
            state = cur_state;                                              \
        }                                                                   \
    }                                                                       \
    ZETA_StaticAssert(TRUE)

size_t Zeta_GenericHashTable_GetSize(void const* ght_) {
    Zeta_GenericHashTable const* ght = ght_;
    CheckCntr_(ght);

    return ght->cur_table_cnt + ght->nxt_table_cnt;
}

void* Zeta_GenericHashTable_Find(void* ght_, void const* key) {
    Zeta_GenericHashTable* ght = ght_;
    CheckCntr_(ght);

    int state = GetState_(ght);

    void** cur_table = GetCurTable_(ght);
    void** nxt_table = GetNxtTable_(ght);

    size_t cur_table_capacity = ght->cur_table_capacity;
    size_t nxt_table_capacity = ght->nxt_table_capacity;

    if (state == null_state) { return NULL; }

    size_t cur_bucket_idx =
        GetBucketIdx_(ght->KeyHash(ght->key_hash_context, key, ght->cur_salt),
                      cur_table_capacity);

    void* cur_target_n = NULL;

    for (void* n = cur_table[cur_bucket_idx]; n != NULL;) {
        int cmp = ght->NodeKeyCompare(
            ght->node_key_cmp_context,
            ZETA_MemberToStruct(Zeta_GenericHashTable_Node, n, n), key);

        if (cmp == 0) { cur_target_n = n; }

        if (cmp <= 0) {
            n = Zeta_OrdRBTreeNode_GetR(NULL, n);
        } else {
            n = Zeta_OrdRBTreeNode_GetL(NULL, n);
        }
    }

    if (cur_target_n != NULL || state != nxt_state) {
        return ZETA_MemberToStruct(Zeta_GenericHashTable_Node, n, cur_target_n);
    }

    size_t nxt_bucket_idx =
        GetBucketIdx_(ght->KeyHash(ght->key_hash_context, key, ght->nxt_salt),
                      nxt_table_capacity);

    void* nxt_target_n = NULL;

    for (void* n = nxt_table[nxt_bucket_idx]; n != NULL;) {
        int cmp = ght->NodeKeyCompare(
            ght->node_key_cmp_context,
            ZETA_MemberToStruct(Zeta_GenericHashTable_Node, n, n), key);

        if (cmp == 0) { nxt_target_n = n; }

        if (cmp <= 0) {
            n = Zeta_OrdRBTreeNode_GetR(NULL, n);
        } else {
            n = Zeta_OrdRBTreeNode_GetL(NULL, n);
        }
    }

    return nxt_target_n == NULL
               ? NULL
               : ZETA_MemberToStruct(Zeta_GenericHashTable_Node, n,
                                     nxt_target_n);
}

void Zeta_GenericHashTable_Insert(void* ght_, void* node_) {
    Zeta_GenericHashTable* ght = ght_;
    CheckCntr_(ght);

    Zeta_GenericHashTable_Node* node = node_;
    ZETA_DebugAssert(node != NULL);

    int state = GetState_(ght);

    void** cur_table = GetCurTable_(ght);
    void** nxt_table = GetNxtTable_(ght);

    size_t cur_table_cnt = ght->cur_table_cnt;
    size_t nxt_table_cnt = ght->nxt_table_cnt;

    size_t cur_table_capacity = ght->cur_table_capacity;
    size_t nxt_table_capacity = ght->nxt_table_capacity;

    Zeta_BinTreeNodeOperator const* btn_opr = &zeta_ord_rb_tree_node_opr;

    if (state == null_state) {
        cur_table_capacity = GetNxtTableCapacity_(0);

        ght->cur_salt = GetSalt_(ght);

        cur_table = AllocateTable_(ght, cur_table_capacity);

        for (size_t i = 0; i < cur_table_capacity; ++i) { cur_table[i] = NULL; }

        state = cur_state;
    }

    TryDo_;

    if (state == nxt_state) {
        InsertNode_(
            ght,
            nxt_table + GetBucketIdx_(ght->NodeHash(ght->node_hash_context,
                                                    node, ght->nxt_salt),
                                      nxt_table_capacity),
            node);

        ++nxt_table_cnt;
    } else {
        InsertNode_(
            ght,
            cur_table + GetBucketIdx_(ght->NodeHash(ght->node_hash_context,
                                                    node, ght->cur_salt),
                                      cur_table_capacity),
            node);

        ++cur_table_cnt;

        if (state == cur_state && cur_table_capacity * 8 <= cur_table_cnt) {
            state = trans_state;

            ght->nxt_salt = GetSalt_(ght);

            nxt_table_capacity = GetNxtTableCapacity_(cur_table_capacity);

            nxt_table = AllocateTable_(ght, nxt_table_capacity);
        }
    }

    ZETA_ColorPtr_Set(&ght->cur_table, alignof(void*), cur_table, state);
    ght->nxt_table = nxt_table;

    ght->cur_table_cnt = cur_table_cnt;
    ght->nxt_table_cnt = nxt_table_cnt;

    ght->cur_table_capacity = cur_table_capacity;
    ght->nxt_table_capacity = nxt_table_capacity;
}

void Zeta_GenericHashTable_Erase(void* ght_, void* node_) {
    Zeta_GenericHashTable* ght = ght_;
    CheckCntr_(ght);

    Zeta_GenericHashTable_Node* node = node_;
    ZETA_DebugAssert(node != NULL);

    int state = GetState_(ght);

    void** cur_table = GetCurTable_(ght);
    void** nxt_table = GetNxtTable_(ght);

    size_t cur_table_cnt = ght->cur_table_cnt;
    size_t nxt_table_cnt = ght->nxt_table_cnt;

    size_t cur_table_capacity = ght->cur_table_capacity;
    size_t nxt_table_capacity = ght->nxt_table_capacity;

    Zeta_BinTreeNodeOperator const* btn_opr = &zeta_ord_rb_tree_node_opr;

    void* root = Zeta_GetMostLink(NULL, Zeta_OrdRBTreeNode_GetP, &node->n);

    if (state != null_state) {
        size_t bucket_idx = GetBucketIdx_(
            ght->NodeHash(ght->node_hash_context, node, ght->cur_salt),
            cur_table_capacity);

        if (cur_table[bucket_idx] == root) {
            cur_table[bucket_idx] = Zeta_RBTree_Extract(btn_opr, &node->n);
            --cur_table_cnt;
            goto RET;
        }
    }

    if (state == nxt_state) {
        size_t bucket_idx = GetBucketIdx_(
            ght->NodeHash(ght->node_hash_context, node, ght->nxt_salt),
            nxt_table_capacity);

        if (nxt_table[bucket_idx] == root) {
            nxt_table[bucket_idx] = Zeta_RBTree_Extract(btn_opr, &node->n);
            --nxt_table_cnt;
            goto RET;
        }
    }

    ZETA_DebugAssert(FALSE);

RET:;

    TryDo_;

    ZETA_ColorPtr_Set(&ght->cur_table, alignof(void*), cur_table, state);
    ght->nxt_table = nxt_table;

    ght->cur_table_cnt = cur_table_cnt;
    ght->nxt_table_cnt = nxt_table_cnt;

    ght->cur_table_capacity = cur_table_capacity;
    ght->nxt_table_capacity = nxt_table_capacity;
}

unsigned long long Zeta_GenericHashTable_GetEffFactor(void const* ght_) {
    Zeta_GenericHashTable const* ght = ght_;
    CheckCntr_(ght);

    int state = GetState_(ght);

    void** cur_table = GetCurTable_(ght);
    void** nxt_table = GetNxtTable_(ght);

    size_t cur_table_cnt = ght->cur_table_cnt;
    size_t nxt_table_cnt = ght->nxt_table_cnt;

    size_t cur_table_capacity = ght->cur_table_capacity;
    size_t nxt_table_capacity = ght->nxt_table_capacity;

    if (state == null_state || cur_table_cnt + nxt_table_cnt == 0) { return 0; }

    Zeta_BinTreeNodeOperator const* btn_opr = &zeta_ord_rb_tree_node_opr;

    unsigned long long total_height = 0;

    {
        size_t mid = 0;

        if (state == nxt_state) {
            void** p = cur_table[-1];

            ZETA_DebugAssert(cur_table <= p &&
                             p < cur_table + cur_table_capacity);

            mid = p - cur_table;

            for (size_t i = 1; i < mid; ++i) {
                ZETA_DebugAssert(cur_table[i] == NULL);
            }
        }

        for (size_t i = mid; i < cur_table_capacity; ++i) {
            size_t tree_size = Zeta_BinTree_Count(btn_opr, cur_table[i]);

            total_height +=
                tree_size == 0
                    ? 0
                    : tree_size * ZETA_RoundIntDiv(
                                      Zeta_FixedPointLog2(tree_size *
                                                          ZETA_FixedPoint_Base),
                                      ZETA_FixedPoint_Base);
        }
    }

    if (state == nxt_state) {
        for (size_t i = 0; i < nxt_table_capacity; ++i) {
            size_t tree_size = Zeta_BinTree_Count(btn_opr, nxt_table[i]);

            total_height +=
                tree_size == 0
                    ? 0
                    : tree_size * ZETA_RoundIntDiv(
                                      Zeta_FixedPointLog2(tree_size *
                                                          ZETA_FixedPoint_Base),
                                      ZETA_FixedPoint_Base);
        }
    }

    return total_height * ZETA_FixedPoint_Base /
           (cur_table_cnt + nxt_table_cnt);
}

void Zeta_GenericHashTable_Check(void const* ght_) {
    Zeta_GenericHashTable const* ght = ght_;
    ZETA_DebugAssert(ght != NULL);

    int state = GetState_(ght);

    void** cur_table = GetCurTable_(ght);
    void** nxt_table = GetNxtTable_(ght);

    size_t cur_table_capacity = ght->cur_table_capacity;
    size_t nxt_table_capacity = ght->nxt_table_capacity;

    ZETA_DebugAssert(state == null_state || cur_state || state == trans_state ||
                     state == nxt_state);

    ZETA_DebugAssert((cur_table == &ght->cur_table) ==
                     (cur_table_capacity == 0));

    ZETA_DebugAssert((nxt_table == NULL) == (nxt_table_capacity == 0));

    if (state == null_state) {
        ZETA_DebugAssert(cur_table == &ght->cur_table);
        ZETA_DebugAssert(nxt_table == NULL);
        return;
    }

    ZETA_DebugAssert(cur_table != NULL);

    if (state != cur_state) { ZETA_DebugAssert(nxt_table != NULL); }

    if (state == trans_state) {
        void** p = nxt_table[-1];
        ZETA_DebugAssert(nxt_table <= p && p < nxt_table + nxt_table_capacity);
    }

    if (state == nxt_state) {
        void** p = cur_table[-1];
        ZETA_DebugAssert(cur_table <= p && p < cur_table + cur_table_capacity);
    }

    ZETA_DebugAssert(ght->table_allocator != NULL);
    ZETA_DebugAssert(ght->table_allocator->GetAlign != NULL);
    ZETA_DebugAssert(ght->table_allocator->Allocate != NULL);
    ZETA_DebugAssert(ght->table_allocator->Deallocate != NULL);
    ZETA_DebugAssert(
        ZETA_Allocator_GetAlign(ght->table_allocator) % alignof(void*) == 0);

    ZETA_DebugAssert(ght->NodeHash != NULL);
    ZETA_DebugAssert(ght->KeyHash != NULL);
    ZETA_DebugAssert(ght->NodeCompare != NULL);
    ZETA_DebugAssert(ght->NodeKeyCompare != NULL);
}

#if ZETA_EnableDebug

ZETA_DeclareStruct(SanitizeTreeRet);

struct SanitizeTreeRet {
    size_t cnt;
    Zeta_GenericHashTable_Node* most_l_n;
    Zeta_GenericHashTable_Node* most_r_n;
};

static SanitizeTreeRet SanitizeTree_(Zeta_GenericHashTable* ght,
                                     Zeta_MemRecorder* dst_node,
                                     size_t bucket_idx, size_t bucket_capacity,
                                     unsigned long long salt, void* n) {
    if (n == NULL) { return (SanitizeTreeRet){ 0, NULL, NULL }; }

    Zeta_GenericHashTable_Node* node =
        ZETA_MemberToStruct(Zeta_GenericHashTable_Node, n, n);

    ZETA_DebugAssert(
        GetBucketIdx_(ght->NodeHash(ght->node_hash_context, node, salt),
                      bucket_capacity) == bucket_idx);

    if (dst_node != NULL) {
        Zeta_MemRecorder_Record(dst_node, node,
                                sizeof(Zeta_GenericHashTable_Node));
    }

    void* nl = Zeta_OrdRBTreeNode_GetL(NULL, &node->n);
    void* nr = Zeta_OrdRBTreeNode_GetR(NULL, &node->n);

    SanitizeTreeRet l_ret =
        SanitizeTree_(ght, dst_node, bucket_idx, bucket_capacity, salt, nl);

    SanitizeTreeRet r_ret =
        SanitizeTree_(ght, dst_node, bucket_idx, bucket_capacity, salt, nr);

    SanitizeTreeRet ret;
    ret.cnt = l_ret.cnt + 1 + r_ret.cnt;
    ret.most_l_n = n;
    ret.most_r_n = n;

    if (nl != NULL) {
        int cmp = ght->NodeCompare(
            ght->node_cmp_context,
            ZETA_MemberToStruct(Zeta_GenericHashTable_Node, n, l_ret.most_r_n),
            node);

        ZETA_DebugAssert(cmp <= 0);

        ret.most_l_n = l_ret.most_l_n;
    }

    if (nr != NULL) {
        int cmp = ght->NodeCompare(
            ght->node_cmp_context, node,
            ZETA_MemberToStruct(Zeta_GenericHashTable_Node, n, r_ret.most_l_n));

        ZETA_DebugAssert(cmp <= 0);

        ret.most_r_n = r_ret.most_r_n;
    }

    return ret;
}

#endif

void Zeta_GenericHashTable_Sanitize(void* ght_, Zeta_MemRecorder* dst_table,
                                    Zeta_MemRecorder* dst_node) {
    Zeta_GenericHashTable* ght = ght_;
    CheckCntr_(ght);

#if !ZETA_EnableDebug
    ZETA_Unused(dst_table);
    ZETA_Unused(dst_node);
#else
    int state = GetState_(ght);

    void** cur_table = GetCurTable_(ght);
    void** nxt_table = GetNxtTable_(ght);

    size_t cur_table_cnt = ght->cur_table_cnt;
    size_t nxt_table_cnt = ght->nxt_table_cnt;

    size_t cur_table_capacity = ght->cur_table_capacity;
    size_t nxt_table_capacity = ght->nxt_table_capacity;

    if (state == null_state) { return; }

    if (dst_table != NULL) {
        if (cur_table != NULL) {
            Zeta_MemRecorder_Record(dst_table, cur_table - 1,
                                    sizeof(void*) * (cur_table_capacity + 1));
        }

        if (nxt_table != NULL) {
            Zeta_MemRecorder_Record(dst_table, nxt_table - 1,
                                    sizeof(void*) * (nxt_table_capacity + 1));
        }
    }

    {
        size_t mid = 0;

        if (state == nxt_state) {
            void** p = cur_table[-1];

            ZETA_DebugAssert(cur_table <= p &&
                             p < cur_table + cur_table_capacity);

            mid = p - cur_table;

            for (size_t i = 0; i < mid; ++i) {
                ZETA_DebugAssert(cur_table[i] == NULL);
            }
        }

        size_t re_cur_table_cnt = 0;

        for (size_t i = mid; i < cur_table_capacity; ++i) {
            re_cur_table_cnt +=
                SanitizeTree_(ght, dst_node, i, ght->cur_table_capacity,
                              ght->cur_salt, cur_table[i])
                    .cnt;
        }

        ZETA_DebugAssert(cur_table_cnt == re_cur_table_cnt);
    }

    if (state == cur_state) { return; }

    if (state == trans_state) {
        void** p = nxt_table[-1];

        ZETA_DebugAssert(nxt_table <= p && p < nxt_table + nxt_table_capacity);

        size_t end = p - nxt_table;

        for (size_t i = 0; i < end; ++i) {
            ZETA_DebugAssert(nxt_table[i] == NULL);
        }

        return;
    }

    size_t re_nxt_table_cnt = 0;

    for (size_t i = 0; i < nxt_table_capacity; ++i) {
        re_nxt_table_cnt +=
            SanitizeTree_(ght, dst_node, i, ght->nxt_table_capacity,
                          ght->nxt_salt, nxt_table[i])
                .cnt;
    }

    ZETA_DebugAssert(nxt_table_cnt == re_nxt_table_cnt);
#endif
}

void Zeta_GenericHashTable_CheckNode(void const* ght_, void* node_) {
    Zeta_GenericHashTable const* ght = ght_;
    CheckCntr_(ght);

    Zeta_GenericHashTable_Node* node = node_;
    ZETA_DebugAssert(node != NULL);

    int state = GetState_(ght);

    void** cur_table = GetCurTable_(ght);
    void** nxt_table = GetNxtTable_(ght);

    size_t cur_table_capacity = ght->cur_table_capacity;
    size_t nxt_table_capacity = ght->nxt_table_capacity;

    ZETA_DebugAssert(state != null_state);

    void* root = Zeta_GetMostLink(NULL, Zeta_OrdRBTreeNode_GetP, &node->n);

    size_t cur_bucket_idx = GetBucketIdx_(
        ght->NodeHash(ght->node_hash_context, node, ght->cur_salt),
        cur_table_capacity);

    if (cur_table[cur_bucket_idx] == root) { return; }

    ZETA_DebugAssert(state == nxt_state);

    size_t nxt_bucket_idx = GetBucketIdx_(
        ght->NodeHash(ght->node_hash_context, node, ght->nxt_salt),
        nxt_table_capacity);

    ZETA_DebugAssert(nxt_table[nxt_bucket_idx] == root);
}

void Zeta_GenericHashTable_Node_Init(void* node_) {
    Zeta_GenericHashTable_Node* node = node_;
    ZETA_DebugAssert(node != NULL);

    Zeta_OrdRBTreeNode_Init(NULL, &node->n);
}
