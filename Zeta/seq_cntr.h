#pragma once

#include "define.h"

ZETA_ExternC_Beg;

ZETA_DeclareStruct(Zeta_SeqCntr);

/**
 * @brief The interface of sequential container.
 */
struct Zeta_SeqCntr {
    /**
     * @brief The context of the container.
     */
    void* context;

    /**
     * @brief The number of bytes occupied by cursor.
     */
    size_t cursor_size;

    /**
     * @brief Deinitialize the container. Concrete container should implement
     *        this function.
     *
     * @param context The context of the container.
     */
    void (*Deinit)(void* context);

    /**
     * @brief The byte offset of two adjacent elements in an element array.
     *        Concrete container should implement this function.
     *
     * @param context The context of the container.
     */
    size_t (*GetWidth)(void* context);

    /**
     * @brief The number of element in the container. Concrete container should
     *        implement this function.
     *
     * @param context The context of the container.
     */
    size_t (*GetSize)(void* context);

    /**
     * @brief The maximum number of element can be contained by the container.
     *        Concrete container should implement this function.
     *
     * @param context The context of the container.
     */
    size_t (*GetCapacity)(void* context);

    /**
     * @brief Get the left cursor. Concrete container can implement this
     *        function optionally.
     *
     * @param context The context of the container.
     * @param dst_cursor The destination of returned cursor.
     */
    void (*GetLBCursor)(void* context, void* dst_cursor);

    /**
     * @brief Get the right cursor. Concrete container should implement this
     *        function.
     *
     * @param context The context of the container.
     * @param dst_cursor The destination of returned cursor.
     */
    void (*GetRBCursor)(void* context, void* dst_cursor);

    /**
     * @brief Peek the left-most element. Concrete container can implement this
     *        function optionally.
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
     * @brief Peek the right-most element. Concrete container should implement
     *        this function.
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
     * @brief Access element by index. Concrete container can implement this
     *        function optionally.
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
     * @brief Get the pointer of element pointed by cursor. Concrete container
     *        should implement this function.
     *
     * @param context The context of the container.
     * @param pos_cursor The cursor pointing to target element.
     *
     * @return The pointer to target element if it can be referred. Otherwise,
     *         NULL.
     */
    void* (*Refer)(void* context, void const* pos_cursor);

    /**
     * @brief Read \p cnt elements from cursor. Concrete container should
     *        implement this function.
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
     * @brief Write \p cnt elements from cursor. Concrete container should
     *        implement this function.
     *
     * @param context The context of the container.
     * @param pos_cursor The cursor pointing to target element. Will be renewed
     *                   to prevent invalid.
     * @param cnt The number of written elements.
     * @param src The source of written elements.
     * @param dst_cursor Optional. The destination of returned cursor pointing
     *                   right next of last written elements.
     */
    void (*Write)(void* context, void* pos_cursor, size_t cnt, void const* src,
                  void* dst_cursor);

    /**
     * @brief Push \p cnt elements from left end. Concrete container can
     *        implement this function optionally.
     *
     * @param context The context of the container.
     * @param cnt The number of pushing elements.
     * @param dst_cursor Optional. The destination of returned cursor which
     *                   points to the first pushed element.
     *
     * @return The pointer pointing the first pushed element if it can be
     *         referred. Otherwise, NULL.
     */
    void* (*PushL)(void* context, size_t cnt, void* dst_cursor);

    /**
     * @brief Push \p cnt elements from right end. Concrete container should
     *        implement this function.
     *
     * @param context The context of the container.
     * @param cnt The number of pushing elements.
     * @param dst_cursor Optional. The destination of returned cursor which
     *                   points to the first pushed element.
     *
     * @return The pointer pointing the first pushed element if it can be
     *         referred. Otherwise, NULL.
     */
    void* (*PushR)(void* context, size_t cnt, void* dst_cursor);

