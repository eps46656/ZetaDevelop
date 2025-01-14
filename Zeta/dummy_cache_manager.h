#pragma once

#include "cache_manager.h"
#include "seq_cntr.h"

ZETA_ExternC_Beg;

ZETA_DeclareStruct(Zeta_DummyCacheManager);

struct Zeta_DummyCacheManager {
    Zeta_SeqCntr* origin;
};

void Zeta_DummyCacheManager_Init(void* dcm);

Zeta_SeqCntr* Zeta_DummyCacheManager_GetOrigin(void const* dcm);

size_t Zeta_DummyCacheManager_GetCacheSize(void const* dcm);

void* Zeta_DummyCacheManager_Open(void* dcm, size_t max_cache_cnt);

void Zeta_DummyCacheManager_Close(void* dcm, void* sd);

void Zeta_DummyCacheManager_SetMaxCacheCnt(void* dcm, void* sd,
                                           size_t max_cache_cnt);

void Zeta_DummyCacheManager_Read(void* dcm, void* sd, size_t idx, size_t cnt,
                                 void* dst, size_t dst_stride);

void Zeta_DummyCacheManager_WriteBlock(void* dcm, void* sd, size_t idx,
                                       size_t cnt, void* src,
                                       size_t src_stride);

size_t Zeta_DummyCacheManager_Flush(void* dcm, size_t quata);

ZETA_ExternC_End;
