#include <chrono>
#include <csignal>
#include <deque>
#include <iostream>
#include <random>
#include <set>

#include "../Zeta/DebugDeque.h"
#include "../Zeta/Debugger.h"
#include "../Zeta/Logger.h"
#include "CircularArrayUtils.h"
#include "DynamicVectorUtils.h"
#include "MemAllocatorCheck.h"
#include "RadixDequeUtils.h"
#include "RadixVectorUtils.h"
#include "StageVectorUtils.h"

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

std::mt19937_64 en;

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

std::uniform_int_distribution<size_t> val_generator{ 0, 255 };

void GetRandomVal(Val& dst) {
    for (size_t i{ 0 }; i < Val::width; ++i) {
        dst.data[i] = (unsigned char)val_generator(en);
    }
}

void GetRandomVals(std::vector<Val>& dst) {
    for (size_t i{ 0 }; i < dst.size(); ++i) { GetRandomVal(dst[i]); }
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

void InitDD(Zeta_SeqContainer* seq_cntr) {
    Zeta_DebugDeque* dd{ static_cast<Zeta_DebugDeque*>(
        std::malloc(sizeof(Zeta_DebugDeque))) };

    dd->width = sizeof(Val);
    dd->stride = sizeof(Val);

    Zeta_DebugDeque_Init(dd);
    Zeta_DebugDeque_DeploySeqContainer(dd, seq_cntr);
}

Zeta_SeqContainer* CreateDD() {
    Zeta_SeqContainer* seq_cntr{ new Zeta_SeqContainer{} };
    InitDD(seq_cntr);
    return seq_cntr;
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

#define SEG_CAPACITY (32)

void SC_Check(Zeta_SeqContainer const* seq_cntr);

size_t SC_GetSize(Zeta_SeqContainer const* seq_cntr) {
    return seq_cntr->GetSize(seq_cntr->context);
}

Val SC_Access(Zeta_SeqContainer* seq_cntr, size_t idx) {
    Zeta_Cursor cursor;
    Val val;

    Val* ele = (Val*)seq_cntr->Access(seq_cntr->context, &cursor, &val, idx);

    ZETA_DebugAssert(*ele == val);
    ZETA_DebugAssert(ele == seq_cntr->Refer(seq_cntr->context, &cursor));

    return val;
}

void* SC_PushL(Zeta_SeqContainer* seq_cntr, Val val) {
    Zeta_Cursor pos_cursor;

    void* ele = seq_cntr->PushL(seq_cntr->context, &pos_cursor, 1);

    void* re_ele = seq_cntr->Refer(seq_cntr->context, &pos_cursor);

    ZETA_DebugAssert(seq_cntr->Cursor_GetIdx(seq_cntr->context, &pos_cursor) ==
                     0);
    ZETA_DebugAssert(ele == re_ele);

    *(Val*)ele = val;

    return (Val*)ele;
}

Val* SC_PushR(Zeta_SeqContainer* seq_cntr, Val val) {
    Zeta_Cursor pos_cursor;

    void* ele = seq_cntr->PushL(seq_cntr->context, &pos_cursor, 1);

    void* re_ele = seq_cntr->Refer(seq_cntr->context, &pos_cursor);

    ZETA_DebugAssert(seq_cntr->Cursor_GetIdx(seq_cntr->context, &pos_cursor) ==
                     SC_GetSize(seq_cntr) - 1);

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

    Zeta_Cursor pos_cursor;

    seq_cntr->Access(seq_cntr->context, &pos_cursor, NULL, beg);

    ZETA_DebugAssert(seq_cntr->Cursor_GetIdx(seq_cntr->context, &pos_cursor) ==
                     beg);

    seq_cntr->Read(seq_cntr->context, &pos_cursor, cnt_a, dst, &pos_cursor);

    SC_Check(seq_cntr);

    dst += cnt_a;

    ZETA_DebugAssert(seq_cntr->Cursor_GetIdx(seq_cntr->context, &pos_cursor) ==
                     beg + cnt_a);

    seq_cntr->Access(seq_cntr->context, &pos_cursor, NULL, 0);

    ZETA_DebugAssert(seq_cntr->Cursor_GetIdx(seq_cntr->context, &pos_cursor) ==
                     0);

    seq_cntr->Read(seq_cntr->context, &pos_cursor, cnt_b, dst, &pos_cursor);

    SC_Check(seq_cntr);

    ZETA_DebugAssert(seq_cntr->Cursor_GetIdx(seq_cntr->context, &pos_cursor) ==
                     cnt_b);
}

void SC_Write(Zeta_SeqContainer* seq_cntr, size_t beg, size_t cnt,
              Val const* src) {
    if (seq_cntr == NULL) { return; }

    ZETA_DebugLogCurPos;

    size_t size{ ZETA_SeqContainer_GetSize(seq_cntr) };

    size_t cnt_a{ ZETA_GetMinOf(cnt, size - beg) };
    size_t cnt_b{ cnt - cnt_a };

    Zeta_Cursor pos_cursor;

    seq_cntr->Access(seq_cntr->context, &pos_cursor, NULL, beg);

    ZETA_DebugAssert(seq_cntr->Cursor_GetIdx(seq_cntr->context, &pos_cursor) ==
                     beg);

    seq_cntr->Write(seq_cntr->context, &pos_cursor, cnt_a, src, &pos_cursor);

    src += cnt_a;

    ZETA_DebugAssert(seq_cntr->Cursor_GetIdx(seq_cntr->context, &pos_cursor) ==
                     beg + cnt_a);

    seq_cntr->Access(seq_cntr->context, &pos_cursor, NULL, 0);

    ZETA_DebugAssert(seq_cntr->Cursor_GetIdx(seq_cntr->context, &pos_cursor) ==
                     0);

    seq_cntr->Write(seq_cntr->context, &pos_cursor, cnt_b, src, &pos_cursor);

    ZETA_DebugAssert(seq_cntr->Cursor_GetIdx(seq_cntr->context, &pos_cursor) ==
                     cnt_b);
}

void SC_PushL(Zeta_SeqContainer* seq_cntr, Val const* src) {
    if (seq_cntr == NULL) { return; }

    Zeta_Cursor cursor;

    Val* ele = (Val*)seq_cntr->PushL(seq_cntr->context, &cursor, 1);

    SC_Check(seq_cntr);

    ZETA_DebugAssert(ele == seq_cntr->Refer(seq_cntr->context, &cursor));

    ZETA_DebugAssert(
        seq_cntr->Cursor_IsEqual(seq_cntr->context, &cursor, &cursor));

    *ele = *src;
}

void SC_PushR(Zeta_SeqContainer* seq_cntr, Val const* src) {
    if (seq_cntr == NULL) { return; }

    Zeta_Cursor cursor;

    Val* ele = (Val*)seq_cntr->PushR(seq_cntr->context, &cursor, 1);

    SC_Check(seq_cntr);

    ZETA_DebugAssert(ele == seq_cntr->Refer(seq_cntr->context, &cursor));

    ZETA_DebugAssert(
        seq_cntr->Cursor_IsEqual(seq_cntr->context, &cursor, &cursor));

    *ele = *src;
}

void SC_Insert(Zeta_SeqContainer* seq_cntr, size_t idx, size_t cnt,
               Val const* src) {
    if (seq_cntr == NULL) { return; }

    Zeta_Cursor pos_cursor;

    seq_cntr->Access(seq_cntr->context, &pos_cursor, NULL, idx);

    ZETA_DebugAssert(seq_cntr->Cursor_GetIdx(seq_cntr->context, &pos_cursor) ==
                     idx);

    void* ele = seq_cntr->Insert(seq_cntr->context, &pos_cursor, cnt);

    SC_Check(seq_cntr);

    ZETA_DebugAssert(seq_cntr->Cursor_GetIdx(seq_cntr->context, &pos_cursor) ==
                     idx);
    ZETA_DebugAssert(seq_cntr->Refer(seq_cntr->context, &pos_cursor) == ele);

    seq_cntr->Write(seq_cntr->context, &pos_cursor, cnt, src, &pos_cursor);

    SC_Check(seq_cntr);

    ZETA_DebugAssert(seq_cntr->Cursor_GetIdx(seq_cntr->context, &pos_cursor) ==
                     idx + cnt);
}

void SC_PopL(Zeta_SeqContainer* seq_cntr, size_t cnt) {
    if (seq_cntr == NULL) { return; }

    seq_cntr->PopL(seq_cntr->context, cnt);

    SC_Check(seq_cntr);
}

void SC_PopR(Zeta_SeqContainer* seq_cntr, size_t cnt) {
    if (seq_cntr == NULL) { return; }

    seq_cntr->PopR(seq_cntr->context, cnt);

    SC_Check(seq_cntr);
}

void SC_Erase(Zeta_SeqContainer* seq_cntr, size_t idx, size_t cnt) {
    if (seq_cntr == NULL) { return; }

    Zeta_Cursor pos_cursor;

    seq_cntr->Access(seq_cntr->context, &pos_cursor, NULL, idx);

    SC_Check(seq_cntr);

    ZETA_DebugAssert(seq_cntr->Cursor_GetIdx(seq_cntr->context, &pos_cursor) ==
                     idx);

    seq_cntr->Erase(seq_cntr->context, &pos_cursor, cnt);

    SC_Check(seq_cntr);

    ZETA_DebugAssert(seq_cntr->Cursor_GetIdx(seq_cntr->context, &pos_cursor) ==
                     idx);
}

void SC_Print(Zeta_SeqContainer* seq_cntr) {
    if (seq_cntr == NULL) { return; }

    size_t size{ SC_GetSize(seq_cntr) };

    Val tmp;

    Zeta_Cursor pos_cursor;

    seq_cntr->PeekL(seq_cntr->context, &pos_cursor, NULL);

    for (size_t i{ 0 }; i < size; ++i) {
        seq_cntr->Read(seq_cntr->context, &pos_cursor, 1, &tmp, &pos_cursor);
        std::cout << tmp << " ";
    }

    std::cout << "\n";
}

void SC_Assign(Zeta_SeqContainer const* src, Zeta_SeqContainer* dst) {
    ZETA_DebugAssert(src != NULL);
    ZETA_DebugAssert(dst != NULL);

    ZETA_DebugAssert(src->GetWidth(src->context) == sizeof(Val));
    ZETA_DebugAssert(dst->GetWidth(dst->context) == sizeof(Val));

    Zeta_Cursor src_cursor;
    Zeta_Cursor dst_cursor;

    size_t size = src->GetSize(src->context);

    src->PeekL(src->context, &src_cursor, NULL);

    dst->EraseAll(dst->context);
    dst->GetRBCursor(dst->context, &dst_cursor);
    dst->Insert(dst->context, &dst_cursor, size);

    Val tmp;

    for (size_t i = 0; i < size; ++i) {
        src->Read(src->context, &src_cursor, 1, &tmp, &src_cursor);
        dst->Write(dst->context, &dst_cursor, 1, &tmp, &dst_cursor);
    }
}

/*
void EraseAll_(void* context, void* ele) {
    ZETA_Unused(context);

    EraseValFromValSet(*(Val*)ele);
}


void SC_EraseAll(Zeta_SeqContainer* seq_cntr) {

    std::unordered_set<Val, ValHash> vals;

    byte_t iter[CURSOR_WIDTH] __attribute__((aligned(alignof(max_align_t))));
    byte_t end[CURSOR_WIDTH] __attribute__((aligned(alignof(max_align_t))));

    seq_cntr->PeekL(seq_cntr->context, iter, NULL);
    seq_cntr->GetRBCursor(seq_cntr->context, iter);

    for (; !seq_cntr->Cursor_IsEqual(seq_cntr->context, iter, end);
         seq_cntr->Cursor_StepR(seq_cntr->context, iter)) {
        Val tmp;
        seq_cntr->Read(seq_cntr->context, iter, 1, &tmp);
        vals.Insert(tmp);
    }

    seq_cntr->EraseAll(seq_cntr->context, NULL, EraseAll_);
}*/

void SC_CheckIterator(Zeta_SeqContainer* seq_cntr, size_t idx_a, size_t idx_b) {
    Zeta_Cursor cursor_a;
    Zeta_Cursor cursor_b;

    size_t diff = idx_b - idx_a;

    seq_cntr->Access(seq_cntr->context, &cursor_a, NULL, idx_a);
    seq_cntr->Access(seq_cntr->context, &cursor_b, NULL, idx_b);

    ZETA_DebugAssert(seq_cntr->Cursor_GetIdx(seq_cntr->context, &cursor_a) ==
                     idx_a);
    ZETA_DebugAssert(seq_cntr->Cursor_GetIdx(seq_cntr->context, &cursor_b) ==
                     idx_b);

    seq_cntr->Cursor_AdvanceR(seq_cntr->context, &cursor_a, diff);

    ZETA_DebugAssert(seq_cntr->Cursor_GetIdx(seq_cntr->context, &cursor_a) ==
                     idx_b);

    ZETA_DebugAssert(
        seq_cntr->Cursor_IsEqual(seq_cntr->context, &cursor_a, &cursor_b));

    seq_cntr->Access(seq_cntr->context, &cursor_a, NULL, idx_a);
    seq_cntr->Access(seq_cntr->context, &cursor_b, NULL, idx_b);

    ZETA_DebugAssert(seq_cntr->Cursor_GetIdx(seq_cntr->context, &cursor_a) ==
                     idx_a);
    ZETA_DebugAssert(seq_cntr->Cursor_GetIdx(seq_cntr->context, &cursor_b) ==
                     idx_b);

    seq_cntr->Cursor_AdvanceL(seq_cntr->context, &cursor_b, diff);

    ZETA_DebugAssert(seq_cntr->Cursor_GetIdx(seq_cntr->context, &cursor_b) ==
                     idx_a);

    ZETA_DebugAssert(
        seq_cntr->Cursor_IsEqual(seq_cntr->context, &cursor_a, &cursor_b));
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

void SC_Check(Zeta_SeqContainer const* seq_cntr) {
    if (seq_cntr->GetSize == Zeta_StageVector_GetSize) {
        CheckStageVec(seq_cntr);
        return;
    }

    if (seq_cntr->GetSize == Zeta_RadixDeque_GetSize) {
        CheckRadixDeque(seq_cntr);
        return;
    }

    if (seq_cntr->GetSize == Zeta_DynamicVector_GetSize) {
        CheckDynamicVector(seq_cntr);
        return;
    }
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

std::uniform_int_distribution<long long> size_generator{ 0, ZETA_RangeMaxOf(
                                                                long long) };

size_t GetRandomIdx(size_t size) { return size_generator(en) % (size + 2) - 1; }

size_t GetRandomInsertIdx(size_t size) {
    return size_generator(en) % (size + 1);
}

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

    long long cnt{ (size_generator(en)) % (rb - lb + 1) + lb };

    // base_avg + alpha * (size - balance_size)

    long long idx{ size_generator(en) % (size + 1) };

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

    long long cnt{ size_generator(en) % (rb - lb + 1) + lb };

    /*

    1   0 ~ size - 1        size
    2   0 ~ size - 2        size - 1

    */

    long long idx{ size_generator(en) % (size - cnt + 1) };

    return { static_cast<size_t>(idx), static_cast<size_t>(cnt) };
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

void Compare(Zeta_SeqContainer* seq_cntr_a, Zeta_SeqContainer* seq_cntr_b) {
    if (seq_cntr_a == NULL || seq_cntr_b == NULL) { return; }

    size_t size_a{ SC_GetSize(seq_cntr_a) };
    size_t size_b{ SC_GetSize(seq_cntr_b) };

    ZETA_DebugAssert(size_a == size_b);

    Zeta_Cursor cursor_a;
    Zeta_Cursor cursor_b;

    seq_cntr_a->PeekL(seq_cntr_a->context, &cursor_a, NULL);

    seq_cntr_b->PeekL(seq_cntr_b->context, &cursor_b, NULL);

    Val tmp_a;
    Val tmp_b;

    for (size_t i{ 0 }; i < size_a; ++i) {
        ZETA_DebugAssert(
            seq_cntr_a->Cursor_GetIdx(seq_cntr_a->context, &cursor_a) == i);

        ZETA_DebugAssert(
            seq_cntr_b->Cursor_GetIdx(seq_cntr_b->context, &cursor_b) == i);

        seq_cntr_a->Read(seq_cntr_a->context, &cursor_a, 1, &tmp_a, &cursor_a);

        seq_cntr_b->Read(seq_cntr_b->context, &cursor_b, 1, &tmp_b, &cursor_b);

        ZETA_DebugAssert(tmp_a == tmp_b);
    }

    ZETA_DebugAssert(
        seq_cntr_a->Cursor_GetIdx(seq_cntr_a->context, &cursor_a) == size_a);

    ZETA_DebugAssert(
        seq_cntr_b->Cursor_GetIdx(seq_cntr_b->context, &cursor_b) == size_b);

    Zeta_Cursor rb_a;
    Zeta_Cursor rb_b;

    seq_cntr_a->GetRBCursor(seq_cntr_a->context, &rb_a);

    seq_cntr_b->GetRBCursor(seq_cntr_b->context, &rb_b);

    ZETA_DebugAssert(
        seq_cntr_a->Cursor_IsEqual(seq_cntr_a->context, &cursor_a, &rb_a));

    ZETA_DebugAssert(
        seq_cntr_b->Cursor_IsEqual(seq_cntr_b->context, &cursor_b, &rb_b));
}

void CheckCursor(Zeta_SeqContainer* seq_cntr, size_t idx_a_, size_t idx_b_) {
    Zeta_Cursor cursor_a;
    Zeta_Cursor cursor_b;

    size_t idx_a = std::min(idx_a_ + 1, idx_b_ + 1) - 1;
    size_t idx_b = std::max(idx_a_ + 1, idx_b_ + 1) - 1;
    size_t diff = idx_b - idx_a;

    seq_cntr->Access(seq_cntr->context, &cursor_a, NULL, idx_a);

    seq_cntr->Access(seq_cntr->context, &cursor_b, NULL, idx_b);

    ZETA_DebugAssert(seq_cntr->Cursor_GetIdx(seq_cntr->context, &cursor_a) ==
                     idx_a);

    ZETA_DebugAssert(seq_cntr->Cursor_GetIdx(seq_cntr->context, &cursor_b) ==
                     idx_b);

    seq_cntr->Cursor_AdvanceR(seq_cntr->context, &cursor_a, diff);

    ZETA_DebugAssert(seq_cntr->Cursor_GetIdx(seq_cntr->context, &cursor_a) ==
                     idx_b);

    seq_cntr->Cursor_AdvanceL(seq_cntr->context, &cursor_b, diff);

    ZETA_DebugAssert(seq_cntr->Cursor_GetIdx(seq_cntr->context, &cursor_b) ==
                     idx_a);
}

void CheckCursor(Zeta_SeqContainer* seq_cntr) {
    if (seq_cntr == NULL) { return; }

    size_t size{ SC_GetSize(seq_cntr) };

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

std::vector<Val> vals_a;
std::vector<Val> vals_b;

size_t SyncGetSize(Zeta_SeqContainer* seq_cntr_a,
                   Zeta_SeqContainer* seq_cntr_b) {
    if (seq_cntr_a == NULL && seq_cntr_b == NULL) { return 0; }

    size_t size;

    if (seq_cntr_a != NULL) {
        size = SC_GetSize(seq_cntr_a);
    } else {
        size = SC_GetSize(seq_cntr_b);
    }

    ZETA_DebugAssert(seq_cntr_a == NULL || size == SC_GetSize(seq_cntr_a));

    ZETA_DebugAssert(seq_cntr_b == NULL || size == SC_GetSize(seq_cntr_b));

    return size;
}

void SyncRead(Zeta_SeqContainer* seq_cntr_a, Zeta_SeqContainer* seq_cntr_b) {
    if (seq_cntr_a == NULL || seq_cntr_b == NULL) { return; }

    long long size{ static_cast<long long>(
        SyncGetSize(seq_cntr_a, seq_cntr_b)) };

    long long cnt_lb{ 0 };
    long long cnt_rb{ std::min<long long>(MAX_OP_SIZE, size) };

    long long idx{ size_generator(en) % size };
    long long cnt{ size_generator(en) % (cnt_rb - cnt_lb + 1) + cnt_lb };

    vals_a.resize(cnt);
    vals_b.resize(cnt);

    SC_Read(seq_cntr_a, idx, cnt, vals_a.data());
    SC_Read(seq_cntr_b, idx, cnt, vals_b.data());

    ZETA_DebugAssert(vals_a == vals_b);
}

void SyncWrite(Zeta_SeqContainer* seq_cntr_a, Zeta_SeqContainer* seq_cntr_b) {
    if (seq_cntr_a == NULL && seq_cntr_b == NULL) { return; }

    long long size{ static_cast<long long>(
        SyncGetSize(seq_cntr_a, seq_cntr_b)) };

    long long cnt_lb{ 0 };
    long long cnt_rb{ std::min<long long>(MAX_OP_SIZE, size) };

    long long idx{ size_generator(en) % size };
    long long cnt{ size_generator(en) % (cnt_rb - cnt_lb + 1) + cnt_lb };

    vals_a.resize(cnt);
    GetRandomVals(vals_a);

    SC_Write(seq_cntr_a, idx, cnt, vals_a.data());
    SC_Write(seq_cntr_b, idx, cnt, vals_a.data());
}

void SyncInsert(Zeta_SeqContainer* seq_cntr_a, Zeta_SeqContainer* seq_cntr_b) {
    if (seq_cntr_a == NULL && seq_cntr_b == NULL) { return; }

    size_t size{ SyncGetSize(seq_cntr_a, seq_cntr_b) };

    size_t cnt_lb{ 0 };
    size_t cnt_rb{ MAX_OP_SIZE };

    size_t idx{ size_generator(en) % (size + 1) };
    size_t cnt{ size_generator(en) % (cnt_rb - cnt_lb + 1) + cnt_lb };

    vals_a.resize(cnt);
    GetRandomVals(vals_a);

    ZETA_DebugLogCurPos;

    if (seq_cntr_a != NULL) {
        SC_Insert(seq_cntr_a, idx, vals_a.size(), vals_a.data());
    }

    ZETA_DebugLogCurPos;

    if (seq_cntr_b != NULL) {
        SC_Insert(seq_cntr_b, idx, vals_a.size(), vals_a.data());
    }

    ZETA_DebugLogCurPos;
}

void SyncErase(Zeta_SeqContainer* seq_cntr_a, Zeta_SeqContainer* seq_cntr_b) {
    if (seq_cntr_a == NULL && seq_cntr_b == NULL) { return; }

    long long size{ static_cast<long long>(
        SyncGetSize(seq_cntr_a, seq_cntr_b)) };

    long long cnt_lb{ 0 };
    long long cnt_rb{ std::min<long long>(MAX_OP_SIZE, size) };

    long long cnt{ size_generator(en) % (cnt_rb - cnt_lb + 1) + cnt_lb };
    long long idx{ size_generator(en) % (size + 1 - cnt) };

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

    /*
    if (seq_cntr_a == NULL && seq_cntr_b == NULL) { return; }

    size_t size_a{ SC_GetSize(seq_cntr_a) };
    size_t size_b{ SC_GetSize(seq_cntr_b) };

    ZETA_DebugAssert(size_a == size_b);

    size_t idx_a_{ size_generator(en) % (size_a + 1) };
    size_t idx_b_{ size_generator(en) % (size_a + 1) };

    size_t beg{ std::min(idx_a_, idx_b_) };
    size_t end{ std::max(idx_a_, idx_b_) };

    size_t cnt{ end - beg };

    vals_a.resize(cnt);
    vals_b.resize(cnt);

    Zeta_Cursor cursor;

    seq_cntr_a->Access(seq_cntr_a->context, &cursor, NULL, beg);
    seq_cntr_a->Read(seq_cntr_a->context, &cursor, cnt, vals_a.data(), &cursor);

    ZETA_DebugAssert(seq_cntr_a->Cursor_GetIdx(seq_cntr_a->context, &cursor) ==
                     end);

    seq_cntr_b->Access(seq_cntr_b->context, &cursor, NULL, beg);

    seq_cntr_b->Read(seq_cntr_b->context, &cursor, cnt, vals_b.data(), &cursor);

    ZETA_DebugAssert(seq_cntr_b->Cursor_GetIdx(seq_cntr_b->context, &cursor) ==
                     end);

    ZETA_DebugAssert(vals_a == vals_b);
    */
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

void PrintSeqContainer(Zeta_SeqContainer* seq_cntr) {
    size_t size{ SC_GetSize(seq_cntr) };

    Zeta_Cursor cursor;

    seq_cntr->PeekL(seq_cntr->context, &cursor, NULL);

    Val tmp;

    for (size_t i{ 0 }; i < size; ++i) {
        ZETA_DebugAssert(seq_cntr->Cursor_GetIdx(seq_cntr->context, &cursor) ==
                         i);

        seq_cntr->Read(seq_cntr->context, &cursor, 1, &tmp, &cursor);

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

    en.seed(seed);

    Zeta_SeqContainer* seq_cntr_a_origin{ CreateDD() };

    Zeta_SeqContainer* seq_cntr_a{ CreateDD() };

    // Zeta_SeqContainer* seq_cntr_a{ NULL };

    vals_a.resize(16 * 1024);
    GetRandomVals(vals_a);

    SC_Insert(seq_cntr_a_origin, 0, vals_a.size(), vals_a.data());

    Zeta_SeqContainer* seq_cntr_b{ CreateStageVec<Val>(seq_cntr_a_origin,
                                                       SEG_CAPACITY) };

    for (size_t _ = 0; _ < 64; ++_) {
        ZETA_PrintCurPos;

        if (seq_cntr_a != NULL) { SC_Assign(seq_cntr_a_origin, seq_cntr_a); }

        ZETA_PrintCurPos;

        Zeta_StageVector_Reset(seq_cntr_b->context);

        ZETA_PrintCurPos;

        for (size_t test_i = 0; test_i < 256; ++test_i) {
            ZETA_PrintVar(test_i);

            ZETA_PrintCurPos;

            SyncInsert(seq_cntr_a, seq_cntr_b);

            ZETA_PrintCurPos;

            Compare(seq_cntr_a, seq_cntr_b);

            ZETA_PrintCurPos;

            CheckCursor(seq_cntr_a);

            ZETA_PrintCurPos;

            CheckCursor(seq_cntr_b);

            ZETA_PrintCurPos;

            Zeta_Debugger_ClearPipe();

            ZETA_PrintVar(SyncGetSize(seq_cntr_a, seq_cntr_b));

            ZETA_PrintCurPos;

            ZETA_PrintCurPos;

            SyncWrite(seq_cntr_a, seq_cntr_b);
            Compare(seq_cntr_a, seq_cntr_b);
            CheckCursor(seq_cntr_a);
            CheckCursor(seq_cntr_b);

            Zeta_Debugger_ClearPipe();

            ZETA_PrintVar(SyncGetSize(seq_cntr_a, seq_cntr_b));

            ZETA_PrintCurPos;

            SyncErase(seq_cntr_a, seq_cntr_b);
            Compare(seq_cntr_a, seq_cntr_b);
            CheckCursor(seq_cntr_a);
            CheckCursor(seq_cntr_b);

            Zeta_Debugger_ClearPipe();

            ZETA_PrintVar(SyncGetSize(seq_cntr_a, seq_cntr_b));

            ZETA_PrintCurPos;

            if (seq_cntr_b != NULL &&
                seq_cntr_b->GetSize == Zeta_StageVector_GetSize) {
                Zeta_StageVector_Stats stats =
                    Zeta_StageVector_GetStats(seq_cntr_b->context);

                size_t size = SC_GetSize(seq_cntr_b);

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

    en.seed(seed);

    Zeta_SeqContainer* seq_cntr_a{ CreateDD() };
    // Zeta_SeqContainer* seq_cntr_a{ NULL };

    // Zeta_SeqContainer* seq_cntr_b{ CreateRadixDeque<Val>(64, 32, 12) };
    Zeta_SeqContainer* seq_cntr_b{ CreateDynamicVector<Val>(64) };
    // Zeta_SeqContainer* seq_cntr_b{ NULL };

    ZETA_PrintCurPos;

    for (size_t _ = 0; _ < 1024; ++_) {
        SyncPushR(seq_cntr_a, seq_cntr_b);

        SyncRead(seq_cntr_a, seq_cntr_b);
        CheckCursor(seq_cntr_a);
        CheckCursor(seq_cntr_b);
        Zeta_Debugger_ClearPipe();
    }

    ZETA_PrintCurPos;

    for (size_t _ = 0; _ < 64; ++_) {
        // Zeta_StageVector_Reset(seq_cntr_b->context);

        for (size_t test_i = 0; test_i < 256; ++test_i) {
            SyncPushR(seq_cntr_a, seq_cntr_b);

            SyncRead(seq_cntr_a, seq_cntr_b);
            CheckCursor(seq_cntr_a);
            CheckCursor(seq_cntr_b);
            Zeta_Debugger_ClearPipe();

            SyncPopL(seq_cntr_a, seq_cntr_b);

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
}

void main4() {
    ZETA_PrintCurPos;

    Zeta_SeqContainer* seq_cntr{ CreateRadixDeque<Val>(32, 32, 6) };

    SC_Check(seq_cntr);

    ZETA_PrintCurPos;

    ZETA_PrintVar(seq_cntr->GetWidth(seq_cntr->context));

    Zeta_Cursor cursor;
    Val* ele;

    // seq_cntr->GetRBCursor(seq_cntr->context, &cursor);

    ZETA_PrintCurPos;

    for (int _ = 0; _ < 100; ++_) {
        for (int _ = 0; _ < 10; ++_) {
            ele = (Val*)seq_cntr->PushL(seq_cntr->context, &cursor, 1);

            ZETA_PrintCurPos;

            SC_Check(seq_cntr);

            ZETA_PrintVar((void*)ele);
            GetRandomVal(*ele);
        }

        for (int _ = 0; _ < 10; ++_) {
            ele = (Val*)seq_cntr->PushR(seq_cntr->context, &cursor, 1);

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

    ZETA_PrintCurPos;

    std::chrono::steady_clock::time_point beg_time =
        std::chrono::steady_clock::now();

    main3();

    std::chrono::steady_clock::time_point end_time =
        std::chrono::steady_clock::now();

    printf("ok\a\n");

    std::cout << "Time difference = "
              << std::chrono::duration_cast<std::chrono::milliseconds>(
                     end_time - beg_time)
                     .count()
              << "[ms]\n";

    /*

    DD: 4073[ms]

    RadixDeque: 325[ms]

    RadixVector: 233[ms]

    */

    return 0;
}
