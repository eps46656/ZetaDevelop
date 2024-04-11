#include "DummyCacheManager.h"

void Zeta_DummyCacheManager_Init(void* dcm_) {
    Zeta_DummyCacheManager* dcm = dcm_;
    ZETA_DebugAssert(dcm != NULL);

    ZETA_DebugAssert(dcm->blk_vec != NULL);
    ZETA_DebugAssert(dcm->blk_vec->GetBlockSize != NULL);
    ZETA_DebugAssert(dcm->blk_vec->GetBlockNum != NULL);
    ZETA_DebugAssert(dcm->blk_vec->ReadBlock != NULL);
    ZETA_DebugAssert(dcm->blk_vec->WriteBlock != NULL);

    ZETA_DebugAssert(dcm->buffer != NULL);
}

void* Zeta_DummyCacheManager_LogIn(void* dcm_, size_t max_caches_num) {
    Zeta_DummyCacheManager* dcm = dcm_;
    ZETA_DebugAssert(dcm != NULL);

    ZETA_Unused(dcm);
    ZETA_Unused(max_caches_num);

    return NULL;
}

void Zeta_DummyCacheManager_LogOut(void* dcm_, void* u_node) {
    Zeta_DummyCacheManager* dcm = dcm_;
    ZETA_DebugAssert(dcm != NULL);

    ZETA_Unused(dcm);
    ZETA_Unused(u_node);
}

void Zeta_DummyCacheManager_SetCachesNumMax(void* dcm_, void* u_node,
                                            size_t max_caches_num) {
    Zeta_DummyCacheManager* dcm = dcm_;
    ZETA_DebugAssert(dcm != NULL);

    ZETA_Unused(dcm);
    ZETA_Unused(u_node);
    ZETA_Unused(max_caches_num);
}

const byte_t* Zeta_DummyCacheManager_ReadBlock(void* dcm_, void* u_node,
                                               size_t blk_idx) {
    Zeta_DummyCacheManager* dcm = dcm_;
    ZETA_DebugAssert(dcm != NULL);

    ZETA_Unused(u_node);

    ZETA_DebugAssert(dcm->blk_vec != NULL);
    ZETA_DebugAssert(dcm->blk_vec->ReadBlock != NULL);

    dcm->blk_vec->ReadBlock(dcm->blk_vec->context, blk_idx, dcm->buffer);

    return dcm->buffer;
}

void Zeta_DummyCacheManager_WriteBlock(void* dcm_, void* u_node, size_t blk_idx,
                                       byte_t* data) {
    Zeta_DummyCacheManager* dcm = dcm_;
    ZETA_DebugAssert(dcm != NULL);

    ZETA_Unused(u_node);

    ZETA_DebugAssert(dcm->blk_vec != NULL);
    ZETA_DebugAssert(dcm->blk_vec->WriteBlock != NULL);

    dcm->blk_vec->WriteBlock(dcm->blk_vec->context, blk_idx, data);
}

void Zeta_DummyCacheManager_Flush(void* dcm_, void* u_node) {
    Zeta_DummyCacheManager* dcm = dcm_;
    ZETA_DebugAssert(dcm != NULL);

    ZETA_Unused(dcm);
    ZETA_Unused(u_node);
}

void Zeta_DummyCacheManager_FlushAll(void* dcm_) {
    Zeta_DummyCacheManager* dcm = dcm_;
    ZETA_DebugAssert(dcm != NULL);

    ZETA_Unused(dcm);
}
