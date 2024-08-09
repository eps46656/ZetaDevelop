#include "GenericHashTable.h"

#include "Debugger.h"
#include "MemCheck.h"
#include "RBTree.h"
#include "utils.h"

#if ZETA_IsDebug

#define CheckDHT_(ght)

#else

#define CheckDHT_(ght)

#endif

#define GetCurTable_(ght) \
    ((void**)__builtin_align_down(ght->cur_table, alignof(void*)))

#define GetNxtTable_(ght) ght->nxt_table

#define GetState_(ght) \
    ((unsigned char*)ght->cur_table - (unsigned char*)GetCurTable_(ght))

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
    void* table = ght->table_allocator->Allocate(ght->table_allocator->context,
                                                 sizeof(void*) * capacity);

    ZETA_DebugAssert(table != NULL);
    ZETA_DebugAssert(__builtin_is_aligned(table, alignof(void*)));

    return table;
}

static size_t GetBucketIdx_(unsigned long long hash_code,
                            unsigned long long salt, size_t bucket_capacity) {
    return Zeta_SimpleHash(hash_code + salt) % bucket_capacity;
}

static void InsertNode_(Zeta_BinTreeNodeOperator* btn_opr, void** root,
                        void* m_) {
    void* n = *root;

    Zeta_GenericHashTable_Node* m =
        ZETA_MemberToStruct(Zeta_GenericHashTable_Node, n, m_);

    Zeta_GenericHashTable_Node* lt_n = NULL;
    Zeta_GenericHashTable_Node* ge_n = NULL;

    while (n != NULL) {
        Zeta_GenericHashTable_Node* cur_n =
            ZETA_MemberToStruct(Zeta_GenericHashTable_Node, n, n);

        if (cur_n->hash_code < m->hash_code) {
            lt_n = cur_n;
            n = Zeta_OrdRBTreeNode_GetR(NULL, n);
        } else {
            ge_n = cur_n;
            n = Zeta_OrdRBTreeNode_GetL(NULL, n);
        }
    }

    *root = Zeta_RBTree_Insert(btn_opr, lt_n, ge_n, &m->n);
}

void Zeta_GenericHashTable_Init(void* ght_) {
    Zeta_GenericHashTable* ght = ght_;
    CheckDHT_(ght);

    //
}

#define TryDo_                                                                \
    if (state == trans_state) {                                               \
        size_t i = (void**)nxt_table[0] - nxt_table;                          \
        size_t j = ZETA_GetMinOf(i + 32, nxt_table_capacity);                 \
                                                                              \
        for (; i < j; ++i) { nxt_table[i] = NULL; }                           \
                                                                              \
        if (j < nxt_table_capacity) {                                         \
            nxt_table[0] = nxt_table + j;                                     \
        } else {                                                              \
            state = nxt_state;                                                \
            nxt_table[0] = NULL;                                              \
        }                                                                     \
    } else if (state == nxt_state) {                                          \
        void** p = cur_table[0];                                              \
                                                                              \
        if (p < cur_table || cur_table + cur_table_capacity <= p) {           \
            p = cur_table;                                                    \
        }                                                                     \
                                                                              \
        ZETA_CheckAssert(p - cur_table < cur_table_capacity);                 \
                                                                              \
        size_t op_cnt = 16 * 4;                                               \
                                                                              \
        while (0 < op_cnt) {                                                  \
            if (*p == NULL) {                                                 \
                --op_cnt;                                                     \
                if (++p == cur_table + cur_table_capacity) { break; }         \
                continue;                                                     \
            }                                                                 \
                                                                              \
            Zeta_GenericHashTable_Node* trans_node = *p;                      \
                                                                              \
            *p = Zeta_RBTree_Extract(&btn_opr, &trans_node->n);               \
                                                                              \
            --cur_table_cnt;                                                  \
                                                                              \
            InsertNode_(                                                      \
                &btn_opr,                                                     \
                nxt_table + GetBucketIdx_(trans_node->hash_code,              \
                                          ght->nxt_salt, nxt_table_capacity), \
                &trans_node->n);                                              \
                                                                              \
            ++nxt_table_cnt;                                                  \
                                                                              \
            op_cnt = ZETA_GetMaxOf(15, op_cnt) - 15;                          \
        }                                                                     \
                                                                              \
        if (p == cur_table + cur_table_capacity) {                            \
            ght->table_allocator->Deallocate(ght->table_allocator->context,   \
                                             cur_table);                      \
                                                                              \
            cur_table = nxt_table;                                            \
            cur_table_cnt = nxt_table_cnt;                                    \
            cur_table_capacity = nxt_table_capacity;                          \
                                                                              \
            nxt_table = NULL;                                                 \
            nxt_table_cnt = 0;                                                \
            nxt_table_capacity = 0;                                           \
                                                                              \
            state = cur_state;                                                \
        } else {                                                              \
            if (p != cur_table) {                                             \
                ZETA_CheckAssert(cur_table[0] == NULL);                       \
                cur_table[0] = p;                                             \
            }                                                                 \
                                                                              \
            InsertNode_(                                                      \
                &btn_opr,                                                     \
                nxt_table + GetBucketIdx_(node->hash_code, ght->nxt_salt,     \
                                          nxt_table_capacity),                \
                &node->n);                                                    \
                                                                              \
            ++nxt_table_cnt;                                                  \
        }                                                                     \
    }                                                                         \
    ZETA_StaticAssert(TRUE)

