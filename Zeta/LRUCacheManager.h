#pragma once

#include "Allocator.h"
#include "BlockVector.h"
#include "CacheManager.h"
#include "DebugPipe.h"
#include "OrdBinTreeNode.h"
#include "OrdLinkedListNode.h"
#include "OrdRBTreeNode.h"

ZETA_ExternC_Beg;

ZETA_DeclareStruct(Zeta_LRUCacheManager);
ZETA_DeclareStruct(Zeta_LRUCacheManager_SNode);
ZETA_DeclareStruct(Zeta_LRUCacheManager_CNode);
ZETA_DeclareStruct(Zeta_LRUCacheManager_XNode);

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

struct Zeta_LRUCacheManager_SNode {
    Zeta_OrdLinkedListNode sl_node;

    unsigned int max_al_cnt;
    unsigned int al_cnt;

    Zeta_OrdLinkedListNode al_head;

    Zeta_OrdRBTreeNode* at_root;
};

struct Zeta_LRUCacheManager_CNode {
    size_t blk_num;
    void* frame;

    bool_t refered;
    bool_t dirty;

    Zeta_OrdRBTreeNode ct_node;

    /*
        if refered:
            cl_node links to other c_node
            bl_head must be empty.
        else:
            cl_node does not link to other c_node
            bl_head must be not empty.
    */
    union {
        Zeta_OrdLinkedListNode bl_head;
        Zeta_OrdLinkedListNode cl_node;
    };
};

struct Zeta_LRUCacheManager_XNode {
    Zeta_OrdRBTreeNode at_node;
    // in blk_num order

    Zeta_OrdLinkedListNode al_node;
    // in lru order

    Zeta_OrdLinkedListNode bl_node;
    // to next x_node which also ref to same c_node

    Zeta_LRUCacheManager_SNode* s_node;
    Zeta_LRUCacheManager_CNode* c_node;
};

struct Zeta_LRUCacheManager {
    size_t c_node_cnt;
    size_t max_c_node_cnt;

    Zeta_BlockVector* blk_vec;

    Zeta_Allocator* s_node_allocator;
    Zeta_Allocator* c_node_allocator;
    Zeta_Allocator* x_node_allocator;
    Zeta_Allocator* frame_allocator;

    Zeta_OrdLinkedListNode* norm_sl_head;

    Zeta_OrdLinkedListNode* over_sl_head;

    Zeta_OrdRBTreeNode* ct_root;

    Zeta_OrdLinkedListNode cl_head;
};

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

void Zeta_LRUCacheManager_Init(void* lrucm);

size_t Zeta_LRUCacheManager_GetBlockSize(void* lrucm);

void* Zeta_LRUCacheManager_Open(void* lrucm, size_t max_cache_cnt);

void Zeta_LRUCacheManager_Close(void* lrucm, void* sd);

void Zeta_LRUCacheManager_SetMaxCacheCnt(void* lrucm, void* sd,
                                         size_t max_cache_cnt);

void const* Zeta_LRUCacheManager_ReadBlock(void* lrucm, void* sd,
                                           size_t blk_num);

void Zeta_LRUCacheManager_WriteBlock(void* lrucm, void* sd, size_t blk_num,
                                     void const* data);

void Zeta_LRUCacheManager_FlushBlock(void* lrucm, size_t blk_num);

void Zeta_LRUCacheManager_Flush(void* lrucm, void* sd);

void Zeta_LRUCacheManager_FlushAll(void* lrucm);

bool_t Zeta_LRUCacheManager_UnrefOverRef(void* lrcum);

bool_t Zeta_LRUCacheManager_UnrefOverCache(void* lrcum);

void Zeta_LRUCacheManager_DeployCacheManager(void* lrucm,
                                             Zeta_CacheManager* dst);

ZETA_ExternC_End;
