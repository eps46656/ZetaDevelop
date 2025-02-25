#include <debug_deque.h>
#include <debugger.h>
#include <logger.h>

#include <chrono>
#include <csignal>
#include <deque>
#include <iostream>
#include <random>
#include <set>
#include <unordered_set>

#include "cascade_alloc_utils.h"
#include "circular_array_utils.h"
#include "debug_deque_utils.h"
#include "multi_level_circular_array_utils.h"
#include "pod_value.h"
#include "random.h"
#include "seg_vector_utils.h"
#include "seq_cntr_utils.h"
#include "staging_vector_utils.h"
#include "timer.h"

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

void test_seq_cntr() {
    unsigned random_seed = time(NULL);
    unsigned fixed_seed = 1735451159;

    unsigned seed = random_seed;
    // unsigned seed = fixed_seed;

    ZETA_PrintCurPos;

    ZETA_PrintVar(random_seed);
    ZETA_PrintVar(fixed_seed);
    ZETA_PrintVar(seed);

    SetRandomSeed(seed);

    Zeta_SeqCntr seq_cntr_a_origin{ DebugDequeUtils_Create<PODValue>() };

    size_t origin_size{ 1024 * 1024 };

    SeqCntrUtils_SyncRandomInit<PODValue>({ seq_cntr_a_origin }, origin_size);

    Zeta_SeqCntr seq_cntr_a{ DebugDequeUtils_Create<PODValue>() };

    Zeta_SeqCntr seq_cntr_b{ CircularArray_Create<PODValue>(
        sizeof(PODValue) * 3, 1024 * 1024) };

    /*
    Zeta_SeqCntr seq_cntr_b{ MultiLevelCircularArray_Create<PODValue>(
        sizeof(PODValue) * 3, 7) };
    */

    /*
    Zeta_SeqCntr seq_cntr_b{ SegVectorUtils_Create<PODValue>(
        sizeof(PODValue) * 3, 7) };
    */

    size_t max_op_size = 1024;

    for (size_t _ = 0; _ < 16; ++_) {
        ZETA_PrintVar(_);

        // Zeta_SeqCntr_Assign(seq_cntr_a, seq_cntr_a_origin);
        // Zeta_SeqCntr_Assign(seq_cntr_b, seq_cntr_a_origin);

        SeqCntrUtils_DoRandomOperations<PODValue>(
            { seq_cntr_a, seq_cntr_b },

            256,  // iter_cnt

            max_op_size,  // read_max_op_size
            max_op_size,  // write_max_op_size
            max_op_size,  // push_l_max_op_size
            max_op_size,  // push_r_max_op_size
            max_op_size,  // pop_l_max_op_size
            max_op_size,  // pop_r_max_op_size
            max_op_size,  // insert_max_op_size
            max_op_size,  // erase_max_op_size

            max_op_size,  // cursor_step_l_max_op_size
            max_op_size,  // cursor_step_r_max_op_size
            max_op_size,  // cursor_advance_l_op_size
            max_op_size   // cursor_advance_r_op_size
        );
    }

    SeqCntrUtils_Destroy(seq_cntr_a);
    SeqCntrUtils_Destroy(seq_cntr_b);

    SeqCntrUtils_Destroy(seq_cntr_a_origin);
}

