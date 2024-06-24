#include "LRUCacheManager.h"

#include "RBTree.h"
#include "utils.h"

void Zeta_LRUCacheManager_Init(void* lrucm_) {
    Zeta_LRUCacheManager* lrucm = lrucm_;
    ZETA_DebugAssert(lrucm != NULL);

    ZETA_DebugAssert(lrucm->blk_vec != NULL);
    ZETA_DebugAssert(lrucm->blk_vec->ReadBlock != NULL);
    ZETA_DebugAssert(lrucm->blk_vec->WriteBlock != NULL);

    ZETA_DebugAssert(lrucm->s_node_allocator != NULL);
    ZETA_DebugAssert(lrucm->s_node_allocator->Allocate != NULL);
    ZETA_DebugAssert(lrucm->s_node_allocator->Deallocate != NULL);

    ZETA_DebugAssert(lrucm->c_node_allocator != NULL);
    ZETA_DebugAssert(lrucm->c_node_allocator->Allocate != NULL);
    ZETA_DebugAssert(lrucm->c_node_allocator->Deallocate != NULL);

    ZETA_DebugAssert(lrucm->x_node_allocator != NULL);
    ZETA_DebugAssert(lrucm->x_node_allocator->Allocate != NULL);
    ZETA_DebugAssert(lrucm->x_node_allocator->Deallocate != NULL);

    ZETA_DebugAssert(lrucm->frame_allocator != NULL);
    ZETA_DebugAssert(lrucm->frame_allocator->Allocate != NULL);
    ZETA_DebugAssert(lrucm->frame_allocator->Deallocate != NULL);

    lrucm->norm_sl_head = NULL;
    lrucm->over_sl_head = NULL;

    lrucm->ct_root = NULL;

    Zeta_OrdLinkedListNode_Init(&lrucm->cl_head);
}

static Zeta_LRUCacheManager_SNode* AllocateUNode_(Zeta_LRUCacheManager* lrucm) {
    ZETA_DebugAssert(lrucm->s_node_allocator != NULL);
    ZETA_DebugAssert(lrucm->s_node_allocator->Allocate != NULL);

    void* s_node = lrucm->s_node_allocator->Allocate(
        lrucm->s_node_allocator->context, sizeof(Zeta_LRUCacheManager_SNode));

    ZETA_DebugAssert(s_node != NULL);
    ZETA_DebugAssert(
        ZETA_PtrToAddr(s_node) % alignof(Zeta_LRUCacheManager_SNode) == 0);

    return s_node;
}

static Zeta_LRUCacheManager_CNode* AllocateCNode_(Zeta_LRUCacheManager* lrucm) {
    ZETA_DebugAssert(lrucm->blk_vec != NULL);
    ZETA_DebugAssert(lrucm->blk_vec->GetBlockSize != NULL);

    ZETA_DebugAssert(lrucm->c_node_allocator != NULL);
    ZETA_DebugAssert(lrucm->c_node_allocator->Allocate != NULL);

    ZETA_DebugAssert(lrucm->frame_allocator != NULL);
    ZETA_DebugAssert(lrucm->frame_allocator->Allocate != NULL);

    size_t blk_size = lrucm->blk_vec->GetBlockSize(lrucm->blk_vec->context);

    void* c_node_ = lrucm->c_node_allocator->Allocate(
        lrucm->c_node_allocator->context, sizeof(Zeta_LRUCacheManager_CNode));

    ZETA_DebugAssert(c_node_ != NULL);
    ZETA_DebugAssert(
        ZETA_PtrToAddr(c_node_) % alignof(Zeta_LRUCacheManager_CNode) == 0);

    Zeta_LRUCacheManager_CNode* c_node = c_node_;

    void* frame = lrucm->frame_allocator->Allocate(
        lrucm->frame_allocator->context, blk_size);
    ZETA_DebugAssert(frame != NULL);

    c_node->frame = frame;

    return c_node;
}

static Zeta_LRUCacheManager_XNode* AllocateXNode_(Zeta_LRUCacheManager* lrucm) {
    ZETA_DebugAssert(lrucm->x_node_allocator != NULL);
    ZETA_DebugAssert(lrucm->x_node_allocator->Allocate != NULL);

    void* x_node = lrucm->x_node_allocator->Allocate(
        lrucm->x_node_allocator->context, sizeof(Zeta_LRUCacheManager_XNode));

    ZETA_DebugAssert(x_node != NULL);
    ZETA_DebugAssert(
        ZETA_PtrToAddr(x_node) % alignof(Zeta_LRUCacheManager_XNode) == 0);

    return x_node;
}

