#pragma once

#include <debugger.h>
#include <seq_cntr.h>

#include <unordered_map>

#include "ptr_iter.h"
#include "random.h"

Buffer buffer_a;
Buffer buffer_b;

template <typename Value>
size_t SeqCntrUtils_GetRandomStride() {
    return sizeof(Value) + alignof(Value) * GetRandomInt<size_t, size_t>(1, 4);
}

auto& SeqCntrUtils_GetSanitizeFuncs() {
    static std::unordered_map<size_t (*)(void const* constext),
                              void (*)(Zeta_SeqCntr const* seq_cntr)>
        instance;
    return instance;
}

void SeqCntrUtils_AddSanitizeFunc(
    size_t (*GetWidth)(void const* context),
    void (*Sanitize)(Zeta_SeqCntr const* seq_cntr)) {
    auto& map{ SeqCntrUtils_GetSanitizeFuncs() };

    auto iter{ map.insert({ GetWidth, Sanitize }).first };
    ZETA_DebugAssert(iter->second == Sanitize);
}

void SeqCntrUtils_Sanitize(Zeta_SeqCntr const* seq_cntr) {
    if (seq_cntr == NULL) { return; }

    auto& map{ SeqCntrUtils_GetSanitizeFuncs() };

    auto iter{ map.find(seq_cntr->GetWidth) };
    ZETA_DebugAssert(iter != map.end());

    iter->second(seq_cntr);
}

auto& SeqCntrUtils_GetDestroyFuncs() {
    static std::unordered_map<size_t (*)(void const* constext),
                              void (*)(Zeta_SeqCntr* seq_cntr)>
        instance;
    return instance;
}

void SeqCntrUtils_AddDestroyFunc(size_t (*GetWidth)(void const* context),
                                 void (*Destroy)(Zeta_SeqCntr* seq_cntr)) {
    auto& map{ SeqCntrUtils_GetDestroyFuncs() };

    auto iter{ map.insert({ GetWidth, Destroy }).first };
    ZETA_DebugAssert(iter->second == Destroy);
}

void SeqCntrUtils_Destroy(Zeta_SeqCntr* seq_cntr) {
    if (seq_cntr == NULL) { return; }

    auto& map{ SeqCntrUtils_GetDestroyFuncs() };

    auto iter{ map.find(seq_cntr->GetWidth) };
    ZETA_DebugAssert(iter != map.end());

    iter->second(seq_cntr);
}

void SeqCntrUtils_Read(Zeta_SeqCntr* seq_cntr, size_t idx, size_t cnt,
                       void* dst, size_t dst_stride) {
    void* pos_cursor = ZETA_SeqCntr_AllocaCursor(seq_cntr);

    size_t size{ ZETA_SeqCntr_GetSize(seq_cntr) };

    ZETA_DebugAssert(cnt <= size);

    size_t cnt_a{ ZETA_GetMinOf(cnt, size - idx) };
    size_t cnt_b{ cnt - cnt_a };

    if (0 < cnt_a) {
        ZETA_SeqCntr_Access(seq_cntr, idx, pos_cursor, NULL);

        SeqCntrUtils_Sanitize(seq_cntr);

        ZETA_DebugAssert(ZETA_SeqCntr_Cursor_GetIdx(seq_cntr, pos_cursor) ==
                         idx);

        SeqCntrUtils_Sanitize(seq_cntr);

        ZETA_SeqCntr_Read(seq_cntr, pos_cursor, cnt_a, dst, dst_stride,
                          pos_cursor);

        SeqCntrUtils_Sanitize(seq_cntr);

        ZETA_DebugAssert(ZETA_SeqCntr_Cursor_GetIdx(seq_cntr, pos_cursor) ==
                         idx + cnt_a);

        SeqCntrUtils_Sanitize(seq_cntr);

        dst = (char*)dst + dst_stride * cnt_a;
        idx += (idx + cnt_a) % size;
    }

    if (0 < cnt_b) {
        ZETA_SeqCntr_Access(seq_cntr, 0, pos_cursor, NULL);

        SeqCntrUtils_Sanitize(seq_cntr);

        ZETA_DebugAssert(ZETA_SeqCntr_Cursor_GetIdx(seq_cntr, pos_cursor) == 0);

        ZETA_SeqCntr_Read(seq_cntr, pos_cursor, cnt_b, dst, dst_stride,
                          pos_cursor);

        SeqCntrUtils_Sanitize(seq_cntr);

        ZETA_DebugAssert(ZETA_SeqCntr_Cursor_GetIdx(seq_cntr, pos_cursor) ==
                         cnt_b);

        SeqCntrUtils_Sanitize(seq_cntr);
    }
}

