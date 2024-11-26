#pragma once

#include <seg_vector.h>

#include <cstdlib>
#include <memory>

#include "seq_cntr_utils.h"
#include "std_allocator.h"

struct SegVectorPack {
    StdAllocator seg_allocator_;
    Zeta_Allocator seg_allocator;

    StdAllocator data_allocator_;
    Zeta_Allocator data_allocator;

    Zeta_SegVector seg_vector;
};

template <typename Elem>
void SegVector_Init(Zeta_SeqCntr* seq_cntr, size_t seg_capacity) {
    SegVectorPack* pack{ static_cast<SegVectorPack*>(
        std::malloc(sizeof(SegVectorPack))) };

    new (&pack->seg_allocator_) StdAllocator{};
    new (&pack->data_allocator_) StdAllocator{};

    StdAllocator_DeployAllocator(&pack->seg_allocator_, &pack->seg_allocator);
    StdAllocator_DeployAllocator(&pack->data_allocator_, &pack->data_allocator);

    pack->seg_vector.width = sizeof(Elem);
    pack->seg_vector.seg_capacity = seg_capacity;
    pack->seg_vector.seg_allocator = &pack->seg_allocator;
    pack->seg_vector.data_allocator = &pack->data_allocator;

    Zeta_SegVector_Init(&pack->seg_vector);

    Zeta_SegVector_DeploySeqCntr(&pack->seg_vector, seq_cntr);

    SeqCntrUtils_AddSanitizeFunc(Zeta_SegVector_GetWidth, SegVector_Sanitize);

    SeqCntrUtils_AddDestroyFunc(Zeta_SegVector_GetWidth, SegVector_Destroy);
}

void SegVector_Deinit(Zeta_SeqCntr* seq_cntr) {
    if (seq_cntr == NULL || seq_cntr->GetSize != Zeta_SegVector_GetSize) {
        return;
    }

    SegVectorPack* pack{ ZETA_MemberToStruct(SegVectorPack, seg_vector,
                                             seq_cntr->context) };

    Zeta_SegVector_Deinit(seq_cntr->context);

    std::free(pack);
}

template <typename Elem>
Zeta_SeqCntr* SegVector_Create(size_t seg_capacity) {
    Zeta_SeqCntr* seq_cntr{ new Zeta_SeqCntr{} };

    SegVector_Init<Elem>(seq_cntr, seg_capacity);

    return seq_cntr;
}

void SegVector_Destroy(Zeta_SeqCntr* seq_cntr) {
    if (seq_cntr == NULL || seq_cntr->GetSize != Zeta_SegVector_GetSize) {
        return;
    }

    SegVector_Deinit(seq_cntr);

    delete seq_cntr;
}

void SegVector_Sanitize(Zeta_SeqCntr const* seq_cntr) {
    if (seq_cntr->GetSize != Zeta_SegVector_GetSize) { return; }

#if ZETA_EnableDebug
    SegVectorPack* pack{ ZETA_MemberToStruct(SegVectorPack, seg_vector,
                                             seq_cntr->context) };

    Zeta_MemRecorder* seg = Zeta_MemRecorder_Create();
    Zeta_MemRecorder* data = Zeta_MemRecorder_Create();

    Zeta_SegVector_Sanitize(
        const_cast<void*>(static_cast<void const*>(&pack->seg_vector)), seg,
        data);

    Zeta_MemCheck_MatchRecords(pack->seg_allocator_.mem_recorder, seg);
    Zeta_MemCheck_MatchRecords(pack->data_allocator_.mem_recorder, data);

    Zeta_MemRecorder_Destroy(seg);
    Zeta_MemRecorder_Destroy(data);
#endif
}
