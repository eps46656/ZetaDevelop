#pragma once

#include <debugger.h>
#include <seq_cntr.h>

#include <unordered_map>

template <typename Value>
Value* GetBuffer_A(size_t size) {
    static Buffer<unsigned char> buffer;

    return static_cast<Value*>(
        static_cast<void*>(buffer.GetBuffer(sizeof(Value) * size)));
}

template <typename Value>
Value* GetBuffer_B(size_t size) {
    static Buffer<unsigned char> buffer;

    return static_cast<Value*>(
        static_cast<void*>(buffer.GetBuffer(sizeof(Value) * size)));
}

auto& SeqCntrUtils_GetSanitizeFuncs() {
    static std::unordered_map<size_t (*)(void* constext),
                              void (*)(Zeta_SeqCntr const* seq_cntr)>
        instance;
    return instance;
}

void SeqCntrUtils_AddSanitizeFunc(
    size_t (*GetWidth)(void* context),
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
    static std::unordered_map<size_t (*)(void* constext),
                              void (*)(Zeta_SeqCntr* seq_cntr)>
        instance;
    return instance;
}

void SeqCntrUtils_AddDestroyFunc(size_t (*GetWidth)(void* context),
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

template <typename Value>
struct FlattenValueBuffer {
    Value* beg;
    Value* end;
    bool free_after_use;

    ~FlattenValueBuffer() {
        if (free_after_use) { std::free(beg); }
    }
};

template <typename Iterator>
auto FlattenValue(Iterator beg, Iterator end) {
    using Value = typename std::iterator_traits<Iterator>::value_type;

    long long size{ end - beg };
    ZETA_DebugAssert(0 <= size);

    if (size == 0) {
        return FlattenValueBuffer<Value>{ nullptr, nullptr, false };
    }

    Value* v_beg = (Value*)std::malloc(sizeof(Value) * size);
    Value* v_end = beg + size;

    {
        auto iter{ beg };
        auto v_iter{ v_beg };

        for (; iter != end; ++iter, ++v_iter) {
            std::memcpy(v_beg, &*iter, sizeof(Value));
        }
    }

    return FlattenValueBuffer<Value>{ v_beg, v_end, true };
}

template <typename Value>
void SeqCntrUtils_Read(Zeta_SeqCntr* seq_cntr, size_t idx, Value* dst_beg,
                       Value* dst_end) {
    void* pos_cursor = ZETA_SeqCntr_AllocaCursor(seq_cntr);

    size_t size{ ZETA_SeqCntr_GetSize(seq_cntr) };

    size_t cnt = dst_end - dst_beg;

    ZETA_DebugAssert(cnt <= size);

    size_t cnt_a{ ZETA_GetMinOf(cnt, size - idx) };
    size_t cnt_b{ cnt - cnt_a };

    if (0 < cnt_a) {
        ZETA_SeqCntr_Access(seq_cntr, idx, pos_cursor, NULL);

        SeqCntrUtils_Sanitize(seq_cntr);

        ZETA_DebugAssert(ZETA_SeqCntr_Cursor_GetIdx(seq_cntr, pos_cursor) ==
                         idx);

        SeqCntrUtils_Sanitize(seq_cntr);

        ZETA_SeqCntr_Read(seq_cntr, pos_cursor, cnt_a, dst_beg, pos_cursor);

        SeqCntrUtils_Sanitize(seq_cntr);

        ZETA_DebugAssert(ZETA_SeqCntr_Cursor_GetIdx(seq_cntr, pos_cursor) ==
                         idx + cnt_a);

        SeqCntrUtils_Sanitize(seq_cntr);

        dst_beg += cnt_a;
        idx += (idx + cnt_a) % size;
    }

    if (0 < cnt_b) {
        ZETA_SeqCntr_Access(seq_cntr, 0, pos_cursor, NULL);

        SeqCntrUtils_Sanitize(seq_cntr);

        ZETA_DebugAssert(ZETA_SeqCntr_Cursor_GetIdx(seq_cntr, pos_cursor) == 0);

        ZETA_SeqCntr_Read(seq_cntr, pos_cursor, cnt_b, dst_beg, pos_cursor);

        SeqCntrUtils_Sanitize(seq_cntr);

        ZETA_DebugAssert(ZETA_SeqCntr_Cursor_GetIdx(seq_cntr, pos_cursor) ==
                         cnt_b);

        SeqCntrUtils_Sanitize(seq_cntr);
    }
}

template <typename Value>
void SeqCntrUtils_Write(Zeta_SeqCntr* seq_cntr, size_t idx,
                        Value const* src_beg, Value const* src_end) {
    void* pos_cursor = ZETA_SeqCntr_AllocaCursor(seq_cntr);

    size_t size{ ZETA_SeqCntr_GetSize(seq_cntr) };

    size_t cnt = src_end - src_beg;

    ZETA_DebugAssert(cnt <= size);

    size_t cnt_a{ ZETA_GetMinOf(cnt, size - idx) };
    size_t cnt_b{ cnt - cnt_a };

    if (0 < cnt_a) {
        ZETA_SeqCntr_Access(seq_cntr, idx, pos_cursor, NULL);

        SeqCntrUtils_Sanitize(seq_cntr);

        ZETA_DebugAssert(ZETA_SeqCntr_Cursor_GetIdx(seq_cntr, pos_cursor) ==
                         idx);

        SeqCntrUtils_Sanitize(seq_cntr);

        ZETA_SeqCntr_Write(seq_cntr, pos_cursor, cnt_a, src_beg, pos_cursor);

        SeqCntrUtils_Sanitize(seq_cntr);

        ZETA_DebugAssert(ZETA_SeqCntr_Cursor_GetIdx(seq_cntr, pos_cursor) ==
                         idx + cnt_a);

        SeqCntrUtils_Sanitize(seq_cntr);

        src_beg += cnt_a;
        idx += (idx + cnt_a) % size;
    }

    if (0 < cnt_b) {
        ZETA_SeqCntr_Access(seq_cntr, 0, pos_cursor, NULL);

        SeqCntrUtils_Sanitize(seq_cntr);

        ZETA_DebugAssert(ZETA_SeqCntr_Cursor_GetIdx(seq_cntr, pos_cursor) == 0);

        SeqCntrUtils_Sanitize(seq_cntr);

        ZETA_SeqCntr_Write(seq_cntr, pos_cursor, cnt_b, src_beg, pos_cursor);

        SeqCntrUtils_Sanitize(seq_cntr);

        ZETA_DebugAssert(ZETA_SeqCntr_Cursor_GetIdx(seq_cntr, pos_cursor) ==
                         cnt_b);

        SeqCntrUtils_Sanitize(seq_cntr);
    }
}

template <typename Value>
void SeqCntrUtils_PushL(Zeta_SeqCntr* seq_cntr, Value const* src_beg,
                        Value const* src_end) {
    ZETA_SeqCntr_PushL(seq_cntr, src_end - src_beg, NULL);

    SeqCntrUtils_Write(seq_cntr, 0, src_beg, src_end);
}

template <typename Value>
void SeqCntrUtils_PushR(Zeta_SeqCntr* seq_cntr, Value const* src_beg,
                        Value const* src_end) {
    size_t size{ ZETA_SeqCntr_GetSize(seq_cntr) };

    ZETA_SeqCntr_PushR(seq_cntr, src_end - src_beg, NULL);

    SeqCntrUtils_Sanitize(seq_cntr);

    SeqCntrUtils_Write(seq_cntr, size, src_beg, src_end);
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

template <typename Value>
void SeqCntrUtils_Insert(Zeta_SeqCntr* seq_cntr, size_t idx,
                         Value const* src_beg, Value const* src_end) {
    void* pos_cursor = ZETA_SeqCntr_AllocaCursor(seq_cntr);

    size_t cnt = src_end - src_beg;

    ZETA_SeqCntr_Access(seq_cntr, idx, pos_cursor, NULL);

    SeqCntrUtils_Sanitize(seq_cntr);

    ZETA_DebugAssert(ZETA_SeqCntr_Cursor_GetIdx(seq_cntr, pos_cursor) == idx);

    SeqCntrUtils_Sanitize(seq_cntr);

    ZETA_SeqCntr_Insert(seq_cntr, pos_cursor, cnt);

    SeqCntrUtils_Sanitize(seq_cntr);

    ZETA_DebugAssert(ZETA_SeqCntr_Cursor_GetIdx(seq_cntr, pos_cursor) == idx);

    SeqCntrUtils_Write(seq_cntr, idx, src_beg, src_end);
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

/*
void SeqCntrUtils_SyncRandomInsert(Zeta_SeqCntr* seq_cntr_a,
                                   Zeta_SeqCntr* seq_cntr_b) {
    //
}
*/
