#include "lru_cache_manager.h"

#include "debugger.h"
#include "memory.h"
#include "rbtree.h"
#include "utils.h"

#if ZETA_EnableDebug

#define CheckLRUCM_(lrucm) Zeta_LRUCacheManager_Check((lrucm))

#define CheckSNode_(lrucm, sn) \
    Zeta_LRUCacheManager_CheckSessionDescriptor((lrucm), (sn))

#else

#define CheckLRUCM_(lrucm) \
    ZETA_Unused((lrucm));  \
    ZETA_StaticAssert(TRUE)

#define CheckSNode_(lrucm, sn) \
    ZETA_Unused((lrucm));      \
    ZETA_Unused((sn));         \
    ZETA_StaticAssert(TRUE)

#endif

#define BNode Zeta_LRUCacheManager_BNode
#define SNode Zeta_LRUCacheManager_SNode
#define CNode Zeta_LRUCacheManager_CNode
#define XNode Zeta_LRUCacheManager_XNode

#define sn_color Zeta_LRUCacheManager_SNode_color
#define cn_color Zeta_LRUCacheManager_CNode_color
#define xn_color Zeta_LRUCacheManager_XNode_color

#define PairHash_(sn, cache_idx, salt) \
    Zeta_ULLHash(Zeta_ULLHash(ZETA_PtrToAddr((sn)), (cache_idx)), (salt));

#define PairCompare__(tmp_x_sn, tmp_y_sn, x_sn, x_cache_idx, y_sn, \
                      y_cache_idx)                                 \
    ({                                                             \
        ZETA_AutoVar(tmp_x_sn, x_sn);                              \
        ZETA_AutoVar(tmp_y_sn, y_sn);                              \
        tmp_x_sn == tmp_y_sn                                       \
            ? ZETA_ThreeWayCompare((x_cache_idx), (y_cache_idx))   \
            : ZETA_ThreeWayCompare(ZETA_PtrToAddr(tmp_x_sn),       \
                                   ZETA_PtrToAddr(tmp_y_sn));      \
    })

#define PairCompare_(x_sn, x_cache_idx, y_sn, y_cache_idx)                   \
    PairCompare__(ZETA_TmpName, ZETA_TmpName, (x_sn), (x_cache_idx), (y_sn), \
                  (y_cache_idx))

ZETA_DeclareStruct(Pair);

struct Pair {
    void* sn;
    size_t cache_idx;
};

static unsigned long long PairHash(void const* context, void const* x_,
                                   unsigned long long salt) {
    ZETA_Unused(context);

    Pair const* x = x_;
    ZETA_DebugAssert(x != NULL);

    return PairHash_(x->sn, x->cache_idx, salt);
}

static Pair GetPairFromBNode_(BNode* bn) {
    int bnc = Zeta_OrdRBLinkedListNode_GetColor(&bn->ln);
    ZETA_DebugAssert(bnc == sn_color || bnc == cn_color || bnc == xn_color);

    Pair ret;

    switch (bnc) {
        case sn_color:
            ret.sn = ZETA_MemberToStruct(SNode, bn, bn);
            ret.cache_idx = ZETA_SIZE_MAX;
            break;

        case cn_color:
            ret.sn = NULL;
            ret.cache_idx = ZETA_MemberToStruct(CNode, bn, bn)->cache_idx;
            break;

        case xn_color:
            ret.sn = ZETA_MemberToStruct(XNode, bn, bn)->sn;
            ret.cache_idx = ZETA_MemberToStruct(XNode, bn, bn)->cn->cache_idx;
            break;
    }

    return ret;
}

static unsigned long long BNodeHash(void const* context, void const* ghtn,
                                    unsigned long long salt) {
    ZETA_Unused(context);

    ZETA_DebugAssert(ghtn != NULL);

    Pair p = GetPairFromBNode_(ZETA_MemberToStruct(BNode, ghtn, ghtn));

    return PairHash_(p.sn, p.cache_idx, salt);
}

