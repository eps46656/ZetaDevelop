#pragma once

#include <lru_cache_manager.h>

#include <unordered_map>
#include <unordered_set>

#include "cache_manager_utils.h"
#include "std_allocator.h"

struct LRUCacheManagerPack {
    StdAllocator sn_allocator_instance;
    StdAllocator cn_allocator_instance;
    StdAllocator xn_allocator_instance;
    StdAllocator frame_allocator_instance;
    StdAllocator ght_table_node_allocator_instance;

    Zeta_Allocator sn_allocator;
    Zeta_Allocator cn_allocator;
    Zeta_Allocator xn_allocator;
    Zeta_Allocator frame_allocator;
    Zeta_Allocator ght_table_node_allocator;

    Zeta_LRUCacheManager lrucm;
};

// -----------------------------------------------------------------------------

void LRUCacheManagerUtils_Init(Zeta_CacheManager* cm, Zeta_SeqCntr* origin,
                               size_t cache_size);

void LRUCacheManagerUtils_Deinit(Zeta_CacheManager* cm);

Zeta_CacheManager* LRUCacheManagerUtils_Create(Zeta_SeqCntr* origin,
                                               size_t cache_size);

void LRUCacheManagerUtils_Destroy(Zeta_CacheManager* cm);

void LRUCacheManagerUtils_Sanitize(Zeta_CacheManager const* cm);

// -----------------------------------------------------------------------------

void LRUCacheManagerUtils_Init(Zeta_CacheManager* cm, Zeta_SeqCntr* origin,
                               size_t cache_size) {
    ZETA_DebugAssert(cm != NULL);
    ZETA_DebugAssert(origin != NULL);
    ZETA_DebugAssert(0 < cache_size);

    LRUCacheManagerPack* pack{ new LRUCacheManagerPack{} };

    StdAllocator_DeployAllocator(&pack->sn_allocator_instance,
                                 &pack->sn_allocator);
    StdAllocator_DeployAllocator(&pack->cn_allocator_instance,
                                 &pack->cn_allocator);
    StdAllocator_DeployAllocator(&pack->xn_allocator_instance,
                                 &pack->xn_allocator);
    StdAllocator_DeployAllocator(&pack->frame_allocator_instance,
                                 &pack->frame_allocator);
    StdAllocator_DeployAllocator(&pack->ght_table_node_allocator_instance,
                                 &pack->ght_table_node_allocator);

    pack->lrucm.origin = origin;

    pack->lrucm.cache_size = cache_size;

    pack->lrucm.sn_allocator = &pack->sn_allocator;
    pack->lrucm.cn_allocator = &pack->cn_allocator;
    pack->lrucm.xn_allocator = &pack->xn_allocator;
    pack->lrucm.frame_allocator = &pack->frame_allocator;

    pack->lrucm.ght.table_node_allocator = &pack->ght_table_node_allocator;

    Zeta_LRUCacheManager_Init(&pack->lrucm);

    Zeta_LRUCacheManager_DeployCacheManager(&pack->lrucm, cm);

    CacheManagerUtils_AddSanitizeFunc(Zeta_LRUCacheManager_GetCacheSize,
                                      LRUCacheManagerUtils_Sanitize);

    CacheManagerUtils_AddDestroyFunc(Zeta_LRUCacheManager_GetCacheSize,
                                     LRUCacheManagerUtils_Destroy);
}

void LRUCacheManagerUtils_Deinit(Zeta_CacheManager* cm) {
    ZETA_DebugAssert(cm != NULL);
    ZETA_DebugAssert(cm->GetCacheSize == Zeta_LRUCacheManager_GetCacheSize);

    LRUCacheManagerPack* pack{ ZETA_MemberToStruct(LRUCacheManagerPack, lrucm,
                                                   cm->context) };

    Zeta_LRUCacheManager_Deinit(cm->context);

    delete pack;
}

Zeta_CacheManager* LRUCacheManagerUtils_Create(Zeta_SeqCntr* origin,
                                               size_t cache_size) {
    Zeta_CacheManager* cm{ new Zeta_CacheManager{} };

    LRUCacheManagerUtils_Init(cm, origin, cache_size);

    return cm;
}

void LRUCacheManagerUtils_Destroy(Zeta_CacheManager* cm) {
    LRUCacheManagerUtils_Deinit(cm);

    delete cm;
}

