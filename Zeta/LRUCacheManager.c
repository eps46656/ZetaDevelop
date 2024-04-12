#include "LRUCacheManager.h"

#include "RBTree.h"
#include "utils.h"

void Zeta_LRUCacheManager_Init(void* lrucm_) {
    Zeta_LRUCacheManager* lrucm = lrucm_;
    ZETA_DebugAssert(lrucm != NULL);

    ZETA_DebugAssert(lrucm->blk_vec != NULL);
    ZETA_DebugAssert(lrucm->blk_vec->GetBlockSize != NULL);
    ZETA_DebugAssert(lrucm->blk_vec->GetBlockNum != NULL);
    ZETA_DebugAssert(lrucm->blk_vec->ReadBlock != NULL);
    ZETA_DebugAssert(lrucm->blk_vec->WriteBlock != NULL);

    ZETA_DebugAssert(lrucm->u_node_allocator != NULL);
    ZETA_DebugAssert(lrucm->u_node_allocator->Allocate != NULL);
    ZETA_DebugAssert(lrucm->u_node_allocator->Deallocate != NULL);

    ZETA_DebugAssert(lrucm->c_node_allocator != NULL);
    ZETA_DebugAssert(lrucm->c_node_allocator->Allocate != NULL);
    ZETA_DebugAssert(lrucm->c_node_allocator->Deallocate != NULL);

    ZETA_DebugAssert(lrucm->x_node_allocator != NULL);
    ZETA_DebugAssert(lrucm->x_node_allocator->Allocate != NULL);
    ZETA_DebugAssert(lrucm->x_node_allocator->Deallocate != NULL);

    ZETA_DebugAssert(lrucm->frame_allocator != NULL);
    ZETA_DebugAssert(lrucm->frame_allocator->Allocate != NULL);
    ZETA_DebugAssert(lrucm->frame_allocator->Deallocate != NULL);

    lrucm->ct_root = NULL;

    Zeta_OrdLinkedListNode_Init(&lrucm->cl_head);
}

static Zeta_LRUCacheManager_UNode* AllocateUNode_(Zeta_LRUCacheManager* lrucm) {
    ZETA_DebugAssert(lrucm->u_node_allocator != NULL);
    ZETA_DebugAssert(lrucm->u_node_allocator->Allocate != NULL);

    void* ret = lrucm->u_node_allocator->Allocate(
        lrucm->u_node_allocator->context, sizeof(Zeta_LRUCacheManager_UNode));
    ZETA_DebugAssert(ret != NULL);

    return ret;
}

static Zeta_LRUCacheManager_CNode* AllocateCNode_(Zeta_LRUCacheManager* lrucm) {
    ZETA_DebugAssert(lrucm->blk_vec != NULL);
    ZETA_DebugAssert(lrucm->blk_vec->GetBlockSize != NULL);

    ZETA_DebugAssert(lrucm->c_node_allocator != NULL);
    ZETA_DebugAssert(lrucm->c_node_allocator->Allocate != NULL);

    ZETA_DebugAssert(lrucm->frame_allocator != NULL);
    ZETA_DebugAssert(lrucm->frame_allocator->Allocate != NULL);

    size_t blk_size = lrucm->blk_vec->GetBlockSize(lrucm->blk_vec->context);

    Zeta_LRUCacheManager_CNode* c_node = lrucm->c_node_allocator->Allocate(
        lrucm->c_node_allocator->context, sizeof(Zeta_LRUCacheManager_CNode));
    ZETA_DebugAssert(c_node != NULL);

    byte_t* frame = lrucm->frame_allocator->Allocate(
        lrucm->frame_allocator->context, blk_size);
    ZETA_DebugAssert(frame != NULL);

    c_node->frame = frame;

    return c_node;
}

static Zeta_LRUCacheManager_XNode* AllocateXNode_(Zeta_LRUCacheManager* lrucm) {
    ZETA_DebugAssert(lrucm->x_node_allocator != NULL);
    ZETA_DebugAssert(lrucm->x_node_allocator->Allocate != NULL);

    Zeta_LRUCacheManager_XNode* x_node = lrucm->x_node_allocator->Allocate(
        lrucm->x_node_allocator->context, sizeof(Zeta_LRUCacheManager_XNode));
    ZETA_DebugAssert(x_node != NULL);

    return x_node;
}

static Zeta_OrdRBTreeNode* FindAT_(Zeta_OrdRBTreeNode* at_root,
                                   size_t blk_idx) {
    Zeta_OrdRBTreeNode* ret = NULL;

    for (Zeta_OrdRBTreeNode* n = at_root; n != NULL;) {
        Zeta_LRUCacheManager_XNode* x_node =
            ZETA_GetStructFromMember(Zeta_LRUCacheManager_XNode, at_node, n);

        if (x_node->c_node->blk_idx < blk_idx) {
            n = Zeta_OrdRBTreeNode_GetR(NULL, n);
        } else {
            ret = n;
            n = Zeta_OrdRBTreeNode_GetL(NULL, n);
        }
    }

    return ret;
}

