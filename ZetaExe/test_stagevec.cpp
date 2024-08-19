#include <DebugDeque.h>
#include <Debugger.h>
#include <Logger.h>

#include <chrono>
#include <csignal>
#include <deque>
#include <iostream>
#include <random>
#include <set>

#include "CircularArrayUtils.h"
#include "DebugDequeUtils.h"
#include "DynamicVectorUtils.h"
#include "RadixDequeUtils.h"
#include "RadixVectorUtils.h"
#include "StageVectorUtils.h"
#include "Timer.h"

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

struct Val {
    static constexpr size_t width{ 4 };

    unsigned char data[width];
};

bool_t operator==(const Val& val_a, const Val& val_b) {
    for (size_t i{ 0 }; i < Val::width; ++i) {
        if (val_a.data[i] != val_b.data[i]) { return FALSE; }
    }

    return TRUE;
}

std::ostream& operator<<(std::ostream& os, const Val& val) {
    char cs[]{ "0123456789ABCDEF" };

    for (size_t i{ Val::width }; 0 < i--;) {
        os << cs[val.data[i] / 16] << cs[val.data[i] % 16];
    }

    return os;
}

struct ValHash {
    std::hash<unsigned char> hash_;

    size_t operator()(const Val& val) const {
        size_t ret{ 0 };

        for (size_t i{ 0 }; i < Val::width; ++i) {
            ret += this->hash_(val.data[i]);
        }

        return ret;
    }
};

void GetRandomVal(Val& dst) {
    for (size_t i{ 0 }; i < Val::width; ++i) {
        dst.data[i] = (unsigned char)GetRandomInt(0, 255);
    }
}

