#pragma once

#include "Allocator.h"
#include "BlockVector.h"
#include "MultiLevelVector.h"
#include "RelRBTreeNode.h"
#include "SlabAllocator.h"

ZETA_extern_c_beg;

/*

0: all
1: the first user
2: the second user


M: the Mth user

M + 1

*/

typedef struct Zeta_LRUCacheManager_UNode Zeta_LRUCacheManager_UNode;

struct Zeta_LRUCacheManager_UNode {
    unsigned int max_cache_num;
    unsigned int al_num;

    Zeta_RelRBTreeNode* at_root;

    Zeta_RelLinkedListNode al_head;
    Zeta_RelLinkedListNode ext_al_head;
} __attribute__((aligned(2)));

typedef struct Zeta_LRUCacheManager_CNode Zeta_LRUCacheManager_CNode;

struct Zeta_LRUCacheManager_CNode {
    Zeta_RelRBTreeNode ct;
    Zeta_RelRBLinkedListNode cl;  // dirty bit: l color
    Zeta_RelLinkedListNode bl_head;

    size_t blk_idx;
    byte_t* data;

    unsigned int held_cnt;
};

typedef struct Zeta_LRUCacheManager_XNode Zeta_LRUCacheManager_XNode;

struct Zeta_LRUCacheManager_XNode {
    Zeta_RelRBTreeNode at;
    Zeta_RelLinkedListNode al;
    Zeta_RelLinkedListNode bl;

    uintptr_t un;
    uintptr_t cn;
};

typedef struct Zeta_LRUCacheManager Zeta_LRUCacheManager;

struct Zeta_LRUCacheManager {
    size_t cache_size;
    size_t max_cache_sum;

    Zeta_BlockVector* blk_vec;

    Zeta_Allocator* un_allocator;
    Zeta_Allocator* cn_allocator;
    Zeta_Allocator* xn_allocator;
    Zeta_Allocator* cache_allocator;

    Zeta_RelRBTreeNode un_root*;

    Zeta_RelRBLinkedListNode held_cl_head;
    Zeta_RelRBLinkedListNode unheld_cl_head;

    size_t held_cl_cnt;
    size_t unheld_cl_cnt;

    Zeta_RelRBTreeNode* n_root;
};

// void* Zeta_LRUCacheManager_

void Zeta_LRUCacheManager_Lock(void* lrucm, void* desc);

// void Zeta_LRUCacheManager_Lock(void* lrucm, void* desc);

ZETA_extern_c_end;
