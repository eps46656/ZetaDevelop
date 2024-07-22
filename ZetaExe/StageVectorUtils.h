#pragma once

#include <cstdlib>

#include "../Zeta/StageVector.h"
#include "MemAllocatorCheck.h"
#include "StdAllocator.h"

struct StageVectorPack {
    StdAllocator seg_allocator_;
    Zeta_Allocator seg_allocator;

    StdAllocator data_allocator_;
    Zeta_Allocator data_allocator;

    Zeta_StageVector stage_vec;
};

template <typename Val>
void InitStageVec(Zeta_SeqContainer* seq_cntr,
                  Zeta_SeqContainer* origin_seq_cntr, size_t seg_capacity) {
    StageVectorPack* stage_vec_pack{ static_cast<StageVectorPack*>(
        std::malloc(sizeof(StageVectorPack))) };

    new (&stage_vec_pack->seg_allocator_) StdAllocator{};
    new (&stage_vec_pack->data_allocator_) StdAllocator{};

    StdAllocator_DeployAllocator(&stage_vec_pack->seg_allocator_,
                                 &stage_vec_pack->seg_allocator);
    StdAllocator_DeployAllocator(&stage_vec_pack->data_allocator_,
                                 &stage_vec_pack->data_allocator);

    stage_vec_pack->stage_vec.origin = origin_seq_cntr;
    stage_vec_pack->stage_vec.seg_capacity = seg_capacity;
    stage_vec_pack->stage_vec.seg_allocator = &stage_vec_pack->seg_allocator;
    stage_vec_pack->stage_vec.data_allocator = &stage_vec_pack->data_allocator;

    Zeta_StageVector_Init(&stage_vec_pack->stage_vec);
    Zeta_StageVector_DeploySeqContainer(&stage_vec_pack->stage_vec, seq_cntr);
}

template <typename Val>
Zeta_SeqContainer* CreateStageVec(Zeta_SeqContainer* origin_seq_cntr,
                                  size_t seg_capacity) {
    Zeta_SeqContainer* seq_cntr{ new Zeta_SeqContainer{} };
    InitStageVec<Val>(seq_cntr, origin_seq_cntr, seg_capacity);
    return seq_cntr;
}

void CheckStageVec(Zeta_SeqContainer const* seq_cntr) {
    if (seq_cntr->GetSize != Zeta_StageVector_GetSize) { return; }

    StageVectorPack* pack{ ZETA_MemberToStruct(StageVectorPack, stage_vec,
                                               seq_cntr->context) };

    Zeta_DebugHashMap seg_hm;
    Zeta_DebugHashMap data_hm;

    Zeta_DebugHashMap_Create(&seg_hm);
    Zeta_DebugHashMap_Create(&data_hm);

    Zeta_StageVector_Check(
        const_cast<void*>(static_cast<void const*>(&pack->stage_vec)), &seg_hm,
        &data_hm);

    using record_t = std::unordered_map<unsigned long long, unsigned long long>;

    CheckRecords(pack->seg_allocator_.records, *(record_t*)seg_hm.hash_map);

    CheckRecords(pack->data_allocator_.records, *(record_t*)data_hm.hash_map);
}
