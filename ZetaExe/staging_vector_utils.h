#pragma once

#include <staging_vector.h>

#include <cstdlib>
#include <memory>

#include "seq_cntr_utils.h"
#include "std_allocator.h"

struct StagingVectorUtils_Pack {
    StdAllocator seg_allocator;
    StdAllocator data_allocator;

    Zeta_StagingVector staging_vector;
};

template <typename Elem>
Zeta_SeqCntr StagingVectorUtils_Create(Zeta_SeqCntr origin_seq_cntr,
                                       size_t stride, size_t seg_capacity) {
    ZETA_DebugAssert(origin_seq_cntr.vtable != NULL);
    ZETA_DebugAssert(0 < stride);
    ZETA_DebugAssert(0 < seg_capacity);

    ZETA_DebugAssert(sizeof(Elem) <= ZETA_SeqCntr_GetWidth(origin_seq_cntr));

    StagingVectorUtils_Pack* pack{ new StagingVectorUtils_Pack{} };

    pack->staging_vector.stride = stride;
    pack->staging_vector.origin = origin_seq_cntr;
    pack->staging_vector.seg_capacity = seg_capacity;

    pack->staging_vector.seg_allocator = {
        .vtable = &zeta_std_allocator_vtable,
        .context = &pack->seg_allocator,
    };

    pack->staging_vector.data_allocator = {
        .vtable = &zeta_std_allocator_vtable,
        .context = &pack->data_allocator,
    };

    Zeta_StagingVector_Init(&pack->staging_vector);

    SeqCntrUtils_AddSanitizeFunc(&zeta_staging_vector_seq_cntr_vtable,
                                 StagingVectorUtils_Sanitize);

    SeqCntrUtils_AddDestroyFunc(&zeta_staging_vector_seq_cntr_vtable,
                                StagingVectorUtils_Destroy);

    return { &zeta_staging_vector_seq_cntr_vtable, &pack->staging_vector };
}

void StagingVectorUtils_Destroy(Zeta_SeqCntr seq_cntr) {
    ZETA_DebugAssert(seq_cntr.vtable == &zeta_staging_vector_seq_cntr_vtable);
    if (seq_cntr.context == NULL) { return; }

    StagingVectorUtils_Pack* pack{ ZETA_MemberToStruct(
        StagingVectorUtils_Pack, staging_vector, seq_cntr.context) };

    Zeta_StagingVector_Deinit(&pack->staging_vector);

    delete pack;
}

void StagingVectorUtils_Sanitize(Zeta_SeqCntr seq_cntr) {
    ZETA_DebugAssert(seq_cntr.vtable == &zeta_staging_vector_seq_cntr_vtable);
    if (seq_cntr.context == NULL) { return; }

    StagingVectorUtils_Pack* pack{ ZETA_MemberToStruct(
        StagingVectorUtils_Pack, staging_vector, seq_cntr.context) };

    Zeta_MemRecorder* seg = Zeta_MemRecorder_Create();
    Zeta_MemRecorder* data = Zeta_MemRecorder_Create();

    Zeta_StagingVector_Sanitize(
        const_cast<void*>(static_cast<void const*>(&pack->staging_vector)), seg,
        data);

    Zeta_MemCheck_MatchRecords(pack->seg_allocator.mem_recorder, seg);
    Zeta_MemCheck_MatchRecords(pack->data_allocator.mem_recorder, data);

    Zeta_MemRecorder_Destroy(seg);
    Zeta_MemRecorder_Destroy(data);
}
