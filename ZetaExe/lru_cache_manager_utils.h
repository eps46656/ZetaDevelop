#pragma once

#include <lru_cache_manager.h>

#include <unordered_map>
#include <unordered_set>

#include "cache_manager_utils.h"
#include "std_allocator.h"

struct LRUCacheManagerPack {
    StdAllocator sn_allocator;
    StdAllocator cn_allocator;
    StdAllocator xn_allocator;
    StdAllocator frame_allocator;
    StdAllocator ght_table_node_allocator;

    Zeta_LRUCacheManager lrucm;
};

Zeta_CacheManager LRUCacheManagerUtils_Create(Zeta_SeqCntr origin,
                                              size_t cache_size) {
    ZETA_DebugAssert(cm != NULL);
    ZETA_DebugAssert(origin != NULL);
    ZETA_DebugAssert(0 < cache_size);

    LRUCacheManagerPack* pack{ new LRUCacheManagerPack{} };

    pack->lrucm.origin = origin;

    pack->lrucm.cache_size = cache_size;

    pack->lrucm.sn_allocator = {
        .vtable = &zeta_std_allocator_vtable,
        .context = &pack->sn_allocator,
    };

    pack->lrucm.cn_allocator = {
        .vtable = &zeta_std_allocator_vtable,
        .context = &pack->cn_allocator,
    };

    pack->lrucm.xn_allocator = {
        .vtable = &zeta_std_allocator_vtable,
        .context = &pack->xn_allocator,
    };

    pack->lrucm.frame_allocator = {
        .vtable = &zeta_std_allocator_vtable,
        .context = &pack->frame_allocator,
    };

    pack->lrucm.ght.table_node_allocator = {
        .vtable = &zeta_std_allocator_vtable,
        .context = &pack->ght_table_node_allocator,
    };

    Zeta_LRUCacheManager_Init(&pack->lrucm);

    CacheManagerUtils_AddSanitizeFunc(&zeta_lru_cache_manager_vtable,
                                      LRUCacheManagerUtils_Sanitize);

    CacheManagerUtils_AddDestroyFunc(&zeta_lru_cache_manager_vtable,
                                     LRUCacheManagerUtils_Destroy);

    return { .vtable = &zeta_lru_cache_manager_vtable,
             .context = &pack->lrucm };
}

void LRUCacheManagerUtils_Destroy(Zeta_CacheManager cm) {
    ZETA_DebugAssert(cm.vtable == &zeta_lru_cache_manager_vtable);
    if (cm.context == NULL) { return; }

    LRUCacheManagerPack* pack{ ZETA_MemberToStruct(LRUCacheManagerPack, lrucm,
                                                   cm.context) };

    Zeta_LRUCacheManager_Deinit(&pack->lrucm);

    delete pack;
}