void SeqCntrUtils_Write(Zeta_SeqCntr* seq_cntr, size_t idx, size_t cnt,
                        void const* src, size_t src_stride) {
    void* pos_cursor = ZETA_SeqCntr_AllocaCursor(seq_cntr);

    size_t size{ ZETA_SeqCntr_GetSize(seq_cntr) };

    ZETA_DebugAssert(cnt <= size);

    size_t cnt_a{ ZETA_GetMinOf(cnt, size - idx) };
    size_t cnt_b{ cnt - cnt_a };

    if (0 < cnt_a) {
        ZETA_SeqCntr_Access(seq_cntr, idx, pos_cursor, NULL);

        SeqCntrUtils_Sanitize(seq_cntr);

        ZETA_DebugAssert(ZETA_SeqCntr_Cursor_GetIdx(seq_cntr, pos_cursor) ==
                         idx);

        SeqCntrUtils_Sanitize(seq_cntr);

        ZETA_SeqCntr_Write(seq_cntr, pos_cursor, cnt_a, src, src_stride,
                           pos_cursor);

        SeqCntrUtils_Sanitize(seq_cntr);

        ZETA_DebugAssert(ZETA_SeqCntr_Cursor_GetIdx(seq_cntr, pos_cursor) ==
                         idx + cnt_a);

        SeqCntrUtils_Sanitize(seq_cntr);

        src = (char const*)src + src_stride * cnt_a;
        idx += (idx + cnt_a) % size;
    }

    if (0 < cnt_b) {
        ZETA_SeqCntr_Access(seq_cntr, 0, pos_cursor, NULL);

        SeqCntrUtils_Sanitize(seq_cntr);

        ZETA_DebugAssert(ZETA_SeqCntr_Cursor_GetIdx(seq_cntr, pos_cursor) == 0);

        SeqCntrUtils_Sanitize(seq_cntr);

        ZETA_SeqCntr_Write(seq_cntr, pos_cursor, cnt_b, src, src_stride,
                           pos_cursor);

        SeqCntrUtils_Sanitize(seq_cntr);

        ZETA_DebugAssert(ZETA_SeqCntr_Cursor_GetIdx(seq_cntr, pos_cursor) ==
                         cnt_b);

        SeqCntrUtils_Sanitize(seq_cntr);
    }
}

void SeqCntrUtils_PushL(Zeta_SeqCntr* seq_cntr, size_t cnt, void const* src,
                        size_t src_stride) {
    ZETA_SeqCntr_PushL(seq_cntr, cnt, NULL);

    SeqCntrUtils_Write(seq_cntr, 0, cnt, src, src_stride);
}

void SeqCntrUtils_PushR(Zeta_SeqCntr* seq_cntr, size_t cnt, void const* src,
                        size_t src_stride) {
    size_t size{ ZETA_SeqCntr_GetSize(seq_cntr) };

    ZETA_SeqCntr_PushR(seq_cntr, cnt, NULL);

    SeqCntrUtils_Sanitize(seq_cntr);

    SeqCntrUtils_Write(seq_cntr, size, cnt, src, src_stride);
}

void SeqCntrUtils_PopL(Zeta_SeqCntr* seq_cntr, size_t cnt) {
    ZETA_DebugAssert(cnt <= ZETA_SeqCntr_GetSize(seq_cntr));

    ZETA_SeqCntr_PopL(seq_cntr, cnt);

    SeqCntrUtils_Sanitize(seq_cntr);
}

void SeqCntrUtils_PopR(Zeta_SeqCntr* seq_cntr, size_t cnt) {
    ZETA_DebugAssert(cnt <= ZETA_SeqCntr_GetSize(seq_cntr));

    SeqCntrUtils_Sanitize(seq_cntr);

    ZETA_SeqCntr_PopR(seq_cntr, cnt);

    SeqCntrUtils_Sanitize(seq_cntr);
}

