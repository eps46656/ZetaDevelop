#include "Stream.h"

void Zeta_StreamTransfer(Zeta_Stream* dst, Zeta_Stream* src) {
    ZETA_DebugAssert(dst != NULL);
    ZETA_DebugAssert(src != NULL);

    void* src_context = src->context;

    bool_t (*SrcIsEmpty)(void* context) = src->IsEmpty;
    void const* (*SrcPeek)(void* context) = src->Peek;
    void (*SrcRead)(void* context) = src->Read;

    ZETA_DebugAssert(SrcIsEmpty != NULL);
    ZETA_DebugAssert(SrcPeek != NULL);
    ZETA_DebugAssert(SrcRead != NULL);

    void* dst_context = dst->context;

    void (*DstWrite)(void* context, void const* data) = dst->Write;

    ZETA_DebugAssert(DstWrite != NULL);

    while (!SrcIsEmpty(src_context)) {
        DstWrite(dst_context, SrcPeek(src_context));
        SrcRead(src_context);
    }
}
