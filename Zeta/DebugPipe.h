#pragma once

#include "SeqContainer.h"

ZETA_ExternC_Beg;

typedef uintptr_t Zeta_DebugPipe_val_t;

typedef struct Zeta_DebugPipe Zeta_DebugPipe;

struct Zeta_DebugPipe {
    void* deque;
};

void Zeta_DebugPipe_Create(void* dd);

void Zeta_DebugPipe_Destroy(void* dd);

size_t Zeta_DebugPipe_GetSize(void* dd);

Zeta_DebugPipe_val_t Zeta_DebugPipe_Peek(void* dp);

void Zeta_DebugPipe_Push(void* dp, Zeta_DebugPipe_val_t val);

Zeta_DebugPipe_val_t Zeta_DebugPipe_Pop(void* dd);

ZETA_ExternC_End;
