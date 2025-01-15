#pragma once

#include <seg_vector.h>

#include <cstdlib>
#include <memory>

#include "seq_cntr_utils.h"
#include "std_allocator.h"

struct SegVectorUtils_Pack {
    StdAllocator seg_allocator_instance;
    StdAllocator data_allocator_instance;

    Zeta_Allocator seg_allocator;
    Zeta_Allocator data_allocator;

    Zeta_SegVector seg_vector;
};

template <typename Elem>
void SegVectorUtils_Init(Zeta_SeqCntr* seq_cntr, size_t stride,
                         size_t seg_capacity) {
    SegVectorUtils_Pack* pack{ new SegVectorUtils_Pack{} };

    StdAllocator_DeployAllocator(&pack->seg_allocator_instance,
                                 &pack->seg_allocator);
    StdAllocator_DeployAllocator(&pack->data_allocator_instance,
                                 &pack->data_allocator);

    pack->seg_vector.stride = stride;
    pack->seg_vector.width = sizeof(Elem);
    pack->seg_vector.seg_capacity = seg_capacity;
    pack->seg_vector.seg_allocator = &pack->seg_allocator;
    pack->seg_vector.data_allocator = &pack->data_allocator;

    Zeta_SegVector_Init(&pack->seg_vector);

    Zeta_SegVector_DeploySeqCntr(&pack->seg_vector, seq_cntr);

    SeqCntrUtils_AddSanitizeFunc(Zeta_SegVector_GetWidth,
                                 SegVectorUtils_Sanitize);

    SeqCntrUtils_AddDestroyFunc(Zeta_SegVector_GetWidth,
                                SegVectorUtils_Destroy);
}

void SegVectorUtils_Deinit(Zeta_SeqCntr* seq_cntr) {
    ZETA_DebugAssert(seq_cntr != NULL);
    ZETA_DebugAssert(seq_cntr->GetSize == Zeta_SegVector_GetSize);

    SegVectorUtils_Pack* pack{ ZETA_MemberToStruct(
        SegVectorUtils_Pack, seg_vector, seq_cntr->context) };

    Zeta_SegVector_Deinit(seq_cntr->context);

    delete pack;
}

template <typename Elem>
Zeta_SeqCntr* SegVectorUtils_Create(size_t stride, size_t seg_capacity) {
    Zeta_SeqCntr* seq_cntr{ new Zeta_SeqCntr{} };

    SegVectorUtils_Init<Elem>(seq_cntr, stride, seg_capacity);

    return seq_cntr;
}

void SegVectorUtils_Destroy(Zeta_SeqCntr* seq_cntr) {
    SegVectorUtils_Deinit(seq_cntr);

    delete seq_cntr;
}

void SegVectorUtils_Sanitize(Zeta_SeqCntr const* seq_cntr) {
    ZETA_DebugAssert(seq_cntr != NULL);
    ZETA_DebugAssert(seq_cntr->GetSize == Zeta_SegVector_GetSize);

    SegVectorUtils_Pack* pack{ ZETA_MemberToStruct(
        SegVectorUtils_Pack, seg_vector, seq_cntr->context) };

    Zeta_MemRecorder* seg = Zeta_MemRecorder_Create();
    Zeta_MemRecorder* data = Zeta_MemRecorder_Create();

    Zeta_SegVector_Sanitize(
        const_cast<void*>(static_cast<void const*>(&pack->seg_vector)), seg,
        data);

    Zeta_MemCheck_MatchRecords(pack->seg_allocator_instance.mem_recorder, seg);
    Zeta_MemCheck_MatchRecords(pack->data_allocator_instance.mem_recorder,
                               data);

    Zeta_MemRecorder_Destroy(seg);
    Zeta_MemRecorder_Destroy(data);
}
