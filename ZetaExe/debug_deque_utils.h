#pragma once

#include <debug_deque.h>

#include <cstdlib>

#include "seq_cntr_utils.h"

struct DebugDequeUtils_Pack {
    Zeta_DebugDeque debug_deque;
};

template <typename Elem>
void DebugDequeUtils_Init(Zeta_SeqCntr* seq_cntr) {
    DebugDequeUtils_Pack* pack{ new DebugDequeUtils_Pack{} };

    pack->debug_deque.width = sizeof(Elem);

    Zeta_DebugDeque_Init(&pack->debug_deque);

    Zeta_DebugDeque_DeploySeqCntr(&pack->debug_deque, seq_cntr);

    SeqCntrUtils_AddSanitizeFunc(Zeta_DebugDeque_GetWidth,
                                 DebugDequeUtils_Sanitize);

    SeqCntrUtils_AddDestroyFunc(Zeta_DebugDeque_GetWidth,
                                DebugDequeUtils_Destroy);
}

void DebugDequeUtils_Deinit(Zeta_SeqCntr* seq_cntr) {
    if (seq_cntr == NULL || seq_cntr->GetSize != Zeta_DebugDeque_GetSize) {
        return;
    }

    DebugDequeUtils_Pack* pack{ ZETA_MemberToStruct(
        DebugDequeUtils_Pack, debug_deque, seq_cntr->context) };

    Zeta_DebugDeque_Deinit(seq_cntr->context);

    delete pack;
}

template <typename Elem>
Zeta_SeqCntr* DebugDequeUtils_Create() {
    Zeta_SeqCntr* seq_cntr{ new Zeta_SeqCntr{} };

    DebugDequeUtils_Init<Elem>(seq_cntr);

    return seq_cntr;
}

void DebugDequeUtils_Destroy(Zeta_SeqCntr* seq_cntr) {
    if (seq_cntr == NULL || seq_cntr->GetSize != Zeta_DebugDeque_GetSize) {
        return;
    }

    DebugDequeUtils_Deinit(seq_cntr);

    delete seq_cntr;
}

void DebugDequeUtils_Sanitize(Zeta_SeqCntr const* seq_cntr) {
    ZETA_DebugAssert(seq_cntr != NULL);
}
