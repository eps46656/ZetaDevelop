#pragma once

#include <DummyVector.h>
#include <StageVector.h>

#include <cstdlib>

#include "SeqContainerUtils.h"
#include "StdAllocator.h"

struct StageVectorPack {
    StdAllocator seg_allocator_;
    Zeta_Allocator seg_allocator;

    StdAllocator data_allocator_;
    Zeta_Allocator data_allocator;

    Zeta_DummyVector dummy_vec;
    Zeta_SeqContainer dummy_vec_seq_cntr;

    Zeta_StageVector stage_vector;
};

template <typename Elem>
void StageVector_Init(Zeta_SeqContainer* seq_cntr,
                      Zeta_SeqContainer* origin_seq_cntr, size_t seg_capacity) {
    if (origin_seq_cntr != NULL) {
        ZETA_DebugAssert(sizeof(Elem) <=
                         ZETA_SeqContainer_GetStride(origin_seq_cntr));
    }

    StageVectorPack* pack{ static_cast<StageVectorPack*>(
        std::malloc(sizeof(StageVectorPack))) };

    if (origin_seq_cntr == NULL) {
        pack->dummy_vec.width = sizeof(Elem);
        pack->dummy_vec.stride = sizeof(Elem);

        Zeta_DummyVector_DeploySeqContainer(&pack->dummy_vec,
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

    Zeta_StageVector_DeploySeqContainer(&pack->stage_vector, seq_cntr);

    SeqContainerUtils_AddSanitizeFunc(Zeta_StageVector_GetWidth,
                                      StageVector_Sanitize);
}

void StageVector_Deinit(Zeta_SeqContainer* seq_cntr) {
    if (seq_cntr == NULL || seq_cntr->GetSize != Zeta_StageVector_GetSize) {
        return;
    }

    StageVectorPack* pack{ ZETA_MemberToStruct(StageVectorPack, stage_vector,
                                               seq_cntr->context) };

    Zeta_StageVector_Deinit(seq_cntr->context);

    pack->seg_allocator_.~StdAllocator();
    pack->data_allocator_.~StdAllocator();

    std::free(pack);
}

template <typename Elem>
Zeta_SeqContainer* StageVector_Create(Zeta_SeqContainer* origin_seq_cntr,
                                      size_t seg_capacity) {
    Zeta_SeqContainer* seq_cntr{ new Zeta_SeqContainer{} };
    StageVector_Init<Elem>(seq_cntr, origin_seq_cntr, seg_capacity);
    return seq_cntr;
}

void StageVector_Destroy(Zeta_SeqContainer* seq_cntr) {
    if (seq_cntr == NULL || seq_cntr->GetSize != Zeta_StageVector_GetSize) {
        return;
    }

    StageVector_Deinit(seq_cntr);

    delete seq_cntr;
}

void StageVector_Sanitize(Zeta_SeqContainer const* seq_cntr) {
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
