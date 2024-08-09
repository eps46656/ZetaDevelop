#pragma once

#include <cstdlib>

#include "../Zeta/RadixDeque.h"
#include "MemAllocatorCheck.h"
#include "StdAllocator.h"

struct RadixDequePack {
    StdAllocator node_allocator_;
    Zeta_Allocator node_allocator;

    StdAllocator seg_allocator_;
    Zeta_Allocator seg_allocator;

    Zeta_RadixDeque radix_deque;
};

template <typename Val>
void InitRadixDeque(Zeta_SeqContainer* seq_cntr, size_t seg_capacity,
                    size_t branch_num, size_t order) {
    RadixDequePack* radix_deque_pack{ static_cast<RadixDequePack*>(
        std::malloc(sizeof(RadixDequePack))) };

    new (&radix_deque_pack->node_allocator_) StdAllocator{};
    new (&radix_deque_pack->seg_allocator_) StdAllocator{};

    StdAllocator_DeployAllocator(&radix_deque_pack->node_allocator_,
                                 &radix_deque_pack->node_allocator);
    StdAllocator_DeployAllocator(&radix_deque_pack->seg_allocator_,
                                 &radix_deque_pack->seg_allocator);

    radix_deque_pack->radix_deque.width = sizeof(Val);
    radix_deque_pack->radix_deque.stride = sizeof(Val);
    radix_deque_pack->radix_deque.seg_capacity = seg_capacity;
    radix_deque_pack->radix_deque.branch_num = branch_num;
    radix_deque_pack->radix_deque.order = order;

    radix_deque_pack->radix_deque.node_allocator =
        &radix_deque_pack->node_allocator;

    radix_deque_pack->radix_deque.seg_allocator =
        &radix_deque_pack->seg_allocator;

    Zeta_RadixDeque_Init(&radix_deque_pack->radix_deque);

    Zeta_RadixDeque_DeploySeqContainer(&radix_deque_pack->radix_deque,
                                       seq_cntr);
}

void DeinitRadixDeque(Zeta_SeqContainer* seq_cntr) {
    if (seq_cntr == NULL || seq_cntr->GetSize != Zeta_RadixDeque_GetSize) {
        return;
    }

    RadixDequePack* radix_deque_pack{ ZETA_MemberToStruct(
        RadixDequePack, radix_deque, seq_cntr->context) };

    Zeta_RadixDeque_Deinit(seq_cntr->context);

    radix_deque_pack->node_allocator_.~StdAllocator();
    radix_deque_pack->seg_allocator_.~StdAllocator();

    std::free(radix_deque_pack);
}

template <typename Val>
Zeta_SeqContainer* CreateRadixDeque(size_t seg_capacity, size_t branch_num,
                                    size_t order) {
    Zeta_SeqContainer* seq_cntr{ new Zeta_SeqContainer{} };

    InitRadixDeque<Val>(seq_cntr, seg_capacity, branch_num, order);

    return seq_cntr;
}

void DestroyRadixDeque(Zeta_SeqContainer* seq_cntr) {
    if (seq_cntr == NULL || seq_cntr->GetSize != Zeta_RadixDeque_GetSize) {
        return;
    }

    DeinitRadixDeque(seq_cntr);

    delete seq_cntr;
}

void CheckRadixDeque(Zeta_SeqContainer const* seq_cntr) {
    if (seq_cntr->GetSize != Zeta_RadixDeque_GetSize) { return; }

    RadixDequePack* pack{ ZETA_MemberToStruct(RadixDequePack, radix_deque,
                                              seq_cntr->context) };

    Zeta_DebugHashMap node_hm;
    Zeta_DebugHashMap seg_hm;

    Zeta_DebugHashMap_Init(&node_hm);
    Zeta_DebugHashMap_Init(&seg_hm);

    Zeta_RadixDeque_Sanitize(
        const_cast<void*>(static_cast<void const*>(&pack->radix_deque)),
        &node_hm, &seg_hm);

    using record_t = std::unordered_map<unsigned long long, unsigned long long>;

    CheckRecords(pack->node_allocator_.records, *(record_t*)node_hm.hash_map);

    CheckRecords(pack->seg_allocator_.records, *(record_t*)seg_hm.hash_map);
}
