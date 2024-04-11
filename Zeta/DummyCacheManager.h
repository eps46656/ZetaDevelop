#pragma once

#include "BlockVector.h"

ZETA_ExternC_Beg;

typedef struct Zeta_DummyCacheManager Zeta_DummyCacheManager;

struct Zeta_DummyCacheManager {
    Zeta_BlockVector* blk_vec;
    byte_t* buffer;
};

void Zeta_DummyCacheManager_Init(void* dcm);

void* Zeta_DummyCacheManager_LogIn(void* dcm, size_t max_caches_num);

void Zeta_DummyCacheManager_LogOut(void* dcm, void* u_node);

void Zeta_DummyCacheManager_SetCachesNumMax(void* dcm, void* u_node,
                                            size_t max_caches_num);

const byte_t* Zeta_DummyCacheManager_ReadBlock(void* dcm, void* u_node,
                                               size_t blk_idx);

void Zeta_DummyCacheManager_WriteBlock(void* dcm, void* u_node, size_t blk_idx,
                                       byte_t* data);

void Zeta_DummyCacheManager_Flush(void* dcm, void* u_node);

void Zeta_DummyCacheManager_FlushAll(void* dcm);

ZETA_ExternC_End;