void GetRandomVals(std::vector<Val>& dst) {
    for (size_t i{ 0 }; i < dst.size(); ++i) { GetRandomVal(dst[i]); }
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

#define SEG_CAPACITY (32)

void SC_Check(Zeta_SeqContainer const* seq_cntr);

Val SC_Access(Zeta_SeqContainer* seq_cntr, size_t idx) {
    void* pos_cursor = ZETA_SeqContainer_AllocaCursor(seq_cntr);

    Val val;

    Val* ele = (Val*)ZETA_SeqContainer_Access(seq_cntr, pos_cursor, &val, idx);

    ZETA_DebugAssert(*ele == val);
    ZETA_DebugAssert(ele == ZETA_SeqContainer_Refer(seq_cntr, pos_cursor));
    ZETA_DebugAssert(idx ==
                     ZETA_SeqContainer_Cursor_GetIdx(seq_cntr, pos_cursor));

    return val;
}

void* SC_PushL(Zeta_SeqContainer* seq_cntr, Val val) {
    ZETA_DebugAssert(seq_cntr != NULL);

    void* pos_cursor = ZETA_SeqContainer_AllocaCursor(seq_cntr);

    void* ele = ZETA_SeqContainer_PushL(seq_cntr, pos_cursor, 1);

    void* re_ele = ZETA_SeqContainer_Refer(seq_cntr, pos_cursor);

    ZETA_DebugAssert(ZETA_SeqContainer_Cursor_GetIdx(seq_cntr, pos_cursor) ==
                     0);
    ZETA_DebugAssert(ele == re_ele);

    *(Val*)ele = val;

    return (Val*)ele;
}

Val* SC_PushR(Zeta_SeqContainer* seq_cntr, Val val) {
    void* pos_cursor = ZETA_SeqContainer_AllocaCursor(seq_cntr);

    void* ele = ZETA_SeqContainer_PushL(seq_cntr, pos_cursor, 1);

    void* re_ele = ZETA_SeqContainer_Refer(seq_cntr, pos_cursor);

    ZETA_DebugAssert(ZETA_SeqContainer_Cursor_GetIdx(seq_cntr, pos_cursor) ==
                     ZETA_SeqContainer_GetSize(seq_cntr) - 1);

    ZETA_DebugAssert(ele == re_ele);

    *(Val*)ele = val;

    return (Val*)ele;
}

void SC_Read(Zeta_SeqContainer const* seq_cntr, size_t beg, size_t cnt,
             Val* dst) {
    if (seq_cntr == NULL) { return; }

    ZETA_DebugLogCurPos;

    size_t size{ ZETA_SeqContainer_GetSize(seq_cntr) };

    ZETA_DebugAssert(beg < size);

    size_t cnt_a{ ZETA_GetMinOf(cnt, size - beg) };
    size_t cnt_b{ cnt - cnt_a };

    void* pos_cursor = ZETA_SeqContainer_AllocaCursor(seq_cntr);

    ZETA_SeqContainer_Access(seq_cntr, pos_cursor, NULL, beg);

    ZETA_DebugAssert(ZETA_SeqContainer_Cursor_GetIdx(seq_cntr, pos_cursor) ==
                     beg);

    ZETA_SeqContainer_Read(seq_cntr, pos_cursor, cnt_a, dst, pos_cursor);

    SC_Check(seq_cntr);

    dst += cnt_a;

    ZETA_DebugAssert(ZETA_SeqContainer_Cursor_GetIdx(seq_cntr, pos_cursor) ==
                     beg + cnt_a);

    ZETA_SeqContainer_Access(seq_cntr, pos_cursor, NULL, 0);

    ZETA_DebugAssert(ZETA_SeqContainer_Cursor_GetIdx(seq_cntr, pos_cursor) ==
                     0);

    ZETA_SeqContainer_Read(seq_cntr, pos_cursor, cnt_b, dst, pos_cursor);

    SC_Check(seq_cntr);

    ZETA_DebugAssert(ZETA_SeqContainer_Cursor_GetIdx(seq_cntr, pos_cursor) ==
                     cnt_b);
}

void SC_Write(Zeta_SeqContainer* seq_cntr, size_t beg, size_t cnt,
              Val const* src) {
    if (seq_cntr == NULL) { return; }

    ZETA_DebugLogCurPos;

    size_t size{ ZETA_SeqContainer_GetSize(seq_cntr) };

    size_t cnt_a{ ZETA_GetMinOf(cnt, size - beg) };
    size_t cnt_b{ cnt - cnt_a };

    void* pos_cursor = ZETA_SeqContainer_AllocaCursor(seq_cntr);

    ZETA_SeqContainer_Access(seq_cntr, pos_cursor, NULL, beg);

    ZETA_DebugAssert(ZETA_SeqContainer_Cursor_GetIdx(seq_cntr, pos_cursor) ==
                     beg);

    ZETA_SeqContainer_Write(seq_cntr, pos_cursor, cnt_a, src, pos_cursor);

    SC_Check(seq_cntr);

    src += cnt_a;

    ZETA_DebugAssert(ZETA_SeqContainer_Cursor_GetIdx(seq_cntr, pos_cursor) ==
                     beg + cnt_a);

    ZETA_SeqContainer_Access(seq_cntr, pos_cursor, NULL, 0);

    ZETA_DebugAssert(ZETA_SeqContainer_Cursor_GetIdx(seq_cntr, pos_cursor) ==
                     0);

    ZETA_SeqContainer_Write(seq_cntr, pos_cursor, cnt_b, src, pos_cursor);

    ZETA_DebugAssert(ZETA_SeqContainer_Cursor_GetIdx(seq_cntr, pos_cursor) ==
                     cnt_b);
}

void SC_PushL(Zeta_SeqContainer* seq_cntr, Val const* src) {
    if (seq_cntr == NULL) { return; }

    void* pos_cursor = ZETA_SeqContainer_AllocaCursor(seq_cntr);

    Val* ele = (Val*)ZETA_SeqContainer_PushL(seq_cntr, pos_cursor, 1);

    SC_Check(seq_cntr);

    ZETA_DebugAssert(ele == ZETA_SeqContainer_Refer(seq_cntr, pos_cursor));

    ZETA_DebugAssert(
        ZETA_SeqContainer_Cursor_IsEqual(seq_cntr, pos_cursor, pos_cursor));

    *ele = *src;
}

void SC_PushR(Zeta_SeqContainer* seq_cntr, Val const* src) {
    if (seq_cntr == NULL) { return; }

    void* pos_cursor = ZETA_SeqContainer_AllocaCursor(seq_cntr);

    Val* ele = (Val*)ZETA_SeqContainer_PushR(seq_cntr, pos_cursor, 1);

    SC_Check(seq_cntr);

    ZETA_DebugAssert(ele == ZETA_SeqContainer_Refer(seq_cntr, pos_cursor));

    ZETA_DebugAssert(
        ZETA_SeqContainer_Cursor_IsEqual(seq_cntr, pos_cursor, pos_cursor));

    *ele = *src;
}

void SC_Insert(Zeta_SeqContainer* seq_cntr, size_t idx, size_t cnt,
               Val const* src) {
    if (seq_cntr == NULL) { return; }

    void* pos_cursor = ZETA_SeqContainer_AllocaCursor(seq_cntr);

    ZETA_SeqContainer_Access(seq_cntr, pos_cursor, NULL, idx);

    ZETA_DebugAssert(ZETA_SeqContainer_Cursor_GetIdx(seq_cntr, pos_cursor) ==
                     idx);

    void* ele = ZETA_SeqContainer_Insert(seq_cntr, pos_cursor, cnt);

    SC_Check(seq_cntr);

    ZETA_DebugAssert(ZETA_SeqContainer_Cursor_GetIdx(seq_cntr, pos_cursor) ==
                     idx);
    ZETA_DebugAssert(ZETA_SeqContainer_Refer(seq_cntr, pos_cursor) == ele);

    ZETA_SeqContainer_Write(seq_cntr, pos_cursor, cnt, src, pos_cursor);

    SC_Check(seq_cntr);

    ZETA_DebugAssert(ZETA_SeqContainer_Cursor_GetIdx(seq_cntr, pos_cursor) ==
                     idx + cnt);
}

void SC_PopL(Zeta_SeqContainer* seq_cntr, size_t cnt) {
    if (seq_cntr == NULL) { return; }

    ZETA_SeqContainer_PopL(seq_cntr, cnt);

    SC_Check(seq_cntr);
}

void SC_PopR(Zeta_SeqContainer* seq_cntr, size_t cnt) {
    if (seq_cntr == NULL) { return; }

    ZETA_SeqContainer_PopR(seq_cntr, cnt);

    SC_Check(seq_cntr);
}

void SC_Erase(Zeta_SeqContainer* seq_cntr, size_t idx, size_t cnt) {
    if (seq_cntr == NULL) { return; }

    void* pos_cursor = ZETA_SeqContainer_AllocaCursor(seq_cntr);

    ZETA_SeqContainer_Access(seq_cntr, pos_cursor, NULL, idx);

    SC_Check(seq_cntr);

    ZETA_DebugAssert(ZETA_SeqContainer_Cursor_GetIdx(seq_cntr, pos_cursor) ==
                     idx);

    ZETA_SeqContainer_Erase(seq_cntr, pos_cursor, cnt);

    SC_Check(seq_cntr);

    ZETA_DebugAssert(ZETA_SeqContainer_Cursor_GetIdx(seq_cntr, pos_cursor) ==
                     idx);
}

void SC_Print(Zeta_SeqContainer* seq_cntr) {
    if (seq_cntr == NULL) { return; }

    size_t size{ ZETA_SeqContainer_GetSize(seq_cntr) };

    Val tmp;

    void* pos_cursor = ZETA_SeqContainer_AllocaCursor(seq_cntr);

    ZETA_SeqContainer_PeekL(seq_cntr, pos_cursor, NULL);

    for (size_t i{ 0 }; i < size; ++i) {
        ZETA_SeqContainer_Read(seq_cntr, pos_cursor, 1, &tmp, pos_cursor);
        std::cout << tmp << " ";
    }

    std::cout << "\n";
}

void SC_Assign(Zeta_SeqContainer const* src, Zeta_SeqContainer* dst) {
    ZETA_DebugAssert(src != NULL);
    ZETA_DebugAssert(dst != NULL);

    ZETA_DebugAssert(ZETA_SeqContainer_GetWidth(src) == sizeof(Val));
    ZETA_DebugAssert(ZETA_SeqContainer_GetWidth(dst) == sizeof(Val));

    void* src_cursor = ZETA_SeqContainer_AllocaCursor(src);
    void* dst_cursor = ZETA_SeqContainer_AllocaCursor(dst);

    size_t size = ZETA_SeqContainer_GetSize(src);

    ZETA_SeqContainer_PeekL(src, src_cursor, NULL);

    ZETA_SeqContainer_EraseAll(dst);
    ZETA_SeqContainer_GetRBCursor(dst, dst_cursor);
    ZETA_SeqContainer_Insert(dst, dst_cursor, size);

    Val tmp;

    for (size_t i = 0; i < size; ++i) {
        ZETA_SeqContainer_Read(src, src_cursor, 1, &tmp, src_cursor);
        ZETA_SeqContainer_Write(dst, dst_cursor, 1, &tmp, dst_cursor);
    }
}

void SC_CheckIterator(Zeta_SeqContainer* seq_cntr, size_t idx_a, size_t idx_b) {
    void* cursor_a = ZETA_SeqContainer_AllocaCursor(seq_cntr);
    void* cursor_b = ZETA_SeqContainer_AllocaCursor(seq_cntr);

    size_t diff = idx_b - idx_a;

    ZETA_SeqContainer_Access(seq_cntr, cursor_a, NULL, idx_a);
    ZETA_SeqContainer_Access(seq_cntr, cursor_b, NULL, idx_b);

    ZETA_DebugAssert(ZETA_SeqContainer_Cursor_GetIdx(seq_cntr, cursor_a) ==
                     idx_a);
    ZETA_DebugAssert(ZETA_SeqContainer_Cursor_GetIdx(seq_cntr, cursor_b) ==
                     idx_b);

    ZETA_SeqContainer_Cursor_AdvanceR(seq_cntr, cursor_a, diff);

    ZETA_DebugAssert(ZETA_SeqContainer_Cursor_GetIdx(seq_cntr, cursor_a) ==
                     idx_b);

    ZETA_DebugAssert(
        ZETA_SeqContainer_Cursor_IsEqual(seq_cntr, cursor_a, cursor_b));

    ZETA_SeqContainer_Access(seq_cntr, cursor_a, NULL, idx_a);
    ZETA_SeqContainer_Access(seq_cntr, cursor_b, NULL, idx_b);

    ZETA_DebugAssert(ZETA_SeqContainer_Cursor_GetIdx(seq_cntr, cursor_a) ==
                     idx_a);
    ZETA_DebugAssert(ZETA_SeqContainer_Cursor_GetIdx(seq_cntr, cursor_b) ==
                     idx_b);

    ZETA_SeqContainer_Cursor_AdvanceL(seq_cntr, cursor_b, diff);

    ZETA_DebugAssert(ZETA_SeqContainer_Cursor_GetIdx(seq_cntr, cursor_b) ==
                     idx_a);

    ZETA_DebugAssert(
        ZETA_SeqContainer_Cursor_IsEqual(seq_cntr, cursor_a, cursor_b));
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

void SC_Check(Zeta_SeqContainer const* seq_cntr) {
    if (!ZETA_IsDebug) { return; }

    if (seq_cntr->GetSize == Zeta_StageVector_GetSize) {
        CheckStageVector(seq_cntr);
        return;
    }

    if (seq_cntr->GetSize == Zeta_RadixDeque_GetSize) {
        CheckRadixDeque(seq_cntr);
        return;
    }

    if (seq_cntr->GetSize == Zeta_RadixVector_GetSize) {
        SanitizeRadixVector(seq_cntr);
        return;
    }

    if (seq_cntr->GetSize == Zeta_DynamicVector_GetSize) {
        SanitizeDynamicVector(seq_cntr);
        return;
    }
}

void SC_Destroy(Zeta_SeqContainer* seq_cntr) {
    if (seq_cntr == NULL) { return; }

    if (seq_cntr->GetSize == Zeta_DebugDeque_GetSize) {
        DestroyDebugDeque(seq_cntr);
        return;
    }

    if (seq_cntr->GetSize == Zeta_StageVector_GetSize) {
        DestroyStageVector(seq_cntr);
        return;
    }

    if (seq_cntr->GetSize == Zeta_RadixDeque_GetSize) {
        DestroyRadixDeque(seq_cntr);
        return;
    }

    if (seq_cntr->GetSize == Zeta_RadixVector_GetSize) {
        DestroyRadixVector(seq_cntr);
        return;
    }

    if (seq_cntr->GetSize == Zeta_DynamicVector_GetSize) {
        DestroyDynamicVector(seq_cntr);
        return;
    }
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

void Compare(Zeta_SeqContainer* seq_cntr_a, Zeta_SeqContainer* seq_cntr_b) {
    if (seq_cntr_a == NULL || seq_cntr_b == NULL) { return; }

    size_t size_a{ ZETA_SeqContainer_GetSize(seq_cntr_a) };
    size_t size_b{ ZETA_SeqContainer_GetSize(seq_cntr_b) };

    ZETA_DebugAssert(size_a == size_b);

    void* cursor_a = ZETA_SeqContainer_AllocaCursor(seq_cntr_a);
    void* cursor_b = ZETA_SeqContainer_AllocaCursor(seq_cntr_b);

    ZETA_SeqContainer_PeekL(seq_cntr_a, cursor_a, NULL);

    ZETA_SeqContainer_PeekL(seq_cntr_b, cursor_b, NULL);

    Val tmp_a;
    Val tmp_b;

    for (size_t i{ 0 }; i < size_a; ++i) {
        ZETA_DebugAssert(
            ZETA_SeqContainer_Cursor_GetIdx(seq_cntr_a, cursor_a) == i);

        ZETA_DebugAssert(
            ZETA_SeqContainer_Cursor_GetIdx(seq_cntr_b, cursor_b) == i);

        ZETA_SeqContainer_Read(seq_cntr_a, cursor_a, 1, &tmp_a, cursor_a);

        ZETA_SeqContainer_Read(seq_cntr_b, cursor_b, 1, &tmp_b, cursor_b);

        ZETA_DebugAssert(tmp_a == tmp_b);
    }

    ZETA_DebugAssert(ZETA_SeqContainer_Cursor_GetIdx(seq_cntr_a, cursor_a) ==
                     size_a);

    ZETA_DebugAssert(ZETA_SeqContainer_Cursor_GetIdx(seq_cntr_b, cursor_b) ==
                     size_b);

    void* rb_a = ZETA_SeqContainer_AllocaCursor(seq_cntr_a);
    void* rb_b = ZETA_SeqContainer_AllocaCursor(seq_cntr_b);

    ZETA_SeqContainer_GetRBCursor(seq_cntr_a, rb_a);

    ZETA_SeqContainer_GetRBCursor(seq_cntr_b, rb_b);

    ZETA_DebugAssert(
        ZETA_SeqContainer_Cursor_IsEqual(seq_cntr_a, cursor_a, rb_a));

    ZETA_DebugAssert(
        ZETA_SeqContainer_Cursor_IsEqual(seq_cntr_b, cursor_b, rb_b));
}

void CheckCursor(Zeta_SeqContainer* seq_cntr, size_t idx_a_, size_t idx_b_) {
    void* cursor_a = ZETA_SeqContainer_AllocaCursor(seq_cntr);
    void* cursor_b = ZETA_SeqContainer_AllocaCursor(seq_cntr);

    size_t idx_a = std::min(idx_a_ + 1, idx_b_ + 1) - 1;
    size_t idx_b = std::max(idx_a_ + 1, idx_b_ + 1) - 1;
    size_t diff = idx_b - idx_a;

    ZETA_SeqContainer_Access(seq_cntr, cursor_a, NULL, idx_a);

    ZETA_SeqContainer_Access(seq_cntr, cursor_b, NULL, idx_b);

    ZETA_DebugAssert(ZETA_SeqContainer_Cursor_GetIdx(seq_cntr, cursor_a) ==
                     idx_a);

    ZETA_DebugAssert(ZETA_SeqContainer_Cursor_GetIdx(seq_cntr, cursor_b) ==
                     idx_b);

    ZETA_SeqContainer_Cursor_AdvanceR(seq_cntr, cursor_a, diff);

    ZETA_DebugAssert(ZETA_SeqContainer_Cursor_GetIdx(seq_cntr, cursor_a) ==
                     idx_b);

    ZETA_SeqContainer_Cursor_AdvanceL(seq_cntr, cursor_b, diff);

    ZETA_DebugAssert(ZETA_SeqContainer_Cursor_GetIdx(seq_cntr, cursor_b) ==
                     idx_a);
}

void CheckCursor(Zeta_SeqContainer* seq_cntr) {
    if (seq_cntr == NULL) { return; }

    size_t size{ ZETA_SeqContainer_GetSize(seq_cntr) };

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

#define MAX_OP_SIZE (128)

std::vector<Val>& vals_a() {
    static std::vector<Val> v;
    return v;
}

std::vector<Val>& vals_b() {
    static std::vector<Val> v;
    return v;
}

size_t SyncGetSize(Zeta_SeqContainer* seq_cntr_a,
                   Zeta_SeqContainer* seq_cntr_b) {
    if (seq_cntr_a == NULL && seq_cntr_b == NULL) { return 0; }

    size_t size;

    if (seq_cntr_a != NULL) {
        size = ZETA_SeqContainer_GetSize(seq_cntr_a);
    } else {
        size = ZETA_SeqContainer_GetSize(seq_cntr_b);
    }

    ZETA_DebugAssert(seq_cntr_a == NULL ||
                     size == ZETA_SeqContainer_GetSize(seq_cntr_a));

    ZETA_DebugAssert(seq_cntr_b == NULL ||
                     size == ZETA_SeqContainer_GetSize(seq_cntr_b));

    return size;
}

void SyncRead(Zeta_SeqContainer* seq_cntr_a, Zeta_SeqContainer* seq_cntr_b) {
    if (seq_cntr_a == NULL || seq_cntr_b == NULL) { return; }

    long long size{ static_cast<long long>(
        SyncGetSize(seq_cntr_a, seq_cntr_b)) };

    long long cnt_lb{ 0 };
    long long cnt_rb{ std::min<long long>(MAX_OP_SIZE, size) };

    long long idx{ GetRandomInt(0, size - 1) };
    long long cnt{ GetRandomInt(cnt_lb, cnt_rb) };

    vals_a().resize(cnt);
    vals_b().resize(cnt);

    SC_Read(seq_cntr_a, idx, cnt, vals_a().data());
    SC_Read(seq_cntr_b, idx, cnt, vals_b().data());

    ZETA_DebugAssert(vals_a() == vals_b());
}

void SyncWrite(Zeta_SeqContainer* seq_cntr_a, Zeta_SeqContainer* seq_cntr_b) {
    if (seq_cntr_a == NULL && seq_cntr_b == NULL) { return; }

    long long size{ static_cast<long long>(
        SyncGetSize(seq_cntr_a, seq_cntr_b)) };

    long long cnt_lb{ 0 };
    long long cnt_rb{ std::min<long long>(MAX_OP_SIZE, size) };

    long long idx{ GetRandomInt(0, size - 1) };
    long long cnt{ GetRandomInt(cnt_lb, cnt_rb) };

    vals_a().resize(cnt);
    GetRandomVals(vals_a());

    SC_Write(seq_cntr_a, idx, cnt, vals_a().data());
    SC_Write(seq_cntr_b, idx, cnt, vals_a().data());
}

void SyncInsert(Zeta_SeqContainer* seq_cntr_a, Zeta_SeqContainer* seq_cntr_b) {
    if (seq_cntr_a == NULL && seq_cntr_b == NULL) { return; }

    size_t size{ SyncGetSize(seq_cntr_a, seq_cntr_b) };

    size_t cnt_lb{ 0 };
    size_t cnt_rb{ MAX_OP_SIZE };

    size_t idx{ static_cast<size_t>(GetRandomInt(0, size)) };
    size_t cnt{ static_cast<size_t>(GetRandomInt(cnt_lb, cnt_rb)) };

    vals_a().resize(cnt);
    GetRandomVals(vals_a());

    ZETA_DebugLogCurPos;

    if (seq_cntr_a != NULL) {
        SC_Insert(seq_cntr_a, idx, vals_a().size(), vals_a().data());
    }

    ZETA_DebugLogCurPos;

    if (seq_cntr_b != NULL) {
        SC_Insert(seq_cntr_b, idx, vals_a().size(), vals_a().data());
    }

    ZETA_DebugLogCurPos;
}

void SyncErase(Zeta_SeqContainer* seq_cntr_a, Zeta_SeqContainer* seq_cntr_b) {
    if (seq_cntr_a == NULL && seq_cntr_b == NULL) { return; }

    long long size{ static_cast<long long>(
        SyncGetSize(seq_cntr_a, seq_cntr_b)) };

    long long cnt_lb{ 0 };
    long long cnt_rb{ std::min<long long>(MAX_OP_SIZE, size) };

    long long cnt{ GetRandomInt(cnt_lb, cnt_rb) };
    long long idx{ GetRandomInt(0, size - cnt) };

    ZETA_DebugLogCurPos;

    if (seq_cntr_a != NULL) { SC_Erase(seq_cntr_a, idx, cnt); }

    ZETA_DebugLogCurPos;

    if (seq_cntr_b != NULL) { SC_Erase(seq_cntr_b, idx, cnt); }

    ZETA_DebugLogCurPos;
}

void SyncPushL(Zeta_SeqContainer* seq_cntr_a, Zeta_SeqContainer* seq_cntr_b) {
    if (seq_cntr_a == NULL && seq_cntr_b == NULL) { return; }

    Val val;
    GetRandomVal(val);

    SC_PushL(seq_cntr_a, &val);
    SC_PushL(seq_cntr_b, &val);
}

void SyncPushR(Zeta_SeqContainer* seq_cntr_a, Zeta_SeqContainer* seq_cntr_b) {
    if (seq_cntr_a == NULL && seq_cntr_b == NULL) { return; }

    Val val;
    GetRandomVal(val);

    SC_PushR(seq_cntr_a, &val);
    SC_PushR(seq_cntr_b, &val);
}

void SyncPopL(Zeta_SeqContainer* seq_cntr_a, Zeta_SeqContainer* seq_cntr_b) {
    if (seq_cntr_a == NULL && seq_cntr_b == NULL) { return; }

    Val val;
    GetRandomVal(val);

    SC_PopL(seq_cntr_a, 1);
    SC_PopL(seq_cntr_b, 1);
}

void SyncPopR(Zeta_SeqContainer* seq_cntr_a, Zeta_SeqContainer* seq_cntr_b) {
    if (seq_cntr_a == NULL && seq_cntr_b == NULL) { return; }

    SC_PopR(seq_cntr_a, 1);
    SC_PopR(seq_cntr_b, 1);
}

void SyncCompare(Zeta_SeqContainer* seq_cntr_a, Zeta_SeqContainer* seq_cntr_b) {
    for (size_t i{ 0 }; i < 8; ++i) { SyncRead(seq_cntr_a, seq_cntr_b); }
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

void PrintSeqContainer(Zeta_SeqContainer* seq_cntr) {
    size_t size{ ZETA_SeqContainer_GetSize(seq_cntr) };

    void* cursor = ZETA_SeqContainer_AllocaCursor(seq_cntr);

    ZETA_SeqContainer_PeekL(seq_cntr, &cursor, NULL);

    Val tmp;

    for (size_t i{ 0 }; i < size; ++i) {
        ZETA_DebugAssert(ZETA_SeqContainer_Cursor_GetIdx(seq_cntr, &cursor) ==
                         i);

        ZETA_SeqContainer_Read(seq_cntr, &cursor, 1, &tmp, &cursor);

        std::cout << tmp << " ";
    }

    std::cout << "\n";
}

void main2() {
    unsigned random_seed = time(NULL);
    unsigned fixed_seed = 1721022724;

    unsigned seed = random_seed;
    // unsigned seed = fixed_seed;

    ZETA_PrintCurPos;

    ZETA_PrintVar(random_seed);
    ZETA_PrintVar(fixed_seed);
    ZETA_PrintVar(seed);

    RandomEngine().seed(seed);

    Zeta_SeqContainer* seq_cntr_a_origin{ CreateDebugDeque<Val>() };

    Zeta_SeqContainer* seq_cntr_a{ CreateDebugDeque<Val>() };

    // Zeta_SeqContainer* seq_cntr_a{ NULL };

    vals_a().resize(1024);
    GetRandomVals(vals_a());

    SC_Insert(seq_cntr_a_origin, 0, vals_a().size(), vals_a().data());

    Zeta_SeqContainer* seq_cntr_b{ CreateStageVector<Val>(seq_cntr_a_origin,
                                                          SEG_CAPACITY) };

    for (size_t _ = 0; _ < 16; ++_) {
        if (seq_cntr_a != NULL) { SC_Assign(seq_cntr_a_origin, seq_cntr_a); }

        Zeta_StageVector_Reset(seq_cntr_b->context);

        for (size_t test_i = 0; test_i < 256; ++test_i) {
            SyncInsert(seq_cntr_a, seq_cntr_b);

            Compare(seq_cntr_a, seq_cntr_b);

            CheckCursor(seq_cntr_a);

            CheckCursor(seq_cntr_b);

            Zeta_Debugger_ClearPipe();

            SyncWrite(seq_cntr_a, seq_cntr_b);
            Compare(seq_cntr_a, seq_cntr_b);
            CheckCursor(seq_cntr_a);
            CheckCursor(seq_cntr_b);

            Zeta_Debugger_ClearPipe();

            SyncErase(seq_cntr_a, seq_cntr_b);
            Compare(seq_cntr_a, seq_cntr_b);
            CheckCursor(seq_cntr_a);
            CheckCursor(seq_cntr_b);

            Zeta_Debugger_ClearPipe();

            if (seq_cntr_b != NULL &&
                seq_cntr_b->GetSize == Zeta_StageVector_GetSize) {
                Zeta_StageVector_Stats stats =
                    Zeta_StageVector_GetStats(seq_cntr_b->context);

                size_t size = ZETA_SeqContainer_GetSize(seq_cntr_b);

                ZETA_PrintVar(stats.ref_seg_cnt);
                ZETA_PrintVar(stats.dat_seg_cnt);
                ZETA_PrintVar(stats.ref_size);
                ZETA_PrintVar(stats.dat_size);

                //

                size_t mem_usage = sizeof(Zeta_StageVector_Seg) *
                                       (stats.ref_seg_cnt + stats.dat_seg_cnt) +
                                   sizeof(Val) * SEG_CAPACITY;

                std::cout << "eff: = "
                          << (double)mem_usage / (double)(sizeof(Val) * size)
                          << "\n";
            }

            // Zeta_StageVector_PrintState(seq_cntr_b->context);
        }
    }

    SC_Destroy(seq_cntr_a);
    SC_Destroy(seq_cntr_b);
    SC_Destroy(seq_cntr_a_origin);
}

void main3() {
    unsigned random_seed = time(NULL);
    unsigned fixed_seed = 1721022724;

    unsigned seed = random_seed;
    // unsigned seed = fixed_seed;

    ZETA_PrintCurPos;

    ZETA_PrintVar(random_seed);
    ZETA_PrintVar(fixed_seed);
    ZETA_PrintVar(seed);

    RandomEngine().seed(seed);

    // Zeta_SeqContainer* seq_cntr_a{ NULL };
    Zeta_SeqContainer* seq_cntr_a{ CreateDebugDeque<Val>() };

    // Zeta_SeqContainer* seq_cntr_b{ NULL };
    // Zeta_SeqContainer* seq_cntr_b{ CreateRadixDeque<Val>(64, 3, 12) };
    Zeta_SeqContainer* seq_cntr_b{ CreateDynamicVector<Val>(7) };

    ZETA_PrintCurPos;

    for (size_t _ = 0; _ < 1024; ++_) {
        SyncPushR(seq_cntr_a, seq_cntr_b);

        SyncRead(seq_cntr_a, seq_cntr_b);
        CheckCursor(seq_cntr_a);
        CheckCursor(seq_cntr_b);
        Zeta_Debugger_ClearPipe();
    }

    for (size_t _ = 0; _ < 256; ++_) {
        ZETA_PrintVar(_);

        // Zeta_StageVector_Reset(seq_cntr_b->context);

        for (size_t test_i = 0; test_i < 256; ++test_i) {
            SyncPushR(seq_cntr_a, seq_cntr_b);

            SyncRead(seq_cntr_a, seq_cntr_b);
            CheckCursor(seq_cntr_a);
            CheckCursor(seq_cntr_b);
            Zeta_Debugger_ClearPipe();

            // SyncPopL(seq_cntr_a, seq_cntr_b);

            SyncRead(seq_cntr_a, seq_cntr_b);
            CheckCursor(seq_cntr_a);
            CheckCursor(seq_cntr_b);
            Zeta_Debugger_ClearPipe();

            SyncWrite(seq_cntr_a, seq_cntr_b);

            SyncRead(seq_cntr_a, seq_cntr_b);
            CheckCursor(seq_cntr_a);
            CheckCursor(seq_cntr_b);
            Zeta_Debugger_ClearPipe();
        }
    }

    SC_Destroy(seq_cntr_a);
    SC_Destroy(seq_cntr_b);
}

void main4() {
    ZETA_PrintCurPos;

    Zeta_SeqContainer* seq_cntr{ CreateRadixDeque<Val>(32, 32, 6) };

    SC_Check(seq_cntr);

    ZETA_PrintCurPos;

    ZETA_PrintVar(ZETA_SeqContainer_GetWidth(seq_cntr));

    void* cursor = ZETA_SeqContainer_AllocaCursor(seq_cntr);
    Val* ele;

    // seq_cntr->GetRBCursor(seq_cntr->context, &cursor);

    ZETA_PrintCurPos;

    for (int _ = 0; _ < 100; ++_) {
        for (int _ = 0; _ < 10; ++_) {
            ele = (Val*)ZETA_SeqContainer_PushL(seq_cntr, &cursor, 1);

            ZETA_PrintCurPos;

            SC_Check(seq_cntr);

            ZETA_PrintVar((void*)ele);
            GetRandomVal(*ele);
        }

        for (int _ = 0; _ < 10; ++_) {
            ele = (Val*)ZETA_SeqContainer_PushR(seq_cntr, &cursor, 1);

            ZETA_PrintCurPos;

            SC_Check(seq_cntr);

            ZETA_PrintVar((void*)ele);
            GetRandomVal(*ele);
        }
    }
}

int main() {
    // signal(SIGINT, SignalHandler);
    // signal(SIGILL, SignalHandler);
    // signal(SIGFPE, SignalHandler);
    // signal(SIGSEGV, SignalHandler);
    // signal(SIGTERM, SignalHandler);
    // signal(SIGBREAK, SignalHandler);
    // signal(SIGABRT, SignalHandler);
    // signal(SIGABRT_COMPAT, SignalHandler);

    printf("main start\n");

    ZETA_PrintCurPos;

    unsigned long long beg_time{ GetTime() };

    main2();

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
