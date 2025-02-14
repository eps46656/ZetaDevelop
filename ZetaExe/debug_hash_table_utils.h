#pragma once

#include <debug_hash_table.h>
#include <debugger.h>

#include <cstdlib>

#include "assoc_cntr_utils.h"

struct DebugHashTablePack {
    Zeta_DebugHashTable debug_ht;
};

template <typename Elem>
Zeta_AssocCntr DebugHashTable_Create() {
    DebugHashTablePack* pack{ new DebugHashTablePack{} };

    pack->debug_ht.width = sizeof(Elem);

    pack->debug_ht.elem_hash_context = NULL;
    pack->debug_ht.ElemHash = &ZetaHash<Elem>;

    pack->debug_ht.elem_cmp_context = NULL;
    pack->debug_ht.ElemCompare = &ZetaCompare<Elem, Elem>;

    Zeta_DebugHashTable_Init(&pack->debug_ht);

    AssocCntrUtils_AddSanitizeFunc(&zeta_debug_hash_table_assoc_cntr_vtable,
                                   DebugHashTable_Sanitize);

    AssocCntrUtils_AddDestroyFunc(&zeta_debug_hash_table_assoc_cntr_vtable,
                                  DebugHashTable_Destroy);

    return { &zeta_debug_hash_table_assoc_cntr_vtable, &pack->debug_ht };
}

void DebugHashTable_Destroy(Zeta_AssocCntr assoc_cntr) {
    ZETA_DebugAssert(assoc_cntr.vtable ==
                     &zeta_debug_hash_table_assoc_cntr_vtable);
    if (assoc_cntr.context == NULL) { return; }

    DebugHashTablePack* pack{ ZETA_MemberToStruct(DebugHashTablePack, debug_ht,
                                                  assoc_cntr.context) };

    Zeta_DebugHashTable_Deinit(&pack->debug_ht);

    std::free(pack);
}

void DebugHashTable_Sanitize(Zeta_AssocCntr assoc_cntr) {
    ZETA_DebugAssert(assoc_cntr.vtable ==
                     &zeta_debug_hash_table_assoc_cntr_vtable);
    if (assoc_cntr.context == NULL) { return; }
}