static int BNodeCompare(void const* context, void const* x_ghtn,
                        void const* y_ghtn) {
    ZETA_Unused(context);

    ZETA_DebugAssert(x_ghtn != NULL);
    ZETA_DebugAssert(y_ghtn != NULL);

    Pair x_p = GetPairFromBNode_(ZETA_MemberToStruct(BNode, ghtn, x_ghtn));
    Pair y_p = GetPairFromBNode_(ZETA_MemberToStruct(BNode, ghtn, y_ghtn));

    return PairCompare_(x_p.sn, x_p.cache_idx, y_p.sn, y_p.cache_idx);
}

static int PairBNodeCompare(void const* context, void const* x_p_,
                            void const* y_ghtn) {
    ZETA_Unused(context);

    Pair const* x_p = x_p_;
    ZETA_DebugAssert(x_p != NULL);

    Pair y_p = GetPairFromBNode_(ZETA_MemberToStruct(BNode, ghtn, y_ghtn));

    return PairCompare_(x_p->sn, x_p->cache_idx, y_p.sn, y_p.cache_idx);
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

    Zeta_OrdRBLinkedListNode_Init(&sn->sln);

    return sn;
}

static CNode* AllocateCNode_(Zeta_LRUCacheManager* lrucm) {
    CNode* cn = ZETA_Allocator_SafeAllocate(lrucm->cn_allocator, alignof(CNode),
                                            sizeof(CNode));

    InitBNode_(&cn->bn);

    Zeta_OrdRBLinkedListNode_SetColor(&cn->bn.ln, cn_color);

    cn->frame = ZETA_Allocator_SafeAllocate(
        lrucm->frame_allocator, 1,
        ZETA_SeqCntr_GetWidth(lrucm->origin) * lrucm->cache_size);

    return cn;
}

static XNode* AllocateXNode_(Zeta_LRUCacheManager* lrucm) {
    XNode* xn = ZETA_Allocator_SafeAllocate(lrucm->xn_allocator, alignof(XNode),
                                            sizeof(XNode));

    InitBNode_(&xn->bn);

    Zeta_OrdRBLinkedListNode_SetColor(&xn->bn.ln, xn_color);

    return xn;
}

static bool_t WriteBack_(Zeta_LRUCacheManager* lrucm, CNode* cn) {
    if (!cn->dirty) { return FALSE; }

    size_t width = ZETA_SeqCntr_GetWidth(lrucm->origin);

    size_t cache_size = lrucm->cache_size;

    void* origin_cursor = ZETA_SeqCntr_AllocaCursor(lrucm->origin);

    ZETA_SeqCntr_Access(lrucm->origin, cache_size * cn->cache_idx,
                        &origin_cursor, NULL);

    ZETA_SeqCntr_Write(lrucm->origin, origin_cursor, cache_size, cn->frame,
                       width, NULL);

    cn->dirty = FALSE;

    return TRUE;
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

    lrucm->fit_sl = ZETA_Allocator_SafeAllocate(sn_allocator,
                                                alignof(Zeta_OrdLinkedListNode),
                                                sizeof(Zeta_OrdLinkedListNode));

    lrucm->over_sl = ZETA_Allocator_SafeAllocate(
        sn_allocator, alignof(Zeta_OrdLinkedListNode),
        sizeof(Zeta_OrdLinkedListNode));

    lrucm->hot_clear_cl = ZETA_Allocator_SafeAllocate(
        cn_allocator, alignof(Zeta_OrdRBLinkedListNode),
        sizeof(Zeta_OrdRBLinkedListNode));

    lrucm->hot_dirty_cl = ZETA_Allocator_SafeAllocate(
        cn_allocator, alignof(Zeta_OrdRBLinkedListNode),
        sizeof(Zeta_OrdRBLinkedListNode));

    lrucm->cold_clear_cl = ZETA_Allocator_SafeAllocate(
        cn_allocator, alignof(Zeta_OrdRBLinkedListNode),
        sizeof(Zeta_OrdRBLinkedListNode));

    lrucm->cold_dirty_cl = ZETA_Allocator_SafeAllocate(
        cn_allocator, alignof(Zeta_OrdRBLinkedListNode),
        sizeof(Zeta_OrdRBLinkedListNode));

    Zeta_OrdLinkedListNode_Init(lrucm->fit_sl);
    Zeta_OrdLinkedListNode_Init(lrucm->over_sl);

    Zeta_OrdRBLinkedListNode_Init(lrucm->hot_clear_cl);
    Zeta_OrdRBLinkedListNode_Init(lrucm->hot_dirty_cl);
    Zeta_OrdRBLinkedListNode_Init(lrucm->cold_clear_cl);
    Zeta_OrdRBLinkedListNode_Init(lrucm->cold_dirty_cl);
}