void test_staging_vector() {
    unsigned random_seed = time(NULL);
    unsigned fixed_seed = 1729615114;

    unsigned seed = random_seed;
    // unsigned seed = fixed_seed;

    ZETA_PrintCurPos;

    ZETA_PrintVar(random_seed);
    ZETA_PrintVar(fixed_seed);
    ZETA_PrintVar(seed);

    SetRandomSeed(seed);

    Zeta_SeqCntr seq_cntr_a_origin{ DebugDequeUtils_Create<PODValue>() };

    Zeta_SeqCntr seq_cntr_b_origin{ DebugDequeUtils_Create<PODValue>() };

    size_t origin_size{ 1024 * 1024 };

    SeqCntrUtils_SyncRandomInit<PODValue>({ seq_cntr_a_origin }, origin_size);

    Zeta_SeqCntr seq_cntr_a{ DebugDequeUtils_Create<PODValue>() };

    Zeta_SeqCntr seq_cntr_b{ StagingVectorUtils_Create<PODValue>(
        seq_cntr_b_origin, sizeof(PODValue) * 3, 7) };

    size_t max_op_size = 1024;

    for (size_t _ = 0; _ < 4; ++_) {
        ZETA_PrintVar(_);

        Zeta_SeqCntr_Assign(seq_cntr_a, seq_cntr_a_origin);

        Zeta_SeqCntr_Assign(seq_cntr_b_origin, seq_cntr_a_origin);

        Zeta_StagingVector_Reset(seq_cntr_b.context);

        SeqCntrUtils_DoRandomOperations<PODValue>(
            { seq_cntr_a, seq_cntr_b },

            16,  // iter_cnt

            max_op_size,  // read_max_op_size
            max_op_size,  // write_max_op_size
            max_op_size,  // push_l_max_op_size
            max_op_size,  // push_r_max_op_size
            max_op_size,  // pop_l_max_op_size
            max_op_size,  // pop_r_max_op_size
            max_op_size,  // insert_max_op_size
            max_op_size,  // erase_max_op_size

            max_op_size,  // cursor_step_l_max_op_size
            max_op_size,  // cursor_step_r_max_op_size
            max_op_size,  // cursor_advance_l_op_size
            max_op_size   // cursor_advance_r_op_size
        );
    }

    SeqCntrUtils_Destroy(seq_cntr_a);
    SeqCntrUtils_Destroy(seq_cntr_b);

    SeqCntrUtils_Destroy(seq_cntr_a_origin);
    SeqCntrUtils_Destroy(seq_cntr_b_origin);
}

void test_staging_vector_copy() {
    unsigned random_seed = time(NULL);
    unsigned fixed_seed = 1729615114;

    unsigned seed = random_seed;
    // unsigned seed = fixed_seed;

    ZETA_PrintCurPos;

    ZETA_PrintVar(random_seed);
    ZETA_PrintVar(fixed_seed);
    ZETA_PrintVar(seed);

    SetRandomSeed(seed);

    Zeta_SeqCntr seq_cntr_a_origin{ DebugDequeUtils_Create<PODValue>() };

    Zeta_SeqCntr seq_cntr_b_origin{ DebugDequeUtils_Create<PODValue>() };

    Zeta_SeqCntr seq_cntr_c_origin{ DebugDequeUtils_Create<PODValue>() };

    size_t origin_size{ 1024 * 1024 };

    SeqCntrUtils_SyncRandomInit<PODValue>({ seq_cntr_a_origin }, origin_size);

    Zeta_SeqCntr seq_cntr_a{ DebugDequeUtils_Create<PODValue>() };

    Zeta_SeqCntr seq_cntr_b{ StagingVectorUtils_Create<PODValue>(
        seq_cntr_b_origin, sizeof(PODValue) * 3, 7) };

    Zeta_SeqCntr seq_cntr_c{ StagingVectorUtils_Create<PODValue>(
        seq_cntr_c_origin, sizeof(PODValue) * 3, 7) };

    for (size_t _ = 0; _ < 16; ++_) {
        ZETA_PrintVar(_);

        Zeta_SeqCntr_Assign(seq_cntr_a_origin, seq_cntr_a);
        Zeta_SeqCntr_Assign(seq_cntr_b_origin, seq_cntr_a);
        Zeta_SeqCntr_Assign(seq_cntr_c_origin, seq_cntr_a);

        Zeta_StagingVector_Reset(seq_cntr_b.context);
        Zeta_StagingVector_Reset(seq_cntr_c.context);

        SeqCntrUtils_DoRandomOperations<PODValue>(
            { seq_cntr_a, seq_cntr_b },

            512,  // iter_cnt

            16,  // read_max_op_size
            16,  // write_max_op_size
            16,  // push_l_max_op_size
            16,  // push_r_max_op_size
            16,  // pop_l_max_op_size
            16,  // pop_r_max_op_size
            16,  // insert_max_op_size
            16,  // erase_max_op_size

            16,  // cursor_step_l_max_op_size
            16,  // cursor_step_r_max_op_size
            16,  // cursor_advance_l_op_size
            16   // cursor_advance_r_op_size
        );

        SeqCntrUtils_DoRandomOperations<PODValue>(
            { seq_cntr_c },

            512,  // iter_cnt

            16,  // read_max_op_size
            16,  // write_max_op_size
            16,  // push_l_max_op_size
            16,  // push_r_max_op_size
            16,  // pop_l_max_op_size
            16,  // pop_r_max_op_size
            16,  // insert_max_op_size
            16,  // erase_max_op_size

            16,  // cursor_step_l_max_op_size
            16,  // cursor_step_r_max_op_size
            16,  // cursor_advance_l_op_size
            16   // cursor_advance_r_op_size
        );

        Zeta_StagingVector_Copy(seq_cntr_c.context, seq_cntr_b.context);

        SeqCntrUtils_Sanitize(seq_cntr_b);
        SeqCntrUtils_Sanitize(seq_cntr_c);

        SeqCntrUtils_DoRandomOperations<PODValue>(
            { seq_cntr_a, seq_cntr_c },

            512,  // iter_cnt

            16,  // read_max_op_size
            16,  // write_max_op_size
            16,  // push_l_max_op_size
            16,  // push_r_max_op_size
            16,  // pop_l_max_op_size
            16,  // pop_r_max_op_size
            16,  // insert_max_op_size
            16,  // erase_max_op_size

            16,  // cursor_step_l_max_op_size
            16,  // cursor_step_r_max_op_size
            16,  // cursor_advance_l_op_size
            16   // cursor_advance_r_op_size
        );
    }

    SeqCntrUtils_Destroy(seq_cntr_a);
    SeqCntrUtils_Destroy(seq_cntr_b);
    SeqCntrUtils_Destroy(seq_cntr_c);

    SeqCntrUtils_Destroy(seq_cntr_a_origin);
    SeqCntrUtils_Destroy(seq_cntr_b_origin);
    SeqCntrUtils_Destroy(seq_cntr_c_origin);
}

