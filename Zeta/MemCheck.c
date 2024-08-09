#include "MemCheck.h"

#include "Algorithm.h"
#include "Debugger.h"
#include "utils.h"

#if ZETA_IsDebug

#include <stdio.h>

#endif

unsigned long long Zeta_PtrSize_HashPtr(void const* ptr_) {
    void const* const* ptr = ptr_;
    return Zeta_SimpleHash(ZETA_PtrToAddr(ptr));
}

unsigned long long Zeta_PtrSize_Hash(void const* ptr_size_) {
    Zeta_PtrSize const* ptr_size = ptr_size_;
    return Zeta_SimpleHash(ZETA_PtrToAddr(ptr_size->ptr));
}

void Zeta_MemCheck_AddPtrSize(Zeta_AssocContainer* dst_assoc, void const* ptr,
                              size_t size) {
    ZETA_DebugAssert(dst_assoc != NULL);

    ZETA_DebugAssert(sizeof(Zeta_PtrSize) <=
                     dst_assoc->GetWidth(dst_assoc->context));

    Zeta_PtrSize* ptr_size = dst_assoc->Find(dst_assoc->context, NULL, &ptr);
    ZETA_DebugAssert(ptr_size == NULL);

    ptr_size = dst_assoc->Insert(dst_assoc->context, NULL, &ptr);

    ptr_size->ptr = ptr;
    ptr_size->size = size;
}

static int PtrSizeCompare_(void* context, void const* x_, void const* y_) {
    ZETA_Unused(context);

    Zeta_PtrSize const* x = x_;
    Zeta_PtrSize const* y = y_;

    if (x->ptr < y->ptr) { return -1; }
    if (y->ptr < x->ptr) { return 1; }
    return 0;
}

void Zeta_MemCheck_CheckOverlapping(Zeta_AssocContainer* assoc) {
#if ZETA_IsDebug

    ZETA_DebugAssert(assoc != NULL);
    ZETA_DebugAssert(assoc->GetSize != NULL);

    size_t size = assoc->GetSize(assoc->context);

    Zeta_PtrSize* data = malloc(sizeof(Zeta_PtrSize) * size);

    void* cursor = __builtin_alloca_with_align(assoc->cursor_size,
                                               CHAR_BIT * alignof(max_align_t));

    assoc->PeekL(assoc->context, cursor);

    for (size_t i = 0; i < size; ++i) {
        Zeta_MemCopy(data + i, assoc->Refer(assoc->context, cursor),
                     sizeof(Zeta_PtrSize));
    }

    Zeta_Sort(data, sizeof(Zeta_PtrSize), sizeof(Zeta_PtrSize), size, NULL,
              PtrSizeCompare_);

    for (size_t i = 1; i < size; ++i) {
        Zeta_PtrSize* x = data + (i - 1);
        Zeta_PtrSize* y = data + i;

        ZETA_DebugAssert(ZETA_AreOverlapped(x->ptr, x->size, y->ptr, y->size));
    }

    free(data);

#endif
}