void SeqCntrUtils_Insert(Zeta_SeqCntr* seq_cntr, size_t idx, size_t cnt,
                         void const* src, size_t src_stride) {
    void* pos_cursor = ZETA_SeqCntr_AllocaCursor(seq_cntr);

    ZETA_SeqCntr_Access(seq_cntr, idx, pos_cursor, NULL);

    SeqCntrUtils_Sanitize(seq_cntr);

    ZETA_DebugAssert(ZETA_SeqCntr_Cursor_GetIdx(seq_cntr, pos_cursor) == idx);

    SeqCntrUtils_Sanitize(seq_cntr);

    ZETA_SeqCntr_Insert(seq_cntr, pos_cursor, cnt);

    SeqCntrUtils_Sanitize(seq_cntr);

    ZETA_DebugAssert(ZETA_SeqCntr_Cursor_GetIdx(seq_cntr, pos_cursor) == idx);

    SeqCntrUtils_Write(seq_cntr, idx, cnt, src, src_stride);
}

void SeqCntrUtils_Erase(Zeta_SeqCntr* seq_cntr, size_t idx, size_t cnt) {
    if (seq_cntr == NULL) { return; }

    size_t size{ ZETA_SeqCntr_GetSize(seq_cntr) };

    ZETA_DebugAssert(cnt <= size);

    void* pos_cursor = ZETA_SeqCntr_AllocaCursor(seq_cntr);

    ZETA_SeqCntr_Access(seq_cntr, idx, pos_cursor, NULL);

    SeqCntrUtils_Sanitize(seq_cntr);

    ZETA_DebugAssert(ZETA_SeqCntr_Cursor_GetIdx(seq_cntr, pos_cursor) == idx);

    SeqCntrUtils_Sanitize(seq_cntr);

    size_t cur_cnt{ std::min(size - idx, cnt) };

    ZETA_SeqCntr_Erase(seq_cntr, pos_cursor, cur_cnt);

    SeqCntrUtils_Sanitize(seq_cntr);

    cnt -= cur_cnt;

    ZETA_DebugAssert(ZETA_SeqCntr_GetSize(seq_cntr) == size - cur_cnt);

    SeqCntrUtils_Sanitize(seq_cntr);

    ZETA_DebugAssert(ZETA_SeqCntr_Cursor_GetIdx(seq_cntr, pos_cursor) == idx);

    SeqCntrUtils_Sanitize(seq_cntr);

    ZETA_SeqCntr_PeekL(seq_cntr, pos_cursor, NULL);

    SeqCntrUtils_Sanitize(seq_cntr);

    ZETA_DebugAssert(ZETA_SeqCntr_Cursor_GetIdx(seq_cntr, pos_cursor) == 0);

    ZETA_SeqCntr_Erase(seq_cntr, pos_cursor, cnt);

    SeqCntrUtils_Sanitize(seq_cntr);
}