    /**
     * @brief Insert \p cnt elements at \p pos_cursor. Concrete container can
     *        implement this function optionally.
     *
     * @param context The context of the container.
     * @param pos_cursor The cursor pointing to inserting point. Will be renewed
     *                   to prevent invalid.
     * @param cnt The number of inserting elements.
     *
     * @return The pointer pointing the first inserted element if it can be
     *         referred. Otherwise, NULL.
     */
    void* (*Insert)(void* context, void* pos_cursor, size_t cnt);

    /**
     * @brief Pop \p cnt elements from left end. Concrete container can
     *        implement this function optionally.
     *
     * @param context The context of the container.
     * @param cnt The number of popped elements.
     */
    void (*PopL)(void* context, size_t cnt);

    /**
     * @brief Pop \p cnt elements from right end. Concrete container should
     *        implement this function.
     *
     * @param context The context of the container.
     * @param cnt The number of popped elements.
     */
    void (*PopR)(void* context, size_t cnt);

    /**
     * @brief Erase \p cnt elements at \p pos_cursor. Concrete container can
     *        implement this function optionally.
     *
     * @param context The context of the container.
     * @param pos_cursor The cursor pointing to erasing point. Will be renewed
     *                   to prevent invalid.
     * @param cnt The number of erasing elements.
     */
    void (*Erase)(void* context, void* pos_cursor, size_t cnt);

    /**
     * @brief Erase all elements in the container. Concrete container should
     *        implement this function.
     *
     * @param context The context of the container.
     */
    void (*EraseAll)(void* context);

    /**
     * @brief Determine if \p cursor_a and \p cursor_b point to the same
     *        position. Concrete container should implement this function.
     *
     * @param context The context of the container.
     * @param cursor_a The first target cursor.
     * @param cursor_b The second target cursor.
     *
     * @return If \p cursor_a and \p cursor_b point to the same position.
     */
    bool_t (*Cursor_AreEqual)(void* context, void const* cursor_a,
                              void const* cursor_b);

    /**
     * @brief Compare the direction between \p cursor_a and \p cursor_b.
     *        Concrete container can implement this function optionally.
     *
     * @param context The context of the container.
     * @param cursor_a The first target cursor.
     * @param cursor_b The second target cursor.
     *
     * @return Negative, if \p cursor_a is to the left of \p cursor_b. Positive,
     *         if \p cursor_a is to the right of \p cursor_b. Zero, if
     *         \p cursor_a and \p cursor_b point to the same position.
     */
    int (*Cursor_Compare)(void* context, void const* cursor_a,
                          void const* cursor_b);

    /**
     * @brief Get the distance from \p cursor_a to \p cursor_b. Concrete
     *        container can implement this function optionally.
     *
     * @param context The context of the container.
     * @param cursor_a The first target cursor.
     * @param cursor_b The second target cursor.
     *
     * @return The distance from \p cursor_a to \p cursor_b.
     */
    size_t (*Cursor_GetDist)(void* context, void const* cursor_a,
                             void const* cursor_b);

    /**
     * @brief Get the index pointed by \p cursor. Concrete container can
     *        implement this function optionally.
     *
     * @param context The context of the container.
     * @param cursor The target cursor.
     *
     * @return The index pointed by \p cursor.
     */
    size_t (*Cursor_GetIdx)(void* context, void const* cursor);

    /**
     * @brief Step \p cursor left. Concrete container can implement this
     *        function optionally.
     *
     * @param context The context of the container.
     * @param cursor The target cursor.
     */
    void (*Cursor_StepL)(void* context, void* cursor);

    /**
     * @brief Step \p cursor right. Concrete container should implement this
     *        function.
     *
     * @param context The context of the container.
     * @param cursor The target cursor.
     */
    void (*Cursor_StepR)(void* context, void* cursor);

    /**
     * @brief Advance cursor left \p step. Concrete container can implement
     *        this function optionally.
     *
     * @param context The context of the container.
     * @param cursor The target cursor.
     * @param step The number of steps to advance cursor.
     */
    void (*Cursor_AdvanceL)(void* context, void* cursor, size_t step);