static Zeta_OrdRBTreeNode* FindCT_(Zeta_OrdRBTreeNode* ct_root,
                                   size_t blk_idx) {
    Zeta_OrdRBTreeNode* ret = NULL;

    for (Zeta_OrdRBTreeNode* n = ct_root; n != NULL;) {
        Zeta_LRUCacheManager_CNode* c_node =
            ZETA_GetStructFromMember(Zeta_LRUCacheManager_CNode, ct_node, n);

        if (c_node->blk_idx < blk_idx) {
            n = Zeta_OrdRBTreeNode_GetR(NULL, n);
        } else {
            ret = n;
            n = Zeta_OrdRBTreeNode_GetL(NULL, n);
        }
    }

    return ret;
}

static void WriteBack_(Zeta_LRUCacheManager* lrucm,
                       Zeta_LRUCacheManager_CNode* c_node) {
    if (!c_node->dirty) { return; }

    c_node->dirty = FALSE;

    ZETA_DebugAssert(lrucm->blk_vec != NULL);
    ZETA_DebugAssert(lrucm->blk_vec->WriteBlock != NULL);

    lrucm->blk_vec->WriteBlock(lrucm->blk_vec->context, c_node->blk_idx,
                               c_node->frame);
}

static void Unref_(Zeta_LRUCacheManager* lrucm,
                   Zeta_BinTreeNodeOperator* rbtn_opr,
                   Zeta_LRUCacheManager_XNode* x_node) {
    Zeta_LRUCacheManager_UNode* u_node = x_node->u_node;

    --u_node->al_num;

    u_node->at_root = Zeta_RBTree_Extract(rbtn_opr, &x_node->at_node);

    Zeta_OrdLinkedListNode_Extract(&x_node->al_node);
    Zeta_OrdLinkedListNode_Extract(&x_node->bl_node);

    Zeta_LRUCacheManager_CNode* c_node = x_node->c_node;

    if (&c_node->bl_head == Zeta_OrdLinkedListNode_GetR(&c_node->bl_head)) {
        c_node->refered = FALSE;

        Zeta_OrdLinkedListNode_Init(&c_node->cl_node);

        Zeta_OrdLinkedListNode_InsertR(&lrucm->cl_head, &c_node->cl_node);
    }
}

