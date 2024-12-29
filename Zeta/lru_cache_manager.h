#pragma once

#include "allocator.h"
#include "cache_manager.h"
#include "generic_hash_table.h"
#include "ord_bin_tree_node.h"
#include "ord_linked_list_node.h"
#include "ord_rb_linked_list_node.h"
#include "ord_rb_tree_node.h"
#include "seq_cntr.h"

ZETA_ExternC_Beg;

#define ZETA_LRUCachaManager_max_cn_cnts (ZETA_UINT_MAX)

ZETA_DeclareStruct(Zeta_LRUCacheManager);
ZETA_DeclareStruct(Zeta_LRUCacheManager_SNode);
ZETA_DeclareStruct(Zeta_LRUCacheManager_CXNodeBase);
ZETA_DeclareStruct(Zeta_LRUCacheManager_CNode);
ZETA_DeclareStruct(Zeta_LRUCacheManager_XNode);

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

#define Zeta_LRUCacheManager_SNode_color (1)
#define Zeta_LRUCacheManager_CNode_color (2)
#define Zeta_LRUCacheManager_XNode_color (3)

struct Zeta_LRUCacheManager_SNode {
    Zeta_OrdRBLinkedListNode lh;
    // p: color

    unsigned cn_cnt;
    unsigned max_cn_cnt;
};

struct Zeta_LRUCacheManager_CXNodeBase {
    Zeta_GenericHashTable_Node ghtn;

    Zeta_OrdRBLinkedListNode ln;
    // p: color

    size_t blk_num;
};

struct Zeta_LRUCacheManager_CNode {
    Zeta_LRUCacheManager_CXNodeBase base;

    void* frame;

    unsigned ref_cnt;
    bool_t dirty;
};

struct Zeta_LRUCacheManager_XNode {
    Zeta_LRUCacheManager_CXNodeBase base;

    Zeta_LRUCacheManager_SNode* sn;
    Zeta_LRUCacheManager_CNode* cn;
};

struct Zeta_LRUCacheManager {
    size_t cn_cnt;
    size_t max_cn_cnt;

    Zeta_SeqCntr* origin;

    Zeta_Allocator* sn_allocator;
    Zeta_Allocator* cn_allocator;
    Zeta_Allocator* xn_allocator;
    Zeta_Allocator* frame_allocator;

    Zeta_GenericHashTable ght;

    Zeta_OrdRBLinkedListNode* cold_cl;
};

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

void Zeta_LRUCacheManager_Init(void* lrucm);

size_t Zeta_LRUCacheManager_GetWidth(void* lrucm);

void* Zeta_LRUCacheManager_Open(void* lrucm, size_t max_cache_cnt);

void Zeta_LRUCacheManager_Close(void* lrucm, void* sd);

void Zeta_LRUCacheManager_SetMaxCacheCnt(void* lrucm, void* sd,
                                         size_t max_cache_cnt);

void const* Zeta_LRUCacheManager_Read(void* lrucm, void* sd, size_t blk_num);

void Zeta_LRUCacheManager_Write(void* lrucm, void* sd, size_t blk_num,
                                void const* data);

void Zeta_LRUCacheManager_FlushBlock(void* lrucm, size_t blk_num);

void Zeta_LRUCacheManager_Flush(void* lrucm, void* sd);

void Zeta_LRUCacheManager_FlushAll(void* lrucm);

bool_t Zeta_LRUCacheManager_UnrefOverRef(void* lrcum);

bool_t Zeta_LRUCacheManager_UnrefOverCache(void* lrcum);

void Zeta_LRUCacheManager_DeployCacheManager(void* lrucm,
                                             Zeta_CacheManager* dst);

ZETA_ExternC_End;
