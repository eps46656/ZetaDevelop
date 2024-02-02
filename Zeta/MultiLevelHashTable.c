#include "MultiLevelHashTable.h"

static void GetIdxes_(size_t* dst_idxes, Zeta_MultiLevelEntryTable* mlet,
                      size_t hash_code) {
    size_t level = mlet->level;
    size_t* branch_nums = mlet->branch_nums;

    for (size_t level_i = 0; level_i < level; ++level_i) {
        dst_idxes[level_i] = hash_code % branch_nums[level_i];
        hash_code /= branch_nums[level_i];
    }
}

void Zeta_MultiLevelHashTableNode_Init(void* mlhtn_) {
    Zeta_MultiLevelHashTableNode* mlhtn = mlhtn_;
    ZETA_DEBUG_ASSERT(mlhtn != NULL);

    mlhtn->prev = NULL;
    mlhtn->next = NULL;
}

void Zeta_MultiLevelHashTable_Init(void* mlht_) {
    Zeta_MultiLevelHashTable* mlht = mlht_;
    ZETA_DEBUG_ASSERT(mlht != NULL);

    mlht->mlet = NULL;
}

void* Zeta_MultiLevelHashTable_GetFirst(void* mlht_) {
    Zeta_MultiLevelHashTable* mlht = mlht_;
    ZETA_DEBUG_ASSERT(mlht != NULL);

    Zeta_MultiLevelEntryTable* mlet = mlht->mlet;

    size_t idxes[Zeta_MultiLevelEntryTable_max_level];
    GetIdxes_(idxes, mlet, 0);

    void** n = Zeta_MultiLevelEntryTable_FindNextNotNull(mlet, idxes, 1);

    return n == NULL ? NULL : *n;
}

void* Zeta_MultiLevelHashTable_GetNext(void* mlht_, void* mlhtn_) {
    Zeta_MultiLevelHashTable* mlht = mlht_;
    ZETA_DEBUG_ASSERT(mlht != NULL);

    Zeta_MultiLevelHashTableNode* mlhtn = mlhtn_;
    ZETA_DEBUG_ASSERT(mlhtn != NULL);

    Zeta_MultiLevelHashTableNode* nxt_mlhtn = mlhtn->next;
    if (nxt_mlhtn != NULL) { return nxt_mlhtn; }

    Zeta_MultiLevelEntryTable* mlet = mlht->mlet;

    size_t idxes[Zeta_MultiLevelEntryTable_max_level];
    GetIdxes_(idxes, mlet, mlhtn->hash_code);

    void** n = Zeta_MultiLevelEntryTable_FindNextNotNull(mlet, idxes, 0);

    return n == NULL ? NULL : *n;
}

void Zeta_MultiLevelHashTable_Insert(void* mlht_, void* mlhtn_) {
    Zeta_MultiLevelHashTable* mlht = mlht_;
    ZETA_DEBUG_ASSERT(mlht != NULL);

    Zeta_MultiLevelHashTableNode* mlhtn = mlhtn_;
    ZETA_DEBUG_ASSERT(mlhtn != NULL);
    ZETA_DEBUG_ASSERT(mlhtn->prev == NULL);
    ZETA_DEBUG_ASSERT(mlhtn->next == NULL);

    Zeta_MultiLevelEntryTable* mlet = mlht->mlet;
    ZETA_DEBUG_ASSERT(mlet != NULL);

    size_t idxes[Zeta_MultiLevelEntryTable_max_level];
    GetIdxes_(idxes, mlet, mlhtn->hash_code);

    void** n = Zeta_MultiLevelEntryTable_Insert(mlht->mlet, idxes);

    Zeta_MultiLevelHashTableNode* head_mlhtn = *n;
    *n = mlhtn;

    if (head_mlhtn != NULL) {
        mlhtn->next = head_mlhtn;
        head_mlhtn->prev = mlhtn;
    }
}

void Zeta_MultiLevelHashTable_Extract(void* mlht_, void* mlhtn_) {
    Zeta_MultiLevelHashTable* mlht = mlht_;
    ZETA_DEBUG_ASSERT(mlht != NULL);

    Zeta_MultiLevelHashTableNode* mlhtn = mlhtn_;
    ZETA_DEBUG_ASSERT(mlhtn != NULL);

    Zeta_MultiLevelEntryTable* mlet = mlht->mlet;
    ZETA_DEBUG_ASSERT(mlet != NULL);

    size_t idxes[Zeta_MultiLevelEntryTable_max_level];
    GetIdxes_(idxes, mlet, mlhtn->hash_code);

    void** n = Zeta_MultiLevelEntryTable_Insert(mlht->mlet, idxes);

    ZETA_DEBUG_ASSERT(n != NULL);

    if (ZETA_DEBUG) {
        bool_t found = 0;

        for (Zeta_MultiLevelHashTableNode* m = *n; m != NULL; m = m->next) {
            if (mlhtn == m) {
                found = 1;
                break;
            }
        }

        ZETA_DEBUG_ASSERT(found);
    }

    Zeta_MultiLevelHashTableNode* prev_mlhtn = mlhtn->prev;
    Zeta_MultiLevelHashTableNode* next_mlhtn = mlhtn->next;

    mlhtn->prev = NULL;
    mlhtn->next = NULL;

    if (prev_mlhtn == NULL) {
        if (next_mlhtn == NULL) {
            Zeta_MultiLevelEntryTable_Erase(mlht->mlet, idxes);
        } else {
            *n = next_mlhtn;
            next_mlhtn->prev = NULL;
        }
    } else {
        if (next_mlhtn == NULL) {
            prev_mlhtn->next = NULL;
        } else {
            prev_mlhtn->next = next_mlhtn;
            next_mlhtn->prev = prev_mlhtn;
        }
    }
}

void* Zeta_MultiLevelHashTable_Find(void* mlht_, size_t hash_code) {
    Zeta_MultiLevelHashTable* mlht = mlht_;
    ZETA_DEBUG_ASSERT(mlht != NULL);

    Zeta_MultiLevelEntryTable* mlet = mlht->mlet;
    ZETA_DEBUG_ASSERT(mlet != NULL);

    size_t idxes[Zeta_MultiLevelEntryTable_max_level];
    GetIdxes_(idxes, mlet, hash_code);

    void** n = Zeta_MultiLevelEntryTable_Insert(mlht->mlet, idxes);

    if (n == NULL) { return NULL; }

    for (Zeta_MultiLevelHashTableNode* m = *n; m != NULL; m = m->next) {
        if (hash_code == m->hash_code) { return m; }
    }

    return NULL;
}

void* Zeta_MultiLevelHashTable_FindNext(void* mlhtn_) {
    Zeta_MultiLevelHashTableNode* mlhtn = mlhtn_;
    ZETA_DEBUG_ASSERT(mlhtn != NULL);

    size_t hash_code = mlhtn->hash_code;

    for (Zeta_MultiLevelHashTableNode* m = mlhtn->next; m != NULL;
         m = m->next) {
        if (hash_code == m->hash_code) { return m; }
    }

    return NULL;
}