Zeta_SeqCntr* Zeta_LRUCacheManager_GetOrigin(void const* lrucm_) {
    Zeta_LRUCacheManager const* lrucm = lrucm_;
    ZETA_DebugAssert(lrucm != NULL);

    return lrucm->origin;
}

size_t Zeta_LRUCacheManager_GetCacheSize(void const* lrucm_) {
    Zeta_LRUCacheManager const* lrucm = lrucm_;
    ZETA_DebugAssert(lrucm != NULL);

    return lrucm->cache_size;
}

void Zeta_LRUCacheManager_Deinit(void* lrucm_) {
    Zeta_LRUCacheManager* lrucm = lrucm_;
    ZETA_DebugAssert(lrucm != NULL);

    void* ght = &lrucm->ght;

    for (;;) {
        void* ghtn = Zeta_GenericHashTable_ExtractAny(ght);
        if (ghtn == NULL) { break; }

        Zeta_LRUCacheManager_BNode* bn =
            ZETA_MemberToStruct(Zeta_LRUCacheManager_BNode, ghtn, ghtn);

        int bnc = Zeta_OrdRBLinkedListNode_GetColor(&bn->ln);
        ZETA_DebugAssert(bnc == cn_color || bnc == xn_color);

        switch (bnc) {
            case sn_color:
                ZETA_Allocator_Deallocate(lrucm->sn_allocator,
                                          ZETA_MemberToStruct(SNode, bn, bn));
                break;

            case cn_color: {
                Zeta_LRUCacheManager_CNode* cn =
                    ZETA_MemberToStruct(CNode, bn, bn);

                WriteBack_(lrucm, cn);

                ZETA_Allocator_Deallocate(lrucm->frame_allocator, cn->frame);

                ZETA_Allocator_Deallocate(lrucm->cn_allocator, cn);

                break;
            }

            case xn_color:
                ZETA_Allocator_Deallocate(lrucm->xn_allocator,
                                          ZETA_MemberToStruct(XNode, bn, bn));
                break;
        }
    }

    ZETA_Allocator_Deallocate(lrucm->cn_allocator, lrucm->hot_clear_cl);
    ZETA_Allocator_Deallocate(lrucm->cn_allocator, lrucm->hot_dirty_cl);
    ZETA_Allocator_Deallocate(lrucm->cn_allocator, lrucm->cold_clear_cl);
    ZETA_Allocator_Deallocate(lrucm->cn_allocator, lrucm->cold_dirty_cl);
}

static unsigned TryRelease_(Zeta_LRUCacheManager* lrucm, SNode* sn,
                            size_t max_cn_cnt, size_t quata) {
    unsigned cnt = ZETA_GetMinOf(
        quata, ZETA_GetMaxOf(sn->cn_cnt, max_cn_cnt) - max_cn_cnt);

    for (size_t cnt_i = 0; cnt_i < cnt; ++cnt_i) {
        XNode* xn = ZETA_MemberToStruct(
            XNode, bn.ln, Zeta_OrdRBLinkedListNode_GetL(&sn->bn.ln));

        CNode* cn = xn->cn;

        Zeta_GenericHashTable_Extract(&lrucm->ght, &xn->bn.ghtn);

        Zeta_OrdRBLinkedListNode_Extract(&xn->bn.ln);

        ZETA_Allocator_Deallocate(lrucm->xn_allocator, xn);

        --sn->cn_cnt;

        if (0 < --cn->ref_cnt) { continue; }

        Zeta_OrdRBLinkedListNode_Extract(&cn->bn.ln);

        if (cn->dirty) {
            Zeta_OrdRBLinkedListNode_InsertL(lrucm->cold_dirty_cl, &cn->bn.ln);
        } else {
            Zeta_OrdRBLinkedListNode_InsertL(lrucm->cold_clear_cl, &cn->bn.ln);
        }
    }

    return cnt;
}

