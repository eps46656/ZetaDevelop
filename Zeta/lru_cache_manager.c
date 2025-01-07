#include "lru_cache_manager.h"

#include "debugger.h"
#include "rbtree.h"
#include "utils.h"

#if ZETA_EnableDebug

#define CheckLRUCM_(lrucm)

#define CheckSNode_(lrucm, s_node)

#else

#define CheckLRUCM_(lrucm) \
    ZETA_Unused((lrucm));  \
    ZETA_StaticAssert(TRUE)

#define CheckSNode_(lrucm, s_node) \
    ZETA_Unused((lrucm));          \
    ZETA_Unused((s_node));         \
    ZETA_StaticAssert(TRUE)

#endif

#define BNode Zeta_LRUCacheManager_BNode
#define SNode Zeta_LRUCacheManager_SNode
#define CNode Zeta_LRUCacheManager_CNode
#define XNode Zeta_LRUCacheManager_XNode

#define sn_color (0)
#define cn_color (1)
#define xn_color (2)

#define PairHash_(sn, blk_num, salt) \
    Zeta_ULLHash(Zeta_ULLHash(ZETA_PtrToAddr((sn)), (blk_num)), (salt));

#define PairCompare__(tmp_x_sn, tmp_y_sn, x_sn, x_blk_num, y_sn, y_blk_num)    \
    ({                                                                         \
        ZETA_AutoVar(tmp_x_sn, x_sn);                                          \
        ZETA_AutoVar(tmp_y_sn, y_sn);                                          \
        tmp_x_sn == tmp_y_sn ? ZETA_ThreeWayCompare((x_blk_num), (y_blk_num))  \
                             : ZETA_ThreeWayCompare(ZETA_PtrToAddr(tmp_x_sn),  \
                                                    ZETA_PtrToAddr(tmp_y_sn)); \
    })

#define PairCompare_(x_sn, x_blk_num, y_sn, y_blk_num)                     \
    PairCompare__(ZETA_TmpName, ZETA_TmpName, (x_sn), (x_blk_num), (y_sn), \
                  (y_blk_num))

ZETA_DeclareStruct(Pair);

struct Pair {
    void* sn;
    unsigned long long blk_num;
};

static unsigned long long PairHash(void* context, void* x_,
                                   unsigned long long salt) {
    Pair* x = x_;
    ZETA_DebugAssert(x != NULL);

    return PairHash_(x->sn, x->blk_num, salt);
}

static int PairCompare(void* x_sn, unsigned long long x_blk_num, void* y_sn,
                       unsigned long long y_blk_num) {
    return PairCompare_(x_sn, x_blk_num, y_sn, y_blk_num);
}

static Pair GetPairFromBNode_(BNode* bn) {
    int bnc = Zeta_OrdRBLinkedListNode_GetColor(&bn->ln);
    ZETA_DebugAssert(bnc == sn_color || bnc == cn_color || bnc == xn_color);

    Pair ret;

    switch (bnc) {
        case sn_color:
            ret.sn = ZETA_MemberToStruct(SNode, bn, bn);
            ret.blk_num = ZETA_ULLONG_MAX;
            break;

        case cn_color:
            ret.sn = NULL;
            ret.blk_num = ZETA_MemberToStruct(CNode, bn, bn)->blk_num;
            break;

        case xn_color:
            ret.sn = ZETA_MemberToStruct(XNode, bn, bn)->sn;
            ret.blk_num = ZETA_MemberToStruct(XNode, bn, bn)->cn->blk_num;
            break;
    }

    return ret;
}

static unsigned long long BNodeHash(void const* context, void* ghtn,
                                    unsigned long long salt) {
    ZETA_Unused(context);

    ZETA_DebugAssert(ghtn != NULL);

    Pair p = GetPairFromBNode_(ZETA_MemberToStruct(BNode, ghtn, ghtn));

    return PairHash_(p.sn, p.blk_num, salt);
}

