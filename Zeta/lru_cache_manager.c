#include "lru_cache_manager.h"

#include "debugger.h"
#include "rbtree.h"
#include "utils.h"

#define CXNodeBase Zeta_LRUCacheManager_CXNodeBase
#define SNode Zeta_LRUCacheManager_SNode
#define CNode Zeta_LRUCacheManager_CNode
#define XNode Zeta_LRUCacheManager_XNode

#define cn_color (Zeta_LRUCacheManager_CNode_color)
#define xn_color (Zeta_LRUCacheManager_XNode_color)

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

static unsigned long long CXNodeHash(void const* context, void* x_,
                                     unsigned long long salt) {
    ZETA_Unused(context);

    ZETA_DebugAssert(x_ != NULL);

    CXNodeBase* x = ZETA_MemberToStruct(CXNodeBase, ghtn, x_);

    int xc = Zeta_OrdRBLinkedListNode_GetColor(&x->ln);
    ZETA_DebugAssert(xc == cn_color || xc == xn_color);

    return PairHash_(
        xc == cn_color ? NULL : ZETA_MemberToStruct(XNode, base, x)->sn,
        x->blk_num, salt);
}

static int CXNodeCompare(void const* context, void* x_, void* y_) {
    ZETA_Unused(context);

    ZETA_DebugAssert(x_ != NULL);
    ZETA_DebugAssert(y_ != NULL);

    CXNodeBase* x = ZETA_MemberToStruct(CXNodeBase, ghtn, x_);
    CXNodeBase* y = ZETA_MemberToStruct(CXNodeBase, ghtn, y_);

    int xc = Zeta_OrdRBLinkedListNode_GetColor(&x->ln);
    int yc = Zeta_OrdRBLinkedListNode_GetColor(&y->ln);

    ZETA_DebugAssert(xc == cn_color || xc == xn_color);
    ZETA_DebugAssert(yc == cn_color || yc == xn_color);

    return PairCompare_(
        xc == cn_color ? NULL : ZETA_MemberToStruct(XNode, base, x)->sn,
        x->blk_num,
        yc == cn_color ? NULL : ZETA_MemberToStruct(XNode, base, y)->sn,
        y->blk_num);
}

static int PairCXNodeCompare(void const* context, void* x_, void* y_) {
    ZETA_Unused(context);

    Pair* x = x_;
    ZETA_DebugAssert(x_ != NULL);

    CXNodeBase* y = ZETA_MemberToStruct(CXNodeBase, ghtn, y_);
    ZETA_DebugAssert(y != NULL);

    int yc = Zeta_OrdRBLinkedListNode_GetColor(&y->ln);
    ZETA_DebugAssert(yc == cn_color || yc == xn_color);

    return PairCompare_(
        x->sn, x->blk_num,
        yc == cn_color ? NULL : ZETA_MemberToStruct(XNode, base, y)->sn,
        y->blk_num);
}

void Zeta_LRUCacheManager_Init(void* lrucm_) {}

static void InitCXNodeBase_(CXNodeBase* cxnb) {
    Zeta_GenericHashTable_Node_Init(&cxnb->ghtn);
    Zeta_OrdRBLinkedListNode_Init(&cxnb->ln);
}

static CNode* AllocateCNode_(Zeta_LRUCacheManager* lrucm) {
    CNode* cn = ZETA_Allocator_SafeAllocate(lrucm->cn_allocator, alignof(CNode),
                                            sizeof(CNode));

    InitCXNodeBase_(&cn->base);

    Zeta_OrdRBLinkedListNode_SetColor(&cn->base.ln, cn_color);

    cn->frame = ZETA_Allocator_SafeAllocate(
        lrucm->frame_allocator, 1, ZETA_SeqCntr_GetWidth(lrucm->origin));

    return cn;
}

static XNode* AllocateXNode_(Zeta_LRUCacheManager* lrucm) {
    XNode* xn = ZETA_Allocator_SafeAllocate(lrucm->xn_allocator, alignof(XNode),
                                            sizeof(XNode));

    InitCXNodeBase_(&xn->base);

    Zeta_OrdRBLinkedListNode_SetColor(&xn->base.ln, xn_color);

    return xn;
}

static void WriteBack_(Zeta_LRUCacheManager* lrucm, CNode* cn) {
    if (!cn->dirty) { return; }

    void* origin_cursor = ZETA_SeqCntr_AllocaCursor(lrucm->origin);

    ZETA_SeqCntr_Access(lrucm->origin, cn->base.blk_num, &origin_cursor, NULL);

    ZETA_SeqCntr_Write(lrucm->origin, origin_cursor, 1, cn->frame, NULL,
                       ZETA_SeqCntr_GetWidth(lrucm->origin), NULL);

    cn->dirty = FALSE;
}