static bool_t RunPending_(Zeta_LRUCacheManager* lrucm, size_t quata) {
    size_t old_quata = quata;

    while (0 < quata) {
        Zeta_OrdLinkedListNode* sln =
            Zeta_OrdLinkedListNode_GetR(lrucm->over_sl);

        if (sln == lrucm->over_sl) { break; }

        SNode* sn = ZETA_MemberToStruct(Zeta_LRUCacheManager_SNode, sln, sln);

        quata -= TryRelease_(lrucm, sn, sn->max_cn_cnt, quata);

        if (sn->max_cn_cnt < sn->cn_cnt) { break; }

        Zeta_OrdLinkedListNode_Extract(&sn->sln);

        if (sn->max_cn_cnt == 0) {
            ZETA_Allocator_Deallocate(lrucm->sn_allocator, sn);
        } else {
            Zeta_OrdLinkedListNode_InsertL(lrucm->fit_sl, &sn->sln);
        }
    }

    for (; 0 < quata && lrucm->max_cn_cnt < lrucm->cn_cnt; --quata) {
        Zeta_OrdRBLinkedListNode* ln =
            Zeta_OrdRBLinkedListNode_GetR(lrucm->cold_clear_cl);

        if (ln == lrucm->cold_clear_cl) { break; }

        CNode* cn = ZETA_MemberToStruct(Zeta_LRUCacheManager_CNode, bn.ln, ln);

        Zeta_OrdRBLinkedListNode_Extract(ln);

        Zeta_GenericHashTable_Extract(&lrucm->ght, &cn->bn.ghtn);

        ZETA_Allocator_Deallocate(lrucm->frame_allocator, cn->frame);

        ZETA_Allocator_Deallocate(lrucm->cn_allocator, cn);

        --lrucm->cn_cnt;
    }

    return old_quata - quata;
}

#define FMode_Find 0
#define FMode_Read 1
#define FMode_Write 2

static XNode* F_(Zeta_LRUCacheManager* lrucm, SNode* sn, size_t cache_idx,
                 int fmode) {
    ZETA_SanitizeAssert(fmode == FMode_Find || fmode == FMode_Read ||
                        fmode == FMode_Write);

    TryRelease_(lrucm, sn, sn->max_cn_cnt - 1, 4);

    Zeta_GenericHashTable_Node* ghtn;

    Pair pair = {
        .sn = sn,
        .cache_idx = cache_idx,
    };

    ghtn = Zeta_GenericHashTable_Find(&lrucm->ght, &pair, NULL, PairHash, NULL,
                                      PairBNodeCompare);

    if (ghtn != NULL) {
        XNode* xn = ZETA_MemberToStruct(XNode, bn.ghtn, ghtn);

        ZETA_SanitizeAssert(xn->cn->cache_idx == cache_idx);

        Zeta_OrdRBLinkedListNode_Extract(&xn->bn.ln);
        Zeta_OrdRBLinkedListNode_InsertR(&sn->bn.ln, &xn->bn.ln);

        RunPending_(lrucm, 8);

        return xn;
    }

    if (fmode == FMode_Find) {
        RunPending_(lrucm, 8);
        return NULL;
    }

    RunPending_(lrucm, 4);

    pair.sn = NULL;
    pair.cache_idx = cache_idx;

    ghtn = Zeta_GenericHashTable_Find(&lrucm->ght, &pair, NULL, PairHash, NULL,
                                      PairBNodeCompare);

    CNode* cn;

    if (ghtn == NULL) {
        cn = AllocateCNode_(lrucm);

        cn->cache_idx = cache_idx;

        if (fmode == FMode_Read) {
            cn->dirty = FALSE;

            void* cursor = ZETA_SeqCntr_AllocaCursor(lrucm->origin);

            size_t k = lrucm->cache_size * cache_idx;

            ZETA_SeqCntr_Access(lrucm->origin, k, cursor, NULL);

            ZETA_SeqCntr_Read(
                lrucm->origin, cursor,
                ZETA_GetMinOf(ZETA_SeqCntr_GetSize(lrucm->origin) - k,
                              lrucm->cache_size),
                cn->frame, ZETA_SeqCntr_GetWidth(lrucm->origin), NULL);
        } else {
            cn->dirty = TRUE;
        }

        cn->ref_cnt = 1;

        Zeta_GenericHashTable_Insert(&lrucm->ght, &cn->bn.ghtn);

        ++lrucm->cn_cnt;
    } else {
        cn = ZETA_MemberToStruct(CNode, bn.ghtn, ghtn);

        if (++cn->ref_cnt == 1) {
            Zeta_OrdRBLinkedListNode_Extract(&cn->bn.ln);
        }
    }

    if (cn->ref_cnt == 1) {
        if (cn->dirty) {
            Zeta_OrdRBLinkedListNode_InsertL(lrucm->hot_dirty_cl, &cn->bn.ln);
        } else {
            Zeta_OrdRBLinkedListNode_InsertL(lrucm->hot_clear_cl, &cn->bn.ln);
        }
    }

    XNode* xn = AllocateXNode_(lrucm);

    xn->sn = sn;
    xn->cn = cn;

    ++sn->cn_cnt;

    Zeta_GenericHashTable_Insert(&lrucm->ght, &xn->bn.ghtn);

    Zeta_OrdRBLinkedListNode_InsertR(&sn->bn.ln, &xn->bn.ln);

    return xn;
}