void* Zeta_GenericHashTable_Find(void* ght_, unsigned long long hash_code) {
    Zeta_GenericHashTable* ght = ght_;
    CheckDHT_(ght);

    int state = GetState_(ght);

    void** cur_table = GetCurTable_(ght);
    void** nxt_table = GetNxtTable_(ght);

    size_t cur_table_cnt = ght->cur_table_cnt;
    size_t nxt_table_cnt = ght->nxt_table_cnt;

    size_t cur_table_capacity = ght->cur_table_capacity;
    size_t nxt_table_capacity = ght->nxt_table_capacity;

    Zeta_BinTreeNodeOperator btn_opr;
    Zeta_OrdRBTreeNode_DeployBinTreeNodeOperator(NULL, &btn_opr);

    if (state == null_state) { return NULL; }

    size_t cur_bucket_idx =
        GetBucketIdx_(hash_code, ght->cur_salt, cur_table_capacity);

    Zeta_GenericHashTable_Node* cur_le_n = NULL;

    for (void* n = cur_table[cur_bucket_idx]; n != NULL;) {
        Zeta_GenericHashTable_Node* cur_n =
            ZETA_MemberToStruct(Zeta_GenericHashTable_Node, n, n);

        if (cur_n->hash_code <= hash_code) {
            cur_le_n = cur_n;
            n = Zeta_OrdRBTreeNode_GetR(NULL, n);
        } else {
            n = Zeta_OrdRBTreeNode_GetL(NULL, n);
        }
    }

    if (state != nxt_state) {
        return cur_le_n->hash_code == hash_code ? cur_le_n : NULL;
    }

    Zeta_GenericHashTable_Node* nxt_lt_n = NULL;
    Zeta_GenericHashTable_Node* nxt_ge_n = NULL;

    size_t nxt_bucket_idx =
        GetBucketIdx_(hash_code, ght->nxt_salt, nxt_table_capacity);

    for (void* n = nxt_table[nxt_bucket_idx]; n != NULL;) {
        Zeta_GenericHashTable_Node* nxt_n =
            ZETA_MemberToStruct(Zeta_GenericHashTable_Node, n, n);

        if (nxt_n->hash_code < hash_code) {
            nxt_lt_n = nxt_n;
            n = Zeta_OrdRBTreeNode_GetR(NULL, n);
        } else {
            nxt_ge_n = nxt_n;
            n = Zeta_OrdRBTreeNode_GetL(NULL, n);
        }
    }

    while (cur_le_n != NULL && cur_le_n->hash_code == hash_code) {
        Zeta_GenericHashTable_Node* nxt_cur_le_n =
            ZETA_MemberToStruct(Zeta_GenericHashTable_Node, n,
                                Zeta_BinTree_StepR(&btn_opr, &cur_le_n->n));

        cur_table[cur_bucket_idx] = Zeta_RBTree_Extract(&btn_opr, &cur_le_n->n);

        --cur_table_cnt;

        nxt_table[nxt_bucket_idx] =
            Zeta_RBTree_Insert(&btn_opr, nxt_lt_n, nxt_ge_n, cur_le_n);

        ++nxt_table_cnt;

        nxt_ge_n = cur_le_n;
        cur_le_n = nxt_cur_le_n;
    }

    ght->cur_table_cnt = cur_table_cnt;
    ght->nxt_table_cnt = nxt_table_cnt;

    return nxt_ge_n;
}

