#pragma once

#include <debug_deque.h>

#include <cstdlib>

#include "seq_cntr_utils.h"

struct DebugDequePack {
    Zeta_DebugDeque debug_deque;
};

template <typename Elem>
void DebugDeque_Init(Zeta_SeqCntr* seq_cntr) {
    DebugDequePack* debug_deque_pack{ static_cast<DebugDequePack*>(
        std::malloc(sizeof(DebugDequePack))) };

    debug_deque_pack->debug_deque.width = sizeof(Elem);
    debug_deque_pack->debug_deque.stride = sizeof(Elem);

    Zeta_DebugDeque_Init(&debug_deque_pack->debug_deque);

    Zeta_DebugDeque_DeploySeqCntr(&debug_deque_pack->debug_deque, seq_cntr);

    SeqCntrUtils_AddSanitizeFunc(Zeta_DebugDeque_GetWidth, DebugDeque_Sanitize);
}

void DebugDeque_Deinit(Zeta_SeqCntr* seq_cntr) {
    if (seq_cntr == NULL || seq_cntr->GetSize != Zeta_DebugDeque_GetSize) {
        return;
    }

    DebugDequePack* debug_deque_pack{ ZETA_MemberToStruct(
        DebugDequePack, debug_deque, seq_cntr->context) };

    Zeta_DebugDeque_Deinit(seq_cntr->context);

    std::free(debug_deque_pack);
}

template <typename Elem>
Zeta_SeqCntr* DebugDeque_Create() {
    Zeta_SeqCntr* seq_cntr{ new Zeta_SeqCntr{} };
    DebugDeque_Init<Elem>(seq_cntr);
    return seq_cntr;
}

void DebugDeque_Destroy(Zeta_SeqCntr* seq_cntr) {
    if (seq_cntr == NULL || seq_cntr->GetSize != Zeta_DebugDeque_GetSize) {
        return;
    }

    DebugDeque_Deinit(seq_cntr);

    delete seq_cntr;
}

void DebugDeque_Sanitize(Zeta_SeqCntr const* seq_cntr) {
    ZETA_DebugAssert(seq_cntr != NULL);
}
