#pragma once

#include <seg_vector.h>

#include <cstdlib>
#include <memory>

#include "seq_cntr_utils.h"
#include "std_allocator.h"

struct SegVectorUtils_Pack {
    StdAllocator seg_allocator;
    StdAllocator data_allocator;

    Zeta_SegVector seg_vector;
};

template <typename Elem>
Zeta_SeqCntr SegVectorUtils_Create(size_t stride, size_t seg_capacity) {
    SegVectorUtils_Pack* pack{ new SegVectorUtils_Pack{} };

    pack->seg_vector.stride = stride;
    pack->seg_vector.width = sizeof(Elem);
    pack->seg_vector.seg_capacity = seg_capacity;

    pack->seg_vector.seg_allocator = {
        .vtable = &zeta_std_allocator_vtable,
        .context = &pack->seg_allocator,
    };

    pack->seg_vector.data_allocator = {
        .vtable = &zeta_std_allocator_vtable,
        .context = &pack->data_allocator,
    };

    Zeta_SegVector_Init(&pack->seg_vector);

    SeqCntrUtils_AddSanitizeFunc(&zeta_seg_vector_seq_cntr_vtable,
                                 SegVectorUtils_Sanitize);

    SeqCntrUtils_AddDestroyFunc(&zeta_seg_vector_seq_cntr_vtable,
                                SegVectorUtils_Destroy);

    return { &zeta_seg_vector_seq_cntr_vtable, &pack->seg_vector };
}

void SegVectorUtils_Destroy(Zeta_SeqCntr seq_cntr) {
    ZETA_DebugAssert(seq_cntr.vtable == &zeta_seg_vector_seq_cntr_vtable);
    if (seq_cntr.context == NULL) { return; }

    SegVectorUtils_Pack* pack{ ZETA_MemberToStruct(
        SegVectorUtils_Pack, seg_vector, seq_cntr.context) };

    Zeta_SegVector_Deinit(&pack->seg_vector);

    delete pack;
}

void SegVectorUtils_Sanitize(Zeta_SeqCntr seq_cntr) {
    ZETA_DebugAssert(seq_cntr.vtable == &zeta_seg_vector_seq_cntr_vtable);
    if (seq_cntr.context == NULL) { return; }

    SegVectorUtils_Pack* pack{ ZETA_MemberToStruct(
        SegVectorUtils_Pack, seg_vector, seq_cntr.context) };

    Zeta_MemRecorder* seg = Zeta_MemRecorder_Create();
    Zeta_MemRecorder* data = Zeta_MemRecorder_Create();

    Zeta_SegVector_Sanitize(
        const_cast<void*>(static_cast<void const*>(&pack->seg_vector)), seg,
        data);

    Zeta_MemCheck_MatchRecords(pack->seg_allocator.mem_recorder, seg);
    Zeta_MemCheck_MatchRecords(pack->data_allocator.mem_recorder, data);

    Zeta_MemRecorder_Destroy(seg);
    Zeta_MemRecorder_Destroy(data);
}
