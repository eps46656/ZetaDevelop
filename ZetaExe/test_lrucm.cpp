#include <random>
#include <unordered_map>
#include <unordered_set>

#include "caching_array_utils.h"
#include "debug_deque_utils.h"
#include "lru_cache_manager_utils.h"
#include "pod_value.h"
#include "random.h"
#include "timer.h"

void test_cm() {
    Zeta_SeqCntr* origin{ DebugDequeUtils_Create<PODValue>() };

    SeqCntrUtils_SyncRandomInit<PODValue>({ origin }, 1024 * 1024 * 16);

    Zeta_CacheManager* cm{ LRUCacheManagerUtils_Create(origin, 7) };

    Zeta_SeqCntr* seq_cntr_a{ DebugDequeUtils_Create<PODValue>() };
    Zeta_SeqCntr_Assign(seq_cntr_a, origin);

    Zeta_SeqCntr* seq_cntr_b{ CachingArrayUtils_Create(cm, 8) };

    Zeta_SeqCntr* seq_cntr_c{ CachingArrayUtils_Create(cm, 8) };

    //
};

// -----------------------------------------------------------------------------

int main() {
    unsigned long long beg_time{ GetTime() };

    unsigned long long end_time{ GetTime() };

    unsigned long long duration{ end_time - beg_time };

    ZETA_PrintVar(duration);
}
