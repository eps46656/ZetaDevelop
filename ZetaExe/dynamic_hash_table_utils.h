#pragma once

#include <dynamic_hash_table.h>
#include <mem_check_utils.h>

#include <cstdlib>

#include "assoc_cntr_utils.h"
#include "std_allocator.h"

struct DynamicHashTablePack {
    StdAllocator ght_table_node_allocator;
    StdAllocator node_allocator;

    Zeta_DynamicHashTable dht;
};

template <typename Elem>
Zeta_AssocCntr DynamicHashTable_Create() {
    DynamicHashTablePack* pack{ new DynamicHashTablePack{} };

    pack->dht.width = sizeof(Elem);

    pack->dht.elem_hash_context = NULL;
    pack->dht.ElemHash = &ZetaHash<Elem>;

    pack->dht.elem_cmp_context = NULL;
    pack->dht.ElemCompare = &ZetaCompare<Elem, Elem>;

    pack->dht.ght.table_node_allocator = {
        .vtable = &zeta_std_allocator_vtable,
        .context = &pack->ght_table_node_allocator,
    };

    pack->dht.node_allocator = {
        .vtable = &zeta_std_allocator_vtable,
        .context = &pack->node_allocator,
    };

    Zeta_DynamicHashTable_Init(&pack->dht);

    AssocCntrUtils_AddSanitizeFunc(&zeta_dynamic_hash_table_assoc_cntr_vtable,
                                   DynamicHashTable_Sanitize);

    AssocCntrUtils_AddDestroyFunc(&zeta_dynamic_hash_table_assoc_cntr_vtable,
                                  DynamicHashTable_Destroy);

    return { &zeta_dynamic_hash_table_assoc_cntr_vtable, &pack->dht };
}

void DynamicHashTable_Destroy(Zeta_AssocCntr assoc_cntr) {
    ZETA_DebugAssert(assoc_cntr.vtable ==
                     &zeta_dynamic_hash_table_assoc_cntr_vtable);
    if (assoc_cntr.context == NULL) { return; }

    DynamicHashTablePack* pack{ ZETA_MemberToStruct(DynamicHashTablePack, dht,
                                                    assoc_cntr.context) };

    Zeta_DynamicHashTable_Deinit(&pack->dht);

    std::free(pack);
}

void DynamicHashTable_Sanitize(Zeta_AssocCntr assoc_cntr) {
    ZETA_DebugAssert(assoc_cntr.vtable ==
                     &zeta_dynamic_hash_table_assoc_cntr_vtable);
    if (assoc_cntr.context == NULL) { return; }

#if ZETA_EnableDebug
    DynamicHashTablePack* pack{ ZETA_MemberToStruct(DynamicHashTablePack, dht,
                                                    assoc_cntr.context) };

    Zeta_MemRecorder* table_recorder = Zeta_MemRecorder_Create();
    Zeta_MemRecorder* node_recorder = Zeta_MemRecorder_Create();

    Zeta_DynamicHashTable_Sanitize(&pack->dht, table_recorder, node_recorder);

    Zeta_MemCheck_MatchRecords(pack->ght_table_node_allocator.mem_recorder,
                               table_recorder);
    Zeta_MemCheck_MatchRecords(pack->node_allocator.mem_recorder,
                               node_recorder);

    Zeta_MemRecorder_Destroy(table_recorder);
    Zeta_MemRecorder_Destroy(node_recorder);
#endif
}
