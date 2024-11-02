#pragma once

#include <dummy_vector.h>
#include <stage_vector.h>

#include <cstdlib>
#include <memory>

#include "seq_cntr_utils.h"
#include "std_allocator.h"

struct StageVectorPack {
    StdAllocator seg_allocator_;
    Zeta_Allocator seg_allocator;

    StdAllocator data_allocator_;
    Zeta_Allocator data_allocator;

    Zeta_DummyVector dummy_vec;
    Zeta_SeqCntr dummy_vec_seq_cntr;

    Zeta_StageVector stage_vector;
};

template <typename Elem>
void StageVector_Init(Zeta_SeqCntr* seq_cntr, Zeta_SeqCntr* origin_seq_cntr,
                      size_t seg_capacity) {
    if (origin_seq_cntr != NULL) {
        ZETA_DebugAssert(sizeof(Elem) <=
                         ZETA_SeqCntr_GetWidth(origin_seq_cntr));
    }

    StageVectorPack* pack{ static_cast<StageVectorPack*>(
        std::malloc(sizeof(StageVectorPack))) };

    if (origin_seq_cntr == NULL) {
        pack->dummy_vec.width = sizeof(Elem);

        Zeta_DummyVector_DeploySeqCntr(&pack->dummy_vec,
                                       &pack->dummy_vec_seq_cntr);

        origin_seq_cntr = &pack->dummy_vec_seq_cntr;
    }

    new (&pack->seg_allocator_) StdAllocator{};
    new (&pack->data_allocator_) StdAllocator{};

    StdAllocator_DeployAllocator(&pack->seg_allocator_, &pack->seg_allocator);
    StdAllocator_DeployAllocator(&pack->data_allocator_, &pack->data_allocator);

    pack->stage_vector.origin = origin_seq_cntr;
    pack->stage_vector.seg_capacity = seg_capacity;
    pack->stage_vector.seg_allocator = &pack->seg_allocator;
    pack->stage_vector.data_allocator = &pack->data_allocator;

    Zeta_StageVector_Init(&pack->stage_vector);

    Zeta_StageVector_DeploySeqCntr(&pack->stage_vector, seq_cntr);

    SeqCntrUtils_AddSanitizeFunc(Zeta_StageVector_GetWidth,
                                 StageVector_Sanitize);

    SeqCntrUtils_AddDestroyFunc(Zeta_StageVector_GetWidth, StageVector_Destroy);
}

void StageVector_Deinit(Zeta_SeqCntr* seq_cntr) {
    if (seq_cntr == NULL || seq_cntr->GetSize != Zeta_StageVector_GetSize) {
        return;
    }

    StageVectorPack* pack{ ZETA_MemberToStruct(StageVectorPack, stage_vector,
                                               seq_cntr->context) };

    Zeta_StageVector_Deinit(seq_cntr->context);

    std::free(pack);
}

template <typename Elem>
Zeta_SeqCntr* StageVector_Create(Zeta_SeqCntr* origin_seq_cntr,
                                 size_t seg_capacity) {
    Zeta_SeqCntr* seq_cntr{ new Zeta_SeqCntr{} };

    StageVector_Init<Elem>(seq_cntr, origin_seq_cntr, seg_capacity);

    return seq_cntr;
}

void StageVector_Destroy(Zeta_SeqCntr* seq_cntr) {
    if (seq_cntr == NULL || seq_cntr->GetSize != Zeta_StageVector_GetSize) {
        return;
    }

    StageVector_Deinit(seq_cntr);

    delete seq_cntr;
}

void StageVector_Sanitize(Zeta_SeqCntr const* seq_cntr) {
    if (seq_cntr->GetSize != Zeta_StageVector_GetSize) { return; }

    StageVectorPack* pack{ ZETA_MemberToStruct(StageVectorPack, stage_vector,
                                               seq_cntr->context) };

    Zeta_MemRecorder* seg = Zeta_MemRecorder_Create();
    Zeta_MemRecorder* data = Zeta_MemRecorder_Create();

    Zeta_StageVector_Sanitize(
        const_cast<void*>(static_cast<void const*>(&pack->stage_vector)), seg,
        data);

    Zeta_MemCheck_MatchRecords(pack->seg_allocator_.mem_recorder, seg);
    Zeta_MemCheck_MatchRecords(pack->data_allocator_.mem_recorder, data);

    Zeta_MemRecorder_Destroy(seg);
    Zeta_MemRecorder_Destroy(data);
}
