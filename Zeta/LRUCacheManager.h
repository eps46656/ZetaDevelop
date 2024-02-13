#pragma once

#include "CacheManager.h"
#include "MultiLevelVector.h"
#include "OrdDoublyLinkedNode.h"

ZETA_extern_c_beg;

typedef struct Zeta_LRUCacheManager Zeta_LRUCacheManager;

struct Zeta_LRUCacheManager {
    Zeta_OrdDoublyLinkedNode ln_list_head;
    Zeta_Allocator* allocator;
};

void Zeta_LRUCacheManager_Init(void* lrucm);

/**
 * Find the frame storing the data to key.
 */
Zeta_CacheManager_Node* Zeta_LRUCacheManager_Find(void* lrucm, size_t key);

Zeta_CacheManager_Node* Zeta_LRUCacheManager_Push(void* lrucm, size_t key);

/**
 * @brief Pop the least valuable cache from cache manager. Return the frame.
 *
 * @param lrucm The target lru cache manager.
 *
 * @return The popped frame.
 */
Zeta_CacheManager_Node Zeta_LRUCacheManager_Pop(void* lrucm);

/**
 * @brief Pop the cache.
 */
Zeta_CacheManager_Node Zeta_LRUCacheManager_PopWithKey(void* lrucm, size_t key);

Zeta_CacheManager_Node Zeta_LRUCacheManager_PopWithFrame(void* lrucm,
                                                         void* frame);

void Zeta_LRUCacheManager_ToCacheManager(void* lrucm, Zeta_CacheManager* dst);

void Zeta_LRUCacheManager_PrintState(void* lrucm);

ZETA_extern_c_end;
