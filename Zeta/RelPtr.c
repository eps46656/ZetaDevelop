#include "RelPtr.h"

void* Zeta_RelPtr_Get(void* rel_ptr_) {
    Zeta_RelPtr* rel_ptr = rel_ptr_;
    ZETA_DebugAssert(rel_ptr != NULL);

    return (uintptr_t)(void*)rel_ptr + rel_ptr->val;
}

void Zeta_RelPtr_Set(void* rel_ptr_, void* ptr) {
    Zeta_RelPtr* rel_ptr = rel_ptr_;
    ZETA_DebugAssert(rel_ptr != NULL);

    rel_ptr->val = (uintptr_t)ptr - (uintptr_t)(void*)rel_ptr;
}
