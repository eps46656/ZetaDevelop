#pragma once

#include <dynamic_hash_table.h>
#include <mem_check_utils.h>

#include <cstdlib>

#include "assoc_cntr_utils.h"
#include "std_allocator.h"

struct DynamicHashTablePack {
    StdAllocator node_allocator_instance;
    Zeta_Allocator node_allocator;

    StdAllocator table_node_allocator_instance;
    Zeta_Allocator table_node_allocator;

    Zeta_DynamicHashTable dht;
};

template <typename Elem>
void DynamicHashTable_Init(Zeta_AssocCntr* assoc_cntr) {
    DynamicHashTablePack* pack{ new DynamicHashTablePack{} };

    StdAllocator_DeployAllocator(&pack->node_allocator_instance,
                                 &pack->node_allocator);
    StdAllocator_DeployAllocator(&pack->table_node_allocator_instance,
                                 &pack->table_node_allocator);

    pack->dht.width = sizeof(Elem);

    pack->dht.elem_hash_context = NULL;
    pack->dht.ElemHash = &ZetaHash<Elem>;

    pack->dht.elem_cmp_context = NULL;
    pack->dht.ElemCompare = &ZetaCompare<Elem, Elem>;

    pack->dht.node_allocator = &pack->node_allocator;

    pack->dht.ght.table_node_allocator = &pack->table_node_allocator;

    Zeta_DynamicHashTable_Init(&pack->dht);

    Zeta_DynamicHashTable_DeployAssocCntr(&pack->dht, assoc_cntr);

    AssocCntrUtils_AddSanitizeFunc(Zeta_DynamicHashTable_GetWidth,
                                   DynamicHashTable_Sanitize);

    AssocCntrUtils_AddDestroyFunc(Zeta_DynamicHashTable_GetWidth,
                                  DynamicHashTable_Destroy);
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

template <typename Elem>
Zeta_AssocCntr* DynamicHashTable_Create() {
    Zeta_AssocCntr* assoc_cntr{ new Zeta_AssocCntr{} };

    DynamicHashTable_Init<Elem>(assoc_cntr);

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

#if ZETA_EnableDebug
    DynamicHashTablePack* pack{ ZETA_MemberToStruct(DynamicHashTablePack, dht,
                                                    assoc_cntr->context) };

    Zeta_MemRecorder* table_recorder = Zeta_MemRecorder_Create();
    Zeta_MemRecorder* node_recorder = Zeta_MemRecorder_Create();

    Zeta_DynamicHashTable_Sanitize(&pack->dht, table_recorder, node_recorder);

    Zeta_MemCheck_MatchRecords(pack->table_node_allocator_instance.mem_recorder,
                               table_recorder);
    Zeta_MemCheck_MatchRecords(pack->node_allocator_instance.mem_recorder,
                               node_recorder);

    Zeta_MemRecorder_Destroy(table_recorder);
    Zeta_MemRecorder_Destroy(node_recorder);
#endif
}
