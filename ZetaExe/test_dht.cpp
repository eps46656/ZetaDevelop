#include <debug_deque.h>
#include <debugger.h>
#include <mem_check_utils.h>

#include <memory>
#include <unordered_set>

#include "assoc_cntr_utils.h"
#include "debug_hash_table_utils.h"
#include "dynamic_hash_table_utils.h"
#include "pair.h"
#include "pod_value.h"
#include "random.h"
#include "std_allocator.h"
#include "timer.h"

void main1() {
    unsigned random_seed = time(NULL);
    unsigned fixed_seed = 1729615114;

    unsigned seed = random_seed;
    // unsigned seed = fixed_seed;

    ZETA_PrintCurPos;

    ZETA_PrintVar(random_seed);
    ZETA_PrintVar(fixed_seed);
    ZETA_PrintVar(seed);

    SetRandomSeed(seed);

    using Elem = Pair<unsigned long long, unsigned long long>;

    Zeta_AssocCntr* assoc_cntr_a = DebugHashTable_Create<Elem>();
    Zeta_AssocCntr* assoc_cntr_b = DynamicHashTable_Create<Elem>();

    std::shared_ptr<void> cursor;

    std::vector<Zeta_AssocCntr*> assoc_cntrs{ assoc_cntr_a, assoc_cntr_b };

    std::unordered_set<Elem> elems_s;
    std::vector<Elem> elems_v;

    auto GenerateUniqueElem{ [&] {
        Elem ret{ GetRandom<Elem>() };
        while (!elems_s.insert(ret).second) { ret = GetRandom<Elem>(); }
        elems_v.push_back(ret);
        return ret;
    } };

    auto PopRecordedElem = [&] {
        ZETA_DebugAssert(!elems_s.empty());

        size_t idx{ GetRandomInt<size_t, size_t>(0, elems_v.size() - 1) };

        auto iter{ elems_s.find(elems_v[idx]) };

        ZETA_DebugAssert(iter != elems_s.end());

        auto ret{ *iter };

        elems_s.erase(iter);

        elems_v[idx] = elems_v.back();
        elems_v.pop_back();

        return ret;
    };

    for (unsigned long long _{ 0 }; _ < 4; ++_) {
        for (unsigned long long i{ 0 }; i < 128; ++i) {
            ZETA_PrintVar(i);
            AssocCntrUtils_SyncInsert<Elem>(assoc_cntrs, GenerateUniqueElem());
            AssocCntrUtils_Equal<Elem>(assoc_cntrs);
        }

        for (size_t p{ 0 }; p < 4; ++p) {
            for (unsigned long long i{ 0 }; i < 32; ++i) {
                ZETA_PrintVar(i);
                ZETA_DebugAssert(AssocCntrUtils_SyncInsert<Elem>(
                    assoc_cntrs, GenerateUniqueElem()));
                AssocCntrUtils_Equal<Elem>(assoc_cntrs);
            }

            for (unsigned long long i{ 0 }; i < 32; ++i) {
                ZETA_PrintVar(i);
                ZETA_DebugAssert(AssocCntrUtils_SyncErase<Elem>(
                    assoc_cntrs, PopRecordedElem()));
                AssocCntrUtils_Equal<Elem>(assoc_cntrs);
            }
        }
    }

    DynamicHashTable_Destroy(assoc_cntr_a);
    DynamicHashTable_Destroy(assoc_cntr_b);
}

int main() {
    printf("main start\n");
    main1();
    printf("ok\n");
    return 0;
}