static void SetMaxCacheCnt_(Zeta_LRUCacheManager* lrucm, SNode* sn,
                            size_t max_cn_cnt) {
    size_t old_max_cn_cnt = sn->max_cn_cnt;

    if (old_max_cn_cnt == max_cn_cnt) { return; }

    lrucm->max_cn_cnt += max_cn_cnt - old_max_cn_cnt;

    sn->max_cn_cnt = max_cn_cnt;

    TryRelease_(lrucm, sn, sn->max_cn_cnt, 4);

    bool_t old_is_fit = sn->cn_cnt <= old_max_cn_cnt;
    bool_t new_is_fit = sn->cn_cnt <= max_cn_cnt;

    if (old_is_fit == new_is_fit) { return; }

    Zeta_OrdLinkedListNode_Extract(&sn->sln);

    if (new_is_fit) {
        Zeta_OrdLinkedListNode_InsertL(lrucm->fit_sl, &sn->sln);
    } else {
        Zeta_OrdLinkedListNode_InsertL(lrucm->over_sl, &sn->sln);
    }
}

void* Zeta_LRUCacheManager_Open(void* lrucm_, size_t max_cache_cnt) {
    Zeta_LRUCacheManager* lrucm = lrucm_;
    CheckLRUCM_(lrucm);

    ZETA_DebugAssert(0 < max_cache_cnt);
    ZETA_DebugAssert(max_cache_cnt <= ZETA_LRUCachaManager_max_cn_cnts);

    SNode* sn = AllocateSNode_(lrucm);

    sn->cn_cnt = 0;
    sn->max_cn_cnt = max_cache_cnt;

    Zeta_GenericHashTable_Insert(&lrucm->ght, &sn->bn.ghtn);

    Zeta_OrdLinkedListNode_InsertR(lrucm->fit_sl, &sn->sln);

    lrucm->max_cn_cnt += max_cache_cnt;

    return sn;
}

void Zeta_LRUCacheManager_Close(void* lrucm_, void* sn_) {
    Zeta_LRUCacheManager* lrucm = lrucm_;
    Zeta_LRUCacheManager_SNode* sn = sn_;

    CheckSNode_(lrucm, sn);

    SetMaxCacheCnt_(lrucm, sn, 0);
}

void Zeta_LRUCacheManager_SetMaxCacheCnt(void* lrucm_, void* sn_,
                                         size_t max_cache_cnt) {
    Zeta_LRUCacheManager* lrucm = lrucm_;
    Zeta_LRUCacheManager_SNode* sn = sn_;

    CheckSNode_(lrucm, sn);

    ZETA_DebugAssert(0 < max_cache_cnt);
    ZETA_DebugAssert(max_cache_cnt <= ZETA_LRUCachaManager_max_cn_cnts);

    SetMaxCacheCnt_(lrucm, sn, max_cache_cnt);
}

