#pragma once

#include <cstdlib>

#include "../Zeta/StageVector.h"
#include "StdAllocator.h"

struct StageVectorPack {
    StdAllocator seg_allocator_;
    Zeta_Allocator seg_allocator;

    StdAllocator data_allocator_;
    Zeta_Allocator data_allocator;

    Zeta_StageVector stage_vector;
};

template <typename Val>
void InitStageVector(Zeta_SeqContainer* seq_cntr,
                     Zeta_SeqContainer* origin_seq_cntr, size_t seg_capacity) {
    StageVectorPack* stage_vector_pack{ static_cast<StageVectorPack*>(
        std::malloc(sizeof(StageVectorPack))) };

    new (&stage_vector_pack->seg_allocator_) StdAllocator{};
    new (&stage_vector_pack->data_allocator_) StdAllocator{};

    StdAllocator_DeployAllocator(&stage_vector_pack->seg_allocator_,
                                 &stage_vector_pack->seg_allocator);
    StdAllocator_DeployAllocator(&stage_vector_pack->data_allocator_,
                                 &stage_vector_pack->data_allocator);

    stage_vector_pack->stage_vector.origin = origin_seq_cntr;
    stage_vector_pack->stage_vector.seg_capacity = seg_capacity;
    stage_vector_pack->stage_vector.seg_allocator =
        &stage_vector_pack->seg_allocator;
    stage_vector_pack->stage_vector.data_allocator =
        &stage_vector_pack->data_allocator;

    Zeta_StageVector_Init(&stage_vector_pack->stage_vector);
    Zeta_StageVector_DeploySeqContainer(&stage_vector_pack->stage_vector,
                                        seq_cntr);
}

void DeinitStageVector(Zeta_SeqContainer* seq_cntr) {
    if (seq_cntr == NULL || seq_cntr->GetSize != Zeta_StageVector_GetSize) {
        return;
    }

    StageVectorPack* stage_vector_pack{ ZETA_MemberToStruct(
        StageVectorPack, stage_vector, seq_cntr->context) };

    Zeta_StageVector_Deinit(seq_cntr->context);

    stage_vector_pack->seg_allocator_.~StdAllocator();
    stage_vector_pack->data_allocator_.~StdAllocator();

    std::free(stage_vector_pack);
}

template <typename Val>
Zeta_SeqContainer* CreateStageVector(Zeta_SeqContainer* origin_seq_cntr,
                                     size_t seg_capacity) {
    Zeta_SeqContainer* seq_cntr{ new Zeta_SeqContainer{} };
    InitStageVector<Val>(seq_cntr, origin_seq_cntr, seg_capacity);
    return seq_cntr;
}

void DestroyStageVector(Zeta_SeqContainer* seq_cntr) {
    if (seq_cntr == NULL || seq_cntr->GetSize != Zeta_StageVector_GetSize) {
        return;
    }

    DeinitStageVector(seq_cntr);

    delete seq_cntr;
}

void CheckStageVector(Zeta_SeqContainer const* seq_cntr) {
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
