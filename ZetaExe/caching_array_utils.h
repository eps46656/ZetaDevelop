#pragma once

#include <caching_array.h>

#include <cstdlib>
#include <memory>

#include "cache_manager_utils.h"
#include "seq_cntr_utils.h"
#include "std_allocator.h"

struct CachingArrayPack {
    Zeta_CacheManager cm;
    Zeta_CachingArray ca;
};

// -----------------------------------------------------------------------------

Zeta_SeqCntr CachingArrayUtils_Create(Zeta_CacheManager cm,
                                      size_t max_cache_cnt) {
    CachingArrayPack* pack{ new CachingArrayPack{} };

    pack->cm = cm;

    pack->ca.cache_manager = cm;

    pack->ca.sd = ZETA_CacheManager_Open(cm, max_cache_cnt);

    Zeta_CachingArray_Init(&pack->ca);

    SeqCntrUtils_AddSanitizeFunc(&zeta_caching_array_seq_cntr_vtable,
                                 CachingArrayUtils_Sanitize);

    SeqCntrUtils_AddDestroyFunc(&zeta_caching_array_seq_cntr_vtable,
                                CachingArrayUtils_Destroy);

    return { .vtable = &zeta_caching_array_seq_cntr_vtable,
             .context = &pack->ca };
}

void CachingArrayUtils_Destroy(Zeta_SeqCntr seq_cntr) {
    ZETA_DebugAssert(seq_cntr.vtable == &zeta_caching_array_seq_cntr_vtable);
    if (seq_cntr.context == NULL) { return; }

    CachingArrayPack* ca_pack{ ZETA_MemberToStruct(CachingArrayPack, ca,
                                                   seq_cntr.context) };

    Zeta_CachingArray_Deinit(&ca_pack->ca);

    ZETA_CacheManager_Close(pack->cm, pack->ca.sd);

    delete ca_pack;
}

void CachingArrayUtils_Sanitize(Zeta_SeqCntr seq_cntr) {
    ZETA_DebugAssert(seq_cntr.vtable == &zeta_caching_array_seq_cntr_vtable);
    if (seq_cntr.context == NULL) { return; }

    CachingArrayPack* pack{ ZETA_MemberToStruct(CachingArrayPack, ca,
                                                seq_cntr.context) };

    CacheManagerUtils_Sanitize(pack->cm);
}
