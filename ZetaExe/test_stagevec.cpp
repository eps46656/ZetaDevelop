#include <deque>
#include <iostream>
#include <random>
#include <set>

#include "../Zeta/CheckPointMeter.h"
#include "../Zeta/CircularVector.h"
#include "../Zeta/DebugDeque.h"
#include "../Zeta/Debugger.h"
#include "../Zeta/SegVector.h"
#include "../Zeta/StageVector.h"
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

#define CAPACITY (256 * 1024 * 1024)

void InitCV(Zeta_SeqContainer* seq_cntr, size_t capacity) {
    Zeta_CircularVector* cv{ static_cast<Zeta_CircularVector*>(
        std::malloc(sizeof(Zeta_CircularVector))) };

    cv->data = std::malloc(sizeof(Val) * capacity);
    cv->width = sizeof(Val);
    cv->stride = sizeof(Val);
    cv->offset = 0;
    cv->size = 0;
    cv->capacity = capacity;

    Zeta_CircularVector_DeploySeqContainer(cv, seq_cntr);
}

Zeta_SeqContainer* CreateCV(size_t capacity) {
    Zeta_SeqContainer* seq_cntr{ new Zeta_SeqContainer{} };
    InitCV(seq_cntr, capacity);
    return seq_cntr;
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

struct SegVectorPack {
    StdAllocator node_allocator_;
    Zeta_Allocator node_allocator;

    StdAllocator seg_allocator_;
    Zeta_Allocator seg_allocator;

    Zeta_SegVector sv;
};

void InitSV(Zeta_SeqContainer* seq_cntr) {
    SegVectorPack* sv_pack{ static_cast<SegVectorPack*>(
        std::malloc(sizeof(SegVectorPack))) };

    new (&sv_pack->node_allocator_) StdAllocator{};
    new (&sv_pack->seg_allocator_) StdAllocator{};

    StdAllocator_DeployAllocator(&sv_pack->node_allocator_,
                                 &sv_pack->node_allocator);
    StdAllocator_DeployAllocator(&sv_pack->seg_allocator_,
                                 &sv_pack->seg_allocator);

    sv_pack->sv.width = sizeof(Val);
    sv_pack->sv.stride = sizeof(Val);
    sv_pack->sv.seg_capacity = 6;
    sv_pack->sv.node_allocator = &sv_pack->node_allocator;
    sv_pack->sv.seg_allocator = &sv_pack->seg_allocator;

    Zeta_SegVector_Init(&sv_pack->sv);
    Zeta_SegVector_DeploySeqContainer(&sv_pack->sv, seq_cntr);
}

Zeta_SeqContainer* CreateSV() {
    Zeta_SeqContainer* seq_cntr{ new Zeta_SeqContainer{} };
    InitSV(seq_cntr);
    return seq_cntr;
}

void SV_Check(Zeta_SeqContainer* seq_cntr);

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

struct StageVectorPack {
    StdAllocator node_allocator_;
    Zeta_Allocator node_allocator;

    StdAllocator data_allocator_;
    Zeta_Allocator data_allocator;

    Zeta_StageVector stage_vec;
};

void InitStageVec(Zeta_SeqContainer* seq_cntr,
                  Zeta_SeqContainer* origin_seq_cntr) {
    StageVectorPack* stage_vec_pack{ static_cast<StageVectorPack*>(
        std::malloc(sizeof(StageVectorPack))) };

    new (&stage_vec_pack->node_allocator_) StdAllocator{};
    new (&stage_vec_pack->data_allocator_) StdAllocator{};

    StdAllocator_DeployAllocator(&stage_vec_pack->node_allocator_,
                                 &stage_vec_pack->node_allocator);
    StdAllocator_DeployAllocator(&stage_vec_pack->data_allocator_,
                                 &stage_vec_pack->data_allocator);

    stage_vec_pack->stage_vec.origin = origin_seq_cntr;
    stage_vec_pack->stage_vec.seg_capacity = 6;
    stage_vec_pack->stage_vec.node_allocator = &stage_vec_pack->node_allocator;
    stage_vec_pack->stage_vec.data_allocator = &stage_vec_pack->data_allocator;

    Zeta_StageVector_Init(&stage_vec_pack->stage_vec);
    Zeta_StageVector_DeploySeqContainer(&stage_vec_pack->stage_vec, seq_cntr);
}

Zeta_SeqContainer* CreateStageVec(Zeta_SeqContainer* origin_seq_cntr) {
    Zeta_SeqContainer* seq_cntr{ new Zeta_SeqContainer{} };
    InitStageVec(seq_cntr, origin_seq_cntr);
    return seq_cntr;
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

void SC_Check(Zeta_SeqContainer* seq_cntr);

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

    SC_Check(seq_cntr);

    ZETA_DebugAssert(seq_cntr->Cursor_GetIdx(seq_cntr->context, &pos_cursor) ==
                     idx + cnt);
}

void SC_Insert(Zeta_SeqContainer* seq_cntr, size_t idx, size_t cnt,
               Val const* src) {
    Zeta_Cursor pos_cursor;

    ZETA_RecordStdCheckPoint;

    seq_cntr->Access(seq_cntr->context, &pos_cursor, NULL, idx);

    ZETA_RecordStdCheckPoint;

    ZETA_DebugAssert(seq_cntr->Cursor_GetIdx(seq_cntr->context, &pos_cursor) ==
                     idx);

    ZETA_RecordStdCheckPoint;

    void* ele = seq_cntr->Insert(seq_cntr->context, &pos_cursor, cnt);

    ZETA_RecordStdCheckPoint;

    SC_Check(seq_cntr);

    ZETA_DebugAssert(seq_cntr->Cursor_GetIdx(seq_cntr->context, &pos_cursor) ==
                     idx);
    ZETA_DebugAssert(seq_cntr->Refer(seq_cntr->context, &pos_cursor) == ele);

    ZETA_RecordStdCheckPoint;

    seq_cntr->Write(seq_cntr->context, &pos_cursor, cnt, src, &pos_cursor);

    ZETA_RecordStdCheckPoint;

    SC_Check(seq_cntr);

    ZETA_RecordStdCheckPoint;

    ZETA_DebugAssert(seq_cntr->Cursor_GetIdx(seq_cntr->context, &pos_cursor) ==
                     idx + cnt);

    ZETA_RecordStdCheckPoint;
}

void SC_Erase(Zeta_SeqContainer* seq_cntr, size_t idx, size_t cnt) {
    Zeta_Cursor pos_cursor;

    ZETA_RecordStdCheckPoint;

    seq_cntr->Access(seq_cntr->context, &pos_cursor, NULL, idx);

    ZETA_RecordStdCheckPoint;

    SC_Check(seq_cntr);

    ZETA_RecordStdCheckPoint;

    ZETA_DebugAssert(seq_cntr->Cursor_GetIdx(seq_cntr->context, &pos_cursor) ==
                     idx);

    ZETA_RecordStdCheckPoint;

    seq_cntr->Erase(seq_cntr->context, &pos_cursor, cnt);

    ZETA_RecordStdCheckPoint;

    SC_Check(seq_cntr);

    ZETA_RecordStdCheckPoint;

    ZETA_DebugAssert(seq_cntr->Cursor_GetIdx(seq_cntr->context, &pos_cursor) ==
                     idx);

    ZETA_RecordStdCheckPoint;
}

void SC_Print(Zeta_SeqContainer* seq_cntr) {
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

void SV_Check(Zeta_SeqContainer* seq_cntr) {
    if (seq_cntr->GetSize != Zeta_SegVector_GetSize) { return; }

    SegVectorPack* pack{ ZETA_MemberToStruct(SegVectorPack, sv,
                                             seq_cntr->context) };

    Zeta_DebugHashMap node_hm;
    Zeta_DebugHashMap seg_hm;

    Zeta_DebugHashMap_Create(&node_hm);
    Zeta_DebugHashMap_Create(&seg_hm);

    Zeta_SegVector_Check(&pack->sv, &node_hm, &seg_hm);

    using record_t = std::unordered_map<unsigned long long, unsigned long long>;

    CheckRecords(pack->node_allocator_.records, *(record_t*)node_hm.hash_map);

    CheckRecords(pack->seg_allocator_.records, *(record_t*)seg_hm.hash_map);
}

void StageVec_Check(Zeta_SeqContainer* seq_cntr) {
    if (seq_cntr->GetSize != Zeta_StageVector_GetSize) { return; }

    StageVectorPack* pack{ ZETA_MemberToStruct(StageVectorPack, stage_vec,
                                               seq_cntr->context) };

    Zeta_DebugHashMap node_hm;
    Zeta_DebugHashMap seg_hm;

    Zeta_DebugHashMap_Create(&node_hm);
    Zeta_DebugHashMap_Create(&seg_hm);

    Zeta_StageVector_Check(&pack->stage_vec, &node_hm, &seg_hm);

    using record_t = std::unordered_map<unsigned long long, unsigned long long>;

    CheckRecords(pack->node_allocator_.records, *(record_t*)node_hm.hash_map);

    CheckRecords(pack->data_allocator_.records, *(record_t*)seg_hm.hash_map);
}

void SC_Check(Zeta_SeqContainer* seq_cntr) {
    if (seq_cntr->GetSize == Zeta_SegVector_GetSize) {
        SV_Check(seq_cntr);
        return;
    }

    if (seq_cntr->GetSize == Zeta_StageVector_GetSize) {
        StageVec_Check(seq_cntr);
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

#define MAX_OP_SIZE (128)

std::vector<Val> vals_a;
std::vector<Val> vals_b;

void SyncInsert(Zeta_SeqContainer* seq_cntr_a, Zeta_SeqContainer* seq_cntr_b) {
    size_t size_a{ SC_GetSize(seq_cntr_a) };
    size_t size_b{ SC_GetSize(seq_cntr_b) };

    ZETA_DebugAssert(size_a == size_b);

    size_t cnt_lb{ 0 };
    size_t cnt_rb{ MAX_OP_SIZE };

    size_t idx{ size_generator(en) % (size_a + 1) };
    size_t cnt{ size_generator(en) % (cnt_rb - cnt_lb + 1) + cnt_lb };

    vals_a.resize(cnt);
    GetRandomVal(vals_a);

    ZETA_RecordStdCheckPoint;

    SC_Insert(seq_cntr_a, idx, vals_a.size(), vals_a.data());

    ZETA_RecordStdCheckPoint;

    SC_Insert(seq_cntr_b, idx, vals_a.size(), vals_a.data());

    ZETA_RecordStdCheckPoint;
}

void SyncErase(Zeta_SeqContainer* seq_cntr_a, Zeta_SeqContainer* seq_cntr_b) {
    long long size_a{ static_cast<long long>(SC_GetSize(seq_cntr_a)) };
    long long size_b{ static_cast<long long>(SC_GetSize(seq_cntr_b)) };

    ZETA_DebugAssert(size_a == size_b);

    long long cnt_lb{ 0 };
    long long cnt_rb{ std::min<long long>(MAX_OP_SIZE, size_a) };

    long long cnt{ size_generator(en) % (cnt_rb - cnt_lb + 1) + cnt_lb };
    long long idx{ size_generator(en) % (size_a + 1 - cnt) };

    ZETA_RecordStdCheckPoint;

    SC_Erase(seq_cntr_a, idx, cnt);

    ZETA_RecordStdCheckPoint;

    SC_Erase(seq_cntr_b, idx, cnt);

    ZETA_RecordStdCheckPoint;
}

void SyncWrite(Zeta_SeqContainer* seq_cntr_a, Zeta_SeqContainer* seq_cntr_b) {
    long long size_a{ static_cast<long long>(SC_GetSize(seq_cntr_a)) };
    long long size_b{ static_cast<long long>(SC_GetSize(seq_cntr_b)) };

    ZETA_DebugAssert(size_a == size_b);

    long long cnt_lb{ 0 };
    long long cnt_rb{ std::min<long long>(MAX_OP_SIZE, size_a) };

    long long cnt{ size_generator(en) % (cnt_rb - cnt_lb + 1) + cnt_lb };
    long long idx{ size_generator(en) % (size_a + 1 - cnt) };

    vals_a.resize(cnt);
    GetRandomVal(vals_a);

    SC_Write(seq_cntr_a, idx, cnt, vals_a.data());

    SC_Write(seq_cntr_b, idx, cnt, vals_a.data());
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
    unsigned int random_seed = time(NULL);
    unsigned int fixed_seed = 1718129024;

    unsigned int seed = random_seed;
    // unsigned int seed = fixed_seed;

    ZETA_PrintVar(random_seed);
    ZETA_PrintVar(fixed_seed);
    ZETA_PrintVar(seed);

    en.seed(seed);

    Zeta_SeqContainer* seq_cntr_a{ CreateDD() };
    Zeta_SeqContainer* seq_cntr_a_copy{ CreateDD() };

    vals_a.resize(1024);
    GetRandomVal(vals_a);
    SC_Insert(seq_cntr_a, 0, vals_a.size(), vals_a.data());

    SC_Assign(seq_cntr_a, seq_cntr_a_copy);

    Zeta_SeqContainer* seq_cntr_b{ CreateStageVec(seq_cntr_a_copy) };

    for (size_t test_i = 0; test_i < 1024; ++test_i) {
        ZETA_PrintVar(test_i);

        ZETA_PrintPos;

        SyncInsert(seq_cntr_a, seq_cntr_b);
        Compare(seq_cntr_a, seq_cntr_b);
        CheckCursor(seq_cntr_a);
        CheckCursor(seq_cntr_b);

        Zeta_CheckPointMeter_Clear(std_check_point_recorder);

        ZETA_PrintVar(SC_GetSize(seq_cntr_a));

        ZETA_PrintPos;

        ZETA_PrintPos;

        SyncWrite(seq_cntr_a, seq_cntr_b);
        Compare(seq_cntr_a, seq_cntr_b);
        CheckCursor(seq_cntr_a);
        CheckCursor(seq_cntr_b);

        Zeta_CheckPointMeter_Clear(std_check_point_recorder);

        ZETA_PrintVar(SC_GetSize(seq_cntr_a));

        ZETA_PrintPos;

        // SyncErase(seq_cntr_a, seq_cntr_b);
        // Compare(seq_cntr_a, seq_cntr_b);
        // CheckCursor(seq_cntr_a);
        // CheckCursor(seq_cntr_b);

        Zeta_CheckPointMeter_Clear(std_check_point_recorder);

        ZETA_PrintVar(SC_GetSize(seq_cntr_a));

        ZETA_PrintPos;

        for (size_t i{ 0 }; i < 5; ++i) { SyncCompare(seq_cntr_a, seq_cntr_b); }

        // Zeta_StageVector_PrintState(seq_cntr_b->context);
    }
}

void main3() {
    ZETA_RecordStdCheckPoint;

    int a = 3;

    ZETA_PrintVar(a);
    ZETA_PrintVar(a + 5);

    ZETA_RecordStdCheckPoint;

    ZETA_DebugAssert(FALSE);
}

int main() {
    main2();
    printf("ok\a\n");
    return 0;
}

/*

SegVector Insertion Strategy

If m_node can contain more cnt elements:
    Directly insert cnt elements into m_node.
    return

Choose the more vacant side of m_node as k_node

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