static XNode* F_(Zeta_LRUCacheManager* lrucm, SNode* sn, size_t blk_num,
                 bool_t fetch) {
    Zeta_GenericHashTable_Node* ghtn;

    Pair pair = {
        .sn = sn,
        .blk_num = blk_num,
    };

    ghtn = Zeta_GenericHashTable_Find(&lrucm->ght, &pair, NULL, PairHash, NULL,
                                      PairCXNodeCompare);

    if (ghtn != NULL) {
        XNode* xn = ZETA_MemberToStruct(XNode, base.ghtn, ghtn);

        Zeta_OrdRBLinkedListNode_Extract(&xn->base.ln);
        Zeta_OrdRBLinkedListNode_InsertR(&sn->lh, &xn->base.ln);

        return xn;
    }

    for (int quata = 4; 0 < quata && sn->max_cn_cnt <= sn->cn_cnt; --quata) {
        XNode* xn = ZETA_MemberToStruct(XNode, base.ln,
                                        Zeta_OrdRBLinkedListNode_GetL(&sn->lh));

        CNode* cn = xn->cn;

        Zeta_OrdRBLinkedListNode_Extract(&xn->base.ln);
        --sn->cn_cnt;

        if (0 < --cn->ref_cnt) { continue; }

        if (lrucm->cold_cl == NULL) {
            lrucm->cold_cl = &cn->base.ln;
        } else {
            Zeta_OrdRBLinkedListNode_InsertL(lrucm->cold_cl, &cn->base.ln);
        }
    }

    pair.sn = NULL;
    pair.blk_num = blk_num;

    ghtn = Zeta_GenericHashTable_Find(&lrucm->ght, &pair, NULL, PairHash, NULL,
                                      PairCXNodeCompare);

    CNode* cn;

    if (ghtn == NULL) {
        cn = AllocateCNode_(lrucm);

        if (fetch) {
            ZETA_SeqCntr_Access(lrucm->origin, blk_num, NULL, cn->frame);
        }

        cn->dirty = !fetch;

        cn->ref_cnt = 1;
    } else {
        cn = ZETA_MemberToStruct(CNode, base.ghtn, ghtn);
    }

    XNode* xn = AllocateXNode_(lrucm);

    xn->sn = sn;
    ++sn->cn_cnt;

    Zeta_OrdRBLinkedListNode_InsertR(&sn->lh, &xn->base.ln);

    xn->cn = cn;

    if (ghtn != NULL && ++cn->ref_cnt == 1) {
        if (lrucm->cold_cl == &cn->base.ln) {
            lrucm->cold_cl = Zeta_OrdRBLinkedListNode_GetR(lrucm->cold_cl);

            if (lrucm->cold_cl == &cn->base.ln) { lrucm->cold_cl = NULL; }
        }

        Zeta_OrdRBLinkedListNode_Extract(&cn->base.ln);
    }

    return xn;
}

static void SetMaxCacheCnt_(Zeta_LRUCacheManager* lrucm, SNode* sn,
                            size_t max_cache_cnt) {}

size_t Zeta_LRUCacheManager_GetBlockSize(void* lrucm_) {
    Zeta_LRUCacheManager* lrucm = lrucm_;

    return ZETA_SeqCntr_GetWidth(lrucm->origin);
}

void* Zeta_LRUCacheManager_Open(void* lrucm_, size_t max_cache_cnt) {
    //
}

void Zeta_LRUCacheManager_Close(void* lrucm_, void* sd) {}

void Zeta_LRUCacheManager_SetMaxCacheCnt(void* lrucm_, void* sd,
                                         size_t max_cache_cnt) {}

void const* Zeta_LRUCacheManager_Read(void* lrucm_, void* sd, size_t blk_num) {
    Zeta_LRUCacheManager* lrucm = lrucm_;

    SNode* sn = sd;

    XNode* xn = F_(lrucm, sn, blk_num, TRUE);

    return xn->cn->frame;
}

void Zeta_LRUCacheManager_Write(void* lrucm_, void* sd, size_t blk_num,
                                void const* data) {
    Zeta_LRUCacheManager* lrucm = lrucm_;

    SNode* sn = sd;

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
        &lrucm->ght, &pair, NULL, PairHash, NULL, PairCXNodeCompare);

    if (ghtn == NULL) { return; }

    CNode* cn = ZETA_MemberToStruct(CNode, base.ghtn, ghtn);

    WriteBack_(lrucm, cn);
}

void Zeta_LRUCacheManager_Flush(void* lrucm_, void* sd) {
    //
}

static void FlushAll_(Zeta_LRUCacheManager* lrucm,
                      Zeta_OrdRBTreeNode* ct_node) {}

void Zeta_LRUCacheManager_FlushAll(void* lrucm_) {}

void Zeta_LRUCacheManager_DeployCacheManager(void* lrucm_,
                                             Zeta_CacheManager* dst) {}