void SeqCntrUtils_CheckCursor(Zeta_SeqCntr* seq_cntr, size_t max_op_size) {
    ZETA_DebugAssert(seq_cntr != NULL);

    size_t size = ZETA_SeqCntr_GetSize(seq_cntr);

    void* cursor = ZETA_SeqCntr_AllocaCursor(seq_cntr);

    ZETA_SeqCntr_GetLBCursor(seq_cntr, cursor);

    for (size_t idx = -1; idx != size; ++idx) {
        ZETA_DebugAssert(ZETA_SeqCntr_Cursor_GetIdx(seq_cntr, cursor) == idx);
        ZETA_SeqCntr_Cursor_StepR(seq_cntr, cursor);
    }

    ZETA_SeqCntr_GetRBCursor(seq_cntr, cursor);

    for (size_t idx = size; idx != (size_t)(-1); --idx) {
        ZETA_DebugAssert(ZETA_SeqCntr_Cursor_GetIdx(seq_cntr, cursor) == idx);
        ZETA_SeqCntr_Cursor_StepL(seq_cntr, cursor);
    }

    void* cursor_a = ZETA_SeqCntr_AllocaCursor(seq_cntr);
    void* cursor_b = ZETA_SeqCntr_AllocaCursor(seq_cntr);
    void* cursor_c = ZETA_SeqCntr_AllocaCursor(seq_cntr);

    for (size_t i = 0; i < max_op_size; ++i) {
        size_t idx_a{ GetRandomInt<size_t, size_t>(-1, size) };
        size_t idx_b{ GetRandomInt<size_t, size_t>(-1, size) };

        ZETA_SeqCntr_Access(seq_cntr, idx_a, cursor_a, NULL);
        ZETA_SeqCntr_Access(seq_cntr, idx_b, cursor_b, NULL);

        ZETA_DebugAssert(ZETA_SeqCntr_Cursor_GetIdx(seq_cntr, cursor_a) ==
                         idx_a);
        ZETA_DebugAssert(ZETA_SeqCntr_Cursor_GetIdx(seq_cntr, cursor_b) ==
                         idx_b);

        ZETA_DebugAssert(ZETA_SeqCntr_Cursor_AreEqual(
                             seq_cntr, cursor_a, cursor_b) == (idx_a == idx_b));

        ZETA_DebugAssert(
            ZETA_SeqCntr_Cursor_Compare(seq_cntr, cursor_a, cursor_b) ==
            ZETA_ThreeWayCompare(idx_a + 1, idx_b + 1));

        ZETA_DebugAssert(ZETA_SeqCntr_Cursor_GetDist(
                             seq_cntr, cursor_a, cursor_b) == idx_b - idx_a);

        // ---

        ZETA_SeqCntr_Access(seq_cntr, idx_a, cursor_c, NULL);

        if (idx_a <= idx_b) {
            ZETA_SeqCntr_Cursor_AdvanceR(seq_cntr, cursor_c, idx_b - idx_a);
        } else {
            ZETA_SeqCntr_Cursor_AdvanceL(seq_cntr, cursor_c, idx_a - idx_b);
        }

        ZETA_DebugAssert(ZETA_SeqCntr_Cursor_GetIdx(seq_cntr, cursor_c) ==
                         idx_b);

        // ---

        ZETA_SeqCntr_Access(seq_cntr, idx_b, cursor_c, NULL);

        if (idx_a <= idx_b) {
            ZETA_SeqCntr_Cursor_AdvanceL(seq_cntr, cursor_c, idx_b - idx_a);
        } else {
            ZETA_SeqCntr_Cursor_AdvanceR(seq_cntr, cursor_c, idx_a - idx_b);
        }

        ZETA_DebugAssert(ZETA_SeqCntr_Cursor_GetIdx(seq_cntr, cursor_c) ==
                         idx_a);
    }
}

// -----------------------------------------------------------------------------

size_t SeqCntrUtils_SyncGetSize(Zeta_SeqCntr* seq_cntr_a,
                                Zeta_SeqCntr* seq_cntr_b) {
    if (seq_cntr_a == NULL && seq_cntr_b == NULL) { return 0; }

    size_t size = seq_cntr_a == NULL ? ZETA_SeqCntr_GetSize(seq_cntr_b)
                                     : ZETA_SeqCntr_GetSize(seq_cntr_a);

    ZETA_DebugAssert(seq_cntr_a == NULL ||
                     size == ZETA_SeqCntr_GetSize(seq_cntr_a));

    ZETA_DebugAssert(seq_cntr_b == NULL ||
                     size == ZETA_SeqCntr_GetSize(seq_cntr_b));

    return size;
}

size_t SeqCntrUtils_SyncGetSize(const std::vector<Zeta_SeqCntr*>& seq_cntrs) {
    ZETA_DebugAssert(!seq_cntrs.empty());

    for (auto seq_cntr : seq_cntrs) { ZETA_DebugAssert(seq_cntr != NULL); }

    size_t size{ ZETA_SeqCntr_GetSize(seq_cntrs[0]) };

    for (auto seq_cntr : seq_cntrs) {
        ZETA_DebugAssert(size == ZETA_SeqCntr_GetSize(seq_cntr));
    }

    return size;
}

