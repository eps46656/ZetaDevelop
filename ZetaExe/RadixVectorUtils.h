#pragma once

#include <MemCheck.h>
#include <RadixVector.h>

#include <cstdlib>

#include "StdAllocator.h"

struct RadixVectorPack {
    StdAllocator node_allocator_;
    Zeta_Allocator node_allocator;

    StdAllocator seg_allocator_;
    Zeta_Allocator seg_allocator;

    Zeta_RadixVector radix_vector;
};

template <typename Val>
void RadixVector_Init(Zeta_SeqContainer* seq_cntr, size_t seg_capacity,
                      size_t branch_num, size_t order) {
    RadixVectorPack* radix_vector_pack{ static_cast<RadixVectorPack*>(
        std::malloc(sizeof(RadixVectorPack))) };

    new (&radix_vector_pack->node_allocator_) StdAllocator{};
    new (&radix_vector_pack->seg_allocator_) StdAllocator{};

    StdAllocator_DeployAllocator(&radix_vector_pack->node_allocator_,
                                 &radix_vector_pack->node_allocator);
    StdAllocator_DeployAllocator(&radix_vector_pack->seg_allocator_,
                                 &radix_vector_pack->seg_allocator);

    radix_vector_pack->radix_vector.width = sizeof(Val);
    radix_vector_pack->radix_vector.stride = sizeof(Val);
    radix_vector_pack->radix_vector.seg_capacity = seg_capacity;
    radix_vector_pack->radix_vector.branch_num = branch_num;
    radix_vector_pack->radix_vector.order = order;

    radix_vector_pack->radix_vector.node_allocator =
        &radix_vector_pack->node_allocator;

    radix_vector_pack->radix_vector.seg_allocator =
        &radix_vector_pack->seg_allocator;

    Zeta_RadixVector_Init(&radix_vector_pack->radix_vector);

    Zeta_RadixVector_DeploySeqContainer(&radix_vector_pack->radix_vector,
                                        seq_cntr);

    SeqContainer_AddSanitizeFunc(Zeta_RadixVector_GetWidth,
                                 RadixVector_Sanitize);
}

template <typename Val>
Zeta_SeqContainer* RadixVector_Create(size_t seg_capacity, size_t branch_num,
                                      size_t order) {
    Zeta_SeqContainer* seq_cntr{ new Zeta_SeqContainer{} };
    RadixVector_Init<Val>(seq_cntr, seg_capacity, branch_num, order);
    return seq_cntr;
}

void RadixVector_Destroy(Zeta_SeqContainer* seq_cntr) {
    if (seq_cntr == NULL || seq_cntr->GetSize != Zeta_RadixVector_GetSize) {
        return;
    }

    Zeta_RadixVector_Deinit(seq_cntr->context);

    delete seq_cntr;
}

void RadixVector_Sanitize(Zeta_SeqContainer const* seq_cntr) {
    if (seq_cntr->GetSize != Zeta_RadixVector_GetSize) { return; }

    RadixVectorPack* pack{ ZETA_MemberToStruct(RadixVectorPack, radix_vector,
                                               seq_cntr->context) };

    Zeta_MemRecorder* node = Zeta_MemRecorder_Create();
    Zeta_MemRecorder* seg = Zeta_MemRecorder_Create();

    Zeta_RadixVector_Sanitize(
        const_cast<void*>(static_cast<void const*>(&pack->radix_vector)), node,
        seg);

    Zeta_MemCheck_MatchRecords(pack->node_allocator_.mem_recorder, node);
    Zeta_MemCheck_MatchRecords(pack->seg_allocator_.mem_recorder, seg);

    Zeta_MemRecorder_Destroy(node);
    Zeta_MemRecorder_Destroy(seg);
}
