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

template <typename Elem>
void MultiLevelCircularArray_Init(Zeta_SeqCntr* seq_cntr, size_t stride,
                                  size_t seg_capacity) {
    MultiLevelCircularArrayPack* pack{ new MultiLevelCircularArrayPack{} };

    StdAllocator_DeployAllocator(&pack->node_allocator_instance,
                                 &pack->node_allocator);
    StdAllocator_DeployAllocator(&pack->seg_allocator_instance,
                                 &pack->seg_allocator);

    pack->mlca.width = sizeof(Elem);
    pack->mlca.stride = stride;
    pack->mlca.seg_capacity = seg_capacity;

    pack->mlca.node_allocator = &pack->node_allocator;

    pack->mlca.seg_allocator = &pack->seg_allocator;

    Zeta_MultiLevelCircularArray_Init(&pack->mlca);

    Zeta_MultiLevelCircularArray_DeploySeqCntr(&pack->mlca, seq_cntr);

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

    MultiLevelCircularArrayPack* pack{ ZETA_MemberToStruct(
        MultiLevelCircularArrayPack, mlca, seq_cntr->context) };

    Zeta_MultiLevelCircularArray_Deinit(seq_cntr->context);

    delete pack;
}

template <typename Elem>
Zeta_SeqCntr* MultiLevelCircularArray_Create(size_t stride,
                                             size_t seg_capacity) {
    Zeta_SeqCntr* seq_cntr{ new Zeta_SeqCntr{} };
    MultiLevelCircularArray_Init<Elem>(seq_cntr, stride, seg_capacity);
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
