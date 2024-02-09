#include "../Zeta/MultiLevelHashTable.h"
#include "../Zeta/utils.h"

typedef unsigned long long int mlmap_key_t;
typedef unsigned long long int mlmap_val_t;

typedef struct MLMapNode MLMapNode;

struct MLMapNode {
    Zeta_MultiLevelHashTableNode mlhtn;
    mlmap_key_t key;
    mlmap_val_t val;
};

typedef struct MLMap MLMap;

struct MLMap {
    size_t size;
    Zeta_MultiLevelVector mlv;
    Zeta_MultiLevelHashTable mlht;
};

void* MLMap_Allocate(void* context, size_t size) {
    ZETA_Unused(context);
    return malloc(size);
}

void MLMap_Deallocate(void* context, void* ptr) {
    ZETA_Unused(context);
    free(ptr);
}

size_t SimpleHashKey(mlmap_key_t key) {
    size_t x = key;
    x = (x ^ (x >> 30)) * 0xbf58476d1ce4e5b9ULL;
    x = (x ^ (x >> 27)) * 0x94d049bb133111ebULL;
    x = x ^ (x >> 31);
    return x;
}

void MLMap_Init(void* mlmap_) {
    MLMap* mlmap = mlmap_;

    Zeta_MultiLevelVector* mlv = &mlmap->mlv;
    Zeta_MultiLevelHashTable* mlht = &mlmap->mlht;

    mlmap->size = 0;

    Zeta_MultiLevelVector_Init(mlv);
    Zeta_MultiLevelHashTable_Init(mlht);

    mlv->level = 3;
    mlv->branch_nums[0] = 256;
    mlv->branch_nums[1] = 256;
    mlv->branch_nums[2] = 256;
    mlv->allocator = malloc(sizeof(Zeta_Allocator));
    mlv->allocator->Allocate = MLMap_Allocate;
    mlv->allocator->Deallocate = MLMap_Deallocate;

    mlht->mlv = mlv;
}

void* MLMap_Create() {
    void* mlmap = malloc(sizeof(MLMap));
    MLMap_Init(mlmap);
    return mlmap;
}

size_t MLMap_GetSize(void* mlmap_) {
    MLMap* mlmap = mlmap_;
    ZETA_DebugAssert(mlmap != NULL);

    return mlmap->size;
}

bool_t MLMap_IsNull(void* mlmapn_) { return mlmapn_ == NULL; }

mlmap_val_t MLMap_GetVal(void* mlmapn_) {
    MLMapNode* mlmapn = mlmapn_;
    ZETA_DebugAssert(mlmapn != NULL);
    return mlmapn->val;
}

void MLMap_SetVal(void* mlmapn_, mlmap_val_t val) {
    MLMapNode* mlmapn = mlmapn_;
    ZETA_DebugAssert(mlmapn != NULL);
    mlmapn->val = val;
}

void* MLMap_Find(void* mlmap_, mlmap_key_t key) {
    MLMap* mlmap = mlmap_;
    ZETA_DebugAssert(mlmap != NULL);

    Zeta_MultiLevelHashTable* mlht = &mlmap->mlht;
    size_t hash_code = SimpleHashKey(key);

    Zeta_MultiLevelHashTableNode* mlhtn =
        Zeta_MultiLevelHashTable_Find(mlht, hash_code);

    while (mlhtn != NULL) {
        MLMapNode* mlmapn = ZETA_GetStructFromMem(MLMapNode, mlhtn, mlhtn);
        if (key == mlmapn->key) { return mlmapn; }
        mlhtn = Zeta_MultiLevelHashTable_FindNext(mlhtn);
    }

    return NULL;
}

void* MLMap_FindNext(void* mlmapn_, mlmap_key_t key) {
    MLMapNode* mlmapn = mlmapn_;
    ZETA_DebugAssert(mlmapn != NULL);

    Zeta_MultiLevelHashTableNode* mlhtn = &mlmapn->mlhtn;
    mlhtn = Zeta_MultiLevelHashTable_FindNext(mlhtn);

    while (mlhtn != NULL) {
        MLMapNode* mlmapn = ZETA_GetStructFromMem(MLMapNode, mlhtn, mlhtn);
        if (key == mlmapn->key) { return mlmapn; }
        mlhtn = Zeta_MultiLevelHashTable_FindNext(mlhtn);
    }

    return NULL;
}

void* MLMap_Insert(void* mlmap_, mlmap_key_t key, mlmap_val_t val) {
    MLMap* mlmap = mlmap_;
    ZETA_DebugAssert(mlmap != NULL);

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
    ZETA_DebugAssert(mlmap != NULL);

    MLMapNode* mlmapn = mlmapn_;
    ZETA_DebugAssert(mlmapn != NULL);

    Zeta_MultiLevelHashTable* mlht = &mlmap->mlht;
    Zeta_MultiLevelHashTableNode* mlhtn = &mlmapn->mlhtn;

    Zeta_MultiLevelHashTable_Extract(mlht, mlhtn);
    --mlmap->size;

    free(mlmapn);
}
