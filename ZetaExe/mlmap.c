#include "../Zeta/MultiLevelHashTable.h"
#include "../Zeta/utils.h"

typedef unsigned long long int key_t;
typedef unsigned long long int val_t;

ZETA_DECL_STRUCT(MLMapNode) {
    Zeta_MultiLevelHashTableNode mlhtn;
    key_t key;
    val_t val;
};

ZETA_DECL_STRUCT(MLMap) {
    diff_t size;
    Zeta_MultiLevelEntryTable mlet;
    Zeta_MultiLevelHashTable mlht;
};

void* MyAllocate(diff_t size) { return malloc(size); }
void MyDeallocate(void* ptr) { free(ptr); }

size_t SimpleHashKey(key_t key) {
    size_t x = key;
    x = (x ^ (x >> 30)) * 0xbf58476d1ce4e5b9ULL;
    x = (x ^ (x >> 27)) * 0x94d049bb133111ebULL;
    x = x ^ (x >> 31);
    return x;
}

void MLMap_Init(void* mlmap_) {
    MLMap* mlmap = mlmap_;

    Zeta_MultiLevelEntryTable* mlet = &mlmap->mlet;
    Zeta_MultiLevelHashTable* mlht = &mlmap->mlht;

    mlmap->size = 0;

    Zeta_MultiLevelEntryTable_Init(mlet);
    Zeta_MultiLevelHashTable_Init(mlht);

    mlet->level = 3;
    mlet->branch_nums[0] = 256;
    mlet->branch_nums[1] = 256;
    mlet->branch_nums[2] = 256;
    mlet->Allocate = MyAllocate;
    mlet->Deallocate = MyDeallocate;

    mlht->mlet = mlet;
}

void* MLMap_Create() {
    void* mlmap = malloc(sizeof(MLMap));
    MLMap_Init(mlmap);
    return mlmap;
}

diff_t MLMap_GetSize(void* mlmap_) {
    MLMap* mlmap = mlmap_;
    ZETA_DEBUG_ASSERT(mlmap != NULL);

    return mlmap->size;
}

_Bool MLMap_IsNull(void* mlmapn_) { return mlmapn_ == NULL; }

val_t MLMap_GetVal(void* mlmapn_) {
    MLMapNode* mlmapn = mlmapn_;
    ZETA_DEBUG_ASSERT(mlmapn != NULL);
    return mlmapn->val;
}

void MLMap_SetVal(void* mlmapn_, val_t val) {
    MLMapNode* mlmapn = mlmapn_;
    ZETA_DEBUG_ASSERT(mlmapn != NULL);
    mlmapn->val = val;
}

void* MLMap_Find(void* mlmap_, key_t key) {
    MLMap* mlmap = mlmap_;
    ZETA_DEBUG_ASSERT(mlmap != NULL);

    Zeta_MultiLevelHashTable* mlht = &mlmap->mlht;
    size_t hash_code = SimpleHashKey(key);

    Zeta_MultiLevelHashTableNode* mlhtn =
        Zeta_MultiLevelHashTable_Find(mlht, hash_code);

    while (mlhtn != NULL) {
        MLMapNode* mlmapn = ZETA_FROM_MEM(MLMapNode, mlhtn, mlhtn);
        if (key == mlmapn->key) { return mlmapn; }
        mlhtn = Zeta_MultiLevelHashTable_FindNext(mlhtn);
    }

    return NULL;
}

void* MLMap_FindNext(void* mlmapn_, key_t key) {
    MLMapNode* mlmapn = mlmapn_;
    ZETA_DEBUG_ASSERT(mlmapn != NULL);

    Zeta_MultiLevelHashTableNode* mlhtn = &mlmapn->mlhtn;
    mlhtn = Zeta_MultiLevelHashTable_FindNext(mlhtn);

    while (mlhtn != NULL) {
        MLMapNode* mlmapn = ZETA_FROM_MEM(MLMapNode, mlhtn, mlhtn);
        if (key == mlmapn->key) { return mlmapn; }
        mlhtn = Zeta_MultiLevelHashTable_FindNext(mlhtn);
    }

    return NULL;
}

void* MLMap_Insert(void* mlmap_, key_t key, val_t val) {
    MLMap* mlmap = mlmap_;
    ZETA_DEBUG_ASSERT(mlmap != NULL);

    Zeta_MultiLevelHashTable* mlht = &mlmap->mlht;

    MLMapNode* mlmapn = malloc(sizeof(MLMapNode));
    Zeta_MultiLevelHashTableNode* mlhtn = &mlmapn->mlhtn;
    Zeta_MultiLevelHashTableNode_Init(mlhtn);

    mlhtn->hash_code = SimpleHashKey(key);
    mlmapn->key = key;
    mlmapn->val = val;

    ++mlmap->size;
    Zeta_MultiLevelHashTable_Insert(mlht, mlhtn);

    return mlmapn;
}

void MLMap_Erase(void* mlmap_, void* mlmapn_) {
    MLMap* mlmap = mlmap_;
    ZETA_DEBUG_ASSERT(mlmap != NULL);

    MLMapNode* mlmapn = mlmapn_;
    ZETA_DEBUG_ASSERT(mlmapn != NULL);

    Zeta_MultiLevelHashTable* mlht = &mlmap->mlht;
    Zeta_MultiLevelHashTableNode* mlhtn = &mlmapn->mlhtn;

    Zeta_MultiLevelHashTable_Extract(mlht, mlhtn);
    --mlmap->size;

    free(mlmapn);
}