static Zeta_LRUCacheManager_XNode* F_(Zeta_LRUCacheManager* lrucm,
                                      Zeta_LRUCacheManager_UNode* u_node,
                                      size_t blk_idx, bool_t fetch) {
    Zeta_OrdRBTreeNode* at_node = FindAT_(u_node->at_root, blk_idx);

    Zeta_LRUCacheManager_XNode* x_node;

    if (at_node != NULL && (x_node = ZETA_GetStructFromMember(
                                Zeta_LRUCacheManager_XNode, at_node, at_node))
                                   ->c_node->blk_idx == blk_idx) {
        Zeta_OrdLinkedListNode_Extract(&x_node->al_node);
        Zeta_OrdLinkedListNode_InsertR(&u_node->al_head, &x_node->al_node);

        return x_node;
    }

    Zeta_BinTreeNodeOperator rbtn_opr;
    Zeta_BinTree_InitOpr(&rbtn_opr);
    Zeta_OrdRBTreeNode_DeployBinTreeNodeOperator(NULL, &rbtn_opr);

    if (u_node->al_num == u_node->max_al_num) {
        x_node = ZETA_GetStructFromMember(
            Zeta_LRUCacheManager_XNode, al_node,
            Zeta_OrdLinkedListNode_GetL(&u_node->al_head));

        Zeta_OrdRBTreeNode* nxt_at_node =
            &x_node->at_node == at_node ? Zeta_BinTree_StepR(&rbtn_opr, at_node)
                                        : at_node;

        Unref_(lrucm, &rbtn_opr, x_node);

        at_node = nxt_at_node;
    } else {
        x_node = AllocateXNode_(lrucm);
        Zeta_OrdRBTreeNode_Init(NULL, &x_node->at_node);
        Zeta_OrdLinkedListNode_Init(&x_node->al_node);
        Zeta_OrdLinkedListNode_Init(&x_node->bl_node);
        x_node->u_node = u_node;
    }

    ++u_node->al_num;

    u_node->at_root = Zeta_RBTree_GeneralInsertL(&rbtn_opr, u_node->at_root,
                                                 at_node, &x_node->at_node);

    Zeta_OrdLinkedListNode_InsertR(&u_node->al_head, &x_node->al_node);

    Zeta_OrdRBTreeNode* ct_node = FindCT_(lrucm->ct_root, blk_idx);

    Zeta_LRUCacheManager_CNode* c_node;

    if (ct_node != NULL && (c_node = ZETA_GetStructFromMember(
                                Zeta_LRUCacheManager_CNode, ct_node, ct_node))
                                   ->blk_idx == blk_idx) {
        if (!c_node->refered) {
            c_node->refered = TRUE;
            Zeta_OrdLinkedListNode_Extract(&c_node->cl_node);
            Zeta_OrdLinkedListNode_Init(&c_node->bl_head);
        }

        Zeta_OrdLinkedListNode_InsertL(&c_node->bl_head, &x_node->bl_node);

        x_node->c_node = c_node;

        return x_node;
    }

    if (lrucm->c_nodes_num < lrucm->max_c_nodes_num) {
        c_node = AllocateCNode_(lrucm);
        Zeta_OrdRBTreeNode_Init(NULL, &c_node->ct_node);
    } else {
        Zeta_OrdLinkedListNode* cl_node =
            Zeta_OrdLinkedListNode_GetL(&lrucm->cl_head);

        Zeta_OrdLinkedListNode_Extract(cl_node);

        c_node = ZETA_GetStructFromMember(Zeta_LRUCacheManager_CNode, cl_node,
                                          cl_node);

        lrucm->ct_root = Zeta_RBTree_Extract(&rbtn_opr, &c_node->ct_node);

        WriteBack_(lrucm, c_node);
    }

    c_node->blk_idx = blk_idx;
    c_node->refered = TRUE;

    lrucm->ct_root = Zeta_RBTree_GeneralInsertL(&rbtn_opr, lrucm->ct_root,
                                                ct_node, &c_node->ct_node);

    Zeta_OrdLinkedListNode_Init(&c_node->bl_head);
    Zeta_OrdLinkedListNode_InsertL(&c_node->bl_head, &x_node->bl_node);

    if (fetch) {
        ZETA_DebugAssert(lrucm->blk_vec != NULL);
        ZETA_DebugAssert(lrucm->blk_vec->ReadBlock != NULL);

        lrucm->blk_vec->ReadBlock(lrucm->blk_vec->context, blk_idx,
                                  c_node->frame);

        c_node->dirty = FALSE;
    }

    x_node->c_node = c_node;

    return x_node;
}

static void SetCachesNumMax_(Zeta_LRUCacheManager* lrucm,
                             Zeta_LRUCacheManager_UNode* u_node,
                             size_t max_caches_num) {
    size_t origin_max_al_num = u_node->max_al_num;

    u_node->max_al_num = max_caches_num;

    if (origin_max_al_num <= max_caches_num) { return; }

    Zeta_BinTreeNodeOperator rbtn_opr;
    Zeta_BinTree_InitOpr(&rbtn_opr);
    Zeta_OrdRBTreeNode_DeployBinTreeNodeOperator(NULL, &rbtn_opr);

    ZETA_DebugAssert(lrucm->x_node_allocator != NULL);
    ZETA_DebugAssert(lrucm->x_node_allocator->Deallocate != NULL);

    while (max_caches_num < u_node->al_num) {
        Zeta_LRUCacheManager_XNode* x_node = ZETA_GetStructFromMember(
            Zeta_LRUCacheManager_XNode, al_node,
            Zeta_OrdLinkedListNode_GetL(&u_node->al_head));

        Unref_(lrucm, &rbtn_opr, x_node);

        lrucm->x_node_allocator->Deallocate(lrucm->x_node_allocator->context,
                                            x_node);
    }

    lrucm->max_c_nodes_num -= u_node->max_al_num;
}

void* Zeta_LRUCacheManager_LogIn(void* lrucm_, size_t max_caches_num) {
    Zeta_LRUCacheManager* lrucm = lrucm_;
    ZETA_DebugAssert(lrucm != NULL);

    ZETA_DebugAssert(0 < max_caches_num);

    Zeta_LRUCacheManager_UNode* u_node = AllocateUNode_(lrucm);

    u_node->max_al_num = max_caches_num;
    u_node->al_num = 0;
    u_node->at_root = NULL;
    Zeta_OrdLinkedListNode_Init(&u_node->al_head);

    lrucm->max_c_nodes_num += max_caches_num;

    return u_node;
}

