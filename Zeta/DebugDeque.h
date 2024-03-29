#pragma once

#include "Vector.h"

ZETA_extern_c_beg;

typedef struct Zeta_DebugDeque Zeta_DebugDeque;

struct Zeta_DebugDeque {
    void* deque;
    size_t width;
};

void Zeta_DebugDeque_Create(void* dd);

void Zeta_DebugDeque_Destroy(void* dd);

size_t Zeta_DebugDeque_GetWidth(void* dd);

size_t Zeta_DebugDeque_GetSize(void* dd);

void* Zeta_DebugDeque_PeekL(void* dd);

void* Zeta_DebugDeque_PeekR(void* dd);

void* Zeta_DebugDeque_Access(void* dd, size_t idx);

void* Zeta_DebugDeque_PushL(void* dd);

void* Zeta_DebugDeque_PushR(void* dd);

void* Zeta_DebugDeque_Insert(void* dd, size_t idx);

void Zeta_DebugDeque_PopL(void* dd);

void Zeta_DebugDeque_PopR(void* dd);

void Zeta_DebugDeque_Erase(void* dd, size_t idx);

void Zeta_DebugDeque_ToVector(void* dd, Zeta_Vector* dst);

ZETA_extern_c_end;