template <typename Value>
void SeqCntrUtils_SyncRandomRead(const std::vector<Zeta_SeqCntr*>& seq_cntrs,
                                 size_t max_op_size) {
    size_t stride{ SeqCntrUtils_GetRandomStride<Value>() };

    size_t size{ SeqCntrUtils_SyncGetSize(seq_cntrs) };

    size_t idx{ GetRandomInt<size_t, size_t>(0, size) };
    size_t cnt{ GetRandomInt<size_t, size_t>(0, std::min(max_op_size, size)) };

    Value* ba{ buffer_a.GetBuffer<Value>(stride * cnt) };
    Value* bb{ buffer_b.GetBuffer<Value>(stride * cnt) };

    bool read{ false };

    for (auto seq_cntr : seq_cntrs) {
        SeqCntrUtils_Write(seq_cntr, idx, cnt, ba, stride);

        if (read) {
            ZETA_DebugAssert(std::memcmp(ba, bb, sizeof(Value) * cnt) == 0);
        } else {
            read = true;
            std::memcpy(bb, ba, sizeof(Value) * cnt);
        }
    }
}

template <typename Value>
void SeqCntrUtils_SyncRandomWrite(const std::vector<Zeta_SeqCntr*>& seq_cntrs,
                                  size_t max_op_size) {
    size_t stride{ SeqCntrUtils_GetRandomStride<Value>() };

    size_t size{ SeqCntrUtils_SyncGetSize(seq_cntrs) };

    size_t idx{ GetRandomInt<size_t, size_t>(0, size) };
    size_t cnt{ GetRandomInt<size_t, size_t>(0, std::min(max_op_size, size)) };

    Value* ba{ buffer_a.GetBuffer<Value>(stride * cnt) };
    Value* bb{ buffer_b.GetBuffer<Value>(stride * cnt) };

    GetRandoms<Value>(
        PtrIter<Value>{ ba, static_cast<ptrdiff_t>(stride) },
        PtrIter<Value>{ ba, static_cast<ptrdiff_t>(stride) } + cnt);

    std::memcpy(bb, ba, stride * cnt);

    for (auto seq_cntr : seq_cntrs) {
        SeqCntrUtils_Write(seq_cntr, idx, cnt, ba, stride);

        ZETA_DebugAssert(std::memcmp(ba, bb, sizeof(Value) * cnt) == 0);
    }
}

template <typename Value>
void SeqCntrUtils_SyncRandomPushL(const std::vector<Zeta_SeqCntr*>& seq_cntrs,
                                  size_t max_op_size) {
    size_t stride{ SeqCntrUtils_GetRandomStride<Value>() };

    size_t cnt{ GetRandomInt<size_t, size_t>(0, max_op_size) };

    Value* ba{ buffer_a.GetBuffer<Value>(stride * cnt) };
    Value* bb{ buffer_b.GetBuffer<Value>(stride * cnt) };

    GetRandoms<Value>(
        PtrIter<Value>{ ba, static_cast<ptrdiff_t>(stride) },
        PtrIter<Value>{ ba, static_cast<ptrdiff_t>(stride) } + cnt);

    std::memcpy(bb, ba, sizeof(Value) * cnt);

    for (auto seq_cntr : seq_cntrs) {
        SeqCntrUtils_PushL(seq_cntr, cnt, ba, stride);

        ZETA_DebugAssert(std::memcmp(ba, bb, sizeof(Value) * cnt) == 0);
    }
}

template <typename Value>
void SeqCntrUtils_SyncRandomPushR(const std::vector<Zeta_SeqCntr*>& seq_cntrs,
                                  size_t max_op_size) {
    size_t stride{ SeqCntrUtils_GetRandomStride<Value>() };

    size_t cnt{ GetRandomInt<size_t, size_t>(0, max_op_size) };

    Value* ba{ buffer_a.GetBuffer<Value>(stride * cnt) };
    Value* bb{ buffer_b.GetBuffer<Value>(stride * cnt) };

    GetRandoms<Value>(
        PtrIter<Value>{ ba, static_cast<ptrdiff_t>(stride) },
        PtrIter<Value>{ ba, static_cast<ptrdiff_t>(stride) } + cnt);

    std::memcpy(bb, ba, sizeof(Value) * cnt);

    for (auto seq_cntr : seq_cntrs) {
        SeqCntrUtils_PushR(seq_cntr, cnt, ba, stride);

        ZETA_DebugAssert(std::memcmp(ba, bb, sizeof(Value) * cnt) == 0);
    }
}

