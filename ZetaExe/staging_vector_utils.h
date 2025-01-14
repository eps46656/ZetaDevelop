#pragma once

#include <staging_vector.h>

#include <cstdlib>
#include <memory>

#include "seq_cntr_utils.h"
#include "std_allocator.h"

struct StagingVectorUtils_Pack {
    StdAllocator seg_allocator_instance;
    StdAllocator data_allocator_instance;

    Zeta_Allocator seg_allocator;
    Zeta_Allocator data_allocator;

    Zeta_StagingVector staging_vector;
};

template <typename Elem>
void StagingVectorUtils_Init(Zeta_SeqCntr* seq_cntr,
                             Zeta_SeqCntr* origin_seq_cntr, size_t stride,
                             size_t seg_capacity) {
    ZETA_DebugAssert(seq_cntr != NULL);
    ZETA_DebugAssert(origin_seq_cntr != NULL);
    ZETA_DebugAssert(0 < stride);
    ZETA_DebugAssert(0 < seg_capacity);

    ZETA_DebugAssert(sizeof(Elem) <= ZETA_SeqCntr_GetWidth(origin_seq_cntr));

    StagingVectorUtils_Pack* pack{ new StagingVectorUtils_Pack{} };

    StdAllocator_DeployAllocator(&pack->seg_allocator_instance,
                                 &pack->seg_allocator);
    StdAllocator_DeployAllocator(&pack->data_allocator_instance,
                                 &pack->data_allocator);

    pack->staging_vector.stride = stride;
    pack->staging_vector.origin = origin_seq_cntr;
    pack->staging_vector.seg_capacity = seg_capacity;
    pack->staging_vector.seg_allocator = &pack->seg_allocator;
    pack->staging_vector.data_allocator = &pack->data_allocator;

    Zeta_StagingVector_Init(&pack->staging_vector);

    Zeta_StagingVector_DeploySeqCntr(&pack->staging_vector, seq_cntr);

    SeqCntrUtils_AddSanitizeFunc(Zeta_StagingVector_GetWidth,
                                 StagingVectorUtils_Sanitize);

    SeqCntrUtils_AddDestroyFunc(Zeta_StagingVector_GetWidth,
                                StagingVectorUtils_Destroy);
}

void StagingVectorUtils_Deinit(Zeta_SeqCntr* seq_cntr) {
    if (seq_cntr == NULL || seq_cntr->GetSize != Zeta_StagingVector_GetSize) {
        return;
    }

    StagingVectorUtils_Pack* pack{ ZETA_MemberToStruct(
        StagingVectorUtils_Pack, staging_vector, seq_cntr->context) };

    Zeta_StagingVector_Deinit(seq_cntr->context);

    delete pack;
}

template <typename Elem>
Zeta_SeqCntr* StagingVectorUtils_Create(Zeta_SeqCntr* origin_seq_cntr,
                                        size_t stride, size_t seg_capacity) {
    Zeta_SeqCntr* seq_cntr{ new Zeta_SeqCntr{} };

    StagingVectorUtils_Init<Elem>(seq_cntr, origin_seq_cntr, stride,
                                  seg_capacity);

    return seq_cntr;
}

void StagingVectorUtils_Destroy(Zeta_SeqCntr* seq_cntr) {
    if (seq_cntr == NULL || seq_cntr->GetSize != Zeta_StagingVector_GetSize) {
        return;
    }

    StagingVectorUtils_Deinit(seq_cntr);

    delete seq_cntr;
}

void StagingVectorUtils_Sanitize(Zeta_SeqCntr const* seq_cntr) {
    if (seq_cntr == NULL || seq_cntr->GetSize != Zeta_StagingVector_GetSize) {
        return;
    }

    StagingVectorUtils_Pack* pack{ ZETA_MemberToStruct(
        StagingVectorUtils_Pack, staging_vector, seq_cntr->context) };

    Zeta_MemRecorder* seg = Zeta_MemRecorder_Create();
    Zeta_MemRecorder* data = Zeta_MemRecorder_Create();

    Zeta_StagingVector_Sanitize(
        const_cast<void*>(static_cast<void const*>(&pack->staging_vector)), seg,
        data);

    Zeta_MemCheck_MatchRecords(pack->seg_allocator_instance.mem_recorder, seg);
    Zeta_MemCheck_MatchRecords(pack->data_allocator_instance.mem_recorder,
                               data);

    Zeta_MemRecorder_Destroy(seg);
    Zeta_MemRecorder_Destroy(data);
}
