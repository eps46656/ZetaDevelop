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

#include "buffer.h"
#include "circular_array_utils.h"
#include "debug_deque_utils.h"
#include "dynamic_vector_utils.h"
#include "pod_value.h"
#include "seq_cntr_utils.h"
#include "stage_vector_utils.h"
#include "timer.h"

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

std::mt19937_64& RandomEngine() {
    static std::mt19937_64 en;
    return en;
}

long long GetRandomInt(long long lb, long long rb) {
    static std::uniform_int_distribution<long long> ll_generator{
        0, ZETA_RangeMaxOf(long long)
    };

    ZETA_DebugAssert(lb <= rb);

    return ll_generator(RandomEngine()) % (rb - lb + 1) + lb;
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

void SignalHandler(int sig) {
    std::cout << "sig: " << sig << "\n";

    Zeta_Debugger_FlushPipe();
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

void GetRandomVal(PODValue& dst) {
    for (size_t i{ 0 }; i < PODValue::width; ++i) {
        dst.data[i] = (unsigned char)GetRandomInt(0, 255);
    }
}

template <typename Iterator>
void GetRandomVals(Iterator beg, Iterator end) {
    for (; beg != end; ++beg) { GetRandomVal(*beg); }
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

#define SEG_CAPACITY (32)

void SeqCntrUtils_SanitizeIterator(Zeta_SeqCntr* seq_cntr, size_t idx_a,
                                   size_t idx_b) {
    void* cursor_a = ZETA_SeqCntr_AllocaCursor(seq_cntr);
    void* cursor_b = ZETA_SeqCntr_AllocaCursor(seq_cntr);

    size_t diff = idx_b - idx_a;

    ZETA_SeqCntr_Access(seq_cntr, idx_a, cursor_a, NULL);
    ZETA_SeqCntr_Access(seq_cntr, idx_b, cursor_b, NULL);

    ZETA_DebugAssert(ZETA_SeqCntr_Cursor_GetIdx(seq_cntr, cursor_a) == idx_a);
    ZETA_DebugAssert(ZETA_SeqCntr_Cursor_GetIdx(seq_cntr, cursor_b) == idx_b);

    ZETA_SeqCntr_Cursor_AdvanceR(seq_cntr, cursor_a, diff);

    ZETA_DebugAssert(ZETA_SeqCntr_Cursor_GetIdx(seq_cntr, cursor_a) == idx_b);

    ZETA_DebugAssert(
        ZETA_SeqCntr_Cursor_AreEqual(seq_cntr, cursor_a, cursor_b));

    ZETA_SeqCntr_Access(seq_cntr, idx_a, cursor_a, NULL);
    ZETA_SeqCntr_Access(seq_cntr, idx_b, cursor_b, NULL);

    ZETA_DebugAssert(ZETA_SeqCntr_Cursor_GetIdx(seq_cntr, cursor_a) == idx_a);
    ZETA_DebugAssert(ZETA_SeqCntr_Cursor_GetIdx(seq_cntr, cursor_b) == idx_b);

    ZETA_SeqCntr_Cursor_AdvanceL(seq_cntr, cursor_b, diff);

    ZETA_DebugAssert(ZETA_SeqCntr_Cursor_GetIdx(seq_cntr, cursor_b) == idx_a);

    ZETA_DebugAssert(
        ZETA_SeqCntr_Cursor_AreEqual(seq_cntr, cursor_a, cursor_b));
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

size_t GetRandomIdx(size_t size) { return GetRandomInt(-1, size); }

size_t GetRandomInsertIdx(size_t size) { return GetRandomInt(0, size); }

std::pair<size_t, size_t> GetRandomInsertIdxCnt(size_t size_) {
    long long size{ static_cast<long long>(size_) };

    double size_a{ 0 };
    double cnt_avg_a{ 64 };

    double size_b{ 4 * 1024 };
    double cnt_avg_b{ 16 };

    long long min_cnt{ 8 };
    long long max_cnt{ 64 };

    double range{ 8 };

    double avg{ cnt_avg_a * (size - size_b) / (size_a - size_b) +
                cnt_avg_b * (size - size_a) / (size_b - size_a) };

    long long lb{ static_cast<long long>(std::floor(avg - range)) };
    lb = std::max(min_cnt, lb);
    lb = std::min(max_cnt, lb);

    long long rb{ static_cast<long long>(std::ceil(avg + range)) };
    rb = std::max(min_cnt, rb);
    rb = std::min(max_cnt, rb);

    /*
    (cnt_avg_b - cnt_avg_a) / (size_b - size_a) * (size - size_a) + cnt_avg_a
    */

    long long cnt{ GetRandomInt(lb, rb) };

    // base_avg + alpha * (size - balance_size)

    long long idx{ GetRandomInt(0, size) };

    return { static_cast<size_t>(idx), static_cast<size_t>(cnt) };
}

std::pair<size_t, size_t> GetRandomEraseIdxCnt(size_t size_) {
    long long size{ static_cast<long long>(size_) };

    double size_a{ 0 };
    double cnt_avg_a{ 0 };

    double size_b{ 4 * 1024 };
    double cnt_avg_b{ 64 };

    long long min_cnt{ 0 };
    long long max_cnt{ std::min(64LL, size) };

    double range{ 8 };

    double avg{ cnt_avg_a * (size - size_b) / (size_a - size_b) +
                cnt_avg_b * (size - size_a) / (size_b - size_a) };

    long long lb{ std::max(min_cnt,
                           static_cast<long long>(std::floor(avg - range))) };

    long long rb{ std::min(max_cnt,
                           static_cast<long long>(std::ceil(avg + range))) };

    long long cnt{ GetRandomInt(lb, rb) };

    /*

    1   0 ~ size - 1        size
    2   0 ~ size - 2        size - 1

    */

    long long idx{ GetRandomInt(0, size - cnt) };

    return { static_cast<size_t>(idx), static_cast<size_t>(cnt) };
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

void Compare(Zeta_SeqCntr* seq_cntr_a, Zeta_SeqCntr* seq_cntr_b) {
    if (seq_cntr_a == NULL || seq_cntr_b == NULL) { return; }

    size_t size_a{ ZETA_SeqCntr_GetSize(seq_cntr_a) };
    size_t size_b{ ZETA_SeqCntr_GetSize(seq_cntr_b) };

    ZETA_DebugAssert(size_a == size_b);

    void* cursor_a = ZETA_SeqCntr_AllocaCursor(seq_cntr_a);
    void* cursor_b = ZETA_SeqCntr_AllocaCursor(seq_cntr_b);

    ZETA_SeqCntr_PeekL(seq_cntr_a, cursor_a, NULL);

    ZETA_SeqCntr_PeekL(seq_cntr_b, cursor_b, NULL);

    PODValue tmp_a;
    PODValue tmp_b;

    for (size_t i{ 0 }; i < size_a; ++i) {
        ZETA_DebugAssert(ZETA_SeqCntr_Cursor_GetIdx(seq_cntr_a, cursor_a) == i);

        ZETA_DebugAssert(ZETA_SeqCntr_Cursor_GetIdx(seq_cntr_b, cursor_b) == i);

        ZETA_SeqCntr_Read(seq_cntr_a, cursor_a, 1, &tmp_a, cursor_a);

        ZETA_SeqCntr_Read(seq_cntr_b, cursor_b, 1, &tmp_b, cursor_b);

        ZETA_DebugAssert(tmp_a == tmp_b);
    }

    ZETA_DebugAssert(ZETA_SeqCntr_Cursor_GetIdx(seq_cntr_a, cursor_a) ==
                     size_a);

    ZETA_DebugAssert(ZETA_SeqCntr_Cursor_GetIdx(seq_cntr_b, cursor_b) ==
                     size_b);

    void* rb_a = ZETA_SeqCntr_AllocaCursor(seq_cntr_a);
    void* rb_b = ZETA_SeqCntr_AllocaCursor(seq_cntr_b);

    ZETA_SeqCntr_GetRBCursor(seq_cntr_a, rb_a);

    ZETA_SeqCntr_GetRBCursor(seq_cntr_b, rb_b);

    ZETA_DebugAssert(ZETA_SeqCntr_Cursor_AreEqual(seq_cntr_a, cursor_a, rb_a));

    ZETA_DebugAssert(ZETA_SeqCntr_Cursor_AreEqual(seq_cntr_b, cursor_b, rb_b));
}

void CheckCursor(Zeta_SeqCntr* seq_cntr, size_t idx_a_, size_t idx_b_) {
    void* cursor_a = ZETA_SeqCntr_AllocaCursor(seq_cntr);
    void* cursor_b = ZETA_SeqCntr_AllocaCursor(seq_cntr);

    size_t idx_a = std::min(idx_a_ + 1, idx_b_ + 1) - 1;
    size_t idx_b = std::max(idx_a_ + 1, idx_b_ + 1) - 1;
    size_t diff = idx_b - idx_a;

    ZETA_SeqCntr_Access(seq_cntr, idx_a, cursor_a, NULL);

    ZETA_SeqCntr_Access(seq_cntr, idx_b, cursor_b, NULL);

    ZETA_DebugAssert(ZETA_SeqCntr_Cursor_GetIdx(seq_cntr, cursor_a) == idx_a);

    ZETA_DebugAssert(ZETA_SeqCntr_Cursor_GetIdx(seq_cntr, cursor_b) == idx_b);

    ZETA_SeqCntr_Cursor_AdvanceR(seq_cntr, cursor_a, diff);

    ZETA_DebugAssert(ZETA_SeqCntr_Cursor_GetIdx(seq_cntr, cursor_a) == idx_b);

    ZETA_SeqCntr_Cursor_AdvanceL(seq_cntr, cursor_b, diff);

    ZETA_DebugAssert(ZETA_SeqCntr_Cursor_GetIdx(seq_cntr, cursor_b) == idx_a);
}

void CheckCursor(Zeta_SeqCntr* seq_cntr) {
    if (seq_cntr == NULL) { return; }

    size_t size{ ZETA_SeqCntr_GetSize(seq_cntr) };

    size_t test_num{ static_cast<size_t>(std::sqrt(size)) };
    test_num = std::max(static_cast<size_t>(16), test_num);
    test_num = std::min(static_cast<size_t>(48), test_num);

    for (size_t test_i{ 0 }; test_i < test_num; ++test_i) {
        size_t idx_a = GetRandomIdx(size);
        size_t idx_b = GetRandomIdx(size);

        CheckCursor(seq_cntr, idx_a, idx_b);
    }

    CheckCursor(seq_cntr, -1, size);
    CheckCursor(seq_cntr, -1, GetRandomIdx(size));
    CheckCursor(seq_cntr, GetRandomIdx(size), size);
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

PODValue* GetValBuffer_A(size_t size) {
    static Buffer<PODValue> buffer;
    return buffer.GetBuffer(size);
}

PODValue* GetValBuffer_B(size_t size) {
    static Buffer<PODValue> buffer;
    return buffer.GetBuffer(size);
}

void SyncRead(Zeta_SeqCntr* seq_cntr_a, Zeta_SeqCntr* seq_cntr_b,
              size_t max_op_size) {
    if (seq_cntr_a == NULL || seq_cntr_b == NULL) { return; }

    ZETA_DebugAssert(0 < max_op_size);

    long long size{ static_cast<long long>(
        SeqCntrUtils_SyncGetSize(seq_cntr_a, seq_cntr_b)) };

    if (size == 0) { return; }

    long long cnt_lb{ 1 };
    long long cnt_rb{ std::min<long long>(max_op_size, size) };

    if (cnt_rb < cnt_lb) { return; }

    long long idx{ GetRandomInt(0, size - 1) };
    long long cnt{ GetRandomInt(cnt_lb, cnt_rb) };

    PODValue* buffer_a{ GetValBuffer_A(cnt) };
    PODValue* buffer_b{ GetValBuffer_B(cnt) };

    if (seq_cntr_a != NULL) {
        SeqCntrUtils_Read(seq_cntr_a, idx, buffer_a, buffer_a + cnt);
    }

    if (seq_cntr_b != NULL) {
        SeqCntrUtils_Read(seq_cntr_b, idx, buffer_b, buffer_b + cnt);
    }

    ZETA_DebugAssert(std::memcmp(buffer_a, buffer_b, sizeof(PODValue) * cnt) ==
                     0);
}

void SyncWrite(Zeta_SeqCntr* seq_cntr_a, Zeta_SeqCntr* seq_cntr_b,
               size_t max_op_size) {
    if (seq_cntr_a == NULL && seq_cntr_b == NULL) { return; }

    ZETA_DebugAssert(0 < max_op_size);

    long long size{ static_cast<long long>(
        SeqCntrUtils_SyncGetSize(seq_cntr_a, seq_cntr_b)) };

    if (size == 0) { return; }

    long long cnt_lb{ 0 };
    long long cnt_rb{ std::min<long long>(max_op_size, size) };

    long long idx{ GetRandomInt(0, size - 1) };
    long long cnt{ GetRandomInt(cnt_lb, cnt_rb) };

    PODValue* buffer_a{ GetValBuffer_A(cnt) };

    GetRandomVals(buffer_a, buffer_a + cnt);

    if (seq_cntr_a != NULL) {
        SeqCntrUtils_Write(seq_cntr_a, idx, buffer_a, buffer_a + cnt);
    }

    if (seq_cntr_b != NULL) {
        SeqCntrUtils_Write(seq_cntr_b, idx, buffer_a, buffer_a + cnt);
    }
}

void SyncPushL(Zeta_SeqCntr* seq_cntr_a, Zeta_SeqCntr* seq_cntr_b,
               size_t max_op_size) {
    if (seq_cntr_a == NULL && seq_cntr_b == NULL) { return; }

    ZETA_DebugAssert(0 < max_op_size);

    long long cnt_lb{ 0 };
    long long cnt_rb{ static_cast<long long>(max_op_size) };

    size_t cnt{ static_cast<size_t>(GetRandomInt(cnt_lb, cnt_rb)) };

    PODValue* buffer_a{ GetValBuffer_A(cnt) };

    GetRandomVals(buffer_a, buffer_a + cnt);

    if (seq_cntr_a != NULL) {
        SeqCntrUtils_PushL(seq_cntr_a, buffer_a, buffer_a + cnt);
    }

    if (seq_cntr_b != NULL) {
        SeqCntrUtils_PushL(seq_cntr_b, buffer_a, buffer_a + cnt);
    }
}

void SyncPushR(Zeta_SeqCntr* seq_cntr_a, Zeta_SeqCntr* seq_cntr_b,
               size_t max_op_size) {
    if (seq_cntr_a == NULL && seq_cntr_b == NULL) { return; }

    ZETA_DebugAssert(0 < max_op_size);

    long long cnt_lb{ 0 };
    long long cnt_rb{ static_cast<long long>(max_op_size) };

    size_t cnt{ static_cast<size_t>(GetRandomInt(cnt_lb, cnt_rb)) };

    PODValue* buffer_a{ GetValBuffer_A(cnt) };

    GetRandomVals(buffer_a, buffer_a + cnt);

    if (seq_cntr_a != NULL) {
        SeqCntrUtils_PushR(seq_cntr_a, buffer_a, buffer_a + cnt);
    }

    if (seq_cntr_b != NULL) {
        SeqCntrUtils_PushR(seq_cntr_b, buffer_a, buffer_a + cnt);
    }
}

void SyncPopL(Zeta_SeqCntr* seq_cntr_a, Zeta_SeqCntr* seq_cntr_b,
              size_t max_op_size) {
    if (seq_cntr_a == NULL && seq_cntr_b == NULL) { return; }

    ZETA_DebugAssert(0 < max_op_size);

    long long size{ static_cast<long long>(
        SeqCntrUtils_SyncGetSize(seq_cntr_a, seq_cntr_b)) };

    long long cnt_lb{ 0 };
    long long cnt_rb{ std::min<long long>(max_op_size, size) };

    size_t cnt{ static_cast<size_t>(GetRandomInt(cnt_lb, cnt_rb)) };

    if (seq_cntr_a != NULL) { SeqCntrUtils_PopL(seq_cntr_a, cnt); }
    if (seq_cntr_b != NULL) { SeqCntrUtils_PopL(seq_cntr_b, cnt); }
}

void SyncPopR(Zeta_SeqCntr* seq_cntr_a, Zeta_SeqCntr* seq_cntr_b,
              size_t max_op_size) {
    if (seq_cntr_a == NULL && seq_cntr_b == NULL) { return; }

    ZETA_DebugAssert(0 < max_op_size);

    long long size{ static_cast<long long>(
        SeqCntrUtils_SyncGetSize(seq_cntr_a, seq_cntr_b)) };

    long long cnt_lb{ 0 };
    long long cnt_rb{ std::min<long long>(max_op_size, size) };

    size_t cnt{ static_cast<size_t>(GetRandomInt(cnt_lb, cnt_rb)) };

    if (seq_cntr_a != NULL) { SeqCntrUtils_PopR(seq_cntr_a, cnt); }
    if (seq_cntr_b != NULL) { SeqCntrUtils_PopR(seq_cntr_b, cnt); }
}

void SyncInsert(Zeta_SeqCntr* seq_cntr_a, Zeta_SeqCntr* seq_cntr_b,
                size_t max_op_size) {
    if (seq_cntr_a == NULL && seq_cntr_b == NULL) { return; }

    ZETA_DebugAssert(0 < max_op_size);

    size_t size{ SeqCntrUtils_SyncGetSize(seq_cntr_a, seq_cntr_b) };

    size_t cnt_lb{ 0 };
    size_t cnt_rb{ max_op_size };

    size_t idx{ static_cast<size_t>(GetRandomInt(0, size)) };
    size_t cnt{ static_cast<size_t>(GetRandomInt(cnt_lb, cnt_rb)) };

    PODValue* buffer_a{ GetValBuffer_A(cnt) };

    GetRandomVals(buffer_a, buffer_a + cnt);

    ZETA_DebugLogCurPos;

    if (seq_cntr_a != NULL) {
        SeqCntrUtils_Insert<PODValue>(seq_cntr_a, idx, buffer_a,
                                      buffer_a + cnt);
    }

    ZETA_DebugLogCurPos;

    if (seq_cntr_b != NULL) {
        SeqCntrUtils_Insert<PODValue>(seq_cntr_b, idx, buffer_a,
                                      buffer_a + cnt);
    }

    ZETA_DebugLogCurPos;
}

void SyncErase(Zeta_SeqCntr* seq_cntr_a, Zeta_SeqCntr* seq_cntr_b,
               size_t max_op_size) {
    if (seq_cntr_a == NULL && seq_cntr_b == NULL) { return; }

    ZETA_DebugAssert(0 < max_op_size);

    long long size{ static_cast<long long>(
        SeqCntrUtils_SyncGetSize(seq_cntr_a, seq_cntr_b)) };

    long long cnt_lb{ 0 };
    long long cnt_rb{ std::min<long long>(max_op_size, size) };

    long long cnt{ GetRandomInt(cnt_lb, cnt_rb) };
    long long idx{ GetRandomInt(0, size - cnt) };

    ZETA_DebugLogCurPos;

    if (seq_cntr_a != NULL) { SeqCntrUtils_Erase(seq_cntr_a, idx, cnt); }

    ZETA_DebugLogCurPos;

    if (seq_cntr_b != NULL) { SeqCntrUtils_Erase(seq_cntr_b, idx, cnt); }

    ZETA_DebugLogCurPos;
}

void SyncCompare(Zeta_SeqCntr* seq_cntr_a, Zeta_SeqCntr* seq_cntr_b) {
    for (size_t i{ 0 }; i < 8; ++i) { SyncRead(seq_cntr_a, seq_cntr_b, 16); }
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

enum OpEnum : int {
    READ,
    WRITE,
    PUSH_L,
    PUSH_R,
    POP_L,
    POP_R,
    INSERT,
    ERASE,

    CURSOR_STEP_L,
    CURSOR_STEP_R,
    CURSOR_ADVANCE_L,
    CURSOR_ADVANCE_R,
};

void DoRandomOperations(Zeta_SeqCntr* seq_cntr_a, Zeta_SeqCntr* seq_cntr_b,
                        size_t iter_cnt,  //

                        size_t read_max_op_size,   //
                        size_t write_max_op_size,  //
                        size_t push_l_max_op_size,
                        size_t push_r_max_op_size,  //
                        size_t pop_l_max_op_size,   //
                        size_t pop_r_max_op_size,   //
                        size_t insert_max_op_size,  //
                        size_t erase_max_op_size,   //

                        size_t cursor_step_l_max_op_size,  //
                        size_t cursor_step_r_max_op_size,  //
                        size_t cursor_advance_l_op_size,   //
                        size_t cursor_advance_r_op_size    //
) {
    ZETA_Unused(cursor_step_l_max_op_size);
    ZETA_Unused(cursor_step_r_max_op_size);
    ZETA_Unused(cursor_advance_l_op_size);
    ZETA_Unused(cursor_advance_r_op_size);

    std::vector<int> ops;

    if (0 < read_max_op_size) { ops.push_back(OpEnum::READ); }
    if (0 < write_max_op_size) { ops.push_back(OpEnum::WRITE); }
    if (0 < push_l_max_op_size) { ops.push_back(OpEnum::PUSH_L); }
    if (0 < push_r_max_op_size) { ops.push_back(OpEnum::PUSH_R); }
    if (0 < pop_l_max_op_size) { ops.push_back(OpEnum::POP_L); }
    if (0 < pop_r_max_op_size) { ops.push_back(OpEnum::POP_R); }
    if (0 < insert_max_op_size) { ops.push_back(OpEnum::INSERT); }
    if (0 < erase_max_op_size) { ops.push_back(OpEnum::ERASE); }

    ZETA_DebugAssert(!ops.empty());

    for (size_t iter_i{ 0 }; iter_i < iter_cnt; ++iter_i) {
        ZETA_PrintVar(iter_i);

        switch (GetRandomInt(0, ops.size() - 1)) {
            case OpEnum::READ:
                SyncRead(seq_cntr_a, seq_cntr_b, read_max_op_size);
                break;

            case OpEnum::WRITE:
                SyncWrite(seq_cntr_a, seq_cntr_b, write_max_op_size);
                break;

            case OpEnum::PUSH_L:
                SyncPushL(seq_cntr_a, seq_cntr_b, push_l_max_op_size);
                break;

            case OpEnum::PUSH_R:
                SyncPushR(seq_cntr_a, seq_cntr_b, push_r_max_op_size);
                break;

            case OpEnum::POP_L:
                SyncPopL(seq_cntr_a, seq_cntr_b, pop_l_max_op_size);
                break;

            case OpEnum::POP_R:
                SyncPopR(seq_cntr_a, seq_cntr_b, pop_r_max_op_size);
                break;

            case OpEnum::INSERT:
                SyncInsert(seq_cntr_a, seq_cntr_b, insert_max_op_size);
                break;

            case OpEnum::ERASE:
                SyncErase(seq_cntr_a, seq_cntr_b, erase_max_op_size);
                break;
        }

        Compare(seq_cntr_a, seq_cntr_b);
        CheckCursor(seq_cntr_a);
        CheckCursor(seq_cntr_b);

        Zeta_Debugger_ClearPipe();
    }
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

void PrintSeqCntr(Zeta_SeqCntr* seq_cntr) {
    size_t size{ ZETA_SeqCntr_GetSize(seq_cntr) };

    void* cursor = ZETA_SeqCntr_AllocaCursor(seq_cntr);

    ZETA_SeqCntr_PeekL(seq_cntr, &cursor, NULL);

    PODValue tmp;

    for (size_t i{ 0 }; i < size; ++i) {
        ZETA_DebugAssert(ZETA_SeqCntr_Cursor_GetIdx(seq_cntr, &cursor) == i);

        ZETA_SeqCntr_Read(seq_cntr, &cursor, 1, &tmp, &cursor);

        std::cout << tmp << " ";
    }

    std::cout << "\n";
}

void main2() {
    unsigned random_seed = time(NULL);
    unsigned fixed_seed = 1729615114;

    unsigned seed = random_seed;
    // unsigned seed = fixed_seed;

    ZETA_PrintCurPos;

    ZETA_PrintVar(random_seed);
    ZETA_PrintVar(fixed_seed);
    ZETA_PrintVar(seed);

    RandomEngine().seed(seed);

    Zeta_SeqCntr* seq_cntr_a_origin{ DebugDeque_Create<PODValue>() };

    Zeta_SeqCntr* seq_cntr_a{ DebugDeque_Create<PODValue>() };

    // Zeta_SeqCntr* seq_cntr_a{ NULL };

    size_t origin_size{ 1024 * 1024 };

    PODValue* buffer_a{ GetValBuffer_A(origin_size) };
    GetRandomVals(buffer_a, buffer_a + origin_size);

    SeqCntrUtils_Insert<PODValue>(seq_cntr_a_origin, 0, buffer_a,
                                  buffer_a + origin_size);

    Zeta_SeqCntr* seq_cntr_b{ StageVector_Create<PODValue>(seq_cntr_a_origin,
                                                           64) };

    for (size_t _ = 0; _ < 16; ++_) {
        if (seq_cntr_a != NULL) {
            Zeta_SeqCntr_Assign(seq_cntr_a_origin, seq_cntr_a);
        }

        Zeta_StageVector_Reset(seq_cntr_b->context);

        for (size_t test_i = 0; test_i < 256; ++test_i) {
            DoRandomOperations(seq_cntr_a, seq_cntr_b,

                               4,  // iter_cnt

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

            if (seq_cntr_b != NULL &&
                seq_cntr_b->GetSize == Zeta_StageVector_GetSize) {
                Zeta_StageVector_Stats stats =
                    Zeta_StageVector_GetStats(seq_cntr_b->context);

                size_t size = ZETA_SeqCntr_GetSize(seq_cntr_b);

                ZETA_PrintVar(stats.ref_seg_cnt);
                ZETA_PrintVar(stats.dat_seg_cnt);
                ZETA_PrintVar(stats.ref_size);
                ZETA_PrintVar(stats.dat_size);

                //

                size_t mem_usage = sizeof(Zeta_StageVector_Seg) *
                                       (stats.ref_seg_cnt + stats.dat_seg_cnt) +
                                   sizeof(PODValue) * SEG_CAPACITY;

                std::cout << "eff: = "
                          << (double)mem_usage /
                                 (double)(sizeof(PODValue) * size)
                          << "\n";
            }

            // Zeta_StageVector_PrintState(seq_cntr_b->context);
        }
    }

    SeqCntrUtils_Destroy(seq_cntr_a);
    SeqCntrUtils_Destroy(seq_cntr_b);
    SeqCntrUtils_Destroy(seq_cntr_a_origin);
}

void main3() {
    unsigned random_seed = time(NULL);
    unsigned fixed_seed = 1729767089;

    // unsigned seed = random_seed;
    unsigned seed = fixed_seed;

    ZETA_PrintCurPos;

    ZETA_PrintVar(random_seed);
    ZETA_PrintVar(fixed_seed);
    ZETA_PrintVar(seed);

    RandomEngine().seed(seed);

    // Zeta_SeqCntr* seq_cntr_a{ NULL };
    Zeta_SeqCntr* seq_cntr_a{ DebugDeque_Create<PODValue>() };

    // Zeta_SeqCntr* seq_cntr_b{ NULL };

    /*
    Zeta_SeqCntr* seq_cntr_b{ CircularArray_Create<PODValue>(16 * 1024 *
                                                             1024) };
*/

    // Zeta_SeqCntr* seq_cntr_b{ RadixDeque_Create<PODValue>(64, 3, 12)
    //  }; Zeta_SeqCntr* seq_cntr_b{ DynamicVector_Create<PODValue>(7) };
    Zeta_SeqCntr* seq_cntr_b{ StageVector_Create<PODValue>(NULL, 32) };

    ZETA_PrintCurPos;

    size_t max_size{ 0 };

    size_t max_op_size{ 16 };

    for (size_t _ = 0; _ < 1024; ++_) {
        SyncPushR(seq_cntr_a, seq_cntr_b, max_op_size);

        SyncRead(seq_cntr_a, seq_cntr_b, max_op_size);
        CheckCursor(seq_cntr_a);
        CheckCursor(seq_cntr_b);
        Zeta_Debugger_ClearPipe();

        max_size = std::max(max_size, ZETA_SeqCntr_GetSize(seq_cntr_a));
    }

    ZETA_PrintVar(max_size);

    for (size_t _ = 0; _ < 32; ++_) {
        ZETA_PrintVar(_);

        // Zeta_StageVector_Reset(seq_cntr_b->context);

        for (size_t test_i = 0; test_i < 256; ++test_i) {
            SyncInsert(seq_cntr_a, seq_cntr_b, max_op_size);

            SyncRead(seq_cntr_a, seq_cntr_b, max_op_size);
            CheckCursor(seq_cntr_a);
            CheckCursor(seq_cntr_b);
            Zeta_Debugger_ClearPipe();

            SyncErase(seq_cntr_a, seq_cntr_b, max_op_size);

            SyncRead(seq_cntr_a, seq_cntr_b, max_op_size);
            CheckCursor(seq_cntr_a);
            CheckCursor(seq_cntr_b);
            Zeta_Debugger_ClearPipe();

            SyncWrite(seq_cntr_a, seq_cntr_b, max_op_size);

            SyncRead(seq_cntr_a, seq_cntr_b, max_op_size);
            CheckCursor(seq_cntr_a);
            CheckCursor(seq_cntr_b);
            Zeta_Debugger_ClearPipe();

            max_size = std::max(max_size, ZETA_SeqCntr_GetSize(seq_cntr_a));
        }
    }

    ZETA_PrintVar(max_size);

    SeqCntrUtils_Destroy(seq_cntr_a);
    SeqCntrUtils_Destroy(seq_cntr_b);
}

void main4() {
    ZETA_PrintCurPos;

    Zeta_SeqCntr* seq_cntr{ DebugDeque_Create<PODValue>() };

    SeqCntrUtils_Sanitize(seq_cntr);

    ZETA_PrintCurPos;

    ZETA_PrintVar(ZETA_SeqCntr_GetWidth(seq_cntr));

    void* cursor = ZETA_SeqCntr_AllocaCursor(seq_cntr);
    PODValue* ele;

    // seq_cntr->GetRBCursor(seq_cntr->context, &cursor);

    ZETA_PrintCurPos;

    for (int _ = 0; _ < 100; ++_) {
        for (int _ = 0; _ < 10; ++_) {
            ele = (PODValue*)ZETA_SeqCntr_PushL(seq_cntr, 1, &cursor);

            ZETA_PrintCurPos;

            SeqCntrUtils_Sanitize(seq_cntr);

            ZETA_PrintVar((void*)ele);
            GetRandomVal(*ele);
        }

        for (int _ = 0; _ < 10; ++_) {
            ele = (PODValue*)ZETA_SeqCntr_PushR(seq_cntr, 1, &cursor);

            ZETA_PrintCurPos;

            SeqCntrUtils_Sanitize(seq_cntr);

            ZETA_PrintVar((void*)ele);
            GetRandomVal(*ele);
        }
    }
}

void main5() {
    unsigned random_seed = time(NULL);
    unsigned fixed_seed = 1729615114;

    unsigned seed = random_seed;
    // unsigned seed = fixed_seed;

    ZETA_PrintCurPos;

    ZETA_PrintVar(random_seed);
    ZETA_PrintVar(fixed_seed);
    ZETA_PrintVar(seed);

    RandomEngine().seed(seed);

    Zeta_SeqCntr* seq_cntr_a_origin{ DebugDeque_Create<PODValue>() };
    Zeta_SeqCntr* seq_cntr_b_origin{ DebugDeque_Create<PODValue>() };

    size_t origin_size{ 1024 * 1024 };

    PODValue* buffer_a{ GetValBuffer_A(origin_size) };
    GetRandomVals(buffer_a, buffer_a + origin_size);
    SeqCntrUtils_Insert<PODValue>(seq_cntr_a_origin, 0, buffer_a,
                                  buffer_a + origin_size);

    Zeta_SeqCntr* seq_cntr_a{ DebugDeque_Create<PODValue>() };

    Zeta_SeqCntr* seq_cntr_b{ StageVector_Create<PODValue>(seq_cntr_b_origin,
                                                           7) };

    for (size_t _ = 0; _ < 16; ++_) {
        ZETA_PrintVar(_);

        Zeta_SeqCntr_Assign(seq_cntr_a, seq_cntr_a_origin);
        Zeta_SeqCntr_Assign(seq_cntr_b_origin, seq_cntr_a_origin);

        Zeta_StageVector_Reset(seq_cntr_b->context);

        DoRandomOperations(seq_cntr_a, seq_cntr_b,

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
    }

    SeqCntrUtils_Destroy(seq_cntr_a);
    SeqCntrUtils_Destroy(seq_cntr_b);

    SeqCntrUtils_Destroy(seq_cntr_a_origin);
    SeqCntrUtils_Destroy(seq_cntr_b_origin);
}

void main6() {
    unsigned random_seed = time(NULL);
    unsigned fixed_seed = 1729615114;

    unsigned seed = random_seed;
    // unsigned seed = fixed_seed;

    ZETA_PrintCurPos;

    ZETA_PrintVar(random_seed);
    ZETA_PrintVar(fixed_seed);
    ZETA_PrintVar(seed);

    RandomEngine().seed(seed);

    Zeta_SeqCntr* seq_cntr_a_origin{ DebugDeque_Create<PODValue>() };
    Zeta_SeqCntr* seq_cntr_b_origin{ DebugDeque_Create<PODValue>() };
    Zeta_SeqCntr* seq_cntr_c_origin{ DebugDeque_Create<PODValue>() };

    size_t origin_size{ 1024 * 1024 };

    PODValue* buffer_a{ GetValBuffer_A(origin_size) };
    GetRandomVals(buffer_a, buffer_a + origin_size);
    SeqCntrUtils_Insert<PODValue>(seq_cntr_a_origin, 0, buffer_a,
                                  buffer_a + origin_size);

    Zeta_SeqCntr* seq_cntr_a{ DebugDeque_Create<PODValue>() };

    Zeta_SeqCntr* seq_cntr_b{ StageVector_Create<PODValue>(seq_cntr_b_origin,
                                                           7) };

    Zeta_SeqCntr* seq_cntr_c{ StageVector_Create<PODValue>(seq_cntr_c_origin,
                                                           7) };

    for (size_t _ = 0; _ < 16; ++_) {
        ZETA_PrintVar(_);

        Zeta_SeqCntr_Assign(seq_cntr_a_origin, seq_cntr_a);
        Zeta_SeqCntr_Assign(seq_cntr_b_origin, seq_cntr_a);
        Zeta_SeqCntr_Assign(seq_cntr_c_origin, seq_cntr_a);

        Zeta_StageVector_Reset(seq_cntr_b->context);
        Zeta_StageVector_Reset(seq_cntr_c->context);

        DoRandomOperations(seq_cntr_a, seq_cntr_b,

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

        DoRandomOperations(NULL, seq_cntr_c,

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

        Zeta_StageVector_AssignFromStageVector(seq_cntr_c->context,
                                               seq_cntr_b->context);

        SeqCntrUtils_Sanitize(seq_cntr_b);
        SeqCntrUtils_Sanitize(seq_cntr_c);

        DoRandomOperations(seq_cntr_a, seq_cntr_c,

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

void main7() {
    unsigned random_seed = time(NULL);
    unsigned fixed_seed = 1729615114;

    unsigned seed = random_seed;
    // unsigned seed = fixed_seed;

    ZETA_PrintCurPos;

    ZETA_PrintVar(random_seed);
    ZETA_PrintVar(fixed_seed);
    ZETA_PrintVar(seed);

    RandomEngine().seed(seed);

    /*

    seq_cntr_base -> seq_cntr_b -> seq_cntr_c

    seq_cntr_a

    collapse

    seq_cntr_base -> seq_cntr_b

    seq_cntr_base -> seq_cntr_c

    seq_cntr_a

    */

    Zeta_SeqCntr* seq_cntr_base{ DebugDeque_Create<PODValue>() };

    size_t origin_size{ 1024 * 1024 };

    PODValue* buffer_a{ GetValBuffer_A(origin_size) };
    GetRandomVals(buffer_a, buffer_a + origin_size);
    SeqCntrUtils_Insert<PODValue>(seq_cntr_base, 0, buffer_a,
                                  buffer_a + origin_size);

    Zeta_SeqCntr* seq_cntr_a{ DebugDeque_Create<PODValue>() };

    Zeta_SeqCntr* seq_cntr_b{ StageVector_Create<PODValue>(seq_cntr_base, 7) };

    for (size_t _ = 0; _ < 16; ++_) {
        ZETA_PrintVar(_);

        Zeta_SeqCntr_Assign(seq_cntr_a, seq_cntr_base);
        Zeta_StageVector_Reset(seq_cntr_b->context);

        DoRandomOperations(seq_cntr_a, seq_cntr_b,

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

        Zeta_SeqCntr* seq_cntr_c{ StageVector_Create<PODValue>(seq_cntr_b, 7) };

        ZETA_PrintCurPos;

        DoRandomOperations(seq_cntr_a, seq_cntr_c,

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

        Zeta_StageVector_Collapse(seq_cntr_c->context);

        ZETA_PrintCurPos;

        SeqCntrUtils_Sanitize(seq_cntr_b);
        SeqCntrUtils_Sanitize(seq_cntr_c);

        ZETA_PrintCurPos;

        DoRandomOperations(seq_cntr_a, seq_cntr_c,

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

int main() {
    printf("main start\n");

    ZETA_PrintCurPos;

    unsigned long long beg_time{ GetTime() };

    main7();

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
