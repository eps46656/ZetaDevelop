#pragma once

#include "define.h"

ZETA_extern_c_beg;

typedef struct Zeta_DebugDeque Zeta_DebugDeque;

typedef void* Zeta_DebugDeque_val_t;

void* Zeta_DebugDeque_Create();

void Zeta_DebugDeque_Destroy(void* dd);

size_t Zeta_DebugDeque_GetSize(void* dd);

Zeta_DebugDeque_val_t* Zeta_DebugDeque_Access(void* dd, size_t idx);

Zeta_DebugDeque_val_t* Zeta_DebugDeque_Insert(void* dd, size_t idx);

Zeta_DebugDeque_val_t* Zeta_DebugDeque_PushL(void* dd);

Zeta_DebugDeque_val_t* Zeta_DebugDeque_PushR(void* dd);

void Zeta_DebugDeque_Erase(void* dd, size_t idx);

void Zeta_DebugDeque_PopL(void* dd);

void Zeta_DebugDeque_PopR(void* dd);

ZETA_extern_c_end;
