#pragma once

#include <dynamic_hash_table.h>
#include <mem_check_utils.h>

#include <cstdlib>

#include "AssocCntrUtils.h"
#include "std_allocator.h"

struct DynamicHashTablePack {
    StdAllocator node_allocator_;
    StdAllocator table_allocator_;

    Zeta_Allocator node_allocator;
    Zeta_Allocator table_allocator;

    Zeta_DynamicHashTable dht;
};

template <typename Val>
void DynamicHashTable_Init(
    Zeta_AssocCntr* assoc_cntr, void* elem_hash_context,
    unsigned long long (*ElemHash)(void* elem_hash_context, void const* elem,
                                   unsigned long long salt),
    void* key_hash_context,
    unsigned long long (*KeyHash)(void* key_hash_context, void const* key,
                                  unsigned long long salt),
    void* elem_cmp_context,
    int (*ElemCompare)(void* elem_cmp_context, void const* elem_a,
                       void const* elem_b),
    void* elem_key_cmp_context,
    int (*ElemKeyCompare)(void* elem_key_cmp_context, void const* elem,
                          void const* key)) {
    DynamicHashTablePack* pack{ new DynamicHashTablePack{} };

    StdAllocator_DeployAllocator(&pack->node_allocator_, &pack->node_allocator);
    StdAllocator_DeployAllocator(&pack->table_allocator_,
                                 &pack->table_allocator);

    pack->dht.width = sizeof(Val);

    pack->dht.elem_hash_context = elem_hash_context;
    pack->dht.ElemHash = ElemHash;

    pack->dht.key_hash_context = key_hash_context;
    pack->dht.KeyHash = KeyHash;

    pack->dht.elem_cmp_context = elem_cmp_context;
    pack->dht.ElemCompare = ElemCompare;

    pack->dht.elem_key_cmp_context = elem_key_cmp_context;
    pack->dht.ElemKeyCompare = ElemKeyCompare;

    pack->dht.node_allocator = &pack->node_allocator;

    pack->dht.ght.table_allocator = &pack->table_allocator;

    Zeta_DynamicHashTable_Init(&pack->dht);

    Zeta_DynamicHashTable_DeployAssocCntr(&pack->dht, assoc_cntr);

    AssocCntr_AddSanitizeFunc(Zeta_DynamicHashTable_GetWidth,
                              DynamicHashTable_Sanitize);
}

void DynamicHashTable_Deinit(Zeta_AssocCntr* assoc_cntr) {
    if (assoc_cntr == NULL ||
        assoc_cntr->GetSize != Zeta_DynamicHashTable_GetSize) {
        return;
    }

    DynamicHashTablePack* pack{ ZETA_MemberToStruct(DynamicHashTablePack, dht,
                                                    assoc_cntr->context) };

    Zeta_DynamicHashTable_Deinit(assoc_cntr->context);

    std::free(pack);
}

template <typename Val>
Zeta_AssocCntr* DynamicHashTable_Create(
    void* elem_hash_context,
    unsigned long long (*ElemHash)(void* elem_hash_context, void const* elem,
                                   unsigned long long salt),
    void* key_hash_context,
    unsigned long long (*KeyHash)(void* key_hash_context, void const* key,
                                  unsigned long long salt),
    void* elem_cmp_context,
    int (*ElemCompare)(void* elem_cmp_context, void const* elem_a,
                       void const* elem_b),
    void* elem_key_cmp_context,
    int (*ElemKeyCompare)(void* elem_key_cmp_context, void const* elem,
                          void const* key)) {
    Zeta_AssocCntr* assoc_cntr{ new Zeta_AssocCntr{} };

    DynamicHashTable_Init<Val>(
        assoc_cntr, elem_hash_context, ElemHash, key_hash_context, KeyHash,
        elem_cmp_context, ElemCompare, elem_key_cmp_context, ElemKeyCompare);

    return assoc_cntr;
}

void DynamicHashTable_Destroy(Zeta_AssocCntr* assoc_cntr) {
    if (assoc_cntr == NULL ||
        assoc_cntr->GetSize != Zeta_DynamicHashTable_GetSize) {
        return;
    }

    DynamicHashTable_Deinit(assoc_cntr);

    delete assoc_cntr;
}

void DynamicHashTable_Sanitize(Zeta_AssocCntr const* assoc_cntr) {
    if (assoc_cntr->GetSize != Zeta_DynamicHashTable_GetSize) { return; }

    DynamicHashTablePack* pack{ ZETA_MemberToStruct(DynamicHashTablePack, dht,
                                                    assoc_cntr->context) };

    Zeta_MemRecorder* table_recorder = Zeta_MemRecorder_Create();
    Zeta_MemRecorder* node_recorder = Zeta_MemRecorder_Create();

    Zeta_DynamicHashTable_Sanitize(&pack->dht, table_recorder, node_recorder);

    Zeta_MemCheck_MatchRecords(pack->table_allocator_.mem_recorder,
                               table_recorder);
    Zeta_MemCheck_MatchRecords(pack->node_allocator_.mem_recorder,
                               node_recorder);

    Zeta_MemRecorder_Destroy(table_recorder);
    Zeta_MemRecorder_Destroy(node_recorder);
}
