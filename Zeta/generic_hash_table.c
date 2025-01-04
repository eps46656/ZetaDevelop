#include "generic_hash_table.h"

#include "bin_tree.h"
#include "debugger.h"
#include "mem_check_utils.h"
#include "multi_level_table.h"
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

#define branch_num ZETA_MultiLevelTable_max_branch_num

static unsigned short branch_nums[] = { [0 ... ZETA_MultiLevelTable_max_level -
                                         1] = branch_num };

static size_t capacities[] = {
    61ULL,       251ULL,       1021ULL,       4093ULL,       16381ULL,
    65521ULL,    262139ULL,    1048573ULL,    4194301ULL,    16777213ULL,
    67108859ULL, 268435399ULL, 1073741789ULL, 4294967291ULL,

#if ZETA_SIZE_WIDTH == 32
};
#else

    17179869143ULL,
    68719476731ULL,
    274877906899ULL,
    1099511627689ULL,
    4398046511093ULL,
    17592186044399ULL,
    70368744177643ULL,
    281474976710597ULL,

#if ZETA_SIZE_WIDTH == 48
};
#else

    1125899906842597ULL,
    4503599627370449ULL,
    18014398509481951ULL,
    72057594037927931ULL,
    288230376151711717ULL,
    1152921504606846883ULL,
    4611686018427387847ULL,
    18446744073709551557ULL,
};

#endif
#endif

static unsigned long long GetSalt_(Zeta_GenericHashTable const* ght) {
    unsigned long long ret = ght->cur_salt;
    return Zeta_SimpleRandomRotate(&ret);
}

static size_t GetBucketIdx_(unsigned long long hash_code, size_t capacity) {
    return Zeta_ULLHash(hash_code, 0) % capacity;
}

static size_t GetIdx_(int level, size_t const* idxes) {
    size_t idx = 0;

    for (int level_i = level - 1; 0 <= level_i; --level_i) {
        idx = idx * branch_num + idxes[level_i];
    }

    return idx;
}

static void SetIdxes_(int level, size_t* idxes, size_t idx) {
    for (int level_i = 0; level_i < level; ++level_i) {
        idxes[level_i] = idx % branch_num;
        idx /= branch_num;
    }
}

static size_t FindPrvCapacity_(size_t capacity) {
    int len = sizeof(capacities) / sizeof(capacities[0]);

    if (capacity <= capacities[0]) { return 0; }

    int lb = 0;
    int rb = len - 1;

    while (lb < rb) {
        int mb = (lb + rb + 1) / 2;

        if (capacities[mb] < capacity) {
            lb = mb;
        } else {
            rb = mb - 1;
        }
    }

    return capacities[lb];
}

static size_t FindNxtCapacity_(size_t capacity) {
    int len = sizeof(capacities) / sizeof(capacities[0]);

    int lb = 0;
    int rb = len;

    while (lb < rb) {
        int mb = (lb + rb) / 2;

        if (capacity < capacities[mb]) {
            rb = mb;
        } else {
            lb = mb + 1;
        }
    }

    return lb < len ? capacities[lb] : 0;
}

static void* Find_(unsigned long long salt, Zeta_MultiLevelTable* table,
                   size_t capacity, void const* key,
                   void const* key_hash_context, Zeta_Hash KeyHash,
                   void const* key_node_cmp_context,
                   Zeta_Compare KeyNodeCompare) {
    size_t idxes[ZETA_MultiLevelTable_max_level];

    SetIdxes_(table->level, idxes,
              GetBucketIdx_(KeyHash(key_hash_context, key, salt), capacity));

    void* target_n = NULL;

    for (void* n = ({
             void** tmp = Zeta_MultiLevelTable_Access(table, idxes);
             tmp == NULL ? NULL : *tmp;
         });
         n != NULL;) {
        int cmp = KeyNodeCompare(
            key_node_cmp_context, key,
            ZETA_MemberToStruct(Zeta_GenericHashTable_Node, n, n));

        if (cmp == 0) { target_n = n; }

        if (cmp <= 0) {
            n = Zeta_OrdRBTreeNode_GetL(NULL, n);
        } else {
            n = Zeta_OrdRBTreeNode_GetR(NULL, n);
        }
    }

    return target_n == NULL
               ? NULL
               : ZETA_MemberToStruct(Zeta_GenericHashTable_Node, n, target_n);
}

