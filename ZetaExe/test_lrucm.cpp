#include <cascade_allocator.h>

#include <random>
#include <unordered_map>
#include <unordered_set>

#include "caching_array_utils.h"
#include "cascade_alloc_utils.h"
#include "debug_deque_utils.h"
#include "lru_cache_manager_utils.h"
#include "pod_value.h"
#include "random.h"
#include "timer.h"

byte_t mem[128 * 1024 * 1024];

Zeta_CascadeAllocator allocator;

void test_cm() {
    unsigned random_seed = time(NULL);
    unsigned fixed_seed = 1735451159;

    unsigned seed = random_seed;
    // unsigned seed = fixed_seed;

    ZETA_PrintCurPos;

    ZETA_PrintVar(random_seed);
    ZETA_PrintVar(fixed_seed);
    ZETA_PrintVar(seed);

    SetRandomSeed(seed);

    InitCascadeAllocator();

    Zeta_SeqCntr* origin{ DebugDequeUtils_Create<PODValue>() };

    SeqCntrUtils_SyncRandomInit<PODValue>({ origin }, 1024 * 256);

    Zeta_CacheManager* cm{ LRUCacheManagerUtils_Create(origin, 7) };

    Zeta_SeqCntr* seq_cntr_a{ DebugDequeUtils_Create<PODValue>() };
    Zeta_SeqCntr_Assign(seq_cntr_a, origin);

    ZETA_PrintCurPos;

    Zeta_SeqCntr* seq_cntr_b{ CachingArrayUtils_Create(cm, 8) };

    ZETA_PrintCurPos;

    Zeta_SeqCntr* seq_cntr_c{ CachingArrayUtils_Create(cm, 8) };

    size_t max_op_size = 1024;

    ZETA_PrintCurPos;

    for (size_t _ = 0; _ < 16; ++_) {
        ZETA_PrintVar(_);

        // Zeta_SeqCntr_Assign(seq_cntr_a, seq_cntr_a_origin);
        // Zeta_SeqCntr_Assign(seq_cntr_b, seq_cntr_a_origin);

        SeqCntrUtils_DoRandomOperations<PODValue>(
            { seq_cntr_a, seq_cntr_b },

            256,  // iter_cnt

            max_op_size,  // read_max_op_size
            max_op_size,  // write_max_op_size
            0,            // push_l_max_op_size
            0,            // push_r_max_op_size
            0,            // pop_l_max_op_size
            0,            // pop_r_max_op_size
            0,            // insert_max_op_size
            0,            // erase_max_op_size

            max_op_size,  // cursor_step_l_max_op_size
            max_op_size,  // cursor_step_r_max_op_size
            max_op_size,  // cursor_advance_l_op_size
            max_op_size   // cursor_advance_r_op_size
        );

        SeqCntrUtils_SyncCompare<PODValue>({ seq_cntr_a, seq_cntr_c });
    }
};

// -----------------------------------------------------------------------------

int main() {
    unsigned long long beg_time{ GetTime() };

    test_cm();

    unsigned long long end_time{ GetTime() };

    unsigned long long duration{ end_time - beg_time };

    ZETA_PrintVar(duration);
}
