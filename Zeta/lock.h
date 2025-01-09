#pragma once

#include "define.h"

ZETA_ExternC_Beg;

ZETA_DeclareStruct(Zeta_SpinLock);

struct Zeta_SpinLock {
    volatile int x;
};

void Zeta_SpinLock_Init(Zeta_SpinLock* spin_lock);

bool_t Zeta_SpinLock_TryLock(Zeta_SpinLock* spin_lock);

void Zeta_SpinLock_Lock(Zeta_SpinLock* spin_lock);

void Zeta_SpinLock_Unlock(Zeta_SpinLock* spin_lock);

ZETA_ExternC_End;