static void Insert_(Zeta_GenericHashTable* ght, Zeta_MultiLevelTable* table,
                    unsigned long long salt, size_t capacity,
                    Zeta_GenericHashTable_Node* node) {
    size_t idxes[ZETA_MultiLevelTable_max_level];

    SetIdxes_(table->level, idxes,
              GetBucketIdx_(ght->NodeHash(ght->node_hash_context, node, salt),
                            capacity));

    void** root_entry = Zeta_MultiLevelTable_Insert(table, idxes);

    void* n = *root_entry;

    void* le_n = NULL;
    void* gt_n = NULL;

    while (n != NULL) {
        int cmp = ght->NodeCompare(
            ght->node_cmp_context, node,
            ZETA_MemberToStruct(Zeta_GenericHashTable_Node, n, n));

        if (cmp < 0) {
            gt_n = n;
            n = Zeta_OrdRBTreeNode_GetL(NULL, n);
        } else {
            le_n = n;
            n = Zeta_OrdRBTreeNode_GetR(NULL, n);
        }
    }

    *root_entry =
        Zeta_RBTree_Insert(&zeta_ord_rb_tree_node_opr, le_n, gt_n, &node->n);
}

static bool_t TryExtract_(Zeta_GenericHashTable* ght,
                          Zeta_MultiLevelTable* table, unsigned long long salt,
                          size_t capacity, Zeta_GenericHashTable_Node* node,
                          void* root) {
    if (capacity == 0) { return FALSE; }

    size_t idxes[ZETA_MultiLevelTable_max_level];

    SetIdxes_(table->level, idxes,
              GetBucketIdx_(ght->NodeHash(ght->node_hash_context, node, salt),
                            capacity));

    void** root_entry = Zeta_MultiLevelTable_Access(table, idxes);

    if (root_entry == NULL || *root_entry != root) { return FALSE; }

    if ((*root_entry = Zeta_RBTree_Extract(&zeta_ord_rb_tree_node_opr,
                                           &node->n)) == NULL) {
        Zeta_MultiLevelTable_Erase(table, idxes);
    }

    return TRUE;
}

static void TryTransfer_(Zeta_GenericHashTable const* ght,
                         Zeta_MultiLevelTable* cur_table,
                         Zeta_MultiLevelTable* nxt_table) {
    if (cur_table->level == 0 || nxt_table->level == 0 ||
        cur_table->size == 0) {
        return;
    }

    size_t idxes[ZETA_MultiLevelTable_max_level];

    void** root_entry = NULL;

    for (int quata = 4; 0 < quata && 0 < cur_table->size; --quata) {
        if (root_entry == NULL) {
            root_entry = Zeta_MultiLevelTable_FindFirst(cur_table, idxes);
        }

        Zeta_GenericHashTable_Node* trans_node =
            ZETA_MemberToStruct(Zeta_GenericHashTable_Node, n, *root_entry);

        if ((*root_entry = Zeta_RBTree_Extract(&zeta_ord_rb_tree_node_opr,
                                               &trans_node->n)) == NULL) {
            Zeta_MultiLevelTable_Erase(cur_table, idxes);
            root_entry = NULL;
        }

        Insert_((Zeta_GenericHashTable*)ght, nxt_table, ght->nxt_salt,
                ght->nxt_capacity, trans_node);
    }
}

void Zeta_GenericHashTable_Init(void* ght_) {
    Zeta_GenericHashTable* ght = ght_;
    ZETA_DebugAssert(ght != NULL);

    ght->cur_salt = GetSalt_(ght);

    ght->cur_table_root = NULL;
    ght->nxt_table_root = NULL;

    ght->cur_table_size = 0;
    ght->nxt_table_size = 0;

    ght->cur_capacity = capacities[0];
    ght->nxt_capacity = 0;

    ght->size = 0;

    Zeta_Allocator* table_node_allocator = ght->table_node_allocator;

    ZETA_DebugAssert(table_node_allocator != NULL);
    ZETA_DebugAssert(table_node_allocator->GetAlign != NULL);
    ZETA_DebugAssert(table_node_allocator->Allocate != NULL);
    ZETA_DebugAssert(table_node_allocator->Deallocate != NULL);
    ZETA_DebugAssert(ZETA_Allocator_GetAlign(ght->table_node_allocator) %
                         alignof(void*) ==
                     0);
}