void Zeta_LRUCacheManager_LogOut(void* lrucm_, void* u_node_) {
    Zeta_LRUCacheManager* lrucm = lrucm_;
    ZETA_DebugAssert(lrucm != NULL);

    Zeta_LRUCacheManager_UNode* u_node = u_node_;
    ZETA_DebugAssert(u_node != NULL);

    SetCachesNumMax_(lrucm, u_node, 0);

    ZETA_DebugAssert(lrucm->u_node_allocator != NULL);
    ZETA_DebugAssert(lrucm->u_node_allocator->Deallocate != NULL);

    lrucm->u_node_allocator->Deallocate(lrucm->u_node_allocator->context,
                                        u_node);
}

void Zeta_LRUCacheManager_SetCachesNumMax(void* lrucm_, void* u_node_,
                                          size_t max_caches_num) {
    Zeta_LRUCacheManager* lrucm = lrucm_;
    ZETA_DebugAssert(lrucm != NULL);

    Zeta_LRUCacheManager_UNode* u_node = u_node_;
    ZETA_DebugAssert(u_node != NULL);

    ZETA_DebugAssert(0 < max_caches_num);

    SetCachesNumMax_(lrucm, u_node, max_caches_num);
}

const byte_t* Zeta_LRUCacheManager_ReadBlock(void* lrucm_, void* u_node_,
                                             size_t blk_idx) {
    Zeta_LRUCacheManager* lrucm = lrucm_;
    ZETA_DebugAssert(lrucm != NULL);

    Zeta_LRUCacheManager_UNode* u_node = u_node_;
    ZETA_DebugAssert(u_node != NULL);

    Zeta_LRUCacheManager_XNode* x_node = F_(lrucm, u_node, blk_idx, TRUE);

    return x_node->c_node->frame;
}

void Zeta_LRUCacheManager_WriteBlock(void* lrucm_, void* u_node_,
                                     size_t blk_idx, byte_t* data) {
    Zeta_LRUCacheManager* lrucm = lrucm_;
    ZETA_DebugAssert(lrucm != NULL);

    Zeta_LRUCacheManager_UNode* u_node = u_node_;
    ZETA_DebugAssert(u_node != NULL);

    ZETA_DebugAssert(data != NULL);

    Zeta_LRUCacheManager_XNode* x_node = F_(lrucm, u_node, blk_idx, FALSE);
    Zeta_LRUCacheManager_CNode* c_node = x_node->c_node;

    c_node->dirty = TRUE;

    ZETA_DebugAssert(lrucm->blk_vec != NULL);
    ZETA_DebugAssert(lrucm->blk_vec->GetBlockSize != NULL);
    ZETA_DebugAssert(lrucm->blk_vec->WriteBlock != NULL);

    size_t blk_size = lrucm->blk_vec->GetBlockSize(lrucm->blk_vec->context);

    Zeta_MemCopy(c_node->frame, data, blk_size);
}

void Zeta_LRUCacheManager_Flush(void* lrucm_, void* u_node_) {
    Zeta_LRUCacheManager* lrucm = lrucm_;
    ZETA_DebugAssert(lrucm != NULL);

    Zeta_LRUCacheManager_UNode* u_node = u_node_;
    ZETA_DebugAssert(u_node != NULL);

    for (Zeta_OrdLinkedListNode* al_node = &u_node->al_head;;) {
        al_node = Zeta_OrdLinkedListNode_GetR(al_node);
        if (&u_node->al_head == al_node) { break; }

        Zeta_LRUCacheManager_XNode* x_node = ZETA_GetStructFromMember(
            Zeta_LRUCacheManager_XNode, al_node, al_node);

        WriteBack_(lrucm, x_node->c_node);
    }
}

static void FlushAll_(Zeta_LRUCacheManager* lrucm,
                      Zeta_OrdRBTreeNode* ct_node) {
    Zeta_LRUCacheManager_CNode* c_node =
        ZETA_GetStructFromMember(Zeta_LRUCacheManager_CNode, ct_node, ct_node);

    if (c_node->dirty) { WriteBack_(lrucm, c_node); }

    Zeta_OrdRBTreeNode* ct_node_l = Zeta_OrdRBTreeNode_GetL(NULL, ct_node);
    Zeta_OrdRBTreeNode* ct_node_r = Zeta_OrdRBTreeNode_GetR(NULL, ct_node);

    if (ct_node_l != NULL) { FlushAll_(lrucm, ct_node_l); }
    if (ct_node_r != NULL) { FlushAll_(lrucm, ct_node_r); }
}

void Zeta_LRUCacheManager_FlushAll(void* lrucm_) {
    Zeta_LRUCacheManager* lrucm = lrucm_;
    ZETA_DebugAssert(lrucm != NULL);

    if (lrucm->ct_root != NULL) { FlushAll_(lrucm, lrucm->ct_root); }
}