void test_staging_vector_collapse() {
    unsigned random_seed = time(NULL);
    unsigned fixed_seed = 1729615114;

    unsigned seed = random_seed;
    // unsigned seed = fixed_seed;

    ZETA_PrintCurPos;

    ZETA_PrintVar(random_seed);
    ZETA_PrintVar(fixed_seed);
    ZETA_PrintVar(seed);

    SetRandomSeed(seed);

    /*

    seq_cntr_base -> seq_cntr_b -> seq_cntr_c

    seq_cntr_a

    collapse

    seq_cntr_base -> seq_cntr_b

    seq_cntr_base -> seq_cntr_c

    seq_cntr_a

    */

    Zeta_SeqCntr seq_cntr_base{ DebugDequeUtils_Create<PODValue>() };

    size_t origin_size{ 1024 * 1024 };

    SeqCntrUtils_SyncRandomInit<PODValue>({ seq_cntr_base }, origin_size);

    Zeta_SeqCntr seq_cntr_a{ DebugDequeUtils_Create<PODValue>() };

    Zeta_SeqCntr seq_cntr_b{ StagingVectorUtils_Create<PODValue>(
        seq_cntr_base, sizeof(PODValue) * 3, 7) };

    for (size_t _ = 0; _ < 16; ++_) {
        ZETA_PrintVar(_);

        Zeta_SeqCntr_Assign(seq_cntr_a, seq_cntr_base);

        Zeta_StagingVector_Reset(seq_cntr_b.context);

        SeqCntrUtils_DoRandomOperations<PODValue>(
            { seq_cntr_a, seq_cntr_b },

            16,  // iter_cnt

            16,  // read_max_op_size
            16,  // write_max_op_size
            16,  // push_l_max_op_size
            16,  // push_r_max_op_size
            16,  // pop_l_max_op_size
            16,  // pop_r_max_op_size
            16,  // insert_max_op_size
            16,  // erase_max_op_size

            16,  // cursor_step_l_max_op_size
            16,  // cursor_step_r_max_op_size
            16,  // cursor_advance_l_op_size
            16   // cursor_advance_r_op_size
        );

        ZETA_PrintCurPos;

        Zeta_SeqCntr seq_cntr_c{ StagingVectorUtils_Create<PODValue>(
            seq_cntr_b, sizeof(PODValue) * 3, 7) };

        ZETA_PrintCurPos;

        SeqCntrUtils_DoRandomOperations<PODValue>(
            { seq_cntr_a, seq_cntr_c },

            16,  // iter_cnt

            16,  // read_max_op_size
            16,  // write_max_op_size
            16,  // push_l_max_op_size
            16,  // push_r_max_op_size
            16,  // pop_l_max_op_size
            16,  // pop_r_max_op_size
            16,  // insert_max_op_size
            16,  // erase_max_op_size

            16,  // cursor_step_l_max_op_size
            16,  // cursor_step_r_max_op_size
            16,  // cursor_advance_l_op_size
            16   // cursor_advance_r_op_size
        );

        ZETA_PrintCurPos;

        Zeta_StagingVector_Collapse(seq_cntr_c.context);

        ZETA_PrintCurPos;

        SeqCntrUtils_Sanitize(seq_cntr_b);
        SeqCntrUtils_Sanitize(seq_cntr_c);

        ZETA_PrintCurPos;

        SeqCntrUtils_DoRandomOperations<PODValue>(
            { seq_cntr_a, seq_cntr_c },

            16,  // iter_cnt

            16,  // read_max_op_size
            16,  // write_max_op_size
            16,  // push_l_max_op_size
            16,  // push_r_max_op_size
            16,  // pop_l_max_op_size
            16,  // pop_r_max_op_size
            16,  // insert_max_op_size
            16,  // erase_max_op_size

            16,  // cursor_step_l_max_op_size
            16,  // cursor_step_r_max_op_size
            16,  // cursor_advance_l_op_size
            16   // cursor_advance_r_op_size
        );

        SeqCntrUtils_Destroy(seq_cntr_c);
    }

    SeqCntrUtils_Destroy(seq_cntr_a);
    SeqCntrUtils_Destroy(seq_cntr_b);

    SeqCntrUtils_Destroy(seq_cntr_base);
}

