#pragma once

#include <cstdlib>

#include "../Zeta/DebugDeque.h"
#include "MemAllocatorCheck.h"
#include "StdAllocator.h"

struct DebugDequePack {
    Zeta_DebugDeque debug_deque;
};

template <typename Val>
void InitDebugDeque(Zeta_SeqContainer* seq_cntr) {
    DebugDequePack* debug_deque_pack{ static_cast<DebugDequePack*>(
        std::malloc(sizeof(DebugDequePack))) };

    debug_deque_pack->debug_deque.width = sizeof(Val);
    debug_deque_pack->debug_deque.stride = sizeof(Val);

    Zeta_DebugDeque_Init(&debug_deque_pack->debug_deque);
    Zeta_DebugDeque_DeploySeqContainer(&debug_deque_pack->debug_deque,
                                       seq_cntr);
}

void DeinitDebugDeque(Zeta_SeqContainer* seq_cntr) {
    if (seq_cntr == NULL || seq_cntr->GetSize != Zeta_DebugDeque_GetSize) {
        return;
    }

    DebugDequePack* debug_deque_pack{ ZETA_MemberToStruct(
        DebugDequePack, debug_deque, seq_cntr->context) };

    Zeta_DebugDeque_Deinit(seq_cntr->context);

    std::free(debug_deque_pack);
}

template <typename Val>
Zeta_SeqContainer* CreateDebugDeque() {
    Zeta_SeqContainer* seq_cntr{ new Zeta_SeqContainer{} };
    InitDebugDeque<Val>(seq_cntr);
    return seq_cntr;
}

void DestroyDebugDeque(Zeta_SeqContainer* seq_cntr) {
    if (seq_cntr == NULL || seq_cntr->GetSize != Zeta_DebugDeque_GetSize) {
        return;
    }

    DeinitDebugDeque(seq_cntr);

    delete seq_cntr;
}

void CheckDebugDeque(Zeta_SeqContainer const* seq_cntr) {
    ZETA_Unused(seq_cntr);
}
