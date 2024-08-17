#include <deque>
#include <iostream>
#include <random>
#include <set>

#include "../Zeta/CircularArray.h"
#include "../Zeta/DebugDeque.h"
#include "../Zeta/SegVector.h"
#include "MemAllocatorCheck.h"
#include "StdAllocator.h"

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

std::mt19937_64 en;

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

void GetRandomVal(Val* dst) {
    for (size_t i{ 0 }; i < Val::width; ++i) {
        dst->data[i] = (unsigned char)val_generator(en);
    }
}

void GetRandomVal(std::vector<Val>& dst) {
    for (size_t i{ 0 }; i < dst.size(); ++i) { GetRandomVal(&dst[i]); }
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

Zeta_DebugDeque dd;

void InitDD(Zeta_SeqContainer* seq_cntr) {
    dd.width = sizeof(Val);

    Zeta_DebugDeque_Init(&dd);

    Zeta_DebugDeque_DeploySeqContainer(&dd, seq_cntr);
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

#define CAPACITY (256 * 1024 * 1024)

Zeta_CircularArray ca;

void InitCV(Zeta_SeqContainer* seq_cntr) {
    ca.width = sizeof(Val);
    ca.stride = sizeof(Val);
    ca.offset = 0;
    ca.size = 0;
    ca.capacity = CAPACITY;
    ca.data = std::malloc(sizeof(Val) * CAPACITY);

    Zeta_CircularArray_DeploySeqContainer(&ca, seq_cntr);
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

StdAllocator node_allocator_;
Zeta_Allocator node_allocator;

StdAllocator seg_allocator_;
Zeta_Allocator seg_allocator;

Zeta_SegVector sv;

bool_t sv_inited{ FALSE };

void InitSV(Zeta_SeqContainer* seq_cntr) {
    sv_inited = TRUE;

    StdAllocator_DeployAllocator(&node_allocator_, &node_allocator);
    StdAllocator_DeployAllocator(&seg_allocator_, &seg_allocator);

    sv.width = sizeof(Val);
    sv.stride = sizeof(Val);
    sv.seg_capacity = 6;
    sv.node_allocator = &node_allocator;
    sv.seg_allocator = &seg_allocator;

    Zeta_SegVector_Init(&sv);
    Zeta_SegVector_DeploySeqContainer(&sv, seq_cntr);
}

void SV_Check(Zeta_SeqContainer* seq_cntr);

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

size_t SC_GetSize(Zeta_SeqContainer* seq_cntr) {
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

    void* ele = seq_cntr->PushL(seq_cntr->context, &pos_cursor);

    void* re_ele = seq_cntr->Refer(seq_cntr->context, &pos_cursor);

    ZETA_DebugAssert(seq_cntr->Cursor_GetIdx(seq_cntr->context, &pos_cursor) ==
                     0);
    ZETA_DebugAssert(ele == re_ele);

    *(Val*)ele = val;

    return (Val*)ele;
}

Val* SC_PushR(Zeta_SeqContainer* seq_cntr, Val val) {
    Zeta_Cursor pos_cursor;

    void* ele = seq_cntr->PushL(seq_cntr->context, &pos_cursor);

    void* re_ele = seq_cntr->Refer(seq_cntr->context, &pos_cursor);

    ZETA_DebugAssert(seq_cntr->Cursor_GetIdx(seq_cntr->context, &pos_cursor) ==
                     SC_GetSize(seq_cntr) - 1);

    ZETA_DebugAssert(ele == re_ele);

    *(Val*)ele = val;

    return (Val*)ele;
}

void SC_Read(Zeta_SeqContainer* seq_cntr, size_t idx, size_t cnt, Val* dst) {
    Zeta_Cursor pos_cursor;

    seq_cntr->Access(seq_cntr->context, &pos_cursor, NULL, idx);

    ZETA_DebugAssert(seq_cntr->Cursor_GetIdx(seq_cntr->context, &pos_cursor) ==
                     idx);

    seq_cntr->Read(seq_cntr->context, &pos_cursor, cnt, dst, &pos_cursor);

    ZETA_DebugAssert(seq_cntr->Cursor_GetIdx(seq_cntr->context, &pos_cursor) ==
                     idx + cnt);
}

void SC_Write(Zeta_SeqContainer* seq_cntr, size_t idx, size_t cnt,
              Val const* src) {
    Zeta_Cursor pos_cursor;

    seq_cntr->Access(seq_cntr->context, &pos_cursor, NULL, idx);

    ZETA_DebugAssert(seq_cntr->Cursor_GetIdx(seq_cntr->context, &pos_cursor) ==
                     idx);

    seq_cntr->Write(seq_cntr->context, &pos_cursor, cnt, src, &pos_cursor);

    ZETA_DebugAssert(seq_cntr->Cursor_GetIdx(seq_cntr->context, &pos_cursor) ==
                     idx + cnt);
}

void SC_Insert(Zeta_SeqContainer* seq_cntr, size_t idx, size_t cnt,
               Val const* src) {
    Zeta_Cursor pos_cursor;

    seq_cntr->Access(seq_cntr->context, &pos_cursor, NULL, idx);

    ZETA_DebugAssert(seq_cntr->Cursor_GetIdx(seq_cntr->context, &pos_cursor) ==
                     idx);

    seq_cntr->Insert(seq_cntr->context, &pos_cursor, cnt);

    SV_Check(seq_cntr);

    ZETA_DebugAssert(seq_cntr->Cursor_GetIdx(seq_cntr->context, &pos_cursor) ==
                     idx);

    seq_cntr->Write(seq_cntr->context, &pos_cursor, cnt, src, &pos_cursor);

    SV_Check(seq_cntr);

    ZETA_DebugAssert(seq_cntr->Cursor_GetIdx(seq_cntr->context, &pos_cursor) ==
                     idx + cnt);
}

void SC_Erase(Zeta_SeqContainer* seq_cntr, size_t idx, size_t cnt) {
    Zeta_Cursor pos_cursor;

    seq_cntr->Access(seq_cntr->context, &pos_cursor, NULL, idx);

    ZETA_DebugAssert(seq_cntr->Cursor_GetIdx(seq_cntr->context, &pos_cursor) ==
                     idx);

    seq_cntr->Erase(seq_cntr->context, &pos_cursor, cnt);

    SV_Check(seq_cntr);

    ZETA_DebugAssert(seq_cntr->Cursor_GetIdx(seq_cntr->context, &pos_cursor) ==
                     idx);
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

void SV_Check(Zeta_SeqContainer* seq_cntr) {
    if (seq_cntr->GetSize != Zeta_SegVector_GetSize) { return; }

    Zeta_DebugHashTable node_hm;
    Zeta_DebugHashTable seg_hm;

    Zeta_DebugHashTable_Init(&node_hm);
    Zeta_DebugHashTable_Init(&seg_hm);

    Zeta_SegVector_Check(&sv, &node_hm, &seg_hm);

    using record_t = std::unordered_map<unsigned long long, unsigned long long>;

    CheckRecords(node_allocator_.records, *(record_t*)node_hm.hash_map);

    CheckRecords(seg_allocator_.records, *(record_t*)seg_hm.hash_map);
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

    long long lb{ std::max(min_cnt,
                           static_cast<long long>(std::floor(avg - range))) };
    long long rb{ std::min(max_cnt,
                           static_cast<long long>(std::ceil(avg + range))) };

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

std::vector<Val> vals_a;
std::vector<Val> vals_b;

void SyncInsert(Zeta_SeqContainer* seq_cntr_a, Zeta_SeqContainer* seq_cntr_b) {
    size_t size_a{ SC_GetSize(seq_cntr_a) };
    size_t size_b{ SC_GetSize(seq_cntr_b) };

    ZETA_DebugAssert(size_a == size_b);

    std::pair<size_t, size_t> idx_cnt{ GetRandomInsertIdxCnt(size_a) };
    size_t idx{ idx_cnt.first };
    size_t cnt{ idx_cnt.second };

    vals_a.resize(cnt);
    GetRandomVal(vals_a);

    SC_Insert(seq_cntr_a, idx, vals_a.size(), vals_a.data());

    SC_Insert(seq_cntr_b, idx, vals_a.size(), vals_a.data());
}

void SyncErase(Zeta_SeqContainer* seq_cntr_a, Zeta_SeqContainer* seq_cntr_b) {
    size_t size_a{ SC_GetSize(seq_cntr_a) };
    size_t size_b{ SC_GetSize(seq_cntr_b) };

    ZETA_DebugAssert(size_a == size_b);

    std::pair<size_t, size_t> idx_cnt{ GetRandomEraseIdxCnt(size_a) };
    size_t idx{ idx_cnt.first };
    size_t cnt{ idx_cnt.second };

    SC_Erase(seq_cntr_a, idx, cnt);

    SC_Erase(seq_cntr_b, idx, cnt);
}

void SyncCompare(Zeta_SeqContainer* seq_cntr_a, Zeta_SeqContainer* seq_cntr_b) {
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
    unsigned seed = time(NULL);
    // unsigned seed = 1717313193;

    ZETA_PrintVar(seed);

    en.seed(seed);

    Zeta_SeqContainer seq_cntr_a;
    InitDD(&seq_cntr_a);

    Zeta_SeqContainer seq_cntr_b;
    InitSV(&seq_cntr_b);

    for (size_t test_i = 0; test_i < 10000; ++test_i) {
        ZETA_PrintVar(test_i);

        SyncInsert(&seq_cntr_a, &seq_cntr_b);

        Compare(&seq_cntr_a, &seq_cntr_b);

        CheckCursor(&seq_cntr_a);
        CheckCursor(&seq_cntr_b);

        SyncErase(&seq_cntr_a, &seq_cntr_b);

        Compare(&seq_cntr_a, &seq_cntr_b);
        CheckCursor(&seq_cntr_a);
        CheckCursor(&seq_cntr_b);

        for (size_t i{ 0 }; i < 5; ++i) {
            SyncCompare(&seq_cntr_a, &seq_cntr_b);
        }
    }
}

int main() {
    main2();
    std::cout << "ok\n";

    printf(__PRETTY_FUNCTION__);
}

/*

SegVector Insertion Strategy

If m_node can contain more cnt elements:
    Directly insert cnt elements into m_node.
    return

Choose the more vacant side of m_node, k_node

If m_node and k_node can contain more cnt elements:
    Directly insert cnt elements into m_node and k_node (Shove Operation).
    return

If the insertion point is not at the begining of m_node:
    Split m_node into ml_node and mr_node.

    If ml_node and mr_node can contain more cnt elements:
        Insert cnt elements into ml_node and mr_node (Shove Operation).
        return
Else:
    Insert cnt elements into ml_node and mr_node (Shove Operation).

*/
