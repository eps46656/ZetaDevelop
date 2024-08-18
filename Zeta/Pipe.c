#include "Pipe.h"

#include "Debugger.h"

void Zeta_PipeTransfer(Zeta_Pipe* dst, Zeta_Pipe* src) {
    ZETA_DebugAssert(dst != NULL);
    ZETA_DebugAssert(src != NULL);

    void* src_context = src->context;

    bool_t (*SrcIsEmpty)(void* context) = src->IsEmpty;
    void (*SrcPeek)(void* context, size_t cnt, void* dst) = src->Peek;
    void (*SrcRead)(void* context, size_t cnt, void* dst) = src->Read;

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
