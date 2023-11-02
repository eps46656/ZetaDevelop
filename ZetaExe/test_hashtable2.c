#include "../Zeta/MultiLevelHashTable.h"
#include "../Zeta/utils.h"
#include "SimpleVector.h"
#include "test_head.h"

ZETA_DECL_STRUCT(HashTableNode) {
    Zeta_MultiLevelHashTableNode mlhtn;
    key_t key;
    val_t val;
};

ZETA_DECL_STRUCT(HashTable) {
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

void HashTable_Init(HashTable* ht) {
    Zeta_MultiLevelEntryTable* mlet = &ht->mlet;
    Zeta_MultiLevelHashTable* mlht = &ht->mlht;

    ht->size = 0;

    Zeta_MultiLevelEntryTable_Init(mlet);
    Zeta_MultiLevelHashTable_Init(mlht);

    mlet->level = 5;
    mlet->branch_nums[0] = 512;
    mlet->branch_nums[1] = 512;
    mlet->branch_nums[2] = 512;
    mlet->branch_nums[3] = 512;
    mlet->branch_nums[4] = 512;
    mlet->Allocate = MyAllocate;
    mlet->Deallocate = MyDeallocate;

    mlht->mlet = mlet;
}

diff_t HashTable_GetSize(HashTable* ht) { return ht->size; }

HashTableNode* HashTable_Find(HashTable* ht, key_t key) {
    Zeta_MultiLevelHashTable* mlht = &ht->mlht;
    size_t hash_code = SimpleHashKey(key);

    ZETA_PRINT_POS;

    Zeta_MultiLevelHashTableNode* mlhtn =
        Zeta_MultiLevelHashTable_Find(mlht, hash_code);

    ZETA_PRINT_POS;

    for (; mlhtn != NULL;) {
        HashTableNode* htn = ZETA_FROM_MEM(HashTableNode, mlhtn, mlhtn);
        if (key == htn->key) { return htn; }
        mlhtn = Zeta_MultiLevelHashTable_FindNext(mlhtn);
    }

    return NULL;
}

HashTableNode* HashTable_Insert(HashTable* ht, key_t key, val_t val) {
    Zeta_MultiLevelHashTable* mlht = &ht->mlht;

    HashTableNode* htn = HashTable_Find(ht, key);
    if (htn != NULL) { return htn; }

    htn = malloc(sizeof(HashTableNode));
    Zeta_MultiLevelHashTableNode* mlhtn = &htn->mlhtn;
    Zeta_MultiLevelHashTableNode_Init(mlhtn);

    mlhtn->hash_code = SimpleHashKey(key);
    htn->key = key;
    htn->val = val;

    ++ht->size;

    ZETA_PRINT_POS;

    Zeta_MultiLevelHashTable_Insert(mlht, mlhtn);

    ZETA_PRINT_POS;

    return htn;
}

void HashTable_Erase(HashTable* ht, HashTableNode* htn) {
    Zeta_MultiLevelHashTable* mlht = &ht->mlht;
    Zeta_MultiLevelHashTableNode* mlhtn = &htn->mlhtn;
    Zeta_MultiLevelHashTable_Extract(mlht, mlhtn);
    free(htn);
}

void HashTable_EraseWithKey(HashTable* ht, key_t key) {
    HashTableNode* htn = HashTable_Find(ht, key);
    if (htn == NULL) { return; }
    HashTable_Erase(ht, htn);
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

int main() {
    /*
    s             : get size
    a <key>       : access the val with key
    i <key> <val> : insert (key, val)
    e <key>       : erase with key
    z             : end
    */

    FILE* f = fopen("out.txt", "w");

    HashTable ht;
    HashTable_Init(&ht);

    char command[8];
    key_t key;
    val_t val;

    for (;;) {
        fflush(stdout);
        fflush(f);
        scanf("%s", &command);

        if (strcmp(command, "s") == 0) {
            printf("%lld\n", HashTable_GetSize(&ht));
            continue;
        }

        if (strcmp(command, "a") == 0) {
            scanf("%d", &key);

            HashTableNode* htn = HashTable_Find(&ht, key);

            if (htn == NULL) {
                printf("1000000007\n");
            } else {
                printf("%d\n", htn->val);
            }

            continue;
        }

        if (strcmp(command, "i") == 0) {
            scanf("%d", &key);
            scanf("%d", &val);

            HashTable_Insert(&ht, key, val);

            printf("0\n");
            continue;
        }

        if (strcmp(command, "e") == 0) {
            scanf("%lld", &key);

            HashTable_EraseWithKey(&ht, key);

            printf("0\n");
            continue;
        }

        if (strcmp(command, "z") == 0) {
            printf("0\n");
            break;
        }

        ZETA_DEBUG_ASSERT(0);
    }

    return 0;
}
