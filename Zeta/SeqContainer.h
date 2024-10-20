#pragma once

#include "define.h"

ZETA_ExternC_Beg;

ZETA_DeclareStruct(Zeta_SeqContainer);

struct Zeta_SeqContainer {
    /**
     * @brief The context of the container.
     */
    void* context;

    /**
     * @brief The number of bytes occupied by cursor.
     */
    size_t cursor_size;

    /**
     * @brief The number of bytes occupied by element.
     *
     * @param context The context of the container.
     */
    size_t (*GetWidth)(void* context);

    /**
     * @brief The byte offset of two adjacent elements in an element array.
     *
     * @param context The context of the container.
     */
    size_t (*GetStride)(void* context);

    /**
     * @brief The number of element in the container.
     *
     * @param context The context of the container.
     */
    size_t (*GetSize)(void* context);

    /**
     * @brief The maximum number of element can be contained by the container.
     *
     * @param context The context of the container.
     */
    size_t (*GetCapacity)(void* context);

    /**
     * @brief Get the left cursor.
     *
     * @param context The context of the container.
     * @param dst_cursor The destination of returned cursor.
     */
    void (*GetLBCursor)(void* context, void* dst_cursor);

    /**
     * @brief Get the right cursor.
     *
     * @param context The context of the container.
     * @param dst_cursor The destination of returned cursor.
     */
    void (*GetRBCursor)(void* context, void* dst_cursor);

    /**
     * @brief Peek the left-most element.
     *
     * @param context The context of the container.
     * @param dst_cursor Optional. The destination of returned cursor.
     * @param dst_elem Optional. The destination of returned element.
     *
     * @return The pointer to target element if it can be referred. Otherwise,
     * NULL.
     */
    void* (*PeekL)(void* context, void* dst_cursor, void* dst_elem);

    /**
     * @brief Peek the right-most element.
     *
     * @param context The context of the container.
     * @param dst_cursor Optional. The destination of returned cursor.
     * @param dst_elem Optional. The destination of returned element.
     *
     * @return The pointer to target element if it can be referred. Otherwise,
     * NULL.
     */
    void* (*PeekR)(void* context, void* dst_cursor, void* dst_elem);

    /**
     * @brief Access element by index.
     *
     * @param context The context of the container.
     * @param idx The index of target element.
     * @param dst_cursor Optional. The destination of returned cursor.
     * @param dst_elem Optional. The destination of returned element.
     *
     * @return The pointer to target element if it can be referred. Otherwise,
     * NULL.
     */
    void* (*Access)(void* context, size_t idx, void* dst_cursor,
                    void* dst_elem);

    /**
     * @brief Get the pointer of element pointed by cursor.
     *
     * @param context The context of the container.
     * @param pos_cursor The cursor pointing to target element.
     *
     * @return The pointer to target element if it can be referred. Otherwise,
     * NULL.
     */
    void* (*Refer)(void* context, void const* pos_cursor);

    /**
     * @brief Read <cnt> elements from cursor.
     *
     * @param context The context of the container.
     * @param pos_cursor The cursor pointing to target element. Will be renewed
     *                   to prevent invalid.
     * @param cnt The number of read elements.
     * @param dst The destination of read elements.
     * @param dst_cursor Optional. The destination of returned cursor pointing
     *                   right next of last read elements.
     */
    void (*Read)(void* context, void const* pos_cursor, size_t cnt, void* dst,
                 void* dst_cursor);

    /**
     * @brief Write <cnt> elements from cursor.
     *
     * @param context The context of the container.
     * @param pos_cursor The cursor pointing to target element. Will be renewed
     *                   to prevent invalid.
     * @param cnt The number of written elements.
     * @param dst The destination of written elements.
     * @param dst_cursor Optional. The destination of returned cursor pointing
     *                   right next of last written elements.
     */
    void (*Write)(void* context, void* pos_cursor, size_t cnt, void const* src,
                  void* dst_cursor);

    /**
     * @brief Push <cnt> elements from left end.
     *
     * @param context The context of the container.
     * @param dst_cursor Optional. The destination of returned cursor which
     *                   points to the first pushed element.
     */
    void* (*PushL)(void* context, size_t cnt, void* dst_cursor);

    /**
     * @brief Push <cnt> elements from right end.
     *
     * @param context The context of the container.
     * @param dst_cursor Optional. The destination of returned cursor which
     *                   points to the first pushed element.
     */
    void* (*PushR)(void* context, size_t cnt, void* dst_cursor);

    /**
     * @brief Insert <cnt> elements at <cursor>.
     *
     * @param context The context of the container.
     * @param pos_cursor The cursor pointing to insertion point. Will be renewed
     *                   to prevent invalid.
     * @param cnt The number of insertion elements.
     */
    void* (*Insert)(void* context, void* pos_cursor, size_t cnt);