void Zeta_GenericHashTable_Deinit(void* ght) {
    Zeta_GenericHashTable_ExtractAll(ght);
}

size_t Zeta_GenericHashTable_GetSize(void const* ght_) {
    Zeta_GenericHashTable const* ght = ght_;
    CheckCntr_(ght);

    return ght->size;
}

#define TryDo_                                                              \
    for (;;) {                                                              \
        TryTransfer_(ght, &cur_table, &nxt_table);                          \
                                                                            \
        if (nxt_capacity == 0) {                                            \
            ((Zeta_GenericHashTable*)ght)->cur_table_size = cur_table.size; \
            ((Zeta_GenericHashTable*)ght)->cur_table_root = cur_table.root; \
                                                                            \
            if (cur_table.size * 2 < cur_capacity) {                        \
                ((Zeta_GenericHashTable*)ght)->nxt_capacity =               \
                    FindPrvCapacity_(cur_capacity);                         \
                ((Zeta_GenericHashTable*)ght)->nxt_salt = GetSalt_(ght);    \
            } else if (cur_capacity * 8 <= ght->size) {                     \
                ((Zeta_GenericHashTable*)ght)->nxt_capacity =               \
                    FindNxtCapacity_(cur_capacity);                         \
                ((Zeta_GenericHashTable*)ght)->nxt_salt = GetSalt_(ght);    \
            }                                                               \
                                                                            \
            break;                                                          \
        }                                                                   \
                                                                            \
        if (cur_table.size != 0) {                                          \
            ((Zeta_GenericHashTable*)ght)->cur_table_size = cur_table.size; \
            ((Zeta_GenericHashTable*)ght)->cur_table_root = cur_table.root; \
                                                                            \
            ((Zeta_GenericHashTable*)ght)->nxt_table_size = nxt_table.size; \
            ((Zeta_GenericHashTable*)ght)->nxt_table_root = nxt_table.root; \
                                                                            \
            break;                                                          \
        }                                                                   \
                                                                            \
        if (nxt_table.size == 0) {                                          \
            ((Zeta_GenericHashTable*)ght)->cur_table_size = 0;              \
            ((Zeta_GenericHashTable*)ght)->cur_capacity = capacities[0];    \
            ((Zeta_GenericHashTable*)ght)->cur_table_root = NULL;           \
        } else {                                                            \
            ((Zeta_GenericHashTable*)ght)->cur_table_size = nxt_table.size; \
            ((Zeta_GenericHashTable*)ght)->cur_capacity = nxt_capacity;     \
            ((Zeta_GenericHashTable*)ght)->cur_table_root = nxt_table.root; \
        }                                                                   \
                                                                            \
        ((Zeta_GenericHashTable*)ght)->nxt_table_size = 0;                  \
        ((Zeta_GenericHashTable*)ght)->nxt_capacity = 0;                    \
        ((Zeta_GenericHashTable*)ght)->nxt_table_root = NULL;               \
                                                                            \
        break;                                                              \
    }

