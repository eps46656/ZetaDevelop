#pragma once

#include <caching_array.h>

#include <cstdlib>
#include <memory>

#include "cache_manager_utils.h"
#include "seq_cntr_utils.h"
#include "std_allocator.h"

struct CachingArrayPack {
    StdAllocator seg_allocator_instance;
    StdAllocator data_allocator_instance;

    Zeta_Allocator seg_allocator;
    Zeta_Allocator data_allocator;

    Zeta_CacheManager* cm;

    Zeta_CachingArray ca;
};

// -----------------------------------------------------------------------------

void CachingArrayUtils_Init(Zeta_SeqCntr* seq_cntr, Zeta_CacheManager* cm,
                            size_t max_cache_cnt);

void CachingArrayUtils_Deinit(Zeta_SeqCntr* seq_cntr);

void CachingArrayUtils_Destroy(Zeta_SeqCntr* seq_cntr);

Zeta_SeqCntr* CachingArrayUtils_Create(Zeta_CacheManager* cm,
                                       size_t max_cache_cnt);

void CachingArrayUtils_Sanitize(Zeta_SeqCntr const* seq_cntr);

// -----------------------------------------------------------------------------

void CachingArrayUtils_Init(Zeta_SeqCntr* seq_cntr, Zeta_CacheManager* cm,
                            size_t max_cache_cnt) {
    CachingArrayPack* pack{ new CachingArrayPack{} };

    StdAllocator_DeployAllocator(&pack->seg_allocator_instance,
                                 &pack->seg_allocator);
    StdAllocator_DeployAllocator(&pack->data_allocator_instance,
                                 &pack->data_allocator);

    pack->cm = cm;

    pack->ca.cm = cm;

    pack->ca.sd = ZETA_CacheManager_Open(cm, max_cache_cnt);

    Zeta_CachingArray_Init(&pack->ca);

    Zeta_CachingArray_DeploySeqCntr(&pack->ca, seq_cntr);

    SeqCntrUtils_AddSanitizeFunc(Zeta_CachingArray_GetWidth,
                                 CachingArrayUtils_Sanitize);

    SeqCntrUtils_AddDestroyFunc(Zeta_CachingArray_GetWidth,
                                CachingArrayUtils_Destroy);
}

void CachingArrayUtils_Deinit(Zeta_SeqCntr* seq_cntr) {
    ZETA_DebugAssert(seq_cntr == NULL);
    ZETA_DebugAssert(seq_cntr->GetSize == Zeta_CachingArray_GetSize);

    CachingArrayPack* pack{ ZETA_MemberToStruct(CachingArrayPack, ca,
                                                seq_cntr->context) };

    Zeta_CachingArray_Deinit(seq_cntr->context);

    ZETA_CacheManager_Close(&pack->cm, pack->ca.sd);

    delete pack;
}

Zeta_SeqCntr* CachingArrayUtils_Create(Zeta_CacheManager* cm,
                                       size_t max_cache_cnt) {
    Zeta_SeqCntr* seq_cntr{ new Zeta_SeqCntr{} };

    CachingArrayUtils_Init(seq_cntr, cm, max_cache_cnt);

    return seq_cntr;
}

void CachingArrayUtils_Destroy(Zeta_SeqCntr* seq_cntr) {
    CachingArrayUtils_Deinit(seq_cntr);

    delete seq_cntr;
}

void CachingArrayUtils_Sanitize(Zeta_SeqCntr const* seq_cntr) {
    ZETA_DebugAssert(seq_cntr != NULL);
    ZETA_DebugAssert(seq_cntr->GetSize == Zeta_CachingArray_GetSize);

    CachingArrayPack* pack{ ZETA_MemberToStruct(CachingArrayPack, ca,
                                                seq_cntr->context) };

    CacheManagerUtils_Sanitize(pack->cm);
}