void SeqCntrUtils_SyncRandomPopL(const std::vector<Zeta_SeqCntr*>& seq_cntrs,
                                 size_t max_op_size) {
    size_t size{ SeqCntrUtils_SyncGetSize(seq_cntrs) };

    size_t cnt{ GetRandomInt<size_t, size_t>(0, std::min(max_op_size, size)) };

    for (auto seq_cntr : seq_cntrs) { SeqCntrUtils_PopL(seq_cntr, cnt); }
}

void SeqCntrUtils_SyncRandomPopR(const std::vector<Zeta_SeqCntr*>& seq_cntrs,
                                 size_t max_op_size) {
    size_t size{ SeqCntrUtils_SyncGetSize(seq_cntrs) };

    size_t cnt{ GetRandomInt<size_t, size_t>(0, std::min(max_op_size, size)) };

    for (auto seq_cntr : seq_cntrs) { SeqCntrUtils_PopR(seq_cntr, cnt); }
}

template <typename Value>
void SeqCntrUtils_SyncRandomInsert(const std::vector<Zeta_SeqCntr*>& seq_cntrs,
                                   size_t max_op_size) {
    size_t stride{ SeqCntrUtils_GetRandomStride<Value>() };

    size_t size{ SeqCntrUtils_SyncGetSize(seq_cntrs) };

    size_t idx{ GetRandomInt<size_t, size_t>(0, size) };
    size_t cnt{ GetRandomInt<size_t, size_t>(0, max_op_size) };

    Value* ba{ buffer_a.GetBuffer<Value>(stride * cnt) };
    Value* bb{ buffer_b.GetBuffer<Value>(stride * cnt) };

    GetRandoms<Value>(
        PtrIter<Value>{ ba, static_cast<ptrdiff_t>(stride) },
        PtrIter<Value>{ ba, static_cast<ptrdiff_t>(stride) } + cnt);

    std::memcpy(bb, ba, sizeof(Value) * cnt);

    for (auto seq_cntr : seq_cntrs) {
        SeqCntrUtils_Insert(seq_cntr, idx, cnt, ba, stride);

        ZETA_DebugAssert(std::memcmp(ba, bb, sizeof(Value) * cnt) == 0);
    }
}

void SeqCntrUtils_SyncRandomErase(const std::vector<Zeta_SeqCntr*>& seq_cntrs,
                                  size_t max_op_size) {
    size_t size{ SeqCntrUtils_SyncGetSize(seq_cntrs) };

    size_t idx{ GetRandomInt<size_t, size_t>(0, size) };
    size_t cnt{ GetRandomInt<size_t, size_t>(0, std::min(max_op_size, size)) };

    for (auto seq_cntr : seq_cntrs) { SeqCntrUtils_Erase(seq_cntr, idx, cnt); }
}

template <typename Value>
void SeqCntrUtils_SyncRandomInit(const std::vector<Zeta_SeqCntr*>& seq_cntrs,
                                 size_t size) {
    size_t stride{ SeqCntrUtils_GetRandomStride<Value>() };

    void* ba{ buffer_a.GetBuffer<Value>(stride * size) };
    void* bb{ buffer_a.GetBuffer<Value>(stride * size) };

    GetRandoms<Value>(
        PtrIter<Value>{ ba, static_cast<ptrdiff_t>(stride) },
        PtrIter<Value>{ ba, static_cast<ptrdiff_t>(stride) } + size);

    std::memcpy(bb, ba, sizeof(Value) * size);

    for (auto seq_cntr : seq_cntrs) {
        SeqCntrUtils_Erase(seq_cntr, 0, ZETA_SeqCntr_GetSize(seq_cntr));

        SeqCntrUtils_PushR(seq_cntr, size, ba, stride);

        ZETA_DebugAssert(std::memcmp(ba, bb, sizeof(Value) * size) == 0);
    }
}