void Zeta_LRUCacheManager_Read(void* lrucm_, void* sn_, size_t idx, size_t cnt,
                               void* dst, size_t dst_stride) {
    Zeta_LRUCacheManager* lrucm = lrucm_;
    Zeta_LRUCacheManager_SNode* sn = sn_;

    CheckSNode_(lrucm, sn);

    if (cnt == 0) { return; }

    ZETA_DebugAssert(dst != NULL);

    size_t width = ZETA_SeqCntr_GetWidth(lrucm->origin);

    size_t cache_size = lrucm->cache_size;

    size_t lb_cache_idx = idx / cache_size;
    size_t rb_cache_idx = (idx + cnt - 1) / cache_size + 1;

    bool_t* marks = ZETA_Allocator_SafeAllocate(
        zeta_cascade_allocator, alignof(bool_t),
        sizeof(bool_t) * (rb_cache_idx - lb_cache_idx));

    for (size_t cache_idx = lb_cache_idx; cache_idx < rb_cache_idx;
         ++cache_idx) {
        XNode* xn = F_(lrucm, sn, cache_idx, FMode_Find);

        if (xn == NULL) {
            marks[cache_idx - lb_cache_idx] = FALSE;
            continue;
        }

        marks[cache_idx - lb_cache_idx] = TRUE;

        CNode* cn = xn->cn;

        size_t k = cache_size * cache_idx;

        size_t lb_idx = ZETA_GetMaxOf(k, idx);
        size_t rb_idx = ZETA_GetMinOf(k + cache_size, idx + cnt);

        Zeta_ElemCopy(dst + dst_stride * (lb_idx - idx),  // dst
                      cn->frame + width * (lb_idx - k),   // src
                      width,                              // width
                      dst_stride,                         // dst_stride
                      width,                              // src_stride
                      rb_idx - lb_idx                     // cnt
        );
    }

    for (size_t cache_idx = lb_cache_idx; cache_idx < rb_cache_idx;
         ++cache_idx) {
        if (marks[cache_idx - lb_cache_idx]) { continue; }

        XNode* xn = F_(lrucm, sn, cache_idx, FMode_Read);

        CNode* cn = xn->cn;

        size_t k = cache_size * cache_idx;

        size_t lb_idx = ZETA_GetMaxOf(k, idx);
        size_t rb_idx = ZETA_GetMinOf(k + cache_size, idx + cnt);

        Zeta_ElemCopy(dst + dst_stride * (lb_idx - idx),  // dst
                      cn->frame + width * (lb_idx - k),   // src
                      width,                              // width
                      dst_stride,                         // dst_stride
                      width,                              // src_stride
                      rb_idx - lb_idx                     // cnt
        );
    }

    ZETA_Allocator_Deallocate(zeta_cascade_allocator, marks);
}

