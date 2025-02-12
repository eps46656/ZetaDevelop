#pragma once

#include "allocator.h"
#include "cache_manager.h"
#include "generic_hash_table.h"
#include "ord_bin_tree_node.h"
#include "ord_llist_node.h"
#include "ord_rb_llist_node.h"
#include "ord_rb_tree_node.h"
#include "seq_cntr.h"

ZETA_ExternC_Beg;

#define ZETA_LRUCachaManager_max_cn_cnts (ZETA_UINT_MAX)

ZETA_DeclareStruct(Zeta_LRUCacheManager);
ZETA_DeclareStruct(Zeta_LRUCacheManager_BNode);
ZETA_DeclareStruct(Zeta_LRUCacheManager_SNode);
ZETA_DeclareStruct(Zeta_LRUCacheManager_CNode);
ZETA_DeclareStruct(Zeta_LRUCacheManager_XNode);

#define Zeta_LRUCacheManager_SNode_color (0)
#define Zeta_LRUCacheManager_CNode_color (1)
#define Zeta_LRUCacheManager_XNode_color (2)

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

struct Zeta_LRUCacheManager_BNode {
    ZETA_DebugStructPadding;

    Zeta_GenericHashTable_Node ghtn;

    ZETA_DebugStructPadding;

    Zeta_OrdRBLListNode ln;
    // p: color
};

struct Zeta_LRUCacheManager_SNode {
    ZETA_DebugStructPadding;

    Zeta_LRUCacheManager_BNode bn;

    ZETA_DebugStructPadding;

    Zeta_OrdLListNode sln;

    ZETA_DebugStructPadding;

    unsigned cn_cnt;
    unsigned max_cn_cnt;
};

struct Zeta_LRUCacheManager_CNode {
    ZETA_DebugStructPadding;

    Zeta_LRUCacheManager_BNode bn;

    ZETA_DebugStructPadding;

    size_t cache_idx;
    void* frame;

    unsigned ref_cnt;
    bool_t dirty;
};

struct Zeta_LRUCacheManager_XNode {
    ZETA_DebugStructPadding;

    Zeta_LRUCacheManager_BNode bn;

    ZETA_DebugStructPadding;

    Zeta_LRUCacheManager_SNode* sn;
    Zeta_LRUCacheManager_CNode* cn;
};

struct Zeta_LRUCacheManager {
    size_t cn_cnt;
    size_t max_cn_cnt;

    Zeta_SeqCntr* origin;

    size_t cache_size;

    Zeta_Allocator* sn_allocator;
    Zeta_Allocator* cn_allocator;
    Zeta_Allocator* xn_allocator;
    Zeta_Allocator* frame_allocator;

    Zeta_GenericHashTable ght;

    Zeta_OrdLListNode* fit_sl;
    Zeta_OrdLListNode* over_sl;

    Zeta_OrdRBLListNode* hot_clear_cl;
    Zeta_OrdRBLListNode* hot_dirty_cl;
    Zeta_OrdRBLListNode* cold_clear_cl;
    Zeta_OrdRBLListNode* cold_dirty_cl;
};

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

void Zeta_LRUCacheManager_Init(void* lrucm);

void Zeta_LRUCacheManager_Deinit(void* lrucm);

Zeta_SeqCntr* Zeta_LRUCacheManager_GetOrigin(void const* lrucm);

size_t Zeta_LRUCacheManager_GetCacheSize(void const* lrucm);

void* Zeta_LRUCacheManager_Open(void* lrucm, size_t max_cache_cnt);

void Zeta_LRUCacheManager_Close(void* lrucm, void* sd);

void Zeta_LRUCacheManager_SetMaxCacheCnt(void* lrucm, void* sd,
                                         size_t max_cache_cnt);

void Zeta_LRUCacheManager_Read(void* lrucm, void* sd, size_t idx, size_t cnt,
                               void* dst, size_t dst_stride);

void Zeta_LRUCacheManager_Write(void* lrucm, void* sd, size_t idx, size_t cnt,
                                void const* src, size_t src_stride);

size_t Zeta_LRUCacheManager_Flush(void* lrucm, size_t quata);

bool_t Zeta_LRUCacheManager_RunPending(void* lrucm, size_t calc_quata,
                                       size_t write_quata);

void Zeta_LRUCacheManager_Check(void const* lrucm);

void Zeta_LRUCacheManager_CheckSessionDescriptor(void const* lrucm, void* sd);

void Zeta_LRUCacheManager_DeployCacheManager(void* lrucm,
                                             Zeta_CacheManager* dst);

ZETA_ExternC_End;
