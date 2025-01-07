#pragma once

#include "define.h"

ZETA_ExternC_Beg;

ZETA_DeclareStruct(Zeta_CacheManager);

struct Zeta_CacheManager {
    void* context;

    void* (*Open)(void* context, unsigned max_caches_num);

    void (*Close)(void* context, void* sd);

    size_t (*GetWidth)(void* context);

    void (*SetMaxCacheCnt)(void* context, void* sd, unsigned max_cache_num);

    void const* (*Read)(void* context, void* sd, unsigned long long blk_num);

    void (*Write)(void* context, void* sd, unsigned long long blk_num,
                  void const* data);

    void (*FlushBlock)(void* context, unsigned long long blk_num);

    void (*Flush)(void* context, void* sd);

    void (*FlushAll)(void* context);
};

void Zeta_CacheManager_Init(Zeta_CacheManager* cm);

ZETA_ExternC_End;