template <typename Value>
void SeqCntrUtils_SyncCompare2_(Zeta_SeqCntr* seq_cntr_a,
                                Zeta_SeqCntr* seq_cntr_b) {
    size_t width_a{ ZETA_SeqCntr_GetWidth(seq_cntr_a) };
    size_t width_b{ ZETA_SeqCntr_GetWidth(seq_cntr_b) };

    ZETA_DebugAssert(width_a == width_b);

    size_t size_a{ ZETA_SeqCntr_GetSize(seq_cntr_a) };
    size_t size_b{ ZETA_SeqCntr_GetSize(seq_cntr_b) };

    ZETA_DebugAssert(size_a == size_b);

    void* cursor_a = ZETA_SeqCntr_AllocaCursor(seq_cntr_a);
    void* cursor_b = ZETA_SeqCntr_AllocaCursor(seq_cntr_b);

    Value* ba{ buffer_a.GetBuffer<Value>(sizeof(Value)) };
    Value* bb{ buffer_b.GetBuffer<Value>(sizeof(Value)) };

    // ---

    ZETA_SeqCntr_PeekL(seq_cntr_a, cursor_a, NULL);
    ZETA_SeqCntr_PeekL(seq_cntr_b, cursor_b, NULL);

    for (size_t i{ 0 }; i < size_a; ++i) {
        ZETA_DebugAssert(ZETA_SeqCntr_Cursor_GetIdx(seq_cntr_a, cursor_a) == i);

        ZETA_DebugAssert(ZETA_SeqCntr_Cursor_GetIdx(seq_cntr_b, cursor_b) == i);

        ZETA_SeqCntr_Read(seq_cntr_a, cursor_a, 1, ba, width_a, cursor_a);

        ZETA_SeqCntr_Read(seq_cntr_b, cursor_b, 1, bb, width_b, cursor_b);

        ZETA_DebugAssert(std::memcmp(ba, bb, sizeof(Value)) == 0);
    }

    ZETA_DebugAssert(ZETA_SeqCntr_Cursor_GetIdx(seq_cntr_a, cursor_a) ==
                     size_a);

    ZETA_DebugAssert(ZETA_SeqCntr_Cursor_GetIdx(seq_cntr_b, cursor_b) ==
                     size_b);

    // ---

    ZETA_SeqCntr_PeekR(seq_cntr_a, cursor_a, NULL);
    ZETA_SeqCntr_PeekR(seq_cntr_b, cursor_b, NULL);

    for (size_t i{ size_a }; 0 < i--;) {
        ZETA_DebugAssert(ZETA_SeqCntr_Cursor_GetIdx(seq_cntr_a, cursor_a) == i);

        ZETA_DebugAssert(ZETA_SeqCntr_Cursor_GetIdx(seq_cntr_b, cursor_b) == i);

        ZETA_SeqCntr_Read(seq_cntr_a, cursor_a, 1, ba, width_a, NULL);

        ZETA_SeqCntr_Read(seq_cntr_b, cursor_b, 1, bb, width_b, NULL);

        ZETA_DebugAssert(std::memcmp(ba, bb, sizeof(Value)) == 0);

        ZETA_SeqCntr_Cursor_StepL(seq_cntr_a, cursor_a);

        ZETA_SeqCntr_Cursor_StepL(seq_cntr_b, cursor_b);
    }

    ZETA_DebugAssert(ZETA_SeqCntr_Cursor_GetIdx(seq_cntr_a, cursor_a) ==
                     (size_t)(-1));

    ZETA_DebugAssert(ZETA_SeqCntr_Cursor_GetIdx(seq_cntr_b, cursor_b) ==
                     (size_t)(-1));
}