static int BNodeCompare(void const* context, void* x_ghtn, void* y_ghtn) {
    ZETA_Unused(context);

    ZETA_DebugAssert(x_ghtn != NULL);
    ZETA_DebugAssert(y_ghtn != NULL);

    Pair x_p = GetPairFromBNode_(ZETA_MemberToStruct(BNode, ghtn, x_ghtn));
    Pair y_p = GetPairFromBNode_(ZETA_MemberToStruct(BNode, ghtn, y_ghtn));

    return PairCompare_(x_p.sn, x_p.blk_num, y_p.sn, y_p.blk_num);
}

static int PairBNodeCompare(void const* context, void* x_p_, void* y_ghtn) {
    ZETA_Unused(context);

    Pair* x_p = x_p_;
    ZETA_DebugAssert(x_p != NULL);

    Pair y_p = GetPairFromBNode_(ZETA_MemberToStruct(BNode, ghtn, y_ghtn));

    return PairCompare_(x_p->sn, x_p->blk_num, y_p.sn, y_p.blk_num);
}

void Zeta_LRUCacheManager_Init(void* lrucm_) {
    Zeta_LRUCacheManager* lrucm = lrucm_;
    ZETA_DebugAssert(lrucm != NULL);

    lrucm->cn_cnt = 0;
    lrucm->max_cn_cnt = 0;

    ZETA_DebugAssert(lrucm->origin != NULL);

    Zeta_Allocator* sn_allocator = lrucm->sn_allocator;

    ZETA_DebugAssert(sn_allocator != NULL);
    ZETA_DebugAssert(sn_allocator->GetAlign != NULL);
    ZETA_DebugAssert(sn_allocator->Allocate != NULL);
    ZETA_DebugAssert(sn_allocator->Deallocate != NULL);
    ZETA_DebugAssert(ZETA_Allocator_GetAlign(sn_allocator) % alignof(SNode) ==
                     0);

    Zeta_Allocator* cn_allocator = lrucm->cn_allocator;

    ZETA_DebugAssert(cn_allocator != NULL);
    ZETA_DebugAssert(cn_allocator->GetAlign != NULL);
    ZETA_DebugAssert(cn_allocator->Allocate != NULL);
    ZETA_DebugAssert(cn_allocator->Deallocate != NULL);
    ZETA_DebugAssert(ZETA_Allocator_GetAlign(cn_allocator) % alignof(CNode) ==
                     0);

    Zeta_Allocator* xn_allocator = lrucm->xn_allocator;

    ZETA_DebugAssert(xn_allocator != NULL);
    ZETA_DebugAssert(xn_allocator->GetAlign != NULL);
    ZETA_DebugAssert(xn_allocator->Allocate != NULL);
    ZETA_DebugAssert(xn_allocator->Deallocate != NULL);
    ZETA_DebugAssert(ZETA_Allocator_GetAlign(xn_allocator) % alignof(XNode) ==
                     0);

    lrucm->ght.node_hash_context = NULL;
    lrucm->ght.NodeHash = BNodeHash;

    lrucm->ght.node_cmp_context = NULL;
    lrucm->ght.NodeCompare = BNodeCompare;

    Zeta_GenericHashTable_Init(&lrucm->ght);
}

static void InitBNode_(BNode* bn) {
    Zeta_GenericHashTable_Node_Init(&bn->ghtn);
    Zeta_OrdRBLinkedListNode_Init(&bn->ln);
}

static SNode* AllocateSNode_(Zeta_LRUCacheManager* lrucm) {
    SNode* sn = ZETA_Allocator_SafeAllocate(lrucm->sn_allocator, alignof(SNode),
                                            sizeof(SNode));

    InitBNode_(&sn->bn);

    Zeta_OrdRBLinkedListNode_SetColor(&sn->bn.ln, sn_color);

    return sn;
}

static CNode* AllocateCNode_(Zeta_LRUCacheManager* lrucm) {
    CNode* cn = ZETA_Allocator_SafeAllocate(lrucm->cn_allocator, alignof(CNode),
                                            sizeof(CNode));

    InitBNode_(&cn->bn);

    Zeta_OrdRBLinkedListNode_SetColor(&cn->bn.ln, cn_color);

    cn->frame = ZETA_Allocator_SafeAllocate(
        lrucm->frame_allocator, 1, ZETA_SeqCntr_GetWidth(lrucm->origin));

    return cn;
}

