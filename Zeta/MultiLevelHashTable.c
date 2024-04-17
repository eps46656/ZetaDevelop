#include "MultiLevelHashTable.h"

static void GetIdxes_(size_t* dst_idxes, Zeta_MultiLevelVector* mlv,
                      size_t hash_code) {
    size_t level = mlv->level;
    size_t* branch_nums = mlv->branch_nums;

    for (size_t level_i = 0; level_i < level; ++level_i) {
        dst_idxes[level_i] = hash_code % branch_nums[level_i];
        hash_code /= branch_nums[level_i];
    }
}

void Zeta_MultiLevelHashTableNode_Init(void* mlhtn_) {
    Zeta_MultiLevelHashTableNode* mlhtn = (Zeta_MultiLevelHashTableNode*)mlhtn_;
    ZETA_DebugAssert(mlhtn != NULL);

    mlhtn->prev = NULL;
    mlhtn->next = NULL;
}

void Zeta_MultiLevelHashTable_Init(void* mlht_) {
    Zeta_MultiLevelHashTable* mlht = mlht_;
    ZETA_DebugAssert(mlht != NULL);

    Zeta_MultiLevelVector_Init(&mlht->mlv);
}

void* Zeta_MultiLevelHashTable_GetFirst(void* mlht_) {
    Zeta_MultiLevelHashTable* mlht = mlht_;
    ZETA_DebugAssert(mlht != NULL);

    Zeta_MultiLevelVector* mlv = &mlht->mlv;

    size_t idxes[ZETA_MultiLevelVector_max_level];
    GetIdxes_(idxes, mlv, 0);

    void** n = Zeta_MultiLevelVector_FindNextNotNull(mlv, idxes, 1);

    return n == NULL ? NULL : *n;
}

void* Zeta_MultiLevelHashTable_GetNext(void* mlht_, void* mlhtn_) {
    Zeta_MultiLevelHashTable* mlht = mlht_;
    ZETA_DebugAssert(mlht != NULL);

    Zeta_MultiLevelHashTableNode* mlhtn = (Zeta_MultiLevelHashTableNode*)mlhtn_;
    ZETA_DebugAssert(mlhtn != NULL);

    Zeta_MultiLevelHashTableNode* nxt_mlhtn = mlhtn->next;
    if (nxt_mlhtn != NULL) { return nxt_mlhtn; }

    Zeta_MultiLevelVector* mlv = &mlht->mlv;

    size_t idxes[ZETA_MultiLevelVector_max_level];
    GetIdxes_(idxes, mlv, mlhtn->hash_code);

    void** n = Zeta_MultiLevelVector_FindNextNotNull(mlv, idxes, 0);

    return n == NULL ? NULL : *n;
}

void Zeta_MultiLevelHashTable_Insert(void* mlht_, void* mlhtn_) {
    Zeta_MultiLevelHashTable* mlht = mlht_;
    ZETA_DebugAssert(mlht != NULL);

    Zeta_MultiLevelHashTableNode* mlhtn = (Zeta_MultiLevelHashTableNode*)mlhtn_;
    ZETA_DebugAssert(mlhtn != NULL);
    ZETA_DebugAssert(mlhtn->prev == NULL);
    ZETA_DebugAssert(mlhtn->next == NULL);

    Zeta_MultiLevelVector* mlv = &mlht->mlv;
    ZETA_DebugAssert(mlv != NULL);

    size_t idxes[ZETA_MultiLevelVector_max_level];
    GetIdxes_(idxes, mlv, mlhtn->hash_code);

    void** n = Zeta_MultiLevelVector_Insert(&mlht->mlv, idxes);

    Zeta_MultiLevelHashTableNode* head_mlhtn = *n;
    *n = mlhtn;

    if (head_mlhtn != NULL) {
        mlhtn->next = head_mlhtn;
        head_mlhtn->prev = mlhtn;
    }
}

void Zeta_MultiLevelHashTable_Extract(void* mlht_, void* mlhtn_) {
    Zeta_MultiLevelHashTable* mlht = mlht_;
    ZETA_DebugAssert(mlht != NULL);

    Zeta_MultiLevelHashTableNode* mlhtn = (Zeta_MultiLevelHashTableNode*)mlhtn_;
    ZETA_DebugAssert(mlhtn != NULL);

    Zeta_MultiLevelVector* mlv = &mlht->mlv;
    ZETA_DebugAssert(mlv != NULL);

    size_t idxes[ZETA_MultiLevelVector_max_level];
    GetIdxes_(idxes, mlv, mlhtn->hash_code);

    void** n = Zeta_MultiLevelVector_Access(&mlht->mlv, idxes);

    ZETA_DebugAssert(n != NULL);

    if (ZETA_IsDebug) {
        bool_t found = FALSE;

        for (Zeta_MultiLevelHashTableNode* m = *n; m != NULL; m = m->next) {
            if (mlhtn == m) {
                found = TRUE;
                break;
            }
        }

        ZETA_DebugAssert(found);
    }

    Zeta_MultiLevelHashTableNode* prev_mlhtn = mlhtn->prev;
    Zeta_MultiLevelHashTableNode* next_mlhtn = mlhtn->next;

    mlhtn->prev = NULL;
    mlhtn->next = NULL;

    if (prev_mlhtn == NULL) {
        if (next_mlhtn == NULL) {
            Zeta_MultiLevelVector_Erase(&mlht->mlv, idxes);
        } else {
            *n = next_mlhtn;
            next_mlhtn->prev = NULL;
        }
    } else {
        prev_mlhtn->next = next_mlhtn;
        if (next_mlhtn != NULL) { next_mlhtn->prev = prev_mlhtn; }
    }
}

void* Zeta_MultiLevelHashTable_Find(void* mlht_, size_t hash_code) {
    Zeta_MultiLevelHashTable* mlht = mlht_;
    ZETA_DebugAssert(mlht != NULL);

    Zeta_MultiLevelVector* mlv = &mlht->mlv;
    ZETA_DebugAssert(mlv != NULL);

    size_t idxes[ZETA_MultiLevelVector_max_level];
    GetIdxes_(idxes, mlv, hash_code);

    void** n = Zeta_MultiLevelVector_Insert(&mlht->mlv, idxes);

    if (n == NULL) { return NULL; }

    for (Zeta_MultiLevelHashTableNode* m = *n; m != NULL; m = m->next) {
        if (hash_code == m->hash_code) { return m; }
    }

    return NULL;
}

void* Zeta_MultiLevelHashTable_FindNext(void* mlhtn_) {
    Zeta_MultiLevelHashTableNode* mlhtn = (Zeta_MultiLevelHashTableNode*)mlhtn_;
    ZETA_DebugAssert(mlhtn != NULL);

    size_t hash_code = mlhtn->hash_code;

    for (Zeta_MultiLevelHashTableNode* m = mlhtn->next; m != NULL;
         m = m->next) {
        if (hash_code == m->hash_code) { return m; }
    }

    return NULL;
}
