#include "hash_table_utils.h"

#include "debugger.h"

unsigned long long Zeta_PtrSizePair_Hash(void* context,
                                         void const* ptr_size_pair_) {
    ZETA_Unused(context);

    Zeta_PtrSizePair const* ptr_size_pair = ptr_size_pair_;
    ZETA_DebugAssert(ptr_size_pair != NULL);

    return ZETA_PtrToAddr(ptr_size_pair->ptr);
}

int Zeta_PtrSizePair_Compare(void* context, void const* ptr_size_pair_a_,
                             void const* ptr_size_pair_b_) {
    ZETA_Unused(context);

    Zeta_PtrSizePair const* ptr_size_pair_a = ptr_size_pair_a_;
    Zeta_PtrSizePair const* ptr_size_pair_b = ptr_size_pair_b_;

    ZETA_DebugAssert(ptr_size_pair_a != NULL);
    ZETA_DebugAssert(ptr_size_pair_b != NULL);

    return ZETA_ThreeWayCompare(ptr_size_pair_a->ptr, ptr_size_pair_b->ptr);
}

int Zeta_PtrSizePair_PtrCompare(void* context, void const* ptr_size_pair_,
                                void const* ptr_) {
    ZETA_Unused(context);

    Zeta_PtrSizePair const* ptr_size_pair = ptr_size_pair_;
    ZETA_DebugAssert(ptr_size_pair != NULL);

    void* const* ptr = ptr_;
    ZETA_DebugAssert(ptr != NULL);

    return ZETA_ThreeWayCompare(ptr_size_pair->ptr, *ptr);
}
