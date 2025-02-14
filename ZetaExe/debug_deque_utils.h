#pragma once

#include <debug_deque.h>

#include <cstdlib>

#include "seq_cntr_utils.h"

struct DebugDequeUtils_Pack {
    Zeta_DebugDeque debug_deque;
};

template <typename Elem>
Zeta_SeqCntr DebugDequeUtils_Create() {
    DebugDequeUtils_Pack* pack{ new DebugDequeUtils_Pack{} };

    pack->debug_deque.width = sizeof(Elem);

    Zeta_DebugDeque_Init(&pack->debug_deque);

    SeqCntrUtils_AddSanitizeFunc(&zeta_debug_deque_seq_cntr_vtable,
                                 DebugDequeUtils_Sanitize);

    SeqCntrUtils_AddDestroyFunc(&zeta_debug_deque_seq_cntr_vtable,
                                DebugDequeUtils_Destroy);

    return { &zeta_debug_deque_seq_cntr_vtable, &pack->debug_deque };
}

void DebugDequeUtils_Destroy(Zeta_SeqCntr seq_cntr) {
    ZETA_DebugAssert(seq_cntr.vtable == &zeta_debug_deque_seq_cntr_vtable);
    if (seq_cntr.context == NULL) { return; }

    ZETA_DebugAssert(seq_cntr.vtable == &zeta_debug_deque_seq_cntr_vtable);
    if (seq_cntr.context == NULL) { return; }

    DebugDequeUtils_Pack* pack{ ZETA_MemberToStruct(
        DebugDequeUtils_Pack, debug_deque, seq_cntr.context) };

    Zeta_DebugDeque_Deinit(seq_cntr.context);

    delete pack;
}

void DebugDequeUtils_Sanitize(Zeta_SeqCntr seq_cntr) {
    ZETA_DebugAssert(seq_cntr.vtable == &zeta_debug_deque_seq_cntr_vtable);
    if (seq_cntr.context == NULL) { return; }
}