    /**
     * @brief Advance \p cursor right \p step. Concrete container can implement
     *        this function optionally.
     *
     * @param context The context of the container.
     * @param cursor The target cursor.
     * @param step The number of steps to advance cursor.
     */
    void (*Cursor_AdvanceR)(void* context, void* cursor, size_t step);
};

/**
 * @brief Initialize the fields of interface with NULL.
 */
void Zeta_SeqCntr_Init(Zeta_SeqCntr* seq_cntr);

/**
 * @brief Assign \p src_seq_cntr to \p dst_seq_cntr.
 *
 * @param dst_seq_cntr The destination container.
 * @param src_seq_cntr The source container.
 */
void Zeta_SeqCntr_Assign(Zeta_SeqCntr* dst_seq_cntr,
                         Zeta_SeqCntr* src_seq_cntr);

/**
 * @brief Resize the container with push left or pop left.
 *
 * @param seq_cntr The target container.
 * @param size The number of elements in \p seq_cntr after resizing.
 */
void Zeta_SeqCntr_ResizeL(Zeta_SeqCntr* seq_cntr, size_t size);

/**
 * @brief Resize the container with push right or pop right.
 *
 * @param seq_cntr The target container.
 * @param size The number of elements in \p seq_cntr after resizing.
 */
void Zeta_SeqCntr_ResizeR(Zeta_SeqCntr* seq_cntr, size_t size);

#define ZETA_SeqCntr_AllocaCursor(seq_cntr)                     \
    __builtin_alloca_with_align(                                \
        ((Zeta_SeqCntr*)ZETA_ToVoidPtr(seq_cntr))->cursor_size, \
        __CHAR_BIT__ * alignof(max_align_t))

#define ZETA_SeqCntr_Call_(seq_cntr, member_func, ...)                        \
    ZETA_CallMemberFunc((Zeta_SeqCntr*)ZETA_ToVoidPtr(seq_cntr), member_func, \
                        __VA_ARGS__)

#define ZETA_SeqCntr_Deinit(seq_cntr) ZETA_SeqCntr_Call_((seq_cntr), Deinit)

#define ZETA_SeqCntr_GetWidth(seq_cntr) ZETA_SeqCntr_Call_((seq_cntr), GetWidth)

#define ZETA_SeqCntr_GetSize(seq_cntr) ZETA_SeqCntr_Call_((seq_cntr), GetSize)

#define ZETA_SeqCntr_GetCapacity(seq_cntr) \
    ZETA_SeqCntr_Call_((seq_cntr), GetCapacity)

#define ZETA_SeqCntr_GetLBCursor(seq_cntr, dst_cursor)         \
    ZETA_SeqCntr_Call_((seq_cntr), GetLBCursor, (dst_cursor)); \
    ZETA_StaticAssert(TRUE)

#define ZETA_SeqCntr_GetRBCursor(seq_cntr, dst_cursor)         \
    ZETA_SeqCntr_Call_((seq_cntr), GetRBCursor, (dst_cursor)); \
    ZETA_StaticAssert(TRUE)

#define ZETA_SeqCntr_PeekL(seq_cntr, dst_cursor, dst_elem) \
    ZETA_SeqCntr_Call_((seq_cntr), PeekL, (dst_cursor), (dst_elem))

#define ZETA_SeqCntr_PeekR(seq_cntr, dst_cursor, dst_elem) \
    ZETA_SeqCntr_Call_((seq_cntr), PeekR, (dst_cursor), (dst_elem))

#define ZETA_SeqCntr_Access(seq_cntr, idx, dst_cursor, dst_elem) \
    ZETA_SeqCntr_Call_((seq_cntr), Access, (idx), (dst_cursor), (dst_elem))

