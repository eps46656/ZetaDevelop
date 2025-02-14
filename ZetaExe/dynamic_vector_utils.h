#pragma once

#include <dynamic_vector.h>
#include <mem_check_utils.h>

#include <cstdlib>

#include "seq_cntr_utils.h"
#include "std_allocator.h"

struct DynamicVectorPack {
    StdAllocator data_allocator;
    StdAllocator seg_allocator;

    Zeta_DynamicVector dynamic_vec;
};

template <typename Val>
Zeta_SeqCntr DynamicVector_Create(size_t seg_capacity) {
    DynamicVectorPack* dynamic_vec_pack{ static_cast<DynamicVectorPack*>(
        std::malloc(sizeof(DynamicVectorPack))) };

    new (&dynamic_vec_pack->data_allocator) StdAllocator{};
    new (&dynamic_vec_pack->seg_allocator) StdAllocator{};

    dynamic_vec_pack->dynamic_vec.width = sizeof(Val);
    dynamic_vec_pack->dynamic_vec.seg_capacity = seg_capacity;

    dynamic_vec_pack->dynamic_vec.data_allocator = {
        .vtable = &zeta_std_allocator_vtable,
        .context = &pack->data_allocator,
    };

    dynamic_vec_pack->dynamic_vec.seg_allocator = {
        .vtable = &zeta_std_allocator_vtable,
        .context = &pack->seg_allocator,
    };

    Zeta_DynamicVector_Init(&dynamic_vec_pack->dynamic_vec);

    SeqCntrUtils_AddSanitizeFunc(&zeta_dynamic_vector_seq_cntr_vtable,
                                 DynamicVector_Sanitize);

    SeqCntrUtils_AddDestroyFunc(&zeta_dynamic_vector_seq_cntr_vtable,
                                DynamicVector_Destroy);

    return { &zeta_dynamic_vector_seq_cntr_vtable,
             &dynamic_vec_pack->dynamic_vec };
}

void DynamicVector_Destroy(Zeta_SeqCntr seq_cntr) {
    ZETA_DebugAssert(seq_cntr.vtable == &zeta_dynamic_vector_seq_cntr_vtable);
    if (seq_cntr.context == NULL) { return; }

    DynamicVectorPack* dynamic_vec_pack{ ZETA_MemberToStruct(
        DynamicVectorPack, dynamic_vec, seq_cntr.context) };

    Zeta_DynamicVector_Deinit(&dynamic_vec_pack->dynamic_vec);

    std::free(dynamic_vec_pack);
}

void DynamicVector_Sanitize(Zeta_SeqCntr seq_cntr) {
    ZETA_DebugAssert(seq_cntr.vtable == &zeta_dynamic_vector_seq_cntr_vtable);
    if (seq_cntr.context == NULL) { return; }

#if ZETA_EnableDebug
    DynamicVectorPack* pack{ ZETA_MemberToStruct(DynamicVectorPack, dynamic_vec,
                                                 seq_cntr.context) };

    Zeta_MemRecorder* data = Zeta_MemRecorder_Create();
    Zeta_MemRecorder* seg = Zeta_MemRecorder_Create();

    Zeta_DynamicVector_Sanitize(
        const_cast<void*>(static_cast<void const*>(&pack->dynamic_vec)), data,
        seg);

    Zeta_MemCheck_MatchRecords(pack->data_allocator.mem_recorder, data);
    Zeta_MemCheck_MatchRecords(pack->seg_allocator.mem_recorder, seg);

    Zeta_MemRecorder_Destroy(data);
    Zeta_MemRecorder_Destroy(seg);
#endif
}
