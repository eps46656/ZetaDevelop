#pragma once

#include <cstdlib>

#include "../Zeta/DynamicVector.h"
#include "MemAllocatorCheck.h"
#include "StdAllocator.h"

struct DynamicVectorPack {
    StdAllocator data_allocator_;
    Zeta_Allocator data_allocator;

    StdAllocator seg_allocator_;
    Zeta_Allocator seg_allocator;

    Zeta_DynamicVector dynamic_vec;
};

template <typename Val>
void InitDynamicVector(Zeta_SeqContainer* seq_cntr, size_t seg_capacity) {
    DynamicVectorPack* dynamic_vec_pack{ static_cast<DynamicVectorPack*>(
        std::malloc(sizeof(DynamicVectorPack))) };

    new (&dynamic_vec_pack->data_allocator_) StdAllocator{};
    new (&dynamic_vec_pack->seg_allocator_) StdAllocator{};

    StdAllocator_DeployAllocator(&dynamic_vec_pack->data_allocator_,
                                 &dynamic_vec_pack->data_allocator);
    StdAllocator_DeployAllocator(&dynamic_vec_pack->seg_allocator_,
                                 &dynamic_vec_pack->seg_allocator);

    dynamic_vec_pack->dynamic_vec.width = sizeof(Val);
    dynamic_vec_pack->dynamic_vec.stride = sizeof(Val);
    dynamic_vec_pack->dynamic_vec.seg_capacity = seg_capacity;

    dynamic_vec_pack->dynamic_vec.data_allocator =
        &dynamic_vec_pack->data_allocator;

    dynamic_vec_pack->dynamic_vec.seg_allocator =
        &dynamic_vec_pack->seg_allocator;

    ZETA_PrintCurPos;

    Zeta_DynamicVector_Init(&dynamic_vec_pack->dynamic_vec);

    ZETA_PrintCurPos;

    Zeta_DynamicVector_DeploySeqContainer(&dynamic_vec_pack->dynamic_vec,
                                          seq_cntr);

    ZETA_PrintCurPos;
}

template <typename Val>
Zeta_SeqContainer* CreateDynamicVector(size_t seg_capacity) {
    Zeta_SeqContainer* seq_cntr{ new Zeta_SeqContainer{} };

    InitDynamicVector<Val>(seq_cntr, seg_capacity);

    return seq_cntr;
}

void CheckDynamicVector(Zeta_SeqContainer const* seq_cntr) {
    if (seq_cntr->GetSize != Zeta_DynamicVector_GetSize) { return; }

    DynamicVectorPack* pack{ ZETA_MemberToStruct(DynamicVectorPack, dynamic_vec,
                                                 seq_cntr->context) };

    Zeta_DebugHashMap data_hm;
    Zeta_DebugHashMap seg_hm;

    Zeta_DebugHashMap_Create(&data_hm);
    Zeta_DebugHashMap_Create(&seg_hm);

    Zeta_DynamicVector_Check(
        const_cast<void*>(static_cast<void const*>(&pack->dynamic_vec)),
        &data_hm, &seg_hm);

    using record_t = std::unordered_map<unsigned long long, unsigned long long>;

    CheckRecords(pack->data_allocator_.records, *(record_t*)data_hm.hash_map);

    CheckRecords(pack->seg_allocator_.records, *(record_t*)seg_hm.hash_map);
}
