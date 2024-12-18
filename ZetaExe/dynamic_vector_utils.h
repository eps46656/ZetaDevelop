#pragma once

#include <dynamic_vector.h>
#include <mem_check_utils.h>

#include <cstdlib>

#include "seq_cntr_utils.h"
#include "std_allocator.h"

struct DynamicVectorPack {
    StdAllocator data_allocator_instance;
    Zeta_Allocator data_allocator;

    StdAllocator seg_allocator_instance;
    Zeta_Allocator seg_allocator;

    Zeta_DynamicVector dynamic_vec;
};

template <typename Val>
void DynamicVector_Init(Zeta_SeqCntr* seq_cntr, size_t seg_capacity) {
    DynamicVectorPack* dynamic_vec_pack{ static_cast<DynamicVectorPack*>(
        std::malloc(sizeof(DynamicVectorPack))) };

    new (&dynamic_vec_pack->data_allocator_instance) StdAllocator{};
    new (&dynamic_vec_pack->seg_allocator_instance) StdAllocator{};

    StdAllocator_DeployAllocator(&dynamic_vec_pack->data_allocator_instance,
                                 &dynamic_vec_pack->data_allocator);
    StdAllocator_DeployAllocator(&dynamic_vec_pack->seg_allocator_instance,
                                 &dynamic_vec_pack->seg_allocator);

    dynamic_vec_pack->dynamic_vec.width = sizeof(Val);
    dynamic_vec_pack->dynamic_vec.seg_capacity = seg_capacity;

    dynamic_vec_pack->dynamic_vec.data_allocator =
        &dynamic_vec_pack->data_allocator;

    dynamic_vec_pack->dynamic_vec.seg_allocator =
        &dynamic_vec_pack->seg_allocator;

    Zeta_DynamicVector_Init(&dynamic_vec_pack->dynamic_vec);

    Zeta_DynamicVector_DeploySeqCntr(&dynamic_vec_pack->dynamic_vec, seq_cntr);

    SeqCntrUtils_AddSanitizeFunc(Zeta_DynamicVector_GetWidth,
                                 DynamicVector_Sanitize);

    SeqCntrUtils_AddDestroyFunc(Zeta_DynamicVector_GetWidth,
                                DynamicVector_Destroy);
}

void DynamicVector_Deinit(Zeta_SeqCntr* seq_cntr) {
    if (seq_cntr == NULL || seq_cntr->GetSize != Zeta_DynamicVector_GetSize) {
        return;
    }

    DynamicVectorPack* dynamic_vec_pack{ ZETA_MemberToStruct(
        DynamicVectorPack, dynamic_vec, seq_cntr->context) };

    Zeta_DynamicVector_Deinit(seq_cntr->context);

    std::free(dynamic_vec_pack);
}

template <typename Val>
Zeta_SeqCntr* DynamicVector_Create(size_t seg_capacity) {
    Zeta_SeqCntr* seq_cntr{ new Zeta_SeqCntr{} };
    DynamicVector_Init<Val>(seq_cntr, seg_capacity);
    return seq_cntr;
}

void DynamicVector_Destroy(Zeta_SeqCntr* seq_cntr) {
    if (seq_cntr == NULL || seq_cntr->GetSize != Zeta_DynamicVector_GetSize) {
        return;
    }

    DynamicVector_Deinit(seq_cntr);

    delete seq_cntr;
}

void DynamicVector_Sanitize(Zeta_SeqCntr const* seq_cntr) {
    if (seq_cntr->GetSize != Zeta_DynamicVector_GetSize) { return; }

#if !ZETA_EnableDebug
#else
    DynamicVectorPack* pack{ ZETA_MemberToStruct(DynamicVectorPack, dynamic_vec,
                                                 seq_cntr->context) };

    Zeta_MemRecorder* data = Zeta_MemRecorder_Create();
    Zeta_MemRecorder* seg = Zeta_MemRecorder_Create();

    Zeta_DynamicVector_Sanitize(
        const_cast<void*>(static_cast<void const*>(&pack->dynamic_vec)), data,
        seg);

    Zeta_MemCheck_MatchRecords(pack->data_allocator_instance.mem_recorder,
                               data);
    Zeta_MemCheck_MatchRecords(pack->seg_allocator_instance.mem_recorder, seg);

    Zeta_MemRecorder_Destroy(data);
    Zeta_MemRecorder_Destroy(seg);
#endif
}
