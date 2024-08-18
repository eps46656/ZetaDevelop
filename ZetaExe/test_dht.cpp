#include <DebugDeque.h>
#include <Debugger.h>
#include <DynamicHashTable.h>
#include <MemCheck.h>

#include "StdAllocator.h"
#include "Timer.h"
#include "Value.h"

int ULLCompare(unsigned long long a, unsigned long long b) {
    if (a < b) { return -1; }
    if (b < a) { return 1; }
    return 0;
}

struct Pair {
    unsigned long long key;
    unsigned long long val;
};

unsigned long long Zeta_PairHash(void* context, void const* pair) {
    ZETA_Unused(context);
    return std::hash<unsigned long long>()(((Pair const*)pair)->key);
}

unsigned long long Zeta_KeyHash(void* context, void const* key) {
    ZETA_Unused(context);
    return std::hash<unsigned long long>()(*(unsigned long long const*)key);
}

int Zeta_PairCompare(void* context, void const* pair_x, void const* pair_y) {
    ZETA_Unused(context);
    return ULLCompare(((Pair const*)pair_x)->key, ((Pair const*)pair_y)->key);
}

int Zeta_PairKeyCompare(void* context, void const* pair, void const* key) {
    ZETA_Unused(context);
    return ULLCompare(((Pair const*)pair)->key,
                      *(unsigned long long const*)key);
}

struct DynamicHashTablePack {
    StdAllocator node_allocator_;
    StdAllocator table_allocator_;

    Zeta_Allocator node_allocator;
    Zeta_Allocator table_allocator;

    Zeta_DynamicHashTable dht;
};

Zeta_DynamicHashTable* DynamicHashTable_Create() {
    DynamicHashTablePack* pack{ new DynamicHashTablePack{} };

    StdAllocator_DeployAllocator(&pack->node_allocator_, &pack->node_allocator);
    StdAllocator_DeployAllocator(&pack->table_allocator_,
                                 &pack->table_allocator);

    pack->dht.width = sizeof(Pair);
    pack->dht.stride = sizeof(Pair);

    pack->dht.key_hash_context = NULL;
    pack->dht.KeyHash = Zeta_PairHash;

    pack->dht.elem_hash_context = NULL;
    pack->dht.ElemHash = Zeta_PairHash;

    pack->dht.elem_cmp_context = NULL;
    pack->dht.ElemCompare = Zeta_PairCompare;

    pack->dht.elem_key_cmp_context = NULL;
    pack->dht.ElemKeyCompare = Zeta_PairKeyCompare;

    pack->dht.node_allocator = &pack->node_allocator;

    pack->dht.ght.table_allocator = &pack->table_allocator;

    Zeta_DynamicHashTable_Init(&pack->dht);

    return &pack->dht;
}

void DynamicHashTable_Destroy(Zeta_DynamicHashTable* dht) {
    DynamicHashTablePack* pack{ ZETA_MemberToStruct(DynamicHashTablePack, dht,
                                                    dht) };

    Zeta_DynamicHashTable_Deinit(&pack->dht);

    delete pack;
}

void DynamicHashTable_Sanitize(Zeta_DynamicHashTable* dht) {
    Zeta_MemRecorder* table_recorder = Zeta_MemRecorder_Create();
    Zeta_MemRecorder* node_recorder = Zeta_MemRecorder_Create();

    Zeta_DynamicHashTable_Sanitize(dht, table_recorder, node_recorder);

    Zeta_MemRecorder_Destroy(table_recorder);
    Zeta_MemRecorder_Destroy(node_recorder);
}

Pair* DynamicHashTable_Find(Zeta_DynamicHashTable* dht,
                            unsigned long long key) {
    DynamicHashTable_Sanitize(dht);
    return (Pair*)(Zeta_DynamicHashTable_Find(dht, NULL, &key));
}

Pair* DynamicHashTable_Insert(Zeta_DynamicHashTable* dht,
                              unsigned long long key, unsigned long long val) {
    DynamicHashTable_Sanitize(dht);

    Pair pair{ key, val };
    return (Pair*)Zeta_DynamicHashTable_Insert(dht, NULL, &pair);
}

bool_t DynamicHashTable_Erase(Zeta_DynamicHashTable* dht,
                              unsigned long long key) {
    DynamicHashTable_Sanitize(dht);

    void* cursor =
        __builtin_alloca_with_align(sizeof(Zeta_DynamicHashTable_Cursor),
                                    CHAR_BIT * alignof(Zeta_DynamicHashTable));

    Pair* pair = (Pair*)Zeta_DynamicHashTable_Find(dht, cursor, &key);

    if (pair == NULL) { return FALSE; }

    Zeta_DynamicHashTable_Erase(dht, cursor);
    return TRUE;
}

void main1() {
    Zeta_DynamicHashTable* dht = DynamicHashTable_Create();

    Pair* pair;

    for (unsigned long long i{ 0 }; i < 1000; ++i) {
        pair = DynamicHashTable_Insert(dht, i, 1000 + i);

        ZETA_DebugAssert(pair != NULL);
        ZETA_DebugAssert(pair->key == i);
        ZETA_DebugAssert(pair->val == 1000 + i);
    }

    {
        void* iter = __builtin_alloca_with_align(
            sizeof(Zeta_DynamicHashTable_Cursor),
            CHAR_BIT * alignof(Zeta_DynamicHashTable));

        void* end = __builtin_alloca_with_align(
            sizeof(Zeta_DynamicHashTable_Cursor),
            CHAR_BIT * alignof(Zeta_DynamicHashTable));

        Zeta_DynamicHashTable_PeekL(dht, iter);
        Zeta_DynamicHashTable_GetRBCursor(dht, end);

        for (; !Zeta_DynamicHashTable_Cursor_IsEqual(dht, iter, end);
             Zeta_DynamicHashTable_Cursor_StepR(dht, iter)) {
            pair = (Pair*)Zeta_DynamicHashTable_Refer(dht, iter);
            ZETA_PrintVar(pair->key);
        }
    }

    DynamicHashTable_Destroy(dht);
}

int main() {
    printf("main start\n");
    main1();
    printf("ok\n");
    return 0;
}
