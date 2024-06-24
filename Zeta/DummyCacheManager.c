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

void* Zeta_DummyCacheManager_Open(void* dcm_, size_t max_cache_cnt) {
    Zeta_DummyCacheManager* dcm = dcm_;
    ZETA_DebugAssert(dcm != NULL);

    ZETA_Unused(dcm);
    ZETA_Unused(max_cache_cnt);

    return NULL;
}

void Zeta_DummyCacheManager_Close(void* dcm_, void* sd) {
    Zeta_DummyCacheManager* dcm = dcm_;
    ZETA_DebugAssert(dcm != NULL);

    ZETA_Unused(dcm);
    ZETA_Unused(sd);
}

void Zeta_DummyCacheManager_SetMaxCacheCnt(void* dcm_, void* sd,
                                           size_t max_cache_cnt) {
    Zeta_DummyCacheManager* dcm = dcm_;
    ZETA_DebugAssert(dcm != NULL);

    ZETA_Unused(dcm);
    ZETA_Unused(sd);
    ZETA_Unused(max_cache_cnt);
}

void const* Zeta_DummyCacheManager_ReadBlock(void* dcm_, void* sd,
                                             size_t blk_num) {
    Zeta_DummyCacheManager* dcm = dcm_;
    ZETA_DebugAssert(dcm != NULL);

    ZETA_Unused(sd);

    ZETA_DebugAssert(dcm->blk_vec != NULL);
    ZETA_DebugAssert(dcm->blk_vec->ReadBlock != NULL);

    dcm->blk_vec->ReadBlock(dcm->blk_vec->context, blk_num, dcm->buffer);

    return dcm->buffer;
}

void Zeta_DummyCacheManager_WriteBlock(void* dcm_, void* sd, size_t blk_num,
                                       void const* data) {
    Zeta_DummyCacheManager* dcm = dcm_;
    ZETA_DebugAssert(dcm != NULL);

    ZETA_Unused(sd);

    ZETA_DebugAssert(dcm->blk_vec != NULL);
    ZETA_DebugAssert(dcm->blk_vec->WriteBlock != NULL);

    dcm->blk_vec->WriteBlock(dcm->blk_vec->context, blk_num, data);
}

void Zeta_DummyCacheManager_FlushBlock(void* dcm_, size_t blk_num) {
    Zeta_DummyCacheManager* dcm = dcm_;
    ZETA_DebugAssert(dcm != NULL);

    ZETA_Unused(dcm);

    ZETA_DebugAssert(dcm->blk_vec != NULL);
}

void Zeta_DummyCacheManager_Flush(void* dcm_, void* sd) {
    Zeta_DummyCacheManager* dcm = dcm_;
    ZETA_DebugAssert(dcm != NULL);

    ZETA_Unused(dcm);
    ZETA_Unused(sd);
}

void Zeta_DummyCacheManager_FlushAll(void* dcm_) {
    Zeta_DummyCacheManager* dcm = dcm_;
    ZETA_DebugAssert(dcm != NULL);

    ZETA_Unused(dcm);
}

void Zeta_DummyCacheManager_DeployCacheManager(void* dcm_,
                                               Zeta_CacheManager* dst) {
    Zeta_DummyCacheManager* dcm = dcm_;
    ZETA_DebugAssert(dcm != NULL);

    dst->context = dcm;

    dst->Open = Zeta_DummyCacheManager_Open;
    dst->Close = Zeta_DummyCacheManager_Close;

    dst->SetMaxCacheCnt = Zeta_DummyCacheManager_SetMaxCacheCnt;

    dst->ReadBlock = Zeta_DummyCacheManager_ReadBlock;
    dst->WriteBlock = Zeta_DummyCacheManager_WriteBlock;

    dst->FlushBlock = Zeta_DummyCacheManager_FlushBlock;
    dst->Flush = Zeta_DummyCacheManager_Flush;
    dst->FlushAll = Zeta_DummyCacheManager_FlushAll;
}
