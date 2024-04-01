#pragma once

#include "Allocator.h"
#include "BinHeap.h"
#include "BlockVector.h"
#include "MultiLevelVector.h"
#include "OrdBinTreeNode.h"
#include "OrdRBTreeNode.h"
#include "SegVector.h"
#include "SlabAllocator.h"

ZETA_extern_c_beg;

/*


UNode
    OrdLinkedListNode al_head;
    OrdLinkedListNode ext_al_head;
    map<int, XNode*> at;

CNode
    int blk_idx;
    list<UNode*> u_nodes;
    int held_cnt;
    uintptr_t data; // dirty bit

XNode
    OrdLinkedListNode al_node;
    uintptr_t c_node; // al or ext_al

Manager:
    map<int, CNode*> blk_idx_to_c_node; // blk_idx_to_all_c_nodes;
    set<CNode*> unheld_c_nodes;

RotateFromHeldToUnheld(u_node):
    target_x_node <- find the least x_node in u_node->al_head

    extract target_x_node from u_node->al_head

    set target_x_node->c_node lsb

    push target_x_node into u_node->ext_al_head right

    target_c_node <- x_node->c_node

    dec target_c_node->held_cnt

    if target_c_node->held_cnt == 0:
        insert target_c_node into unheld_ct

RotateFromUnheldToHeld(u_node):
    target_x_node <- find the first x_node in u_node->ext_al_head

    extract target_x_node from u_node->ext_al_head

    unset target_x_node->c_node lsb

    push target_x_node into u_node->al_head left

    target_c_node <- x_node->c_node

    inc target_c_node->held_cnt

    if target_c_node->held_cnt == 1:
        extract target_c_node from unheld_ct


Cache(u_node, blk_idx):



*/

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

    Zeta_SegVector at;
};

typedef struct Zeta_LRUCacheManager_CNode Zeta_LRUCacheManager_CNode;

struct Zeta_LRUCacheManager_CNode {
    Zeta_RelRBTreeNode ct;

    union {
        Zeta_RelLinkedListNode cl;
        Zeta_RelBinTreeNode ch;
    };

    Zeta_RelRBLinkedListNode bl_head;  // dirty bit: l color

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

void Zeta_LRUCacheManager_LogIn(void* lrucm, Zeta_LRUCacheManager_UNode* un);

void Zeta_LRUCacheManager_Lock(void* lrucm, Zeta_LRUCacheManager_UNode* un);

// void Zeta_LRUCacheManager_Lock(void* lrucm, void* desc);

ZETA_extern_c_end;
