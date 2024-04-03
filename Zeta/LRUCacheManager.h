#pragma once

#include "Allocator.h"
#include "BlockVector.h"
#include "OrdLinkedListNode.h"
#include "OrdRBTreeNode.h"

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


Ref(u_node, c_node):
    pass

Unref(x_node):
    u_node <- from x_node
    c_node <- from x_node

    extract x_node from u_node
    extract x_node from c_node

    if x_node is in u_node->al:
        --c_node->held_cnt;

        if c_node->held_cnt == 0:
            move c_node from helf_cl to unheld_cl
    else

Cache(u_node, blk_idx):
    c_node <- allocate c_node from c_node_allocator

    // c_node->data <- allocate




F(u_node, blk_idx):
    if blk_idx is in u_node->al:
        x_node <= u_node->at[blk_idx]
        extract x_node from u_node->al
        push x_node in the front of u_node->al
        return

    if blk_idx is in ct:
        c_node <= ct[blk_idx]
        Ref_(u_node, c_node);
        return

    target_c_node

    if length of cl does not reach max:
        target_c_node <= AllocateCNode_()
        // Init(target_c_node)
    else if ext_cl is empty:
        target_c_node <= the last of u_node->al
        UnRef_(u_node, target_c_node)
    else:
        target_c_node <= random select from ext_cl

    target_c_node->data <= block_vec[blk_idx]
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
        if refered == 0:
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

    Zeta_OrdLinkedListNode* cl_head;
};

void* Zeta_LRUCacheManager_LogIn(void* lrucm, size_t max_caches_num);

void Zeta_LRUCacheManager_LogOut(void* lrucm, void* u_node);

void Zeta_LRUCacheManager_SetCachesNumMax(void* lrucm, void* u_node,
                                          size_t max_caches_num);

const byte_t* Zeta_LRUCacheManager_ReadBlock(void* lrucm, void* u_node,
                                             size_t blk_idx);

void Zeta_LRUCacheManager_WriteBlock(void* lrucm, void* u_node, size_t blk_idx,
                                     byte_t* data);

void Zeta_LRUCacheManager_Flush(void* lrucm, void* u_node);

ZETA_extern_c_end;