bool_t Zeta_GenericHashTable_Contain(void const* ght_, void const* node_) {
    Zeta_GenericHashTable const* ght = ght_;
    CheckCntr_(ght);

    Zeta_GenericHashTable_Node const* node = node_;

    if (node == NULL) { return FALSE; }

    size_t cur_capacity = ght->cur_capacity;
    size_t nxt_capacity = ght->nxt_capacity;

    Zeta_MultiLevelTable cur_table;
    cur_table.level = Zeta_CeilLog(cur_capacity, branch_num);
    cur_table.branch_nums = branch_nums;
    cur_table.size = ght->cur_table_size;
    cur_table.root = ght->cur_table_root;
    cur_table.node_allocator = ght->table_node_allocator;

    Zeta_MultiLevelTable nxt_table;
    nxt_table.level = Zeta_CeilLog(nxt_capacity, branch_num);
    nxt_table.branch_nums = branch_nums;
    nxt_table.size = ght->nxt_table_size;
    nxt_table.root = ght->nxt_table_root;
    nxt_table.node_allocator = ght->table_node_allocator;

    void* root =
        Zeta_GetMostLink(NULL, Zeta_OrdRBTreeNode_GetP, (void*)&node->n);

    size_t idxes[ZETA_MultiLevelTable_max_level];

    void** root_entry;

    bool_t ret;

    SetIdxes_(cur_table.level, idxes,
              GetBucketIdx_(
                  ght->NodeHash(ght->node_hash_context, node, ght->cur_salt),
                  cur_capacity));

    if (!(ret = ((root_entry =
                      Zeta_MultiLevelTable_Access(&cur_table, idxes)) != NULL &&
                 *root_entry == root)) &&
        0 < nxt_capacity) {
        SetIdxes_(nxt_table.level, idxes,
                  GetBucketIdx_(ght->NodeHash(ght->node_hash_context, node,
                                              ght->nxt_salt),
                                nxt_capacity));

        ret = (root_entry = Zeta_MultiLevelTable_Access(&nxt_table, idxes)) !=
                  NULL &&
              *root_entry == root;
    }

    TryDo_;

    return ret;
}

void* Zeta_GenericHashTable_Find(void const* ght_, void const* key,
                                 void const* key_hash_context,
                                 Zeta_Hash KeyHash,
                                 void const* key_node_cmp_context,
                                 Zeta_Compare KeyNodeCompare) {
    Zeta_GenericHashTable const* ght = ght_;
    CheckCntr_(ght);

    size_t cur_capacity = ght->cur_capacity;
    size_t nxt_capacity = ght->nxt_capacity;

    Zeta_MultiLevelTable cur_table;
    cur_table.level = Zeta_CeilLog(cur_capacity, branch_num);
    cur_table.branch_nums = branch_nums;
    cur_table.size = ght->cur_table_size;
    cur_table.root = ght->cur_table_root;
    cur_table.node_allocator = ght->table_node_allocator;

    Zeta_MultiLevelTable nxt_table;
    nxt_table.level = Zeta_CeilLog(nxt_capacity, branch_num);
    nxt_table.branch_nums = branch_nums;
    nxt_table.size = ght->nxt_table_size;
    nxt_table.root = ght->nxt_table_root;
    nxt_table.node_allocator = ght->table_node_allocator;

    void* ret = nxt_capacity == 0
                    ? NULL
                    : Find_(ght->nxt_salt, &nxt_table, nxt_capacity, key,
                            key_hash_context, KeyHash, key_node_cmp_context,
                            KeyNodeCompare);

    if (ret == NULL) {
        ret = Find_(ght->cur_salt, &cur_table, cur_capacity, key,
                    key_hash_context, KeyHash, key_node_cmp_context,
                    KeyNodeCompare);
    }

    TryDo_;

    return ret;
}

void Zeta_GenericHashTable_Insert(void* ght_, void* node_) {
    Zeta_GenericHashTable* ght = ght_;
    CheckCntr_(ght);

    Zeta_GenericHashTable_Node* node = node_;
    ZETA_DebugAssert(node != NULL);

    size_t cur_capacity = ght->cur_capacity;
    size_t nxt_capacity = ght->nxt_capacity;

    Zeta_MultiLevelTable cur_table;
    cur_table.level = Zeta_CeilLog(cur_capacity, branch_num);
    cur_table.branch_nums = branch_nums;
    cur_table.size = ght->cur_table_size;
    cur_table.root = ght->cur_table_root;
    cur_table.node_allocator = ght->table_node_allocator;

    Zeta_MultiLevelTable nxt_table;
    nxt_table.level = Zeta_CeilLog(nxt_capacity, branch_num);
    nxt_table.branch_nums = branch_nums;
    nxt_table.size = ght->nxt_table_size;
    nxt_table.root = ght->nxt_table_root;
    nxt_table.node_allocator = ght->table_node_allocator;

    if (0 < nxt_capacity) {
        Insert_(ght, &nxt_table, ght->nxt_salt, nxt_capacity, node);
    } else {
        Insert_(ght, &cur_table, ght->cur_salt, cur_capacity, node);
    }

    ++ght->size;

    TryDo_;
}