static XNode* AllocateXNode_(Zeta_LRUCacheManager* lrucm) {
    XNode* xn = ZETA_Allocator_SafeAllocate(lrucm->xn_allocator, alignof(XNode),
                                            sizeof(XNode));

    InitBNode_(&xn->bn);

    Zeta_OrdRBLinkedListNode_SetColor(&xn->bn.ln, xn_color);

    return xn;
}

static void WriteBack_(Zeta_LRUCacheManager* lrucm, CNode* cn) {
    if (!cn->dirty) { return; }

    void* origin_cursor = ZETA_SeqCntr_AllocaCursor(lrucm->origin);

    ZETA_SeqCntr_Access(lrucm->origin, cn->blk_num, &origin_cursor, NULL);

    ZETA_SeqCntr_Write(lrucm->origin, origin_cursor, 1, cn->frame, NULL,
                       ZETA_SeqCntr_GetWidth(lrucm->origin), NULL);

    cn->dirty = FALSE;
}

static unsigned TryRelease_(Zeta_LRUCacheManager* lrucm, SNode* sn,
                            size_t quata) {
    unsigned cnt = 0;

    for (; cnt < quata && sn->max_cn_cnt <= sn->cn_cnt; ++cnt) {
        XNode* xn = ZETA_MemberToStruct(
            XNode, bn.ln, Zeta_OrdRBLinkedListNode_GetL(&sn->bn.ln));

        CNode* cn = xn->cn;

        Zeta_OrdRBLinkedListNode_Extract(&xn->bn.ln);

        ZETA_Allocator_Deallocate(lrucm->xn_allocator, xn);

        --sn->cn_cnt;

        if (0 < --cn->ref_cnt) { continue; }

        if (lrucm->cold_cl == NULL) {
            lrucm->cold_cl = &cn->bn.ln;
        } else {
            Zeta_OrdRBLinkedListNode_InsertL(lrucm->cold_cl, &cn->bn.ln);
        }
    }

    return cnt;
}

static XNode* F_(Zeta_LRUCacheManager* lrucm, SNode* sn, size_t blk_num,
                 bool_t fetch) {
    Zeta_GenericHashTable_Node* ghtn;

    Pair pair = {
        .sn = sn,
        .blk_num = blk_num,
    };

    ghtn = Zeta_GenericHashTable_Find(&lrucm->ght, &pair, NULL, PairHash, NULL,
                                      PairBNodeCompare);

    if (ghtn != NULL) {
        XNode* xn = ZETA_MemberToStruct(XNode, bn.ghtn, ghtn);

        Zeta_OrdRBLinkedListNode_Extract(&xn->bn.ln);
        Zeta_OrdRBLinkedListNode_InsertR(&sn->bn.ln, &xn->bn.ln);

        return xn;
    }

    TryRelease_(lrucm, sn, 4);

    pair.sn = NULL;
    pair.blk_num = blk_num;

    ghtn = Zeta_GenericHashTable_Find(&lrucm->ght, &pair, NULL, PairHash, NULL,
                                      PairBNodeCompare);

    CNode* cn;

    if (ghtn == NULL) {
        cn = AllocateCNode_(lrucm);

        if (fetch) {
            ZETA_SeqCntr_Access(lrucm->origin, blk_num, NULL, cn->frame);
        }

        cn->dirty = !fetch;

        cn->ref_cnt = 1;
    } else {
        cn = ZETA_MemberToStruct(CNode, bn.ghtn, ghtn);

        if (1 < ++cn->ref_cnt) { goto L1; }

        if (lrucm->cold_cl == &cn->bn.ln) {
            lrucm->cold_cl = Zeta_OrdRBLinkedListNode_GetR(lrucm->cold_cl);

            if (lrucm->cold_cl == &cn->bn.ln) { lrucm->cold_cl = NULL; }
        }

        Zeta_OrdRBLinkedListNode_Extract(&cn->bn.ln);
    }

L1:;

    XNode* xn = AllocateXNode_(lrucm);

    xn->sn = sn;
    ++sn->cn_cnt;

    Zeta_OrdRBLinkedListNode_InsertR(&sn->bn.ln, &xn->bn.ln);

    xn->cn = cn;

    return xn;
}