static Zeta_OrdRBTreeNode* FindAT_(Zeta_OrdRBTreeNode* at_root,
                                   size_t blk_num) {
    Zeta_OrdRBTreeNode* ret = NULL;

    for (Zeta_OrdRBTreeNode* n = at_root; n != NULL;) {
        Zeta_LRUCacheManager_XNode* x_node =
            ZETA_MemberToStruct(Zeta_LRUCacheManager_XNode, at_node, n);

        if (x_node->c_node->blk_num < blk_num) {
            n = Zeta_OrdRBTreeNode_GetR(NULL, n);
        } else {
            ret = n;
            n = Zeta_OrdRBTreeNode_GetL(NULL, n);
        }
    }

    return ret;
}

static Zeta_OrdRBTreeNode* FindCT_(Zeta_OrdRBTreeNode* ct_root,
                                   size_t blk_num) {
    Zeta_OrdRBTreeNode* ret = NULL;

    for (Zeta_OrdRBTreeNode* n = ct_root; n != NULL;) {
        Zeta_LRUCacheManager_CNode* c_node =
            ZETA_MemberToStruct(Zeta_LRUCacheManager_CNode, ct_node, n);

        if (c_node->blk_num < blk_num) {
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

    ZETA_PrintPos;

    lrucm->blk_vec->WriteBlock(lrucm->blk_vec->context, c_node->blk_num,
                               c_node->frame);
}

static void Unref_(Zeta_LRUCacheManager* lrucm,
                   Zeta_BinTreeNodeOperator* rbtn_opr,
                   Zeta_LRUCacheManager_XNode* x_node) {
    Zeta_LRUCacheManager_SNode* s_node = x_node->s_node;

    --s_node->al_cnt;

    Zeta_OrdLinkedListNode_Extract(&x_node->al_node);

    s_node->at_root = Zeta_RBTree_Extract(rbtn_opr, &x_node->at_node);

    Zeta_OrdLinkedListNode_Extract(&x_node->bl_node);

    Zeta_LRUCacheManager_CNode* c_node = x_node->c_node;

    if (&c_node->bl_head == Zeta_OrdLinkedListNode_GetR(&c_node->bl_head)) {
        c_node->refered = FALSE;

        Zeta_OrdLinkedListNode_Init(&c_node->cl_node);

        Zeta_OrdLinkedListNode_InsertR(&lrucm->cl_head, &c_node->cl_node);
    }
}

void UnrefLast_(Zeta_LRUCacheManager* lrucm,
                Zeta_LRUCacheManager_SNode* s_node) {
    if (s_node->al_cnt <= s_node->max_al_cnt) { return; }

    Zeta_BinTreeNodeOperator rbtn_opr;
    Zeta_BinTree_InitOpr(&rbtn_opr);
    Zeta_OrdRBTreeNode_DeployBinTreeNodeOperator(NULL, &rbtn_opr);

    Zeta_LRUCacheManager_XNode* x_node =
        ZETA_MemberToStruct(Zeta_LRUCacheManager_XNode, al_node,
                            Zeta_OrdLinkedListNode_GetL(&s_node->al_head));

    Unref_(lrucm, &rbtn_opr, x_node);

    lrucm->x_node_allocator->Deallocate(lrucm->x_node_allocator->context,
                                        x_node);
}

bool_t UnrefOverRef_(Zeta_LRUCacheManager* lrucm) {
    Zeta_OrdLinkedListNode* over_sl_head = lrucm->over_sl_head;

    if (over_sl_head == NULL) { return FALSE; }

    Zeta_LRUCacheManager_SNode* s_node =
        ZETA_MemberToStruct(Zeta_LRUCacheManager_SNode, sl_node, over_sl_head);

    UnrefLast_(lrucm, s_node);

    Zeta_OrdLinkedListNode* nxt_over_sl_head =
        Zeta_OrdLinkedListNode_GetR(over_sl_head);

    if (s_node->max_al_cnt < s_node->al_cnt) {
        lrucm->over_sl_head = nxt_over_sl_head;
        return TRUE;
    }

    if (over_sl_head == nxt_over_sl_head) {
        lrucm->over_sl_head = NULL;
    } else {
        Zeta_OrdLinkedListNode_Extract(over_sl_head);
        lrucm->over_sl_head = nxt_over_sl_head;
    }

    return TRUE;
}

static Zeta_LRUCacheManager_XNode* F_(Zeta_LRUCacheManager* lrucm,
                                      Zeta_LRUCacheManager_SNode* s_node,
                                      size_t blk_num, bool_t fetch) {
    ZETA_DebugAssert(lrucm->blk_vec != NULL);

    Zeta_OrdRBTreeNode* at_node = FindAT_(s_node->at_root, blk_num);

    Zeta_LRUCacheManager_XNode* x_node;

    if (at_node != NULL && (x_node = ZETA_MemberToStruct(
                                Zeta_LRUCacheManager_XNode, at_node, at_node))
                                   ->c_node->blk_num == blk_num) {
        Zeta_OrdLinkedListNode_Extract(&x_node->al_node);
        Zeta_OrdLinkedListNode_InsertR(&s_node->al_head, &x_node->al_node);

        UnrefOverRef_(lrucm);

        return x_node;
    }

    ZETA_DebugAssert(lrucm->x_node_allocator != NULL);
    ZETA_DebugAssert(lrucm->x_node_allocator->Deallocate != NULL);

    Zeta_BinTreeNodeOperator rbtn_opr;
    Zeta_BinTree_InitOpr(&rbtn_opr);
    Zeta_OrdRBTreeNode_DeployBinTreeNodeOperator(NULL, &rbtn_opr);

    Zeta_OrdRBTreeNode* ct_node = FindCT_(lrucm->ct_root, blk_num);

    Zeta_LRUCacheManager_CNode* c_node;

    if (ct_node != NULL && (c_node = ZETA_MemberToStruct(
                                Zeta_LRUCacheManager_CNode, ct_node, ct_node))
                                   ->blk_num == blk_num) {
        if (!c_node->refered) {
            c_node->refered = TRUE;
            Zeta_OrdLinkedListNode_Extract(&c_node->cl_node);
            Zeta_OrdLinkedListNode_Init(&c_node->bl_head);
        }
    } else {
        ++lrucm->c_node_cnt;

        c_node = AllocateCNode_(lrucm);
        Zeta_OrdRBTreeNode_Init(NULL, &c_node->ct_node);

        c_node->blk_num = blk_num;
        c_node->refered = TRUE;

        lrucm->ct_root = Zeta_RBTree_GeneralInsertL(&rbtn_opr, lrucm->ct_root,
                                                    ct_node, &c_node->ct_node);

        Zeta_OrdLinkedListNode_Init(&c_node->bl_head);

        if (fetch) {
            ZETA_DebugAssert(lrucm->blk_vec != NULL);
            ZETA_DebugAssert(lrucm->blk_vec->ReadBlock != NULL);

            lrucm->blk_vec->ReadBlock(lrucm->blk_vec->context, blk_num,
                                      c_node->frame);

            c_node->dirty = FALSE;
        }
    }

    // allocate and initialize x_node
    x_node = AllocateXNode_(lrucm);
    Zeta_OrdLinkedListNode_Init(&x_node->al_node);
    Zeta_OrdRBTreeNode_Init(NULL, &x_node->at_node);
    Zeta_OrdLinkedListNode_Init(&x_node->bl_node);

    // s_node links to x_node
    ++s_node->al_cnt;
    Zeta_OrdLinkedListNode_InsertR(&s_node->al_head, &x_node->al_node);
    s_node->at_root = Zeta_RBTree_GeneralInsertL(&rbtn_opr, s_node->at_root,
                                                 at_node, &x_node->at_node);

    // c_node links to x_node
    Zeta_OrdLinkedListNode_InsertL(&c_node->bl_head, &x_node->bl_node);

    // x_node links to s_node and c_node
    x_node->s_node = s_node;
    x_node->c_node = c_node;

    UnrefLast_(lrucm, s_node);

    UnrefOverRef_(lrucm);

    return x_node;
}

static void SetMaxCacheCnt_(Zeta_LRUCacheManager* lrucm,
                            Zeta_LRUCacheManager_SNode* s_node,
                            size_t max_cache_cnt) {
    size_t old_max_cache_cnt = s_node->max_al_cnt;
    s_node->max_al_cnt = max_cache_cnt;

    bool_t old_is_over = old_max_cache_cnt < s_node->al_cnt;
    bool_t new_is_over = max_cache_cnt < s_node->al_cnt;

    if (old_is_over == new_is_over) { return; }

    Zeta_OrdLinkedListNode* sl_node = &s_node->sl_node;

    if (old_is_over) {
        if (lrucm->over_sl_head != sl_node) {
            Zeta_OrdLinkedListNode_Extract(sl_node);
        } else {
            Zeta_OrdLinkedListNode* nxt_sl_node =
                Zeta_OrdLinkedListNode_GetR(sl_node);

            Zeta_OrdLinkedListNode_Extract(sl_node);

            lrucm->over_sl_head = sl_node == nxt_sl_node ? NULL : nxt_sl_node;
        }
    }

    if (new_is_over) {
        if (lrucm->over_sl_head == NULL) {
            lrucm->over_sl_head = sl_node;
        } else {
            Zeta_OrdLinkedListNode_InsertL(lrucm->over_sl_head, sl_node);
        }
    }

    lrucm->max_c_node_cnt += max_cache_cnt - old_max_cache_cnt;
}

size_t Zeta_LRUCacheManager_GetBlockSize(void* lrucm_) {
    Zeta_LRUCacheManager* lrucm = lrucm_;
    ZETA_DebugAssert(lrucm != NULL);

    return lrucm->blk_vec->GetBlockSize(lrucm->blk_vec->context);
}

void* Zeta_LRUCacheManager_Open(void* lrucm_, size_t max_cache_cnt) {
    Zeta_LRUCacheManager* lrucm = lrucm_;
    ZETA_DebugAssert(lrucm != NULL);

    ZETA_DebugAssert(0 < max_cache_cnt);

    Zeta_LRUCacheManager_SNode* s_node = AllocateUNode_(lrucm);

    Zeta_OrdLinkedListNode_Init(&s_node->sl_node);

    s_node->max_al_cnt = max_cache_cnt;
    s_node->al_cnt = 0;

    Zeta_OrdLinkedListNode_Init(&s_node->al_head);

    s_node->at_root = NULL;

    lrucm->max_c_node_cnt += max_cache_cnt;

    return s_node;
}

void Zeta_LRUCacheManager_Close(void* lrucm_, void* sd) {
    Zeta_LRUCacheManager* lrucm = lrucm_;
    ZETA_DebugAssert(lrucm != NULL);

    Zeta_LRUCacheManager_SNode* s_node = sd;
    ZETA_DebugAssert(s_node != NULL);

    SetMaxCacheCnt_(lrucm, s_node, 0);
}

void Zeta_LRUCacheManager_SetMaxCacheCnt(void* lrucm_, void* sd,
                                         size_t max_cache_cnt) {
    Zeta_LRUCacheManager* lrucm = lrucm_;
    ZETA_DebugAssert(lrucm != NULL);

    Zeta_LRUCacheManager_SNode* s_node = sd;
    ZETA_DebugAssert(s_node != NULL);

    ZETA_DebugAssert(0 < max_cache_cnt);

    SetMaxCacheCnt_(lrucm, s_node, max_cache_cnt);
}

void const* Zeta_LRUCacheManager_ReadBlock(void* lrucm_, void* sd,
                                           size_t blk_num) {
    Zeta_LRUCacheManager* lrucm = lrucm_;
    ZETA_DebugAssert(lrucm != NULL);

    Zeta_LRUCacheManager_SNode* s_node = sd;
    ZETA_DebugAssert(s_node != NULL);

    Zeta_LRUCacheManager_XNode* x_node = F_(lrucm, s_node, blk_num, TRUE);

    return x_node->c_node->frame;
}

void Zeta_LRUCacheManager_WriteBlock(void* lrucm_, void* sd, size_t blk_num,
                                     void const* data) {
    Zeta_LRUCacheManager* lrucm = lrucm_;
    ZETA_DebugAssert(lrucm != NULL);

    Zeta_LRUCacheManager_SNode* s_node = sd;
    ZETA_DebugAssert(s_node != NULL);

    ZETA_DebugAssert(data != NULL);

    Zeta_LRUCacheManager_XNode* x_node = F_(lrucm, s_node, blk_num, FALSE);
    Zeta_LRUCacheManager_CNode* c_node = x_node->c_node;

    c_node->dirty = TRUE;

    ZETA_DebugAssert(lrucm->blk_vec != NULL);
    ZETA_DebugAssert(lrucm->blk_vec->GetBlockSize != NULL);
    ZETA_DebugAssert(lrucm->blk_vec->WriteBlock != NULL);

    size_t blk_size = lrucm->blk_vec->GetBlockSize(lrucm->blk_vec->context);

    Zeta_MemCopy(c_node->frame, data, blk_size);
}

void Zeta_LRUCacheManager_FlushBlock(void* lrucm_, size_t blk_num) {
    Zeta_LRUCacheManager* lrucm = lrucm_;
    ZETA_DebugAssert(lrucm != NULL);

    ZETA_DebugAssert(lrucm->blk_vec != NULL);

    Zeta_OrdRBTreeNode* ct_node = FindCT_(lrucm->ct_root, blk_num);

    Zeta_LRUCacheManager_CNode* c_node;

    if (ct_node != NULL && (c_node = ZETA_MemberToStruct(
                                Zeta_LRUCacheManager_CNode, ct_node, ct_node))
                                   ->blk_num == blk_num) {
        WriteBack_(lrucm, c_node);
    }
}

void Zeta_LRUCacheManager_Flush(void* lrucm_, void* sd) {
    Zeta_LRUCacheManager* lrucm = lrucm_;
    ZETA_DebugAssert(lrucm != NULL);

    Zeta_LRUCacheManager_SNode* s_node = sd;
    ZETA_DebugAssert(s_node != NULL);

    for (Zeta_OrdLinkedListNode* al_node = &s_node->al_head;;) {
        al_node = Zeta_OrdLinkedListNode_GetR(al_node);
        if (&s_node->al_head == al_node) { break; }

        Zeta_LRUCacheManager_XNode* x_node =
            ZETA_MemberToStruct(Zeta_LRUCacheManager_XNode, al_node, al_node);

        WriteBack_(lrucm, x_node->c_node);
    }
}

static void FlushAll_(Zeta_LRUCacheManager* lrucm,
                      Zeta_OrdRBTreeNode* ct_node) {
    Zeta_LRUCacheManager_CNode* c_node =
        ZETA_MemberToStruct(Zeta_LRUCacheManager_CNode, ct_node, ct_node);

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

bool_t Zeta_LRUCacheManager_UnrefOverRef(void* lrucm_) {
    Zeta_LRUCacheManager* lrucm = lrucm_;
    ZETA_DebugAssert(lrucm != NULL);

    return UnrefOverRef_(lrucm);
}

bool_t Zeta_LRUCacheManager_UnrefOverCache(void* lrucm_) {
    Zeta_LRUCacheManager* lrucm = lrucm_;
    ZETA_DebugAssert(lrucm != NULL);

    if (lrucm->c_node_cnt <= lrucm->max_c_node_cnt) { return FALSE; }

    Zeta_OrdLinkedListNode* cl_node =
        Zeta_OrdLinkedListNode_GetL(&lrucm->cl_head);

    if (&lrucm->cl_head == cl_node) { return FALSE; }

    Zeta_BinTreeNodeOperator rbtn_opr;
    Zeta_BinTree_InitOpr(&rbtn_opr);
    Zeta_OrdRBTreeNode_DeployBinTreeNodeOperator(NULL, &rbtn_opr);

    --lrucm->c_node_cnt;

    Zeta_OrdLinkedListNode_Extract(cl_node);

    Zeta_LRUCacheManager_CNode* c_node =
        ZETA_MemberToStruct(Zeta_LRUCacheManager_CNode, cl_node, cl_node);

    lrucm->ct_root = Zeta_RBTree_Extract(&rbtn_opr, &c_node->ct_node);

    WriteBack_(lrucm, c_node);

    lrucm->c_node_allocator->Deallocate(lrucm->c_node_allocator->context,
                                        c_node);

    return TRUE;
}

void Zeta_LRUCacheManager_DeployCacheManager(void* lrucm_,
                                             Zeta_CacheManager* dst) {
    Zeta_LRUCacheManager* lrucm = lrucm_;
    ZETA_DebugAssert(lrucm != NULL);

    ZETA_DebugAssert(dst != NULL);

    dst->context = lrucm;

    dst->GetBlockSize = Zeta_LRUCacheManager_GetBlockSize;

    dst->Open = Zeta_LRUCacheManager_Open;

    dst->Close = Zeta_LRUCacheManager_Close;

    dst->SetMaxCacheCnt = Zeta_LRUCacheManager_SetMaxCacheCnt;

    dst->ReadBlock = Zeta_LRUCacheManager_ReadBlock;

    dst->WriteBlock = Zeta_LRUCacheManager_WriteBlock;

    dst->FlushBlock = Zeta_LRUCacheManager_FlushBlock;

    dst->Flush = Zeta_LRUCacheManager_Flush;

    dst->FlushAll = Zeta_LRUCacheManager_FlushAll;
}
