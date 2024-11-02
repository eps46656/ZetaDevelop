#include "seq_cntr.h"

#include "debugger.h"
#include "utils.h"

void Zeta_SeqCntr_Init(Zeta_SeqCntr* seq_cntr) {
    ZETA_DebugAssert(seq_cntr != NULL);

    seq_cntr->context = NULL;

    seq_cntr->cursor_size = 0;

    seq_cntr->Deinit = NULL;

    seq_cntr->GetWidth = NULL;

    seq_cntr->GetSize = NULL;

    seq_cntr->GetCapacity = NULL;

    seq_cntr->GetLBCursor = NULL;

    seq_cntr->GetRBCursor = NULL;

    seq_cntr->PeekL = NULL;

    seq_cntr->PeekR = NULL;

    seq_cntr->Access = NULL;

    seq_cntr->Refer = NULL;

    seq_cntr->Read = NULL;

    seq_cntr->Write = NULL;

    seq_cntr->PushL = NULL;

    seq_cntr->PushL = NULL;

    seq_cntr->Insert = NULL;

    seq_cntr->PopL = NULL;

    seq_cntr->PopR = NULL;

    seq_cntr->Erase = NULL;

    seq_cntr->EraseAll = NULL;

    seq_cntr->Cursor_AreEqual = NULL;

    seq_cntr->Cursor_Compare = NULL;

    seq_cntr->Cursor_GetDist = NULL;

    seq_cntr->Cursor_GetIdx = NULL;

    seq_cntr->Cursor_StepL = NULL;

    seq_cntr->Cursor_StepR = NULL;

    seq_cntr->Cursor_AdvanceL = NULL;

    seq_cntr->Cursor_AdvanceR = NULL;
}

void Zeta_SeqCntr_Assign(Zeta_SeqCntr* dst_seq_cntr,
                         Zeta_SeqCntr* src_seq_cntr) {
    ZETA_DebugAssert(dst_seq_cntr != NULL);
    ZETA_DebugAssert(src_seq_cntr != NULL);

    ZETA_DebugAssert(ZETA_SeqCntr_GetWidth(dst_seq_cntr) ==
                     ZETA_SeqCntr_GetWidth(src_seq_cntr));

    size_t width = ZETA_SeqCntr_GetWidth(dst_seq_cntr);

    size_t prefer_buffer_capacity = sizeof(void*) * 64;

    size_t buffer_capacity =
        ZETA_GetMaxOf(1, ZETA_CeilIntDiv(prefer_buffer_capacity, width));

    void* buffer = __builtin_alloca_with_align(
        width * buffer_capacity, __CHAR_BIT__ * alignof(max_align_t));

    size_t size = ZETA_SeqCntr_GetSize(src_seq_cntr);
    Zeta_SeqCntr_ResizeR(dst_seq_cntr, size);

    void* dst_cursor = ZETA_SeqCntr_AllocaCursor(dst_seq_cntr);
    void* src_cursor = ZETA_SeqCntr_AllocaCursor(src_seq_cntr);

    ZETA_SeqCntr_PeekL(dst_seq_cntr, dst_cursor, NULL);
    ZETA_SeqCntr_PeekL(src_seq_cntr, src_cursor, NULL);

    for (size_t i = 0; i < size;) {
        size_t cur_cnt = ZETA_GetMinOf(buffer_capacity, size - i);

        ZETA_SeqCntr_Read(src_seq_cntr, src_cursor, cur_cnt, buffer,
                          src_cursor);

        ZETA_SeqCntr_Write(dst_seq_cntr, dst_cursor, cur_cnt, buffer,
                           dst_cursor);

        i += cur_cnt;
    }
}

void Zeta_SeqCntr_ResizeL(Zeta_SeqCntr* seq_cntr, size_t size) {
    ZETA_DebugAssert(seq_cntr != NULL);

    size_t origin_size = ZETA_SeqCntr_GetSize(seq_cntr);

    if (origin_size < size) {
        ZETA_SeqCntr_PushL(seq_cntr, size - origin_size, NULL);
    } else if (size < origin_size) {
        ZETA_SeqCntr_PopL(seq_cntr, origin_size - size);
    }
}

void Zeta_SeqCntr_ResizeR(Zeta_SeqCntr* seq_cntr, size_t size) {
    ZETA_DebugAssert(seq_cntr != NULL);

    size_t origin_size = ZETA_SeqCntr_GetSize(seq_cntr);

    if (origin_size < size) {
        ZETA_SeqCntr_PushR(seq_cntr, size - origin_size, NULL);
    } else if (size < origin_size) {
        ZETA_SeqCntr_PopR(seq_cntr, origin_size - size);
    }
}
