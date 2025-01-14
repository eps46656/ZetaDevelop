#include "dummy_cache_manager.h"

#include "debugger.h"

void Zeta_DummyCacheManager_Init(void* dcm_) {
    Zeta_DummyCacheManager* dcm = dcm_;
    ZETA_DebugAssert(dcm != NULL);

    ZETA_DebugAssert(dcm->origin != NULL);
}

Zeta_SeqCntr* Zeta_DummyCacheManager_GetOrigin(void const* dcm_) {
    Zeta_DummyCacheManager const* dcm = dcm_;
    ZETA_DebugAssert(dcm != NULL);

    return dcm->origin;
}

size_t Zeta_DummyCacheManager_GetCacheSize(void const* dcm_) {
    Zeta_DummyCacheManager const* dcm = dcm_;
    ZETA_DebugAssert(dcm != NULL);

    return 1;
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

void Zeta_DummyCacheManager_Read(void* dcm_, void* sd, size_t idx, size_t cnt,
                                 void* dst, size_t dst_stride) {
    Zeta_DummyCacheManager* dcm = dcm_;
    ZETA_DebugAssert(dcm != NULL);

    ZETA_Unused(sd);

    void* cursor = ZETA_SeqCntr_AllocaCursor(dcm->origin);

    ZETA_SeqCntr_Access(dcm->origin, idx, cursor, NULL);

    ZETA_SeqCntr_Read(dcm->origin, cursor, cnt, dst, dst_stride, NULL);
}

void Zeta_DummyCacheManager_Write(void* dcm_, void* sd, size_t idx, size_t cnt,
                                  void const* src, size_t src_stride) {
    Zeta_DummyCacheManager* dcm = dcm_;
    ZETA_DebugAssert(dcm != NULL);

    ZETA_Unused(sd);

    void* cursor = ZETA_SeqCntr_AllocaCursor(dcm->origin);

    ZETA_SeqCntr_Access(dcm->origin, idx, cursor, NULL);

    ZETA_SeqCntr_Write(dcm->origin, cursor, cnt, src, src_stride, NULL);
}

size_t Zeta_DummyCacheManager_Flush(void* dcm_, size_t quata) {
    Zeta_DummyCacheManager* dcm = dcm_;
    ZETA_DebugAssert(dcm != NULL);

    ZETA_Unused(dcm);
    ZETA_Unused(quata);

    return 0;
}

void Zeta_DummyCacheManager_DeployCacheManager(void* dcm_,
                                               Zeta_CacheManager* cm) {
    Zeta_DummyCacheManager* dcm = dcm_;
    ZETA_DebugAssert(dcm != NULL);

    Zeta_CacheManager_Init(cm);

    cm->context = dcm;

    cm->GetOrigin = Zeta_DummyCacheManager_GetOrigin;

    cm->GetCacheSize = Zeta_DummyCacheManager_GetCacheSize;

    cm->Open = Zeta_DummyCacheManager_Open;
    cm->Close = Zeta_DummyCacheManager_Close;

    cm->SetMaxCacheCnt = Zeta_DummyCacheManager_SetMaxCacheCnt;

    cm->Read = Zeta_DummyCacheManager_Read;
    cm->Write = Zeta_DummyCacheManager_Write;

    cm->Flush = Zeta_DummyCacheManager_Flush;
}
