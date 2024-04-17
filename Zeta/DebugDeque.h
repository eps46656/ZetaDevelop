#pragma once

#include "SeqContainer.h"

ZETA_ExternC_Beg;

typedef struct Zeta_DebugDeque Zeta_DebugDeque;

struct Zeta_DebugDeque {
    void* deque;
    size_t width;
};

void Zeta_DebugDeque_Create(void* dd);

void Zeta_DebugDeque_Destroy(void* dd);

size_t Zeta_DebugDeque_GetWidth(void* dd);
size_t Zeta_DebugDeque_GetSize(void* dd);

void Zeta_DebugDeque_GetLBCursor(void* context, void* dst_cursor);
void Zeta_DebugDeque_GetRBCursor(void* context, void* dst_cursor);

void* Zeta_DebugDeque_PeekL(void* dd, void* dst_cursor);
void* Zeta_DebugDeque_PeekR(void* dd, void* dst_cursor);

void* Zeta_DebugDeque_Access(void* dd, void* dst_cursor, size_t idx);

void* Zeta_DebugDeque_PushL(void* dd, void* dst_cursor);
void* Zeta_DebugDeque_PushR(void* dd, void* dst_cursor);

void* Zeta_DebugDeque_Insert(void* dd, void* pos_cursor);

void Zeta_DebugDeque_PopL(void* dd);
void Zeta_DebugDeque_PopR(void* dd);

void Zeta_DebugDeque_Erase(void* dd, void* pos_cursor);

void Zeta_DebugDeque_EraseAll(void* dd, void* callback_context,
                              void (*Callback)(void* context, void* ele));

void Zeta_DebugDeque_DeploySeqContainer(void* dd, Zeta_SeqContainer* dst);

ZETA_ExternC_End;