void Zeta_GenericHashTable_Extract(void* ght_, void* node_) {
    Zeta_GenericHashTable* ght = ght_;
    CheckCntr_(ght);

    Zeta_GenericHashTable_Node* node = node_;
    ZETA_DebugAssert(node != NULL);

    size_t cur_capacity = ght->cur_capacity;
    size_t nxt_capacity = ght->nxt_capacity;

    Zeta_MultiLevelTable cur_table;
    cur_table.level = Zeta_CeilLog(cur_capacity, branch_num);
    cur_table.branch_nums = branch_nums;
    cur_table.size = ght->cur_table_size;
    cur_table.root = ght->cur_table_root;
    cur_table.node_allocator = ght->table_node_allocator;

    Zeta_MultiLevelTable nxt_table;
    nxt_table.level = Zeta_CeilLog(nxt_capacity, branch_num);
    nxt_table.branch_nums = branch_nums;
    nxt_table.size = ght->nxt_table_size;
    nxt_table.root = ght->nxt_table_root;
    nxt_table.node_allocator = ght->table_node_allocator;

    void* root = Zeta_GetMostLink(NULL, Zeta_OrdRBTreeNode_GetP, &node->n);

    if (!TryExtract_(ght, &cur_table, ght->cur_salt, cur_capacity, node,
                     root) &&
        !TryExtract_(ght, &nxt_table, ght->nxt_salt, nxt_capacity, node,
                     root)) {
        ZETA_DebugAssert(FALSE);
    }

    --ght->size;

    TryDo_;
}

void* Zeta_GenericHashTable_ExtractAny(void* ght_) {
    Zeta_GenericHashTable* ght = ght_;
    CheckCntr_(ght);

    if (ght->size == 0) { return NULL; }

    size_t cur_capacity = ght->cur_capacity;
    size_t nxt_capacity = ght->nxt_capacity;

    Zeta_MultiLevelTable cur_table;
    cur_table.level = Zeta_CeilLog(cur_capacity, branch_num);
    cur_table.branch_nums = branch_nums;
    cur_table.size = ght->cur_table_size;
    cur_table.root = ght->cur_table_root;
    cur_table.node_allocator = ght->table_node_allocator;

    Zeta_MultiLevelTable nxt_table;
    nxt_table.level = Zeta_CeilLog(nxt_capacity, branch_num);
    nxt_table.branch_nums = branch_nums;
    nxt_table.size = ght->nxt_table_size;
    nxt_table.root = ght->nxt_table_root;
    nxt_table.node_allocator = ght->table_node_allocator;

    size_t idxes[ZETA_MultiLevelTable_max_level];

    void** root_entry = Zeta_MultiLevelTable_FindFirst(&cur_table, idxes);

    Zeta_GenericHashTable_Node* node =
        ZETA_MemberToStruct(Zeta_GenericHashTable_Node, n, *root_entry);

    if ((*root_entry = Zeta_RBTree_Extract(&zeta_ord_rb_tree_node_opr,
                                           &node->n)) == NULL) {
        Zeta_MultiLevelTable_Erase(&cur_table, idxes);
    }

    --ght->size;

    TryDo_;

    return node;
}

void Zeta_GenericHashTable_ExtractAll(void* ght_) {
    Zeta_GenericHashTable* ght = ght_;
    CheckCntr_(ght);

    size_t cur_capacity = ght->cur_capacity;
    size_t nxt_capacity = ght->nxt_capacity;

    Zeta_MultiLevelTable cur_table;
    cur_table.level = Zeta_CeilLog(cur_capacity, branch_num);
    cur_table.branch_nums = branch_nums;
    cur_table.size = ght->cur_table_size;
    cur_table.root = ght->cur_table_root;
    cur_table.node_allocator = ght->table_node_allocator;

    Zeta_MultiLevelTable nxt_table;
    nxt_table.level = Zeta_CeilLog(nxt_capacity, branch_num);
    nxt_table.branch_nums = branch_nums;
    nxt_table.size = ght->nxt_table_size;
    nxt_table.root = ght->nxt_table_root;
    nxt_table.node_allocator = ght->table_node_allocator;

    Zeta_MultiLevelTable_Deinit(&cur_table);

    if (0 < nxt_capacity) { Zeta_MultiLevelTable_Deinit(&nxt_table); }

    ght->cur_salt = GetSalt_(ght);

    ght->cur_table_root = NULL;
    ght->nxt_table_root = NULL;

    ght->cur_table_size = 0;
    ght->nxt_table_size = 0;

    ght->cur_capacity = capacities[0];

    ght->size = 0;
}