static void SetMaxCacheCnt_(Zeta_LRUCacheManager* lrucm, SNode* sn,
                            size_t max_cache_cnt) {
    sn->max_cn_cnt = max_cache_cnt;
    TryRelease_(lrucm, sn, 4);
}

size_t Zeta_LRUCacheManager_GetBlockSize(void* lrucm_) {
    Zeta_LRUCacheManager* lrucm = lrucm_;

    return ZETA_SeqCntr_GetWidth(lrucm->origin);
}

void* Zeta_LRUCacheManager_Open(void* lrucm_, size_t max_cache_cnt) {
    Zeta_LRUCacheManager* lrucm = lrucm_;
    CheckLRUCM_(lrucm);
}

void Zeta_LRUCacheManager_Close(void* lrucm_, void* sn_) {
    Zeta_LRUCacheManager* lrucm = lrucm_;
    Zeta_LRUCacheManager* sn = sn_;

    CheckSNode_(lrucm, sn);
}

void Zeta_LRUCacheManager_SetMaxCacheCnt(void* lrucm_, void* sd,
                                         size_t max_cache_cnt) {}

void const* Zeta_LRUCacheManager_Read(void* lrucm_, void* sn_, size_t blk_num) {
    Zeta_LRUCacheManager* lrucm = lrucm_;
    Zeta_LRUCacheManager* sn = sn_;

    CheckSNode_(lrucm, sn);

    XNode* xn = F_(lrucm, sn, blk_num, TRUE);

    return xn->cn->frame;
}

void Zeta_LRUCacheManager_Write(void* lrucm_, void* sn_, size_t blk_num,
                                void const* data) {
    Zeta_LRUCacheManager* lrucm = lrucm_;
    Zeta_LRUCacheManager* sn = sn_;

    CheckSNode_(lrucm, sn);

    ZETA_DebugAssert(data != NULL);

    XNode* xn = F_(lrucm, sn, blk_num, FALSE);

    CNode* cn = xn->cn;

    Zeta_MemCopy(cn->frame, data, ZETA_SeqCntr_GetWidth(lrucm->origin));

    cn->dirty = TRUE;
}

void Zeta_LRUCacheManager_FlushBlock(void* lrucm_, size_t blk_num) {
    Zeta_LRUCacheManager* lrucm = lrucm_;
    ZETA_DebugAssert(lrucm != NULL);

    Pair pair = {
        .sn = NULL,
        .blk_num = blk_num,
    };

    Zeta_GenericHashTable_Node* ghtn = Zeta_GenericHashTable_Find(
        &lrucm->ght, &pair, NULL, PairHash, NULL, PairBNodeCompare);

    if (ghtn == NULL) { return; }

    CNode* cn = ZETA_MemberToStruct(CNode, bn.ghtn, ghtn);

    WriteBack_(lrucm, cn);
}

void Zeta_LRUCacheManager_Flush(void* lrucm_, void* sn_) {
    Zeta_LRUCacheManager* lrucm = lrucm_;
    Zeta_LRUCacheManager_SNode* sn = sn_;

    CheckSNode_(lrucm, sn);

    Zeta_OrdRBLinkedListNode* ln = &sn->bn.ln;

    for (;;) {
        ln = Zeta_OrdRBLinkedListNode_GetR(ln);
        if (ln == &sn->bn.ln) { break; }

        WriteBack_(
            lrucm,
            ZETA_MemberToStruct(Zeta_LRUCacheManager_XNode, bn.ln, ln)->cn);
    }
}

static void FlushAll_(Zeta_LRUCacheManager* lrucm,
                      Zeta_OrdRBTreeNode* ct_node) {}

void Zeta_LRUCacheManager_FlushAll(void* lrucm_) {
    Zeta_LRUCacheManager* lrucm = lrucm_;
    CheckLRUCM_(lrucm);
}

void Zeta_LRUCacheManager_DeployCacheManager(void* lrucm_,
                                             Zeta_CacheManager* dst) {}
