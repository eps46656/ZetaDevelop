#include <debug_deque.h>
#include <debugger.h>
#include <seg_vector.h>
#include <staging_vector.h>
#include <utils.h>

#include <chrono>
#include <deque>
#include <iostream>
#include <map>
#include <random>

#include "std_allocator.h"

struct Val {
    static constexpr size_t width{ 4 };

    unsigned char data[width];
};

#define CURSOR_WIDTH ((size_t)(sizeof(void*) * 8))

unsigned long long HashVal(const Val& val) {
    std::hash<unsigned char> hash;

    unsigned long long ret{ 0 };

    for (size_t i{ 0 }; i < Val::width; ++i) { ret ^= hash(val.data[i]); }

    return ret;
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

std::mt19937_64 en;

std::uniform_int_distribution<unsigned long long> ull_generator{ 0,
                                                                 ULLONG_MAX };

unsigned long long RandInt(unsigned long long lb, unsigned long long rb) {
    return ull_generator(en) % (rb - lb + 1) + lb;
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

void GetRandomVal(Val* dst) {
    for (size_t i{ 0 }; i < Val::width; ++i) { dst->data[i] = RandInt(0, 255); }
}

void GetRandomVals(std::vector<Val>& dst) {
    for (size_t i{ 0 }; i < dst.size(); ++i) { GetRandomVal(&dst[i]); }
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

#define SEG_CAPACITY (32)

struct StagingVectorUtils_Pack {
    StdAllocator seg_allocator_;
    Zeta_Allocator seg_allocator;

    StdAllocator data_allocator_;
    Zeta_Allocator data_allocator;

    Zeta_StagingVector stage_vec;
};

void InitStageVec(Zeta_SeqCntr* seq_cntr, Zeta_SeqCntr* origin_seq_cntr) {
    StagingVectorUtils_Pack* stage_vec_pack{
        static_cast<StagingVectorUtils_Pack*>(
            std::malloc(sizeof(StagingVectorUtils_Pack)))
    };

    ZETA_PrintCurPos;

    new (&stage_vec_pack->seg_allocator_) StdAllocator{};
    new (&stage_vec_pack->data_allocator_) StdAllocator{};

    StdAllocator_DeployAllocator(&stage_vec_pack->seg_allocator_,
                                 &stage_vec_pack->seg_allocator);
    StdAllocator_DeployAllocator(&stage_vec_pack->data_allocator_,
                                 &stage_vec_pack->data_allocator);

    ZETA_PrintCurPos;

    stage_vec_pack->stage_vec.origin = origin_seq_cntr;
    stage_vec_pack->stage_vec.seg_capacity = SEG_CAPACITY;
    stage_vec_pack->stage_vec.seg_allocator = &stage_vec_pack->seg_allocator;
    stage_vec_pack->stage_vec.data_allocator = &stage_vec_pack->data_allocator;

    ZETA_PrintCurPos;

    Zeta_StagingVector_Init(&stage_vec_pack->stage_vec);

    ZETA_PrintCurPos;

    Zeta_StagingVector_DeploySeqCntr(&stage_vec_pack->stage_vec, seq_cntr);

    ZETA_PrintCurPos;
}

Zeta_SeqCntr* CreateStageVec(Zeta_SeqCntr* origin_seq_cntr) {
    Zeta_SeqCntr* seq_cntr{ new Zeta_SeqCntr{} };
    InitStageVec(seq_cntr, origin_seq_cntr);
    return seq_cntr;
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

void InitDD(Zeta_SeqCntr* seq_cntr) {
    Zeta_DebugDeque* dd{ static_cast<Zeta_DebugDeque*>(
        std::malloc(sizeof(Zeta_DebugDeque))) };

    dd->width = sizeof(Val);
    dd->width = sizeof(Val);

    Zeta_DebugDeque_Init(dd);
    Zeta_DebugDeque_DeploySeqCntr(dd, seq_cntr);
}

Zeta_SeqCntr* CreateDD() {
    Zeta_SeqCntr* seq_cntr{ new Zeta_SeqCntr{} };
    InitDD(seq_cntr);
    return seq_cntr;
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

/*
void SC_Print(Zeta_SeqCntr* seq_cntr) {
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
*/

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

#define MAX_OP_SIZE (64)

Val* SC_RandomAccess(Zeta_SeqCntr* seq_cntr) {
    size_t size = seq_cntr->GetSize(seq_cntr->context);

    size_t idx = ull_generator(en) % size;

    return (Val*)seq_cntr->Access(seq_cntr->context, NULL, NULL, idx);
}

unsigned long long SC_RandomRead(Zeta_SeqCntr* seq_cntr) {
    size_t size = seq_cntr->GetSize(seq_cntr->context);

    size_t idx = ull_generator(en) % size;
    size_t cnt = ZETA_GetMinOf(size - idx, RandInt(0, MAX_OP_SIZE));

    Zeta_Cursor cursor;
    seq_cntr->Access(seq_cntr->context, &cursor, NULL, idx);

    Val val;

    unsigned long long ret{ 0 };

    for (size_t i{ 0 }; i < cnt; ++i) {
        seq_cntr->Read(seq_cntr->context, &cursor, 1, &val, &cursor);
        ret ^= HashVal(val);
    }

    return ret;
}

void SC_RandomWrite(Zeta_SeqCntr* seq_cntr) {
    size_t size = seq_cntr->GetSize(seq_cntr->context);

    size_t idx = RandInt(0, size);
    size_t cnt = ZETA_GetMinOf(size - idx, RandInt(0, MAX_OP_SIZE));

    std::vector<Val> vals;
    vals.resize(cnt);

    GetRandomVals(vals);

    Zeta_Cursor cursor;
    seq_cntr->Access(seq_cntr->context, &cursor, NULL, idx);

    seq_cntr->Write(seq_cntr->context, &cursor, cnt, vals.data(), NULL);
}

void SC_RandomInsert(Zeta_SeqCntr* seq_cntr) {
    size_t size = seq_cntr->GetSize(seq_cntr->context);

    size_t idx = RandInt(0, size);
    size_t cnt = RandInt(0, MAX_OP_SIZE);

    std::vector<Val> vals;
    vals.resize(cnt);

    GetRandomVals(vals);

    Zeta_Cursor cursor;
    seq_cntr->Access(seq_cntr->context, &cursor, NULL, idx);

    seq_cntr->Insert(seq_cntr->context, &cursor, cnt);

    seq_cntr->Write(seq_cntr->context, &cursor, cnt, vals.data(), NULL);
}

void SC_RandomErase(Zeta_SeqCntr* seq_cntr) {
    size_t size = seq_cntr->GetSize(seq_cntr->context);

    size_t idx = RandInt(0, size);
    size_t cnt = ZETA_GetMinOf(size - idx, RandInt(0, MAX_OP_SIZE));

    Zeta_Cursor cursor;
    seq_cntr->Access(seq_cntr->context, &cursor, NULL, idx);

    seq_cntr->Erase(seq_cntr->context, &cursor, cnt);
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

void main1() {
    // unsigned seed = time(NULL);
    unsigned seed = 1721406405;

    ZETA_PrintVar(seed);

    en.seed(seed);

    Zeta_DummyVector dummy_vec_;
    dummy_vec_.width = sizeof(Val);
    dummy_vec_.width = sizeof(Val);

    Zeta_SeqCntr dummy_vec;
    Zeta_DummyVector_DeploySeqCntr(&dummy_vec_, &dummy_vec);

    Zeta_SeqCntr seq_cntr;

    bool_t is_sv = FALSE;

    if (is_sv) {
        InitStageVec(&seq_cntr, &dummy_vec);
    } else {
        InitDD(&seq_cntr);
    }

    std::chrono::steady_clock::time_point begin =
        std::chrono::steady_clock::now();

    unsigned long long ret{ 0 };

    ZETA_PrintCurPos;
    for (int i = 0; i < 1000; ++i) { SC_RandomInsert(&seq_cntr); }
    ZETA_PrintCurPos;

    for (int _ = 0; _ < 10000; ++_) {
        for (int i = 0; i < 200; ++i) { SC_RandomWrite(&seq_cntr); }
        for (int i = 0; i < 200; ++i) { SC_RandomInsert(&seq_cntr); }
        for (int i = 0; i < 200; ++i) { SC_RandomErase(&seq_cntr); }
        for (int i = 0; i < 200; ++i) { ret ^= SC_RandomRead(&seq_cntr); }
    }

    ZETA_PrintVar(ret);

    std::chrono::steady_clock::time_point end =
        std::chrono::steady_clock::now();

    std::cout << "Time difference = "
              << std::chrono::duration_cast<std::chrono::milliseconds>(end -
                                                                       begin)
                     .count()
              << "[ms]\n";

    /*

2137450604401320

    */
}

int main() {
    main1();
    printf("ok\n");
    return 0;
}