void LRUCacheManagerUtils_Sanitize(Zeta_CacheManager cm) {
    ZETA_DebugAssert(cm.vtable == &zeta_lru_cache_manager_vtable);
    if (cm.context == NULL) { return; }

    Zeta_LRUCacheManager* lrucm = (Zeta_LRUCacheManager*)cm.context;

    using BNode = Zeta_LRUCacheManager_BNode;
    using SNode = Zeta_LRUCacheManager_SNode;
    using CNode = Zeta_LRUCacheManager_CNode;
    using XNode = Zeta_LRUCacheManager_XNode;

    constexpr int sn_color = Zeta_LRUCacheManager_SNode_color;
    constexpr int cn_color = Zeta_LRUCacheManager_CNode_color;
    constexpr int xn_color = Zeta_LRUCacheManager_XNode_color;

    std::unordered_set<SNode*> sns;
    std::unordered_set<CNode*> cns;
    std::unordered_set<XNode*> xns;

    std::unordered_map<size_t, CNode*> cache_idx_to_cn;

    Zeta_MemRecorder* m = Zeta_MemRecorder_Create();

    Zeta_GenericHashTable_Sanitize(&lrucm->ght, NULL, m);

    for (const auto& p : m->records) {
        void const* ghtn = p.first;

        BNode* bn = ZETA_MemberToStruct(BNode, ghtn, ghtn);

        int bnc = Zeta_OrdRBLListNode_GetLColor(&bn->ln);

        ZETA_DebugAssert(bnc == sn_color || bnc == cn_color || bnc == xn_color);

        SNode* sn = NULL;
        CNode* cn = NULL;
        XNode* xn = NULL;

        switch (bnc) {
            case sn_color:
                sn = ZETA_MemberToStruct(SNode, bn, bn);
                ZETA_DebugAssert(sns.insert(sn).second);

                break;

            case cn_color:
                cn = ZETA_MemberToStruct(CNode, bn, bn);

                ZETA_DebugAssert(cns.insert(cn).second);

                ZETA_DebugAssert(
                    cache_idx_to_cn.insert({ cn->cache_idx, cn }).second);

                break;

            case xn_color:
                xn = ZETA_MemberToStruct(XNode, bn, bn);

                sn = xn->sn;

                ZETA_DebugAssert(xns.insert(xn).second);

                break;
        }
    }

    ZETA_DebugAssert(lrucm->cn_cnt == cns.size());

    // ---

    size_t sum_max_cn_cnt = 0;

    for (auto sn : sns) { sum_max_cn_cnt += sn->max_cn_cnt; }

    ZETA_DebugAssert(lrucm->max_cn_cnt == sum_max_cn_cnt);

    // ---

    std::unordered_set<SNode*> fit_sns;
    std::unordered_set<SNode*> over_sns;
    std::unordered_set<SNode*> union_sns;

    for (Zeta_OrdLListNode* sln = lrucm->fit_sl;;) {
        sln = (Zeta_OrdLListNode*)Zeta_OrdRBLListNode_GetR(sln);

        if (sln == lrucm->fit_sl) { break; }

        SNode* sn = ZETA_MemberToStruct(SNode, sln, sln);

        ZETA_DebugAssert(Zeta_OrdRBLListNode_GetLColor(&sn->bn.ln) == sn_color);

        ZETA_DebugAssert(sn->cn_cnt <= sn->max_cn_cnt);

        if (sn->max_cn_cnt == 0) { ZETA_DebugAssert(0 < sn->cn_cnt); }

        ZETA_DebugAssert(fit_sns.insert(sn).second);
        ZETA_DebugAssert(union_sns.insert(sn).second);
    }

    for (Zeta_OrdLListNode* sln = lrucm->over_sl;;) {
        sln = (Zeta_OrdLListNode*)Zeta_OrdLListNode_GetR(sln);

        if (sln == lrucm->over_sl) { break; }

        SNode* sn = ZETA_MemberToStruct(SNode, sln, sln);

        ZETA_DebugAssert(Zeta_OrdRBLListNode_GetLColor(&sn->bn.ln) == sn_color);

        ZETA_DebugAssert(sn->max_cn_cnt < sn->cn_cnt);

        ZETA_DebugAssert(over_sns.insert(sn).second);
        ZETA_DebugAssert(union_sns.insert(sn).second);
    }

    ZETA_DebugAssert(union_sns == sns);

    // ---

    std::unordered_set<CNode*> hot_clear_cns;
    std::unordered_set<CNode*> hot_dirty_cns;
    std::unordered_set<CNode*> cold_clear_cns;
    std::unordered_set<CNode*> cold_dirty_cns;
    std::unordered_set<CNode*> union_cns;

    for (Zeta_OrdRBLListNode* cln = lrucm->hot_clear_cl;;) {
        cln = (Zeta_OrdRBLListNode*)Zeta_OrdRBLListNode_GetR(cln);

        if (cln == lrucm->hot_clear_cl) { break; }

        BNode* bn = ZETA_MemberToStruct(BNode, ln, cln);

        ZETA_DebugAssert(Zeta_OrdRBLListNode_GetLColor(&bn->ln) == cn_color);

        CNode* cn = ZETA_MemberToStruct(CNode, bn, bn);

        ZETA_DebugAssert(0 < cn->ref_cnt);
        ZETA_DebugAssert(!cn->dirty);

        ZETA_DebugAssert(hot_clear_cns.insert(cn).second);
        ZETA_DebugAssert(union_cns.insert(cn).second);
    }

    for (Zeta_OrdRBLListNode* cln = lrucm->hot_dirty_cl;;) {
        cln = (Zeta_OrdRBLListNode*)Zeta_OrdRBLListNode_GetR(cln);

        if (cln == lrucm->hot_dirty_cl) { break; }

        BNode* bn = ZETA_MemberToStruct(BNode, ln, cln);

        ZETA_DebugAssert(Zeta_OrdRBLListNode_GetLColor(&bn->ln) == cn_color);

        CNode* cn = ZETA_MemberToStruct(CNode, bn, bn);

        ZETA_DebugAssert(0 < cn->ref_cnt);
        ZETA_DebugAssert(cn->dirty);

        ZETA_DebugAssert(hot_dirty_cns.insert(cn).second);
        ZETA_DebugAssert(union_cns.insert(cn).second);
    }

    for (Zeta_OrdRBLListNode* cln = lrucm->cold_clear_cl;;) {
        cln = (Zeta_OrdRBLListNode*)Zeta_OrdRBLListNode_GetR(cln);

        if (cln == lrucm->cold_clear_cl) { break; }

        BNode* bn = ZETA_MemberToStruct(BNode, ln, cln);

        ZETA_DebugAssert(Zeta_OrdRBLListNode_GetLColor(&bn->ln) == cn_color);

        CNode* cn = ZETA_MemberToStruct(CNode, bn, bn);

        ZETA_DebugAssert(cn->ref_cnt == 0);
        ZETA_DebugAssert(!cn->dirty);

        ZETA_DebugAssert(cold_clear_cns.insert(cn).second);
        ZETA_DebugAssert(union_cns.insert(cn).second);
    }

    for (Zeta_OrdRBLListNode* cln = lrucm->cold_dirty_cl;;) {
        cln = (Zeta_OrdRBLListNode*)Zeta_OrdRBLListNode_GetR(cln);

        if (cln == lrucm->cold_dirty_cl) { break; }

        BNode* bn = ZETA_MemberToStruct(BNode, ln, cln);

        ZETA_DebugAssert(Zeta_OrdRBLListNode_GetLColor(&bn->ln) == cn_color);

        CNode* cn = ZETA_MemberToStruct(CNode, bn, bn);

        ZETA_DebugAssert(Zeta_OrdRBLListNode_GetLColor(&cn->bn.ln) == cn_color);

        ZETA_DebugAssert(cn->ref_cnt == 0);
        ZETA_DebugAssert(cn->dirty);

        ZETA_DebugAssert(cold_dirty_cns.insert(cn).second);
        ZETA_DebugAssert(union_cns.insert(cn).second);
    }

    ZETA_DebugAssert(union_cns == cns);

    // ---

    std::unordered_map<SNode*, std::unordered_set<XNode*>> sn_to_xns;
    std::unordered_set<XNode*> union_xns;

    std::unordered_map<CNode*, size_t> ref_cnts;

    for (auto cn : cns) { ref_cnts.insert({ cn, 0 }); }

    for (auto sn : sns) {
        auto p{ sn_to_xns.insert({ sn, {} }) };

        ZETA_DebugAssert(p.second);

        auto& xns{ p.first->second };

        std::unordered_set<CNode*> cur_cns;

        for (Zeta_OrdRBLListNode* ln = &sn->bn.ln;;) {
            ln = (Zeta_OrdRBLListNode*)Zeta_OrdRBLListNode_GetR(ln);

            if (ln == &sn->bn.ln) { break; }

            BNode* bn = ZETA_MemberToStruct(BNode, ln, ln);

            ZETA_DebugAssert(Zeta_OrdRBLListNode_GetLColor(&bn->ln) ==
                             xn_color);

            XNode* xn = ZETA_MemberToStruct(XNode, bn, bn);

            ZETA_DebugAssert(xn->sn == sn);

            ZETA_DebugAssert(xns.insert(xn).second);
            ZETA_DebugAssert(union_xns.insert(xn).second);

            CNode* cn = xn->cn;

            ZETA_DebugAssert(0 < cns.count(cn));

            ZETA_DebugAssert(cur_cns.insert(cn).second);

            ++ref_cnts[cn];
        }

        ZETA_DebugAssert(sn->cn_cnt == xns.size());
    }

    ZETA_DebugAssert(union_xns == xns);

    for (auto p : ref_cnts) { ZETA_DebugAssert(p.first->ref_cnt == p.second); }

    // ---

    Zeta_MemRecorder* sn_mem_recorder = Zeta_MemRecorder_Create();
    Zeta_MemRecorder* cn_mem_recorder = Zeta_MemRecorder_Create();
    Zeta_MemRecorder* xn_mem_recorder = Zeta_MemRecorder_Create();
    Zeta_MemRecorder* frame_mem_recorder = Zeta_MemRecorder_Create();

    size_t frame_size =
        ZETA_SeqCntr_GetWidth(lrucm->origin, lrucm->origin_context) *
        lrucm->cache_size;

    for (auto sn : sns) {
        Zeta_MemRecorder_Record(sn_mem_recorder, sn, sizeof(SNode));
    }

    for (auto cn : cns) {
        Zeta_MemRecorder_Record(cn_mem_recorder, cn, sizeof(CNode));
        Zeta_MemRecorder_Record(frame_mem_recorder, cn->frame, frame_size);
    }

    for (auto xn : xns) {
        Zeta_MemRecorder_Record(sn_mem_recorder, xn, sizeof(XNode));
    }

    Zeta_MemRecorder_Record(sn_mem_recorder, lrucm->fit_sl,
                            sizeof(Zeta_OrdLListNode));
    Zeta_MemRecorder_Record(sn_mem_recorder, lrucm->over_sl,
                            sizeof(Zeta_OrdLListNode));

    Zeta_MemRecorder_Record(cn_mem_recorder, lrucm->hot_clear_cl,
                            sizeof(Zeta_OrdRBLListNode));
    Zeta_MemRecorder_Record(cn_mem_recorder, lrucm->hot_dirty_cl,
                            sizeof(Zeta_OrdRBLListNode));
    Zeta_MemRecorder_Record(cn_mem_recorder, lrucm->cold_clear_cl,
                            sizeof(Zeta_OrdRBLListNode));
    Zeta_MemRecorder_Record(cn_mem_recorder, lrucm->cold_dirty_cl,
                            sizeof(Zeta_OrdRBLListNode));

    Zeta_MemRecorder_Destroy(sn_mem_recorder);
    Zeta_MemRecorder_Destroy(cn_mem_recorder);
    Zeta_MemRecorder_Destroy(xn_mem_recorder);
    Zeta_MemRecorder_Destroy(frame_mem_recorder);
}
