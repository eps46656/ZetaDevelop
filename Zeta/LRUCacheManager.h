#pragma once

#include "Allocator.h"
#include "BlockVector.h"
#include "DebugPipe.h"
#include "OrdLinkedListNode.h"
#include "OrdRBTreeNode.h"

ZETA_ExternC_Beg;

typedef struct Zeta_LRUCacheManager_UNode Zeta_LRUCacheManager_UNode;

struct Zeta_LRUCacheManager_UNode {
    unsigned int max_al_num;
    unsigned int al_num;

    Zeta_OrdRBTreeNode* at_root;

    Zeta_OrdLinkedListNode al_head;
};

typedef struct Zeta_LRUCacheManager_CNode Zeta_LRUCacheManager_CNode;

struct Zeta_LRUCacheManager_CNode {
    size_t blk_idx;
    byte_t* frame;

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

typedef struct Zeta_LRUCacheManager_XNode Zeta_LRUCacheManager_XNode;

struct Zeta_LRUCacheManager_XNode {
    Zeta_OrdRBTreeNode at_node;
    // in blk_idx order

    Zeta_OrdLinkedListNode al_node;
    // in lru order

    Zeta_OrdLinkedListNode bl_node;
    // to next x_node which also ref to same c_node

    Zeta_LRUCacheManager_UNode* u_node;
    Zeta_LRUCacheManager_CNode* c_node;
};

typedef struct Zeta_LRUCacheManager Zeta_LRUCacheManager;

struct Zeta_LRUCacheManager {
    size_t c_nodes_num;
    size_t max_c_nodes_num;

    Zeta_BlockVector* blk_vec;

    Zeta_Allocator* u_node_allocator;
    Zeta_Allocator* c_node_allocator;
    Zeta_Allocator* x_node_allocator;
    Zeta_Allocator* frame_allocator;

    Zeta_OrdRBTreeNode* ct_root;

    Zeta_OrdLinkedListNode cl_head;
};

void Zeta_LRUCacheManager_Init(void* lrucm);

void* Zeta_LRUCacheManager_LogIn(void* lrucm, size_t max_caches_num);

void Zeta_LRUCacheManager_LogOut(void* lrucm, void* u_node);

void Zeta_LRUCacheManager_SetCachesNumMax(void* lrucm, void* u_node,
                                          size_t max_caches_num);

const byte_t* Zeta_LRUCacheManager_ReadBlock(void* lrucm, void* u_node,
                                             size_t blk_idx);

void Zeta_LRUCacheManager_WriteBlock(void* lrucm, void* u_node, size_t blk_idx,
                                     byte_t* data);

void Zeta_LRUCacheManager_Flush(void* lrucm, void* u_node);

void Zeta_LRUCacheManager_FlushAll(void* lrucm);

void Zeta_LRUCacheManager_Check(void* lrucm, Zeta_DebugPipe* u_nodes);

ZETA_ExternC_End;
