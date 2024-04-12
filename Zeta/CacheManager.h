#pragma once

#include "define.h"

ZETA_ExternC_Beg;

typedef struct Zeta_CacheManager Zeta_CacheManager;

struct Zeta_CacheManager {
    void* context;

    void* (*Open)(void* context, size_t max_caches_num);

    void (*Close)(void* context, void* sd);

    void (*SetMaxCachesNum)(void* context, void* sd, size_t max_caches_num);

    void const* (*ReadBlock)(void* context, void* sd, size_t blk_idx);

    void (*WriteBlock)(void* context, void* sd, size_t blk_idx,
                       void const* data);

    void (*FlushBlock)(void* context, size_t blk_idx);

    void (*Flush)(void* context, void* sd);
    void (*FlushAll)(void* context);
};

void Zeta_CacheManager_Init(Zeta_CacheManager* cm);

ZETA_ExternC_End;
