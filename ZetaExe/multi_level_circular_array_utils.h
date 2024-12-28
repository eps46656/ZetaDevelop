#pragma once

#include <mem_check_utils.h>
#include <multi_level_circular_array.h>

#include <cstdlib>

#include "seq_cntr_utils.h"
#include "std_allocator.h"

struct MultiLevelCircularArrayPack {
    StdAllocator node_allocator_instance;
    Zeta_Allocator node_allocator;

    StdAllocator seg_allocator_instance;
    Zeta_Allocator seg_allocator;

    Zeta_MultiLevelCircularArray mlca;
};

template <typename Val>
void MultiLevelCircularArray_Init(Zeta_SeqCntr* seq_cntr, size_t seg_capacity) {
    MultiLevelCircularArrayPack* mlca_pack{
        static_cast<MultiLevelCircularArrayPack*>(
            std::malloc(sizeof(MultiLevelCircularArrayPack)))
    };

    new (&mlca_pack->node_allocator_instance) StdAllocator{};
    new (&mlca_pack->seg_allocator_instance) StdAllocator{};

    StdAllocator_DeployAllocator(&mlca_pack->node_allocator_instance,
                                 &mlca_pack->node_allocator);
    StdAllocator_DeployAllocator(&mlca_pack->seg_allocator_instance,
                                 &mlca_pack->seg_allocator);

    mlca_pack->mlca.width = sizeof(Val);
    mlca_pack->mlca.seg_capacity = seg_capacity;

    mlca_pack->mlca.node_allocator = &mlca_pack->node_allocator;

    mlca_pack->mlca.seg_allocator = &mlca_pack->seg_allocator;

    Zeta_MultiLevelCircularArray_Init(&mlca_pack->mlca);

    Zeta_MultiLevelCircularArray_DeploySeqCntr(&mlca_pack->mlca, seq_cntr);

    SeqCntrUtils_AddSanitizeFunc(Zeta_MultiLevelCircularArray_GetWidth,
                                 MultiLevelCircularArray_Sanitize);

    SeqCntrUtils_AddDestroyFunc(Zeta_MultiLevelCircularArray_GetWidth,
                                MultiLevelCircularArray_Destroy);
}

void MultiLevelCircularArray_Deinit(Zeta_SeqCntr* seq_cntr) {
    if (seq_cntr == NULL ||
        seq_cntr->GetSize != Zeta_MultiLevelCircularArray_GetSize) {
        return;
    }

    MultiLevelCircularArrayPack* mlca_pack{ ZETA_MemberToStruct(
        MultiLevelCircularArrayPack, mlca, seq_cntr->context) };

    Zeta_MultiLevelCircularArray_Deinit(seq_cntr->context);

    std::free(mlca_pack);
}

template <typename Val>
Zeta_SeqCntr* MultiLevelCircularArray_Create(size_t seg_capacity) {
    Zeta_SeqCntr* seq_cntr{ new Zeta_SeqCntr{} };
    MultiLevelCircularArray_Init<Val>(seq_cntr, seg_capacity);
    return seq_cntr;
}

void MultiLevelCircularArray_Destroy(Zeta_SeqCntr* seq_cntr) {
    if (seq_cntr == NULL ||
        seq_cntr->GetSize != Zeta_MultiLevelCircularArray_GetSize) {
        return;
    }

    MultiLevelCircularArray_Deinit(seq_cntr);

    delete seq_cntr;
}

void MultiLevelCircularArray_Sanitize(Zeta_SeqCntr const* seq_cntr) {
    if (seq_cntr->GetSize != Zeta_MultiLevelCircularArray_GetSize) { return; }

#if !ZETA_EnableDebug
#else
    MultiLevelCircularArrayPack* pack{ ZETA_MemberToStruct(
        MultiLevelCircularArrayPack, mlca, seq_cntr->context) };

    Zeta_MemRecorder* data = Zeta_MemRecorder_Create();
    Zeta_MemRecorder* seg = Zeta_MemRecorder_Create();

    Zeta_MultiLevelCircularArray_Sanitize(
        const_cast<void*>(static_cast<void const*>(&pack->mlca)), data, seg);

    Zeta_MemCheck_MatchRecords(pack->node_allocator_instance.mem_recorder,
                               data);
    Zeta_MemCheck_MatchRecords(pack->seg_allocator_instance.mem_recorder, seg);

    Zeta_MemRecorder_Destroy(data);
    Zeta_MemRecorder_Destroy(seg);
#endif
}