void LRUCacheManagerUtils_Sanitize(Zeta_CacheManager const* cm) {
    ZETA_DebugAssert(cm != NULL);
    ZETA_DebugAssert(cm->GetCacheSize == Zeta_LRUCacheManager_GetCacheSize);

    Zeta_LRUCacheManager* lrucm = (Zeta_LRUCacheManager*)cm->context;

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

        int bnc = Zeta_OrdRBLinkedListNode_GetColor(&bn->ln);

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

    for (Zeta_OrdLinkedListNode* sln = lrucm->fit_sl;;) {
        sln = (Zeta_OrdLinkedListNode*)Zeta_OrdRBLinkedListNode_GetR(sln);

        if (sln == lrucm->fit_sl) { break; }

        SNode* sn = ZETA_MemberToStruct(SNode, sln, sln);

        ZETA_DebugAssert(Zeta_OrdRBLinkedListNode_GetColor(&sn->bn.ln) ==
                         sn_color);

        ZETA_DebugAssert(sn->cn_cnt <= sn->max_cn_cnt);

        if (sn->max_cn_cnt == 0) { ZETA_DebugAssert(0 < sn->cn_cnt); }

        ZETA_DebugAssert(fit_sns.insert(sn).second);
        ZETA_DebugAssert(union_sns.insert(sn).second);
    }

    for (Zeta_OrdLinkedListNode* sln = lrucm->over_sl;;) {
        sln = (Zeta_OrdLinkedListNode*)Zeta_OrdLinkedListNode_GetR(sln);

        if (sln == lrucm->over_sl) { break; }

        SNode* sn = ZETA_MemberToStruct(SNode, sln, sln);

        ZETA_DebugAssert(Zeta_OrdRBLinkedListNode_GetColor(&sn->bn.ln) ==
                         sn_color);

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

    for (Zeta_OrdRBLinkedListNode* cln = lrucm->hot_clear_cl;;) {
        cln = (Zeta_OrdRBLinkedListNode*)Zeta_OrdRBLinkedListNode_GetR(cln);

        if (cln == lrucm->hot_clear_cl) { break; }

        BNode* bn = ZETA_MemberToStruct(BNode, ln, cln);

        ZETA_DebugAssert(Zeta_OrdRBLinkedListNode_GetColor(&bn->ln) ==
                         cn_color);

        CNode* cn = ZETA_MemberToStruct(CNode, bn, bn);

        ZETA_DebugAssert(0 < cn->ref_cnt);
        ZETA_DebugAssert(!cn->dirty);

        ZETA_DebugAssert(hot_clear_cns.insert(cn).second);
        ZETA_DebugAssert(union_cns.insert(cn).second);
    }

    for (Zeta_OrdRBLinkedListNode* cln = lrucm->hot_dirty_cl;;) {
        cln = (Zeta_OrdRBLinkedListNode*)Zeta_OrdRBLinkedListNode_GetR(cln);

        if (cln == lrucm->hot_dirty_cl) { break; }

        BNode* bn = ZETA_MemberToStruct(BNode, ln, cln);

        ZETA_DebugAssert(Zeta_OrdRBLinkedListNode_GetColor(&bn->ln) ==
                         cn_color);

        CNode* cn = ZETA_MemberToStruct(CNode, bn, bn);

        ZETA_DebugAssert(0 < cn->ref_cnt);
        ZETA_DebugAssert(cn->dirty);

        ZETA_DebugAssert(hot_dirty_cns.insert(cn).second);
        ZETA_DebugAssert(union_cns.insert(cn).second);
    }

    for (Zeta_OrdRBLinkedListNode* cln = lrucm->cold_clear_cl;;) {
        cln = (Zeta_OrdRBLinkedListNode*)Zeta_OrdRBLinkedListNode_GetR(cln);

        if (cln == lrucm->cold_clear_cl) { break; }

        BNode* bn = ZETA_MemberToStruct(BNode, ln, cln);

        ZETA_DebugAssert(Zeta_OrdRBLinkedListNode_GetColor(&bn->ln) ==
                         cn_color);

        CNode* cn = ZETA_MemberToStruct(CNode, bn, bn);

        ZETA_DebugAssert(cn->ref_cnt == 0);
        ZETA_DebugAssert(!cn->dirty);

        ZETA_DebugAssert(cold_clear_cns.insert(cn).second);
        ZETA_DebugAssert(union_cns.insert(cn).second);
    }

    for (Zeta_OrdRBLinkedListNode* cln = lrucm->cold_dirty_cl;;) {
        cln = (Zeta_OrdRBLinkedListNode*)Zeta_OrdRBLinkedListNode_GetR(cln);

        if (cln == lrucm->cold_dirty_cl) { break; }

        BNode* bn = ZETA_MemberToStruct(BNode, ln, cln);

        ZETA_DebugAssert(Zeta_OrdRBLinkedListNode_GetColor(&bn->ln) ==
                         cn_color);

        CNode* cn = ZETA_MemberToStruct(CNode, bn, bn);

        ZETA_DebugAssert(Zeta_OrdRBLinkedListNode_GetColor(&cn->bn.ln) ==
                         cn_color);

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

        for (Zeta_OrdRBLinkedListNode* ln = &sn->bn.ln;;) {
            ln = (Zeta_OrdRBLinkedListNode*)Zeta_OrdRBLinkedListNode_GetR(ln);

            if (ln == &sn->bn.ln) { break; }

            BNode* bn = ZETA_MemberToStruct(BNode, ln, ln);

            ZETA_DebugAssert(Zeta_OrdRBLinkedListNode_GetColor(&bn->ln) ==
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
        ZETA_SeqCntr_GetWidth(lrucm->origin) * lrucm->cache_size;

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
                            sizeof(Zeta_OrdLinkedListNode));
    Zeta_MemRecorder_Record(sn_mem_recorder, lrucm->over_sl,
                            sizeof(Zeta_OrdLinkedListNode));

    Zeta_MemRecorder_Record(cn_mem_recorder, lrucm->hot_clear_cl,
                            sizeof(Zeta_OrdRBLinkedListNode));
    Zeta_MemRecorder_Record(cn_mem_recorder, lrucm->hot_dirty_cl,
                            sizeof(Zeta_OrdRBLinkedListNode));
    Zeta_MemRecorder_Record(cn_mem_recorder, lrucm->cold_clear_cl,
                            sizeof(Zeta_OrdRBLinkedListNode));
    Zeta_MemRecorder_Record(cn_mem_recorder, lrucm->cold_dirty_cl,
                            sizeof(Zeta_OrdRBLinkedListNode));

    Zeta_MemRecorder_Destroy(sn_mem_recorder);
    Zeta_MemRecorder_Destroy(cn_mem_recorder);
    Zeta_MemRecorder_Destroy(xn_mem_recorder);
    Zeta_MemRecorder_Destroy(frame_mem_recorder);
}