void Zeta_LRUCacheManager_Write(void* lrucm_, void* sn_, size_t idx, size_t cnt,
                                void const* src, size_t src_stride) {
    Zeta_LRUCacheManager* lrucm = lrucm_;
    Zeta_LRUCacheManager_SNode* sn = sn_;

    CheckSNode_(lrucm, sn);

    if (cnt == 0) { return; }

    ZETA_DebugAssert(src != NULL);

    size_t width = ZETA_SeqCntr_GetWidth(lrucm->origin);

    size_t cache_size = lrucm->cache_size;

    size_t lb_cache_idx = idx / cache_size;
    size_t rb_cache_idx = (idx + cnt - 1) / cache_size + 1;

    bool_t* marks = ZETA_Allocator_SafeAllocate(
        zeta_cascade_allocator, alignof(bool_t),
        sizeof(bool_t) * (rb_cache_idx - lb_cache_idx));

    for (size_t cache_idx = lb_cache_idx; cache_idx < rb_cache_idx;
         ++cache_idx) {
        XNode* xn = F_(lrucm, sn, cache_idx, FMode_Find);

        if (xn == NULL) {
            marks[cache_idx - lb_cache_idx] = FALSE;
            continue;
        }

        marks[cache_idx - lb_cache_idx] = TRUE;

        CNode* cn = xn->cn;

        size_t k = cache_size * cache_idx;

        size_t lb_idx = ZETA_GetMaxOf(k, idx);
        size_t rb_idx = ZETA_GetMinOf(k + cache_size, idx + cnt);

        if (!cn->dirty) {
            cn->dirty = TRUE;

            Zeta_OrdRBLinkedListNode_Extract(&cn->bn.ln);

            if (cn->ref_cnt == 0) {
                Zeta_OrdRBLinkedListNode_InsertL(lrucm->cold_dirty_cl,
                                                 &cn->bn.ln);
            } else {
                Zeta_OrdRBLinkedListNode_InsertL(lrucm->hot_dirty_cl,
                                                 &cn->bn.ln);
            }
        }

        Zeta_ElemCopy(cn->frame + width * (lb_idx - k),   // dst
                      src + src_stride * (lb_idx - idx),  // src
                      width,                              // width
                      width,                              // dst_stride
                      src_stride,                         // src_stride
                      rb_idx - lb_idx                     // cnt
        );
    }

    for (size_t cache_idx = lb_cache_idx; cache_idx < rb_cache_idx;
         ++cache_idx) {
        if (marks[cache_idx - lb_cache_idx]) { continue; }

        size_t k = cache_size * cache_idx;

        size_t lb_idx = ZETA_GetMaxOf(k, idx);
        size_t rb_idx = ZETA_GetMinOf(k + cache_size, idx + cnt);

        XNode* xn = F_(lrucm, sn, cache_idx,
                       rb_idx - lb_idx < cache_size ? FMode_Read : FMode_Write);

        CNode* cn = xn->cn;

        if (!cn->dirty) {
            cn->dirty = TRUE;

            Zeta_OrdRBLinkedListNode_Extract(&cn->bn.ln);

            if (cn->ref_cnt == 0) {
                Zeta_OrdRBLinkedListNode_InsertL(lrucm->cold_dirty_cl,
                                                 &cn->bn.ln);
            } else {
                Zeta_OrdRBLinkedListNode_InsertL(lrucm->hot_dirty_cl,
                                                 &cn->bn.ln);
            }
        }

        Zeta_ElemCopy(cn->frame + width * (lb_idx - k),   // dst
                      src + src_stride * (lb_idx - idx),  // src
                      width,                              // width
                      width,                              // dst_stride
                      src_stride,                         // src_stride
                      rb_idx - lb_idx                     // cnt
        );
    }

    ZETA_Allocator_Deallocate(zeta_cascade_allocator, marks);
}

void Zeta_LRUCacheManager_FlushBlock(void* lrucm_, size_t cache_idx) {
    Zeta_LRUCacheManager* lrucm = lrucm_;
    ZETA_DebugAssert(lrucm != NULL);

    Pair pair = {
        .sn = NULL,
        .cache_idx = cache_idx,
    };

    Zeta_GenericHashTable_Node* ghtn = Zeta_GenericHashTable_Find(
        &lrucm->ght, &pair, NULL, PairHash, NULL, PairBNodeCompare);

    if (ghtn == NULL) { return; }

    CNode* cn = ZETA_MemberToStruct(CNode, bn.ghtn, ghtn);

    if (!WriteBack_(lrucm, cn)) { return; }

    Zeta_OrdRBLinkedListNode_Extract(&cn->bn.ln);

    if (cn->ref_cnt == 0) {
        Zeta_OrdRBLinkedListNode_InsertL(lrucm->cold_clear_cl, &cn->bn.ln);
    } else {
        Zeta_OrdRBLinkedListNode_InsertL(lrucm->hot_clear_cl, &cn->bn.ln);
    }
}

size_t Zeta_LRUCacheManager_Flush(void* lrucm_, size_t quata) {
    Zeta_LRUCacheManager* lrucm = lrucm_;
    CheckLRUCM_(lrucm);

    if (quata == 0) { return FALSE; }

    RunPending_(lrucm, quata * 8);

    size_t write_quata = quata;
    size_t old_write_quata = write_quata;

    for (; 0 < write_quata; --write_quata) {
        Zeta_OrdRBLinkedListNode* ln =
            Zeta_OrdRBLinkedListNode_GetR(lrucm->cold_dirty_cl);

        if (ln == lrucm->cold_dirty_cl) { break; }

        CNode* cn = ZETA_MemberToStruct(Zeta_LRUCacheManager_CNode, bn.ln, ln);

        WriteBack_(lrucm, cn);

        Zeta_OrdRBLinkedListNode_Extract(ln);

        Zeta_OrdRBLinkedListNode_InsertL(lrucm->cold_clear_cl, &cn->bn.ln);

        Zeta_OrdRBLinkedListNode_InsertL(lrucm->cold_clear_cl, &cn->bn.ln);
    }

    for (; 0 < write_quata; --write_quata) {
        Zeta_OrdRBLinkedListNode* ln =
            Zeta_OrdRBLinkedListNode_GetR(lrucm->hot_dirty_cl);

        if (ln == lrucm->hot_dirty_cl) { break; }

        CNode* cn = ZETA_MemberToStruct(Zeta_LRUCacheManager_CNode, bn.ln, ln);

        WriteBack_(lrucm, cn);

        Zeta_OrdRBLinkedListNode_Extract(ln);

        Zeta_OrdRBLinkedListNode_InsertL(lrucm->hot_clear_cl, &cn->bn.ln);
    }

    return old_write_quata - write_quata;
}

