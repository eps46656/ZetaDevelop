#pragma once

#include <mem_check_utils.h>
#include <multi_level_circular_array.h>

#include <cstdlib>

#include "seq_cntr_utils.h"
#include "std_allocator.h"

struct MultiLevelCircularArrayPack {
    StdAllocator node_allocator;
    StdAllocator seg_allocator;

    Zeta_MultiLevelCircularArray mlca;
};

template <typename Elem>
Zeta_SeqCntr MultiLevelCircularArray_Create(size_t stride,
                                            size_t seg_capacity) {
    MultiLevelCircularArrayPack* pack{ new MultiLevelCircularArrayPack{} };

    pack->mlca.width = sizeof(Elem);
    pack->mlca.stride = stride;
    pack->mlca.seg_capacity = seg_capacity;

    pack->mlca.node_allocator = {
        .vtable = &zeta_std_allocator_vtable,
        .context = &pack->node_allocator,
    };

    pack->mlca.seg_allocator = {
        .vtable = &zeta_std_allocator_vtable,
        .context = &pack->seg_allocator,
    };

    Zeta_MultiLevelCircularArray_Init(&pack->mlca);

    SeqCntrUtils_AddSanitizeFunc(
        &zeta_multi_level_circular_array_seq_cntr_vtable,
        MultiLevelCircularArray_Sanitize);

    SeqCntrUtils_AddDestroyFunc(
        &zeta_multi_level_circular_array_seq_cntr_vtable,
        MultiLevelCircularArray_Destroy);

    return { &zeta_multi_level_circular_array_seq_cntr_vtable, &pack->mlca };
}

void MultiLevelCircularArray_Destroy(Zeta_SeqCntr seq_cntr) {
    ZETA_DebugAssert(seq_cntr.vtable ==
                     &zeta_multi_level_circular_array_seq_cntr_vtable);
    if (seq_cntr.context == NULL) { return; }

    MultiLevelCircularArrayPack* pack{ ZETA_MemberToStruct(
        MultiLevelCircularArrayPack, mlca, seq_cntr.context) };

    Zeta_MultiLevelCircularArray_Deinit(&pack->mlca);

    delete pack;
}

void MultiLevelCircularArray_Sanitize(Zeta_SeqCntr seq_cntr) {
    ZETA_DebugAssert(seq_cntr.vtable ==
                     &zeta_multi_level_circular_array_seq_cntr_vtable);
    if (seq_cntr.context == NULL) { return; }

#if !ZETA_EnableDebug
#else
    MultiLevelCircularArrayPack* pack{ ZETA_MemberToStruct(
        MultiLevelCircularArrayPack, mlca, seq_cntr.context) };

    Zeta_MemRecorder* data = Zeta_MemRecorder_Create();
    Zeta_MemRecorder* seg = Zeta_MemRecorder_Create();

    Zeta_MultiLevelCircularArray_Sanitize(
        const_cast<void*>(static_cast<void const*>(&pack->mlca)), data, seg);

    Zeta_MemCheck_MatchRecords(pack->node_allocator.mem_recorder, data);
    Zeta_MemCheck_MatchRecords(pack->seg_allocator.mem_recorder, seg);

    Zeta_MemRecorder_Destroy(data);
    Zeta_MemRecorder_Destroy(seg);
#endif
}
