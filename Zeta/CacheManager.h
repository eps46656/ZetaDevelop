#pragma once

#include "define.h"

ZETA_extern_c_beg;

typedef struct Zeta_CacheManager_Node Zeta_CacheManager_Node;

struct Zeta_CacheManager_Node {
    size_t key;
    void* frame;
    size_t flags;
};

typedef struct Zeta_CacheManager Zeta_CacheManager;

struct Zeta_CacheManager {
    void* context;

    Zeta_CacheManager_Node* (*Find)(void* context, size_t key);

    Zeta_CacheManager_Node* (*Push)(void* context, size_t key);

    Zeta_CacheManager_Node (*Pop)(void* context);

    Zeta_CacheManager_Node (*PopWithKey)(void* context, size_t key);

    Zeta_CacheManager_Node (*PopWithFrame)(void* context, void* frame);
};

ZETA_extern_c_end;
