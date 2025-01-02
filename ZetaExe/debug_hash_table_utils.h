#pragma once

#include <debug_hash_table.h>
#include <debugger.h>

#include <cstdlib>

#include "assoc_cntr_utils.h"

struct DebugHashTablePack {
    Zeta_DebugHashTable debug_ht;
};

template <typename Val>
void DebugHashTable_Init(
    Zeta_AssocCntr* assoc_cntr, void* elem_hash_context,
    unsigned long long (*ElemHash)(void* elem_hash_context, void const* elem),
    void* key_hash_context,
    unsigned long long (*KeyHash)(void* key_hash_context, void const* key),
    void* elem_cmp_context,
    int (*ElemCompare)(void* elem_cmp_context, void const* elem_a,
                       void const* elem_b),
    void* elem_key_cmp_context,
    int (*ElemKeyCompare)(void* elem_key_cmp_context, void const* elem,
                          void const* key)) {
    DebugHashTablePack* pack{ new DebugHashTablePack{} };

    pack->debug_ht.width = sizeof(Val);

    pack->debug_ht.key_hash_context = key_hash_context;
    pack->debug_ht.KeyHash = KeyHash;

    pack->debug_ht.elem_hash_context = elem_hash_context;
    pack->debug_ht.ElemHash = ElemHash;

    pack->debug_ht.elem_cmp_context = elem_cmp_context;
    pack->debug_ht.ElemCompare = ElemCompare;

    pack->debug_ht.elem_key_cmp_context = elem_key_cmp_context;
    pack->debug_ht.ElemKeyCompare = ElemKeyCompare;

    pack->debug_ht.node_allocator = &pack->node_allocator;

    pack->debug_ht.ght.table_allocator = &pack->table_allocator;

    Zeta_DebugHashTable_Init(&pack->debug_ht);

    Zeta_DebugHashTable_DeployAssocCntr(&pack->debug_ht, assoc_cntr);
}

void DebugHashTable_Deinit(Zeta_AssocCntr* assoc_cntr) {
    if (assoc_cntr == NULL ||
        assoc_cntr->GetSize != Zeta_DebugHashTable_GetSize) {
        return;
    }

    DebugHashTablePack* pack{ ZETA_MemberToStruct(DebugHashTablePack, debug_ht,
                                                  assoc_cntr->context) };

    Zeta_DebugHashTable_Deinit(assoc_cntr->context);

    std::free(pack);
}

template <typename Val>
Zeta_AssocCntr* DebugHashTable_Create(
    void* elem_hash_context,
    unsigned long long (*ElemHash)(void* elem_hash_context, void const* elem),
    void* key_hash_context,
    unsigned long long (*KeyHash)(void* key_hash_context, void const* key),
    void* elem_cmp_context,
    int (*ElemCompare)(void* elem_cmp_context, void const* elem_a,
                       void const* elem_b),
    void* elem_key_cmp_context,
    int (*ElemKeyCompare)(void* elem_key_cmp_context, void const* elem,
                          void const* key)) {
    Zeta_AssocCntr* assoc_cntr{ new Zeta_AssocCntr{} };

    DebugHashTable_Init<Val>(assoc_cntr, elem_hash_context, ElemHash,
                             key_hash_context, KeyHash, elem_cmp_context,
                             ElemCompare, elem_key_cmp_context, ElemKeyCompare);

    return assoc_cntr;
}

void DebugHashTable_Destroy(Zeta_AssocCntr* assoc_cntr) {
    if (assoc_cntr == NULL ||
        assoc_cntr->GetSize != Zeta_DebugHashTable_GetSize) {
        return;
    }

    DebugHashTable_Deinit(assoc_cntr);

    delete assoc_cntr;
}

void DebugHashTable_Sanitize(Zeta_AssocCntr const* assoc_cntr) {
    ZETA_DebugAssert(assoc_cntr != NULL);
    ZETA_DebugAssert(assoc_cntr->GetWidth != Zeta_DebugHashTable_GetWidth);
}