void Zeta_GenericHashTable_Insert(void* ght_, void* node_) {
    Zeta_GenericHashTable* ght = ght_;
    CheckDHT_(ght);

    Zeta_GenericHashTable_Node* node = node_;
    ZETA_DebugAssert(node != NULL);

    int state = GetState_(ght);

    void** cur_table = GetCurTable_(ght);
    void** nxt_table = GetNxtTable_(ght);

    size_t cur_table_cnt = ght->cur_table_cnt;
    size_t nxt_table_cnt = ght->nxt_table_cnt;

    size_t cur_table_capacity = ght->cur_table_capacity;
    size_t nxt_table_capacity = ght->nxt_table_capacity;

    Zeta_BinTreeNodeOperator btn_opr;
    Zeta_OrdRBTreeNode_DeployBinTreeNodeOperator(NULL, &btn_opr);

    if (state == null_state) {
        cur_table_capacity = GetNxtTableCapacity_(0);

        cur_table = AllocateTable_(ght, cur_table_capacity);

        for (size_t i = 0; i < cur_table_capacity; ++i) { cur_table[i] = NULL; }

        state = cur_state;

        // null_state -> cur_state
    }

    TryDo_;

    if (state == nxt_state) {
        // nxt_state -> nxt_state

        InsertNode_(&btn_opr,
                    nxt_table + GetBucketIdx_(node->hash_code, ght->nxt_salt,
                                              nxt_table_capacity),
                    &node->n);

        ++nxt_table_cnt;
    } else {
        InsertNode_(&btn_opr,
                    cur_table + GetBucketIdx_(node->hash_code, ght->cur_salt,
                                              cur_table_capacity),
                    &node->n);

        ++cur_table_cnt;

        if (cur_table_capacity * 4 <= cur_table_cnt * 5) {
            ZETA_CheckAssert(state == cur_state);

            state = trans_state;

            ght->nxt_salt = GetSalt_(ght);

            nxt_table_capacity = GetNxtTableCapacity_(cur_table_capacity);

            nxt_table = AllocateTable_(ght, nxt_table_capacity);

            nxt_table[0] = nxt_table + 1;
        }
    }

    ght->cur_table = (unsigned char*)cur_table + state;
    ght->nxt_table = nxt_table;

    ght->cur_table_cnt = cur_table_cnt;
    ght->nxt_table_cnt = nxt_table_cnt;

    ght->cur_table_capacity = cur_table_capacity;
    ght->nxt_table_capacity = nxt_table_capacity;
}

void Zeta_GenericHashTable_Erase(void* ght_, void* node_) {
    Zeta_GenericHashTable* ght = ght_;
    CheckDHT_(ght);

    Zeta_GenericHashTable_Node* node = node_;
    ZETA_DebugAssert(node != NULL);

    int state = GetState_(ght);

    void** cur_table = GetCurTable_(ght);
    void** nxt_table = GetNxtTable_(ght);

    size_t cur_table_cnt = ght->cur_table_cnt;
    size_t nxt_table_cnt = ght->nxt_table_cnt;

    size_t cur_table_capacity = ght->cur_table_capacity;
    size_t nxt_table_capacity = ght->nxt_table_capacity;

    Zeta_BinTreeNodeOperator btn_opr;
    Zeta_OrdRBTreeNode_DeployBinTreeNodeOperator(NULL, &btn_opr);

    void* root = Zeta_GetMostLink(NULL, Zeta_OrdRBTreeNode_GetP, &node->n);

    if (state != null_state) {
        size_t bucket_idx =
            GetBucketIdx_(node->hash_code, ght->cur_salt, cur_table_capacity);

        if (cur_table[bucket_idx] == root) {
            cur_table[bucket_idx] = Zeta_RBTree_Extract(&btn_opr, &node->n);
            --cur_table_cnt;
            goto RET;
        }
    }

    if (state == nxt_state) {
        size_t bucket_idx =
            GetBucketIdx_(node->hash_code, ght->nxt_salt, cur_table_capacity);

        if (nxt_table[bucket_idx] == root) {
            nxt_table[bucket_idx] = Zeta_RBTree_Extract(&btn_opr, &node->n);
            --nxt_table_cnt;
            goto RET;
        }
    }

    ZETA_DebugAssert(FALSE);

RET:;

    TryDo_;

    ght->cur_table = (unsigned char*)cur_table + state;
    ght->nxt_table = nxt_table;

    ght->cur_table_cnt = cur_table_cnt;
    ght->nxt_table_cnt = nxt_table_cnt;

    ght->cur_table_capacity = cur_table_capacity;
    ght->nxt_table_capacity = nxt_table_capacity;
}

