#include "lock.h"

#if defined(__i386__) || defined(__x86_64__)

#define CPU_RELAX __asm__("pause")

#elif defined(__arm__) || defined(__aarch64__)

#define CPU_RELAX __asm__("yield")

#endif

void Zeta_SpinLock_Init(Zeta_SpinLock* spin_lock) { spin_lock->x = 0; }

bool_t Zeta_SpinLock_TryLock(Zeta_SpinLock* spin_lock) {
    return __sync_lock_test_and_set(&spin_lock->x, 1) == 0;
}

void Zeta_SpinLock_Lock(Zeta_SpinLock* spin_lock) {
    while (__sync_lock_test_and_set(&spin_lock->x, 1) == 1) { CPU_RELAX; }
}

void Zeta_SpinLock_Unlock(Zeta_SpinLock* spin_lock) {
    __sync_lock_release(&spin_lock->x);
}
