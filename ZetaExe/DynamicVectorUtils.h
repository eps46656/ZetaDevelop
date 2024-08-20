#pragma once

#include <DynamicVector.h>
#include <MemCheck.h>

#include <cstdlib>

#include "SeqContainerUtils.h"
#include "StdAllocator.h"

struct DynamicVectorPack {
    StdAllocator data_allocator_;
    Zeta_Allocator data_allocator;

    StdAllocator seg_allocator_;
    Zeta_Allocator seg_allocator;

    Zeta_DynamicVector dynamic_vec;
};

template <typename Val>
void DynamicVector_Init(Zeta_SeqContainer* seq_cntr, size_t seg_capacity) {
    DynamicVectorPack* dynamic_vec_pack{ static_cast<DynamicVectorPack*>(
        std::malloc(sizeof(DynamicVectorPack))) };

    new (&dynamic_vec_pack->data_allocator_) StdAllocator{};
    new (&dynamic_vec_pack->seg_allocator_) StdAllocator{};

    StdAllocator_DeployAllocator(&dynamic_vec_pack->data_allocator_,
                                 &dynamic_vec_pack->data_allocator);
    StdAllocator_DeployAllocator(&dynamic_vec_pack->seg_allocator_,
                                 &dynamic_vec_pack->seg_allocator);

    dynamic_vec_pack->dynamic_vec.width = sizeof(Val);
    dynamic_vec_pack->dynamic_vec.stride = sizeof(Val);
    dynamic_vec_pack->dynamic_vec.seg_capacity = seg_capacity;

    dynamic_vec_pack->dynamic_vec.data_allocator =
        &dynamic_vec_pack->data_allocator;

    dynamic_vec_pack->dynamic_vec.seg_allocator =
        &dynamic_vec_pack->seg_allocator;

    Zeta_DynamicVector_Init(&dynamic_vec_pack->dynamic_vec);

    Zeta_DynamicVector_DeploySeqContainer(&dynamic_vec_pack->dynamic_vec,
                                          seq_cntr);

    SeqContainer_AddSanitizeFunc(Zeta_DynamicVector_GetWidth,
                                 DynamicVector_Sanitize);
}

void DynamicVector_Deinit(Zeta_SeqContainer* seq_cntr) {
    if (seq_cntr == NULL || seq_cntr->GetSize != Zeta_DynamicVector_GetSize) {
        return;
    }

    DynamicVectorPack* dynamic_vec_pack{ ZETA_MemberToStruct(
        DynamicVectorPack, dynamic_vec, seq_cntr->context) };

    Zeta_DynamicVector_Deinit(seq_cntr->context);

    std::free(dynamic_vec_pack);
}

template <typename Val>
Zeta_SeqContainer* DynamicVector_Create(size_t seg_capacity) {
    Zeta_SeqContainer* seq_cntr{ new Zeta_SeqContainer{} };
    DynamicVector_Init<Val>(seq_cntr, seg_capacity);
    return seq_cntr;
}

void DynamicVector_Destroy(Zeta_SeqContainer* seq_cntr) {
    if (seq_cntr == NULL || seq_cntr->GetSize != Zeta_DynamicVector_GetSize) {
        return;
    }

    DynamicVector_Deinit(seq_cntr);

    delete seq_cntr;
}

void DynamicVector_Sanitize(Zeta_SeqContainer const* seq_cntr) {
    if (seq_cntr->GetSize != Zeta_DynamicVector_GetSize) { return; }

    DynamicVectorPack* pack{ ZETA_MemberToStruct(DynamicVectorPack, dynamic_vec,
                                                 seq_cntr->context) };

    Zeta_MemRecorder* data = Zeta_MemRecorder_Create();
    Zeta_MemRecorder* seg = Zeta_MemRecorder_Create();

    Zeta_DynamicVector_Sanitize(
        const_cast<void*>(static_cast<void const*>(&pack->dynamic_vec)), data,
        seg);

    Zeta_MemCheck_MatchRecords(pack->data_allocator_.mem_recorder, data);
    Zeta_MemCheck_MatchRecords(pack->seg_allocator_.mem_recorder, seg);

    Zeta_MemRecorder_Destroy(data);
    Zeta_MemRecorder_Destroy(seg);
}