template <typename Value>
void SeqCntrUtils_SyncCompare(const std::vector<Zeta_SeqCntr*>& seq_cntrs) {
    for (auto seq_cntr : seq_cntrs) { ZETA_DebugAssert(seq_cntr != NULL); }

    size_t seq_cntr_cnt{ seq_cntrs.size() };

    if (seq_cntr_cnt == 1) { return; }

    for (size_t i{ 0 }; i < seq_cntr_cnt; ++i) {
        SeqCntrUtils_SyncCompare2_<Value>(seq_cntrs[i],
                                          seq_cntrs[(i + 1) % seq_cntr_cnt]);
    }
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

enum SeqCntrUtils_OpEnum : int {
    READ,
    WRITE,
    PUSH_L,
    PUSH_R,
    POP_L,
    POP_R,
    INSERT,
    ERASE,

    CURSOR_STEP_L,
    CURSOR_STEP_R,
    CURSOR_ADVANCE_L,
    CURSOR_ADVANCE_R,
};

template <typename Value>
void SeqCntrUtils_DoRandomOperations(std::vector<Zeta_SeqCntr*> seq_cntrs,
                                     size_t iter_cnt,  //

                                     size_t read_max_op_size,   //
                                     size_t write_max_op_size,  //
                                     size_t push_l_max_op_size,
                                     size_t push_r_max_op_size,  //
                                     size_t pop_l_max_op_size,   //
                                     size_t pop_r_max_op_size,   //
                                     size_t insert_max_op_size,  //
                                     size_t erase_max_op_size,   //

                                     size_t cursor_step_l_max_op_size,  //
                                     size_t cursor_step_r_max_op_size,  //
                                     size_t cursor_advance_l_op_size,   //
                                     size_t cursor_advance_r_op_size    //
) {
    ZETA_Unused(cursor_step_l_max_op_size);
    ZETA_Unused(cursor_step_r_max_op_size);
    ZETA_Unused(cursor_advance_l_op_size);
    ZETA_Unused(cursor_advance_r_op_size);

    std::vector<int> ops;

    if (0 < read_max_op_size) { ops.push_back(SeqCntrUtils_OpEnum::READ); }
    if (0 < write_max_op_size) { ops.push_back(SeqCntrUtils_OpEnum::WRITE); }
    if (0 < push_l_max_op_size) { ops.push_back(SeqCntrUtils_OpEnum::PUSH_L); }
    if (0 < push_r_max_op_size) { ops.push_back(SeqCntrUtils_OpEnum::PUSH_R); }
    if (0 < pop_l_max_op_size) { ops.push_back(SeqCntrUtils_OpEnum::POP_L); }
    if (0 < pop_r_max_op_size) { ops.push_back(SeqCntrUtils_OpEnum::POP_R); }
    if (0 < insert_max_op_size) { ops.push_back(SeqCntrUtils_OpEnum::INSERT); }
    if (0 < erase_max_op_size) { ops.push_back(SeqCntrUtils_OpEnum::ERASE); }

    ZETA_DebugAssert(!ops.empty());

    for (size_t iter_i{ 0 }; iter_i < iter_cnt; ++iter_i) {
        switch (ops[GetRandomInt<size_t, size_t>(0, ops.size() - 1)]) {
            case SeqCntrUtils_OpEnum::READ:
                SeqCntrUtils_SyncRandomRead<Value>(seq_cntrs, read_max_op_size);
                break;

            case SeqCntrUtils_OpEnum::WRITE:
                SeqCntrUtils_SyncRandomWrite<Value>(seq_cntrs,
                                                    write_max_op_size);
                break;

            case SeqCntrUtils_OpEnum::PUSH_L:
                SeqCntrUtils_SyncRandomPushL<Value>(seq_cntrs,
                                                    push_l_max_op_size);
                break;

            case SeqCntrUtils_OpEnum::PUSH_R:
                SeqCntrUtils_SyncRandomPushR<Value>(seq_cntrs,
                                                    push_r_max_op_size);
                break;

            case SeqCntrUtils_OpEnum::POP_L:
                SeqCntrUtils_SyncRandomPopL(seq_cntrs, pop_l_max_op_size);
                break;

            case SeqCntrUtils_OpEnum::POP_R:
                SeqCntrUtils_SyncRandomPopR(seq_cntrs, pop_r_max_op_size);
                break;

            case SeqCntrUtils_OpEnum::INSERT:
                SeqCntrUtils_SyncRandomInsert<Value>(seq_cntrs,
                                                     insert_max_op_size);
                break;

            case SeqCntrUtils_OpEnum::ERASE:
                SeqCntrUtils_SyncRandomErase(seq_cntrs, erase_max_op_size);
                break;
        }

        /*
        for (auto seq_cntr : seq_cntrs) {
            SeqCntrUtils_CheckCursor(seq_cntr, 16);
        }
        */

        // SeqCntrUtils_SyncCompare<Value>(seq_cntrs);

        Zeta_Debugger_ClearPipe();
    }
}
