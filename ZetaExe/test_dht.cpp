#include <debug_deque.h>
#include <debugger.h>
#include <mem_check_utils.h>

#include <memory>

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

    for (unsigned long long _{ 0 }; _ < 4; ++_) {
        for (unsigned long long i{ 0 }; i < 512; ++i) {
            ZETA_PrintVar(i);

            AssocCntrUtils_SyncRandomInsert<Elem>(
                { assoc_cntr_a, assoc_cntr_b });
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
