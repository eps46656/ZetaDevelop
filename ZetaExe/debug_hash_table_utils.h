#pragma once

#include <debug_hash_table.h>
#include <debugger.h>

#include <cstdlib>

#include "assoc_cntr_utils.h"

struct DebugHashTablePack {
    Zeta_DebugHashTable debug_ht;
};

template <typename Elem>
void DebugHashTable_Init(Zeta_AssocCntr* assoc_cntr) {
    DebugHashTablePack* pack{ new DebugHashTablePack{} };

    pack->debug_ht.width = sizeof(Elem);

    pack->debug_ht.elem_hash_context = NULL;
    pack->debug_ht.ElemHash = &ZetaHash<Elem>;

    pack->debug_ht.elem_cmp_context = NULL;
    pack->debug_ht.ElemCompare = &ZetaCompare<Elem, Elem>;

    Zeta_DebugHashTable_Init(&pack->debug_ht);

    Zeta_DebugHashTable_DeployAssocCntr(&pack->debug_ht, assoc_cntr);

    AssocCntrUtils_AddSanitizeFunc(Zeta_DebugHashTable_GetWidth,
                                   DebugHashTable_Sanitize);

    AssocCntrUtils_AddDestroyFunc(Zeta_DebugHashTable_GetWidth,
                                  DebugHashTable_Destroy);
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

template <typename Elem>
Zeta_AssocCntr* DebugHashTable_Create() {
    Zeta_AssocCntr* assoc_cntr{ new Zeta_AssocCntr{} };

    DebugHashTable_Init<Elem>(assoc_cntr);

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
    ZETA_DebugAssert(assoc_cntr->GetWidth == Zeta_DebugHashTable_GetWidth);
}
