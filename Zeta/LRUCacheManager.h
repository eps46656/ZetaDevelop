#pragma once

#include "Allocator.h"
#include "BlockVector.h"
#include "MultiLevelVector.h"
#include "SlabAllocator.h"

ZETA_extern_c_beg;

#define ZETA_LRUCacheManager_user_num_max (1024)

/*

0: all
1: the first user
2: the second user


M: the Mth user

M + 1

*/

typedef struct Zeta_LRUCacheManager Zeta_LRUCacheManager;

struct Zeta_LRUCacheManager {
    size_t cache_size;

    Zeta_BlockVector* blk_vec;

    Zeta_MultiLevelVector n_map;

    Zeta_Allocator* allocator;

    Zeta_SlabAllocator xn_allocator;
    Zeta_SlabAllocator un_allocator;
    Zeta_SlabAllocator cn_allocator;

    Zeta_RelLinkedListNode held_cl_head;
    Zeta_RelLinkedListNode unheld_cl_head;

    size_t held_cl_cnt;
    size_t unheld_cl_cnt;
};

// void* Zeta_LRUCacheManager_

void Zeta_LRUCacheManager_Lock(void* lrucm, void* desc);

// void Zeta_LRUCacheManager_Lock(void* lrucm, void* desc);

ZETA_extern_c_end;
