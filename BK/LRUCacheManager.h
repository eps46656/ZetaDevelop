#pragma once

#include "MultiLevelVector.h"
#include "OrdLinkedListNode.h"

ZETA_ExternC_Beg;

ZETA_DeclareStruct(Zeta_LRUCacheManager);

struct Zeta_LRUCacheManager {
    Zeta_MultiLevelVector mlv;

    Zeta_OrdLinkedListNode n_list_head;

    Zeta_Allocator* allocator;
};

/**
 * Find the frame storing the data to key.
 */
void* Zeta_LRUCacheManager_Find(void* lrucm, size_t key);

void* Zeta_LRUCacheManager_Push(void* lrucm, size_t key, void* frame);

void* Zeta_LRUCacheManager_Pop(void* lrucm);

void* Zeta_LRUCacheManager_PopWithKey(void* lrucm, size_t key);

void* Zeta_LRUCacheManager_PopWithFrame(void* lrucm, void* frame);

void Zeta_LRUCacheManager_PrintState(void* lrucm);

ZETA_ExternC_End;