unsigned long long Zeta_GenericHashTable_GetEffFactor(void const* ght_) {
    Zeta_GenericHashTable const* ght = ght_;
    CheckCntr_(ght);

    if (ght->size == 0) { return 0; }

    size_t cur_capacity = ght->cur_capacity;
    size_t nxt_capacity = ght->nxt_capacity;

    Zeta_MultiLevelTable cur_table;
    cur_table.level = Zeta_CeilLog(cur_capacity, branch_num);
    cur_table.branch_nums = branch_nums;
    cur_table.size = ght->cur_table_size;
    cur_table.root = ght->cur_table_root;
    cur_table.node_allocator = ght->table_node_allocator;

    Zeta_MultiLevelTable nxt_table;
    nxt_table.level = Zeta_CeilLog(nxt_capacity, branch_num);
    nxt_table.branch_nums = branch_nums;
    nxt_table.size = ght->nxt_table_size;
    nxt_table.root = ght->nxt_table_root;
    nxt_table.node_allocator = ght->table_node_allocator;

    unsigned long long total_height = 0;

    size_t idxes[ZETA_MultiLevelTable_max_level];

    if (0 < cur_capacity) {
        void** root_entry = Zeta_MultiLevelTable_FindFirst(&cur_table, idxes);

        while (root_entry != NULL) {
            size_t tree_size =
                Zeta_BinTree_Count(&zeta_ord_rb_tree_node_opr, *root_entry);

            total_height += ZETA_CeilLog2(tree_size) * tree_size;

            root_entry =
                Zeta_MultiLevelTable_FindNext(&cur_table, idxes, FALSE);
        }
    }

    if (0 < nxt_capacity) {
        void** root_entry = Zeta_MultiLevelTable_FindFirst(&nxt_table, idxes);

        while (root_entry != NULL) {
            size_t tree_size =
                Zeta_BinTree_Count(&zeta_ord_rb_tree_node_opr, *root_entry);

            total_height += ZETA_CeilLog2(tree_size) * tree_size;

            root_entry =
                Zeta_MultiLevelTable_FindNext(&nxt_table, idxes, FALSE);
        }
    }

    ZETA_PrintVar(total_height);

    return total_height * ZETA_FixedPoint_Base / ght->size;
}

void Zeta_GenericHashTable_Check(void const* ght_) {
    Zeta_GenericHashTable const* ght = ght_;
    ZETA_DebugAssert(ght != NULL);

    size_t cur_capacity = ght->cur_capacity;

    ZETA_DebugAssert(cur_capacity != 0);

    ZETA_DebugAssert(ght->table_node_allocator != NULL);
    ZETA_DebugAssert(ght->table_node_allocator->GetAlign != NULL);
    ZETA_DebugAssert(ght->table_node_allocator->Allocate != NULL);
    ZETA_DebugAssert(ght->table_node_allocator->Deallocate != NULL);
    ZETA_DebugAssert(ZETA_Allocator_GetAlign(ght->table_node_allocator) %
                         alignof(void*) ==
                     0);

    ZETA_DebugAssert(ght->NodeHash != NULL);
    ZETA_DebugAssert(ght->NodeCompare != NULL);
}

ZETA_DeclareStruct(SanitizeTreeRet);

struct SanitizeTreeRet {
    size_t cnt;
    Zeta_GenericHashTable_Node* most_l_n;
    Zeta_GenericHashTable_Node* most_r_n;
};