void Zeta_GenericHashTable_Check(void* ght_) {
    Zeta_GenericHashTable* ght = ght_;
    CheckDHT_(ght);
}

static size_t SanitizeTree_(Zeta_DebugHashMap* dst_node,
                            Zeta_GenericHashTable_Node* n) {
    if (n == NULL) { return 0; }

    Zeta_MemCheck_AddPtrSize(dst_node, n, sizeof(Zeta_GenericHashTable));

    Zeta_GenericHashTable_Node* nl = ZETA_MemberToStruct(
        Zeta_GenericHashTable_Node, n, Zeta_OrdRBTreeNode_GetL(NULL, &n->n));

    Zeta_GenericHashTable_Node* nr = ZETA_MemberToStruct(
        Zeta_GenericHashTable_Node, n, Zeta_OrdRBTreeNode_GetR(NULL, &n->n));

    return SanitizeTree_(dst_node, nl) + 1 + SanitizeTree_(dst_node, nr);
}

void Zeta_GenericHashTable_Sanitize(void* ght_, Zeta_DebugHashMap* dst_table,
                                    Zeta_DebugHashMap* dst_node) {
    Zeta_GenericHashTable* ght = ght_;
    CheckDHT_(ght);

    ZETA_DebugAssert(dst_table != NULL);
    ZETA_DebugAssert(dst_node != NULL);

    int state = GetState_(ght);

    void** cur_table = GetCurTable_(ght);
    void** nxt_table = GetNxtTable_(ght);

    size_t cur_table_cnt = ght->cur_table_cnt;
    size_t nxt_table_cnt = ght->nxt_table_cnt;

    size_t cur_table_capacity = ght->cur_table_capacity;
    size_t nxt_table_capacity = ght->nxt_table_capacity;

    Zeta_BinTreeNodeOperator btn_opr;
    Zeta_OrdRBTreeNode_DeployBinTreeNodeOperator(NULL, &btn_opr);

    if (state == null_state) { return; }

    {
        size_t mid = 0;

        if (state == nxt_state) {
            void** p = cur_table[0];

            if (cur_table <= p && p < cur_table + cur_table_capacity) {
                mid = p - cur_table;

                ZETA_DebugAssert(1 <= mid && mid < cur_table_capacity);

                for (size_t i = 1; i < mid; ++i) {
                    ZETA_DebugAssert(cur_table[i] == NULL);
                }
            }
        }

        size_t re_cur_table_cnt = 0;

        for (size_t i = mid; i < cur_table_capacity; ++i) {
            re_cur_table_cnt += SanitizeTree_(dst_node, cur_table[i]);
        }

        ZETA_DebugAssert(cur_table_cnt == re_cur_table_cnt);
    }

    if (state == cur_state) { return; }

    if (state == trans_state) {
        void** p = nxt_table[0];

        ZETA_DebugAssert(nxt_table + 1 <= p &&
                         p < nxt_table + nxt_table_capacity);

        size_t end = p - nxt_table;

        for (size_t i = 1; i < end; ++i) {
            ZETA_DebugAssert(nxt_table[i] == NULL);
        }

        return;
    }

    size_t re_nxt_table_cnt = 0;

    for (size_t i = 0; i < nxt_table_capacity; ++i) {
        re_nxt_table_cnt += SanitizeTree_(dst_node, nxt_table[i]);
    }

    ZETA_DebugAssert(nxt_table_cnt == re_nxt_table_cnt);
}
