#pragma once

#include "cache_manager.h"

ZETA_ExternC_Beg;

ZETA_DeclareStruct(Zeta_CachingArray);
ZETA_DeclareStruct(Zeta_CachingArray_Cursor);

struct Zeta_CachingArray {
    Zeta_SeqCntr* origin;

    Zeta_CacheManager* cm;
    void* sd;
};

struct Zeta_CachingArray_Cursor {
    Zeta_CachingArray const* ca;
    size_t idx;
};

void Zeta_CachingArray_Init(void* ca);

void Zeta_CachingArray_Deinit(void* ca);

/**
 * @copydoc Zeta_SeqCntr::GetWidth
 */
size_t Zeta_CachingArray_GetWidth(void const* ca);

size_t Zeta_CachingArray_GetStride(void const* ca);

/**
 * @copydoc Zeta_SeqCntr::GetOffset
 */
size_t Zeta_CachingArray_GetOffset(void const* ca);

/**
 * @copydoc Zeta_SeqCntr::GetSize
 */
size_t Zeta_CachingArray_GetSize(void const* ca);

/**
 * @copydoc Zeta_SeqCntr::GetCapacity
 */
size_t Zeta_CachingArray_GetCapacity(void const* ca);

/**
 * @copydoc Zeta_SeqCntr::GetLBCursor
 */
void Zeta_CachingArray_GetLBCursor(void const* ca, void* dst_cursor);

/**
 * @copydoc Zeta_SeqCntr::GetRBCursor
 */
void Zeta_CachingArray_GetRBCursor(void const* ca, void* dst_cursor);

/**
 * @copydoc Zeta_SeqCntr::PeekL
 */
void* Zeta_CachingArray_PeekL(void* ca, void* dst_cursor, void* dst_elem);

/**
 * @copydoc Zeta_SeqCntr::PeekR
 */
void* Zeta_CachingArray_PeekR(void* ca, void* dst_cursor, void* dst_elem);

/**
 * @copydoc Zeta_SeqCntr::Access
 */
void* Zeta_CachingArray_Access(void* ca, size_t idx, void* dst_cursor,
                               void* dst_elem);

/**
 * @copydoc Zeta_SeqCntr::Refer
 */
void* Zeta_CachingArray_Refer(void* ca, void const* pos_cursor);

/**
 * @copydoc Zeta_SeqCntr::Read
 */
void Zeta_CachingArray_Read(void const* ca, void const* pos_cursor, size_t cnt,
                            void* dst, size_t dst_stride, void* dst_cursor);

/**
 * @copydoc Zeta_SeqCntr::Write
 */
void Zeta_CachingArray_Write(void* ca, void* pos_cursor, size_t cnt,
                             void const* src, size_t src_stride,
                             void* dst_cursor);

void Zeta_CachingArray_Check(void const* ca);

/**
 * @copydoc Zeta_SeqCntr::Cursor_AreEqual
 */
bool_t Zeta_CachingArray_Cursor_AreEqual(void const* ca, void const* cursor_a,
                                         void const* cursor_b);
/**
 * @copydoc Zeta_SeqCntr::Cursor_Compare
 */
int Zeta_CachingArray_Cursor_Compare(void const* ca, void const* cursor_a,
                                     void const* cursor_b);
/**
 * @copydoc Zeta_SeqCntr::Cursor_GetDist
 */
size_t Zeta_CachingArray_Cursor_GetDist(void const* ca, void const* cursor_a,
                                        void const* cursor_b);
/**
 * @copydoc Zeta_SeqCntr::Cursor_GetIdx
 */
size_t Zeta_CachingArray_Cursor_GetIdx(void const* ca, void const* cursor);
/**
 * @copydoc Zeta_SeqCntr::Cursor_StepL
 */
void Zeta_CachingArray_Cursor_StepL(void const* ca, void* cursor);
/**
 * @copydoc Zeta_SeqCntr::Cursor_StepR
 */
void Zeta_CachingArray_Cursor_StepR(void const* ca, void* cursor);
/**
 * @copydoc Zeta_SeqCntr::Cursor_AdvanceL
 */
void Zeta_CachingArray_Cursor_AdvanceL(void const* ca, void* cursor,
                                       size_t step);
/**
 * @copydoc Zeta_SeqCntr::Cursor_AdvanceR
 */
void Zeta_CachingArray_Cursor_AdvanceR(void const* ca, void* cursor,
                                       size_t step);

void Zeta_CachingArray_Cursor_Check(void const* ca, void const* cursor);

void Zeta_CachingArray_DeploySeqCntr(void* ca, Zeta_SeqCntr* seq_cntr);

ZETA_ExternC_End;