static SanitizeTreeRet SanitizeTree_(Zeta_GenericHashTable const* ght,
                                     Zeta_MemRecorder* dst_node,
                                     unsigned long long salt, size_t capacity,
                                     size_t bucket_idx, void* n) {
    if (n == NULL) { return (SanitizeTreeRet){ 0, NULL, NULL }; }

    Zeta_GenericHashTable_Node* node =
        ZETA_MemberToStruct(Zeta_GenericHashTable_Node, n, n);

    ZETA_DebugAssert(
        GetBucketIdx_(ght->NodeHash(ght->node_hash_context, node, salt),
                      capacity) == bucket_idx);

    if (dst_node != NULL) {
        Zeta_MemRecorder_Record(dst_node, node,
                                sizeof(Zeta_GenericHashTable_Node));
    }

    void* nl = Zeta_OrdRBTreeNode_GetL(NULL, &node->n);
    void* nr = Zeta_OrdRBTreeNode_GetR(NULL, &node->n);

    SanitizeTreeRet l_ret =
        SanitizeTree_(ght, dst_node, salt, capacity, bucket_idx, nl);

    SanitizeTreeRet r_ret =
        SanitizeTree_(ght, dst_node, salt, capacity, bucket_idx, nr);

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

void Zeta_GenericHashTable_Sanitize(void const* ght_,
                                    Zeta_MemRecorder* dst_table_node,
                                    Zeta_MemRecorder* dst_node) {
    Zeta_GenericHashTable const* ght = ght_;
    CheckCntr_(ght);

    size_t cur_capacity = ght->cur_capacity;
    size_t nxt_capacity = ght->nxt_capacity;

    Zeta_MultiLevelTable cur_table;
    cur_table.level = Zeta_CeilLog(cur_capacity, branch_num);
    cur_table.branch_nums = branch_nums;
    cur_table.size = ght->cur_table_size;
    cur_table.root = ght->cur_table_root;
    cur_table.node_allocator = ght->table_node_allocator;

    Zeta_MultiLevelTable nxt_table;
    nxt_table.level = Zeta_CeilLog(nxt_capacity, branch_num);
    nxt_table.branch_nums = branch_nums;
    nxt_table.size = ght->nxt_table_size;
    nxt_table.root = ght->nxt_table_root;
    nxt_table.node_allocator = ght->table_node_allocator;

    if (0 < cur_capacity) {
        Zeta_MultiLevelTable_Sanitize(&cur_table, dst_table_node);
    }

    if (0 < nxt_capacity) {
        Zeta_MultiLevelTable_Sanitize(&nxt_table, dst_table_node);
    }

    size_t total_size = 0;

    size_t idxes[ZETA_MultiLevelTable_max_level];

    if (0 < cur_capacity) {
        void** root_entry = Zeta_MultiLevelTable_FindFirst(&cur_table, idxes);

        while (root_entry != NULL) {
            total_size +=
                Zeta_BinTree_Count(&zeta_ord_rb_tree_node_opr, *root_entry);

            SanitizeTree_(ght, dst_node, ght->cur_salt, cur_capacity,
                          GetIdx_(cur_table.level, idxes), *root_entry);

            root_entry =
                Zeta_MultiLevelTable_FindNext(&cur_table, idxes, FALSE);
        }
    }

    if (0 < nxt_capacity) {
        void** root_entry = Zeta_MultiLevelTable_FindFirst(&nxt_table, idxes);

        while (root_entry != NULL) {
            total_size +=
                Zeta_BinTree_Count(&zeta_ord_rb_tree_node_opr, *root_entry);

            SanitizeTree_(ght, dst_node, ght->nxt_salt, nxt_capacity,
                          GetIdx_(nxt_table.level, idxes), *root_entry);

            root_entry =
                Zeta_MultiLevelTable_FindNext(&nxt_table, idxes, FALSE);
        }
    }

    ZETA_DebugAssert(ght->size == total_size);
}

void Zeta_GenericHashTable_Node_Init(void* node_) {
    Zeta_GenericHashTable_Node* node = node_;
    ZETA_DebugAssert(node != NULL);

    Zeta_OrdRBTreeNode_Init(NULL, &node->n);
}
