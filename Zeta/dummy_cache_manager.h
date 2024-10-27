#pragma once

#include "block_vector.h"
#include "cache_manager.h"

ZETA_ExternC_Beg;

ZETA_DeclareStruct(Zeta_DummyCacheManager);

struct Zeta_DummyCacheManager {
    Zeta_BlockVector* blk_vec;
    byte_t* buffer;
};

void Zeta_DummyCacheManager_Init(void* dcm);

void* Zeta_DummyCacheManager_Open(void* dcm, size_t max_cache_cnt);

void Zeta_DummyCacheManager_Close(void* dcm, void* sd);

void Zeta_DummyCacheManager_SetMaxCacheCnt(void* dcm, void* sd,
                                           size_t max_cache_cnt);

void const* Zeta_DummyCacheManager_ReadBlock(void* dcm, void* sd,
                                             size_t blk_num);

void Zeta_DummyCacheManager_WriteBlock(void* dcm, void* sd, size_t blk_num,
                                       void const* data);

void Zeta_DummyCacheManager_FlushBlock(void* dcm, size_t blk_num);

void Zeta_DummyCacheManager_Flush(void* dcm, void* sd);

void Zeta_DummyCacheManager_FlushAll(void* dcm);

ZETA_ExternC_End;