void test_staging_vector_write_back() {
    unsigned random_seed = time(NULL);
    unsigned fixed_seed = 1735990806;

    unsigned seed = random_seed;
    // unsigned seed = fixed_seed;

    ZETA_PrintCurPos;

    ZETA_PrintVar(random_seed);
    ZETA_PrintVar(fixed_seed);
    ZETA_PrintVar(seed);

    SetRandomSeed(seed);

    InitCascadeAllocator();

    Zeta_SeqCntr seq_cntr_base{ DebugDequeUtils_Create<PODValue>() };

    Zeta_SeqCntr seq_cntr_a_origin{ DebugDequeUtils_Create<PODValue>() };

    Zeta_SeqCntr seq_cntr_a{ StagingVectorUtils_Create<PODValue>(
        seq_cntr_a_origin, sizeof(PODValue) * 3, 7) };

    Zeta_SeqCntr seq_cntr_b{ DebugDequeUtils_Create<PODValue>() };

    size_t origin_size{ 1024 * 1024 };

    SeqCntrUtils_SyncRandomInit<PODValue>({ seq_cntr_base }, origin_size);

    for (size_t _ = 0; _ < 16; ++_) {
        ZETA_PrintVar(_);

        Zeta_SeqCntr_Assign(seq_cntr_a_origin, seq_cntr_base);

        Zeta_SeqCntr_Assign(seq_cntr_b, seq_cntr_base);

        Zeta_StagingVector_Reset(seq_cntr_a.context);

        SeqCntrUtils_DoRandomOperations<PODValue>(
            { seq_cntr_a, seq_cntr_b },

            256,  // iter_cnt

            16,  // read_max_op_size
            16,  // write_max_op_size
            16,  // push_l_max_op_size
            16,  // push_r_max_op_size
            16,  // pop_l_max_op_size
            16,  // pop_r_max_op_size
            16,  // insert_max_op_size
            16,  // erase_max_op_size

            16,  // cursor_step_l_max_op_size
            16,  // cursor_step_r_max_op_size
            16,  // cursor_advance_l_op_size
            16   // cursor_advance_r_op_size
        );

        Zeta_StagingVector_WriteBack(seq_cntr_a.context,
                                     ZETA_StagingVector_WriteBackStrategy_LR,
                                     1,   // cost_coeff_read
                                     2,   // cost_coeff_write
                                     10,  // cost_coeff_insert
                                     10   // cost_coeff_erase
        );

        SeqCntrUtils_SyncCompare<PODValue>({ seq_cntr_a_origin, seq_cntr_b });
    }

    SeqCntrUtils_Destroy(seq_cntr_base);

    SeqCntrUtils_Destroy(seq_cntr_a);
    SeqCntrUtils_Destroy(seq_cntr_b);

    SeqCntrUtils_Destroy(seq_cntr_a_origin);
}

int main() {
    printf("main start\n");

    ZETA_PrintCurPos;

    unsigned long long beg_time{ GetTime() };

    test_staging_vector_copy();

    unsigned long long end_time{ GetTime() };

    printf("ok\a\n");

    std::cout << "Time difference = " << end_time - beg_time << "[ms]\n";

    /*

    DD: 4073[ms]

    RadixDeque: 325[ms]

    RadixVector: 233[ms]

    */

    return 0;
}