    /**
     * @brief Pop <cnt> elements from left end.
     *
     * @param context The context of the container.
     */
    void (*PopL)(void* context, size_t cnt);

    /**
     * @brief Pop <cnt> elements from right end.
     *
     * @param context The context of the container.
     */
    void (*PopR)(void* context, size_t cnt);

    /**
     * @brief Erase <cnt> elements at <cursor>.
     *
     * @param context The context of the container.
     * @param pos_cursor The cursor pointing to erasion point. Will be renewed
     *                   to prevent invalid.
     * @param cnt The number of erasion elements.
     */
    void (*Erase)(void* context, void* pos_cursor, size_t cnt);

    /**
     * @brief Erase all elements in the container.
     *
     * @param context The context of the container.
     * @param pos_cursor The cursor pointing to erasion point. Will be renewed
     *                   to prevent invalid.
     * @param cnt The number of erasion elements.
     */
    void (*EraseAll)(void* context);

    /**
     * @brief Determine two cursors if they point to the same position.
     *
     * @param context The context of the container.
     * @param cursor_a The first target cursor.
     * @param cursor_b The second target cursor.
     */
    bool_t (*Cursor_AreEqual)(void* context, void const* cursor_a,
                              void const* cursor_b);

    /**
     * @brief Compare the direction between two cursors.
     *
     * @param context The context of the container.
     * @param cursor_a The first target cursor.
     * @param cursor_b The second target cursor.
     *
     * @return Negative, if cursor_a is to the left of cursor_b. Positive, if
     *         cursor_a is to the right of cursor_b. Zero, if cursor_a and
     *         cursor_b point to the same position.
     */
    int (*Cursor_Compare)(void* context, void const* cursor_a,
                          void const* cursor_b);

    /**
     * @brief Get the distance between two cursors.
     *
     * @param context The context of the container.
     * @param cursor_a The first target cursor.
     * @param cursor_b The second target cursor.
     *
     * @return The distance from cursor_a to cursor_b.
     */
    size_t (*Cursor_GetDist)(void* context, void const* cursor_a,
                             void const* cursor_b);

    /**
     * @brief Get the index pointed by cursor.
     *
     * @param context The context of the container.
     * @param cursor The target cursor.
     *
     * @return The index pointed by cursor.
     */
    size_t (*Cursor_GetIdx)(void* context, void const* cursor);

    /**
     * @brief Step cursor left.
     *
     * @param context The context of the container.
     * @param cursor The target cursor.
     */
    void (*Cursor_StepL)(void* context, void* cursor);

    /**
     * @brief Step cursor right.
     *
     * @param context The context of the container.
     * @param cursor The target cursor.
     */
    void (*Cursor_StepR)(void* context, void* cursor);

    /**
     * @brief Advance cursor left <step>.
     *
     * @param context The context of the container.
     * @param cursor The target cursor.
     * @param step The number of steps to advance cursor.
     */
    void (*Cursor_AdvanceL)(void* context, void* cursor, size_t step);

    /**
     * @brief Advance cursor right <step>.
     *
     * @param context The context of the container.
     * @param cursor The target cursor.
     * @param step The number of steps to advance cursor.
     */
    void (*Cursor_AdvanceR)(void* context, void* cursor, size_t step);
};

void Zeta_SeqContainer_Init(Zeta_SeqContainer* seq_cntr);

#define ZETA_SeqContainer_AllocaCursor(seq_cntr)                     \
    __builtin_alloca_with_align(                                     \
        ((Zeta_SeqContainer*)ZETA_ToVoidPtr(seq_cntr))->cursor_size, \
        CHAR_BIT * alignof(max_align_t))

#define ZETA_SeqContainer_Call_(seq_cntr, member_func, args...)       \
    ZETA_CallMemberFunc((Zeta_SeqContainer*)ZETA_ToVoidPtr(seq_cntr), \
                        member_func, args)

#define ZETA_SeqContainer_GetWidth(seq_cntr) \
    ZETA_SeqContainer_Call_((seq_cntr), GetWidth)

#define ZETA_SeqContainer_GetStride(seq_cntr) \
    ZETA_SeqContainer_Call_((seq_cntr), GetStride)

#define ZETA_SeqContainer_GetSize(seq_cntr) \
    ZETA_SeqContainer_Call_((seq_cntr), GetSize)

#define ZETA_SeqContainer_GetCapacity(seq_cntr) \
    ZETA_SeqContainer_Call_((seq_cntr), GetCapacity)

#define ZETA_SeqContainer_GetLBCursor(seq_cntr, dst_cursor)         \
    ZETA_SeqContainer_Call_((seq_cntr), GetLBCursor, (dst_cursor)); \
    ZETA_StaticAssert(TRUE)