#define ZETA_SeqCntr_Refer(seq_cntr, pos_cursor) \
    ZETA_SeqCntr_Call_((seq_cntr), Refer, (pos_cursor))

#define ZETA_SeqCntr_Read(seq_cntr, pos_cursor, cnt, dst, dst_cursor) \
    ZETA_SeqCntr_Call_((seq_cntr), Read, (pos_cursor), (cnt), (dst),  \
                       (dst_cursor))

#define ZETA_SeqCntr_Write(seq_cntr, pos_cursor, cnt, src, dst_cursor) \
    ZETA_SeqCntr_Call_((seq_cntr), Write, (pos_cursor), (cnt), (src),  \
                       (dst_cursor))

#define ZETA_SeqCntr_PushL(seq_cntr, cnt, dst_cursor) \
    ZETA_SeqCntr_Call_((seq_cntr), PushL, (cnt), (dst_cursor));

#define ZETA_SeqCntr_PushR(seq_cntr, cnt, dst_cursor) \
    ZETA_SeqCntr_Call_((seq_cntr), PushR, (cnt), (dst_cursor));

#define ZETA_SeqCntr_Insert(seq_cntr, pos_cursor, cnt) \
    ZETA_SeqCntr_Call_((seq_cntr), Insert, (pos_cursor), (cnt))

#define ZETA_SeqCntr_PopL(seq_cntr, cnt)         \
    ZETA_SeqCntr_Call_((seq_cntr), PopL, (cnt)); \
    ZETA_StaticAssert(TRUE)

#define ZETA_SeqCntr_PopR(seq_cntr, cnt)         \
    ZETA_SeqCntr_Call_((seq_cntr), PopR, (cnt)); \
    ZETA_StaticAssert(TRUE)

#define ZETA_SeqCntr_Erase(seq_cntr, pos_cursor, cnt)           \
    ZETA_SeqCntr_Call_((seq_cntr), Erase, (pos_cursor), (cnt)); \
    ZETA_StaticAssert(TRUE)

#define ZETA_SeqCntr_EraseAll(seq_cntr)       \
    ZETA_SeqCntr_Call_((seq_cntr), EraseAll); \
    ZETA_StaticAssert(TRUE)

#define ZETA_SeqCntr_Cursor_AreEqual(seq_cntr, cursor_a, cursor_b) \
    ZETA_SeqCntr_Call_((seq_cntr), Cursor_AreEqual, (cursor_a), (cursor_b))

#define ZETA_SeqCntr_Cursor_Compare(seq_cntr, cursor_a, cursor_b) \
    ZETA_SeqCntr_Call_((seq_cntr), Cursor_Compare, (cursor_a), (cursor_b))

#define ZETA_SeqCntr_Cursor_GetDist(seq_cntr, cursor_a, cursor_b) \
    ZETA_SeqCntr_Call_((seq_cntr), Cursor_GetDist, (cursor_a), (cursor_b))

#define ZETA_SeqCntr_Cursor_GetIdx(seq_cntr, cursor) \
    ZETA_SeqCntr_Call_((seq_cntr), Cursor_GetIdx, (cursor))

#define ZETA_SeqCntr_Cursor_StepL(seq_cntr, cursor) \
    ZETA_SeqCntr_Call_((seq_cntr), Cursor_StepL, (cursor))

#define ZETA_SeqCntr_Cursor_StepR(seq_cntr, cursor) \
    ZETA_SeqCntr_Call_((seq_cntr), Cursor_StepR, (cursor))

#define ZETA_SeqCntr_Cursor_AdvanceL(seq_cntr, cursor, idx) \
    ZETA_SeqCntr_Call_((seq_cntr), Cursor_AdvanceL, (cursor), (idx))

#define ZETA_SeqCntr_Cursor_AdvanceR(seq_cntr, cursor, idx) \
    ZETA_SeqCntr_Call_((seq_cntr), Cursor_AdvanceR, (cursor), (idx))

ZETA_ExternC_End;
