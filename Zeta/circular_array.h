#pragma once

#include "seq_cntr.h"

ZETA_ExternC_Beg;

ZETA_DeclareStruct(Zeta_CircularArray);
ZETA_DeclareStruct(Zeta_CircularArray_Cursor);

struct Zeta_CircularArray {
    void* data;
    size_t width;
    size_t stride;
    size_t offset;
    size_t size;
    size_t capacity;
};

struct Zeta_CircularArray_Cursor {
    Zeta_CircularArray const* ca;
    size_t idx;
    void* ref;
};

void Zeta_CircularArray_Init(void* ca);

/**
 * @copydoc Zeta_SeqCntr::Deinit
 */
void Zeta_CircularArray_Deinit(void* ca);

/**
 * @copydoc Zeta_SeqCntr::GetWidth
 */
size_t Zeta_CircularArray_GetWidth(void const* ca);

size_t Zeta_CircularArray_GetStride(void const* ca);

/**
 * @copydoc Zeta_SeqCntr::GetOffset
 */
size_t Zeta_CircularArray_GetOffset(void const* ca);

/**
 * @copydoc Zeta_SeqCntr::GetSize
 */
size_t Zeta_CircularArray_GetSize(void const* ca);

/**
 * @copydoc Zeta_SeqCntr::GetCapacity
 */
size_t Zeta_CircularArray_GetCapacity(void const* ca);

/**
 * @copydoc Zeta_SeqCntr::GetLBCursor
 */
void Zeta_CircularArray_GetLBCursor(void const* ca, void* dst_cursor);

/**
 * @copydoc Zeta_SeqCntr::GetRBCursor
 */
void Zeta_CircularArray_GetRBCursor(void const* ca, void* dst_cursor);

/**
 * @copydoc Zeta_SeqCntr::PeekL
 */
void* Zeta_CircularArray_PeekL(void* ca, void* dst_cursor, void* dst_elem);

/**
 * @copydoc Zeta_SeqCntr::PeekR
 */
void* Zeta_CircularArray_PeekR(void* ca, void* dst_cursor, void* dst_elem);

/**
 * @copydoc Zeta_SeqCntr::Access
 */
void* Zeta_CircularArray_Access(void* ca, size_t idx, void* dst_cursor,
                                void* dst_elem);

/**
 * @copydoc Zeta_SeqCntr::Refer
 */
void* Zeta_CircularArray_Refer(void* ca, void const* pos_cursor);

/**
 * @copydoc Zeta_SeqCntr::Read
 */
void Zeta_CircularArray_Read(void const* ca, void const* pos_cursor, size_t cnt,
                             void* dst, size_t dst_stride, void* dst_cursor);

/**
 * @copydoc Zeta_SeqCntr::Write
 */
void Zeta_CircularArray_Write(void* ca, void* pos_cursor, size_t cnt,
                              void const* src, size_t src_stride,
                              void* dst_cursor);

/**
 * @copydoc Zeta_SeqCntr::PushL
 */
void* Zeta_CircularArray_PushL(void* ca, size_t cnt, void* dst_cursor);

/**
 * @copydoc Zeta_SeqCntr::PushR
 */
void* Zeta_CircularArray_PushR(void* ca, size_t cnt, void* dst_cursor);

/**
 * @copydoc Zeta_SeqCntr::Insert
 */
void* Zeta_CircularArray_Insert(void* ca, void* pos_cursor, size_t cnt);

/**
 * @copydoc Zeta_SeqCntr::PopL
 */
void Zeta_CircularArray_PopL(void* ca, size_t cnt);

/**
 * @copydoc Zeta_SeqCntr::PopR
 */
void Zeta_CircularArray_PopR(void* ca, size_t cnt);

/**
 * @copydoc Zeta_SeqCntr::Erase
 */
void Zeta_CircularArray_Erase(void* ca, void* pos_cursor, size_t cnt);

/**
 * @copydoc Zeta_SeqCntr::EraseAll
 */
void Zeta_CircularArray_EraseAll(void* ca);

size_t Zeta_CircularArray_GetLongestContPred(void const* ca, size_t idx);

size_t Zeta_CircularArray_GetLongestContSucr(void const* ca, size_t idx);

void Zeta_CircularArray_Assign(void* dst_ca, void* src_ca, size_t dst_beg,
                               size_t src_beg, size_t cnt);

void Zeta_CircularArray_AssignFromSeqCntr(void* ca, void* ca_cursor,
                                          Zeta_SeqCntr* seq_cntr,
                                          void* seq_cntr_cursor, size_t cnt);

void Zeta_CircularArray_Check(void const* ca);

/**
 * @copydoc Zeta_SeqCntr::Cursor_AreEqual
 */
bool_t Zeta_CircularArray_Cursor_AreEqual(void const* ca, void const* cursor_a,
                                          void const* cursor_b);
/**
 * @copydoc Zeta_SeqCntr::Cursor_Compare
 */
int Zeta_CircularArray_Cursor_Compare(void const* ca, void const* cursor_a,
                                      void const* cursor_b);
/**
 * @copydoc Zeta_SeqCntr::Cursor_GetDist
 */
size_t Zeta_CircularArray_Cursor_GetDist(void const* ca, void const* cursor_a,
                                         void const* cursor_b);
/**
 * @copydoc Zeta_SeqCntr::Cursor_GetIdx
 */
size_t Zeta_CircularArray_Cursor_GetIdx(void const* ca, void const* cursor);
/**
 * @copydoc Zeta_SeqCntr::Cursor_StepL
 */
void Zeta_CircularArray_Cursor_StepL(void const* ca, void* cursor);
/**
 * @copydoc Zeta_SeqCntr::Cursor_StepR
 */
void Zeta_CircularArray_Cursor_StepR(void const* ca, void* cursor);
/**
 * @copydoc Zeta_SeqCntr::Cursor_AdvanceL
 */
void Zeta_CircularArray_Cursor_AdvanceL(void const* ca, void* cursor,
                                        size_t step);
/**
 * @copydoc Zeta_SeqCntr::Cursor_AdvanceR
 */
void Zeta_CircularArray_Cursor_AdvanceR(void const* ca, void* cursor,
                                        size_t step);

void Zeta_CircularArray_Cursor_Check(void const* ca, void const* cursor);

void Zeta_CircularArray_DeploySeqCntr(void* ca, Zeta_SeqCntr* seq_cntr);

ZETA_ExternC_End;