#define ZETA_SeqContainer_GetRBCursor(seq_cntr, dst_cursor)         \
    ZETA_SeqContainer_Call_((seq_cntr), GetRBCursor, (dst_cursor)); \
    ZETA_StaticAssert(TRUE)

#define ZETA_SeqContainer_PeekL(seq_cntr, dst_cursor, dst_elem) \
    ZETA_SeqContainer_Call_((seq_cntr), PeekL, (dst_cursor), (dst_elem))

#define ZETA_SeqContainer_PeekR(seq_cntr, dst_cursor, dst_elem) \
    ZETA_SeqContainer_Call_((seq_cntr), PeekR, (dst_cursor), (dst_elem))

#define ZETA_SeqContainer_Access(seq_cntr, idx, dst_cursor, dst_elem) \
    ZETA_SeqContainer_Call_((seq_cntr), Access, (idx), (dst_cursor), (dst_elem))

#define ZETA_SeqContainer_Refer(seq_cntr, pos_cursor) \
    ZETA_SeqContainer_Call_((seq_cntr), Refer, (pos_cursor))

#define ZETA_SeqContainer_Read(seq_cntr, pos_cursor, cnt, dst, dst_cursor) \
    ZETA_SeqContainer_Call_((seq_cntr), Read, (pos_cursor), (cnt), (dst),  \
                            (dst_cursor))

#define ZETA_SeqContainer_Write(seq_cntr, pos_cursor, cnt, src, dst_cursor) \
    ZETA_SeqContainer_Call_((seq_cntr), Write, (pos_cursor), (cnt), (src),  \
                            (dst_cursor))

#define ZETA_SeqContainer_PushL(seq_cntr, cnt, dst_cursor) \
    ZETA_SeqContainer_Call_((seq_cntr), PushL, (cnt), (dst_cursor));

#define ZETA_SeqContainer_PushR(seq_cntr, cnt, dst_cursor) \
    ZETA_SeqContainer_Call_((seq_cntr), PushR, (cnt), (dst_cursor));

#define ZETA_SeqContainer_Insert(seq_cntr, pos_cursor, cnt) \
    ZETA_SeqContainer_Call_((seq_cntr), Insert, (pos_cursor), (cnt))

#define ZETA_SeqContainer_PopL(seq_cntr, cnt)         \
    ZETA_SeqContainer_Call_((seq_cntr), PopL, (cnt)); \
    ZETA_StaticAssert(TRUE)

#define ZETA_SeqContainer_PopR(seq_cntr, cnt)         \
    ZETA_SeqContainer_Call_((seq_cntr), PopR, (cnt)); \
    ZETA_StaticAssert(TRUE)

#define ZETA_SeqContainer_Erase(seq_cntr, pos_cursor, cnt)           \
    ZETA_SeqContainer_Call_((seq_cntr), Erase, (pos_cursor), (cnt)); \
    ZETA_StaticAssert(TRUE)

#define ZETA_SeqContainer_EraseAll(seq_cntr)       \
    ZETA_SeqContainer_Call_((seq_cntr), EraseAll); \
    ZETA_StaticAssert(TRUE)

#define ZETA_SeqContainer_Cursor_AreEqual(seq_cntr, cursor_a, cursor_b) \
    ZETA_SeqContainer_Call_((seq_cntr), Cursor_AreEqual, (cursor_a), (cursor_b))

#define ZETA_SeqContainer_Cursor_Compare(seq_cntr, cursor_a, cursor_b) \
    ZETA_SeqContainer_Call_((seq_cntr), Cursor_Compare, (cursor_a), (cursor_b))

#define ZETA_SeqContainer_Cursor_GetDist(seq_cntr, cursor_a, cursor_b) \
    ZETA_SeqContainer_Call_((seq_cntr), Cursor_GetDist, (cursor_a), (cursor_b))

#define ZETA_SeqContainer_Cursor_GetIdx(seq_cntr, cursor) \
    ZETA_SeqContainer_Call_((seq_cntr), Cursor_GetIdx, (cursor))

#define ZETA_SeqContainer_Cursor_StepL(seq_cntr, cursor) \
    ZETA_SeqContainer_Call_((seq_cntr), Cursor_StepL, (cursor))

#define ZETA_SeqContainer_Cursor_StepR(seq_cntr, cursor) \
    ZETA_SeqContainer_Call_((seq_cntr), Cursor_StepR, (cursor))

#define ZETA_SeqContainer_Cursor_AdvanceL(seq_cntr, cursor, idx) \
    ZETA_SeqContainer_Call_((seq_cntr), Cursor_AdvanceL, (cursor), (idx))

#define ZETA_SeqContainer_Cursor_AdvanceR(seq_cntr, cursor, idx) \
    ZETA_SeqContainer_Call_((seq_cntr), Cursor_AdvanceR, (cursor), (idx))

ZETA_ExternC_End;
