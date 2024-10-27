#include <Debugger.h>
#include <debug_deque.h>
#include <mem_check_utils.h>

#include <memory>

#include "AssocCntrUtils.h"
#include "Random.h"
#include "Timer.h"
#include "Value.h"
#include "debug_hash_table_utils.h"
#include "dynamic_hash_table_utils.h"
#include "std_allocator.h"

int ULLCompare(unsigned long long a, unsigned long long b) {
    if (a < b) { return -1; }
    if (b < a) { return 1; }
    return 0;
}

struct Pair {
    unsigned long long key;
    unsigned long long val;
};

unsigned long long Zeta_PairHash(void* context, void const* pair,
                                 unsigned long long salt) {
    ZETA_Unused(context);
    return std::hash<unsigned long long>()(((Pair const*)pair)->key + salt);
}

unsigned long long Zeta_KeyHash(void* context, void const* key,
                                unsigned long long salt) {
    ZETA_Unused(context);
    return std::hash<unsigned long long>()(*(unsigned long long const*)key +
                                           salt);
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

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

template <typename Key>
std::shared_ptr<unsigned char> AC_Find(Zeta_AssocCntr* assoc_cntr,
                                       const Key& key) {
    void* cursor{ ZETA_AssocCntr_AllocaCursor(assoc_cntr) };
    assoc_cntr->Find(assoc_cntr->context, cursor, &key);
    return cursor;
}

template <typename Elem>
std::shared_ptr<void> AC_Insert(Zeta_AssocCntr* assoc_cntr, const Elem& elem) {
    void* cursor{ std::malloc(assoc_cntr->cursor_size) };
    ZETA_AssocCntr_Insert(assoc_cntr, &elem, cursor);
    return std::shared_ptr<void>{ (unsigned char*)cursor };
}

template <typename Elem>
void AC_Erase(Zeta_AssocCntr* assoc_cntr,
              const std::shared_ptr<unsigned char>& cursor) {
    void* cursor{ ZETA_AssocCntr_AllocaCursor(assoc_cntr) };
    ZETA_AssocCntr_Erase(assoc_cntr, cursor.get());
    return cursor;
}

void main1() {
    RandomSetSeed();

    Zeta_AssocCntr* dht = DynamicHashTable_Create<Pair>(
        NULL, Zeta_PairHash, NULL, Zeta_KeyHash, NULL, Zeta_PairCompare, NULL,
        Zeta_PairKeyCompare);

    std::shared_ptr<void> cursor;
    Pair* pair;

    for (unsigned long long _{ 0 }; _ < 4; ++_) {
        for (unsigned long long i{ 0 }; i < 1024; ++i) {
            unsigned long long key{ static_cast<unsigned long long>(
                GetRandomInt(0, 1024 * 1024)) };
            unsigned long long val{ static_cast<unsigned long long>(
                GetRandomInt(0, 1024 * 1024)) };

            cursor = AC_Insert(dht, Pair{ key, val });
            Pair* pair = (Pair*)ZETA_AssocCntr_Refer(dht, cursor.get());

            ZETA_DebugAssert(pair != NULL);
            ZETA_DebugAssert(pair->key == key);
            ZETA_DebugAssert(pair->val == val);

            AssocCntr_Sanitize(dht);

            if (i % 256 == 0) {
                ZETA_PrintVar(i);

                printf(
                    "eff factor = %e\n",
                    (double)Zeta_DynamicHashTable_GetEffFactor(dht->context) /
                        ZETA_FixedPoint_Base);
            }

            ZETA_AssocCntr_EraseAll(dht);
        }
    }

    printf("eff factor = %e\n",
           (double)Zeta_DynamicHashTable_GetEffFactor(dht->context) /
               ZETA_FixedPoint_Base);

    unsigned long long sum = 0;

    DynamicHashTable_Sanitize(dht);

    {
        void* iter = ZETA_AssocCntr_AllocaCursor(dht);
        void* end = ZETA_AssocCntr_AllocaCursor(dht);

        ZETA_AssocCntr_PeekL(dht, iter);
        ZETA_AssocCntr_GetRBCursor(dht, end);

        for (; !ZETA_AssocCntr_Cursor_AreEqual(dht, iter, end);
             ZETA_AssocCntr_Cursor_StepR(dht, iter)) {
            pair = (Pair*)ZETA_AssocCntr_Refer(dht, iter);
            sum += pair->key;
            // ZETA_PrintVar(pair->key);
        }
    }

    {
        void* iter = ZETA_AssocCntr_AllocaCursor(dht);
        void* end = ZETA_AssocCntr_AllocaCursor(dht);

        ZETA_AssocCntr_PeekR(dht, iter);
        ZETA_AssocCntr_GetLBCursor(dht, end);

        for (; !ZETA_AssocCntr_Cursor_AreEqual(dht, iter, end);
             ZETA_AssocCntr_Cursor_StepL(dht, iter)) {
            pair = (Pair*)ZETA_AssocCntr_Refer(dht, iter);
            sum += pair->key;
            // ZETA_PrintVar(pair->key);
        }
    }

    ZETA_PrintVar(sum);

    DynamicHashTable_Destroy(dht);
}

int main() {
    printf("main start\n");
    main1();
    printf("ok\n");
    return 0;
}