void Zeta_LRUCacheManager_Check(void const* lrucm_) {
    Zeta_LRUCacheManager const* lrucm = lrucm_;
    ZETA_DebugAssert(lrucm != NULL);

    ZETA_DebugAssert(lrucm->origin != NULL);

    ZETA_DebugAssert(0 < lrucm->cache_size);

    ZETA_DebugAssert(lrucm->sn_allocator != NULL);
    ZETA_DebugAssert(ZETA_Allocator_GetAlign(lrucm->sn_allocator) %
                         alignof(Zeta_LRUCacheManager_SNode) ==
                     0);

    ZETA_DebugAssert(lrucm->cn_allocator != NULL);
    ZETA_DebugAssert(ZETA_Allocator_GetAlign(lrucm->cn_allocator) %
                         alignof(Zeta_LRUCacheManager_CNode) ==
                     0);

    ZETA_DebugAssert(lrucm->xn_allocator != NULL);
    ZETA_DebugAssert(ZETA_Allocator_GetAlign(lrucm->xn_allocator) %
                         alignof(Zeta_LRUCacheManager_XNode) ==
                     0);

    ZETA_DebugAssert(lrucm->frame_allocator != NULL);

    ZETA_DebugAssert(lrucm->ght.NodeHash == BNodeHash);
    ZETA_DebugAssert(lrucm->ght.NodeCompare == BNodeCompare);

    ZETA_DebugAssert(lrucm->hot_clear_cl != NULL);
    ZETA_DebugAssert(lrucm->hot_dirty_cl != NULL);
    ZETA_DebugAssert(lrucm->cold_clear_cl != NULL);
    ZETA_DebugAssert(lrucm->cold_dirty_cl != NULL);
}

void Zeta_LRUCacheManager_CheckSessionDescriptor(void const* lrucm_,
                                                 void* sn_) {
    Zeta_LRUCacheManager const* lrucm = lrucm_;
    CheckLRUCM_(lrucm);

    Zeta_LRUCacheManager_SNode* sn = sn_;
    ZETA_DebugAssert(sn != NULL);

    Pair pair = {
        .sn = sn,
        .cache_idx = ZETA_SIZE_MAX,
    };

    Zeta_GenericHashTable_Node* ghtn = Zeta_GenericHashTable_Find(
        &lrucm->ght, &pair, NULL, PairHash, NULL, PairBNodeCompare);

    ZETA_DebugAssert(ghtn != NULL);

    Zeta_LRUCacheManager_SNode* re_sn =
        ZETA_MemberToStruct(Zeta_LRUCacheManager_SNode, bn.ghtn, ghtn);

    ZETA_DebugAssert(sn == re_sn);
}

void Zeta_LRUCacheManager_DeployCacheManager(void* lrucm_,
                                             Zeta_CacheManager* cm) {
    Zeta_LRUCacheManager* lrucm = lrucm_;
    CheckLRUCM_(lrucm);

    Zeta_CacheManager_Init(cm);

    cm->context = lrucm;

    cm->const_context = lrucm;

    cm->GetOrigin = Zeta_LRUCacheManager_GetOrigin;

    cm->GetCacheSize = Zeta_LRUCacheManager_GetCacheSize;

    cm->Open = Zeta_LRUCacheManager_Open;

    cm->SetMaxCacheCnt = Zeta_LRUCacheManager_SetMaxCacheCnt;

    cm->Close = Zeta_LRUCacheManager_Close;

    cm->Read = Zeta_LRUCacheManager_Read;

    cm->Write = Zeta_LRUCacheManager_Write;

    cm->Flush = Zeta_LRUCacheManager_Flush;
}
