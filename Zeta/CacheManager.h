#pragma once

#include "define.h"

ZETA_ExternC_Beg;

ZETA_DeclareStruct(Zeta_CacheManager);

struct Zeta_CacheManager {
    void* context;

    void* (*Open)(void* context, size_t max_caches_num);

    void (*Close)(void* context, void* sd);

    size_t (*GetBlockSize)(void* context);

    void (*SetMaxCacheCnt)(void* context, void* sd, size_t max_cache_num);

    void const* (*ReadBlock)(void* context, void* sd, size_t blk_num);

    void (*WriteBlock)(void* context, void* sd, size_t blk_num,
                       void const* data);

    void (*FlushBlock)(void* context, size_t blk_num);

    void (*Flush)(void* context, void* sd);
    void (*FlushAll)(void* context);
};

void Zeta_CacheManager_Init(Zeta_CacheManager* cm);

ZETA_ExternC_End;
