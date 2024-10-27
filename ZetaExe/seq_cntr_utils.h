#pragma once

#include <debugger.h>
#include <seq_cntr.h>

#include <unordered_map>

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
void SeqCntrUtils_Write(Zeta_SeqCntr* seq_cntr, size_t idx,
                        Value const* src_beg, Value const* src_end) {
    void* pos_cursor = ZETA_SeqCntr_AllocaCursor(seq_cntr);

    size_t size{ ZETA_SeqCntr_GetSize(seq_cntr) };

    size_t cnt = src_end - src_beg;
    size_t cnt_a{ ZETA_GetMinOf(cnt, size - idx) };
    size_t cnt_b{ cnt - cnt_a };

    if (0 < cnt_a) {
        ZETA_SeqCntr_Access(seq_cntr, idx, pos_cursor, NULL);

        ZETA_DebugAssert(ZETA_SeqCntr_Cursor_GetIdx(seq_cntr, pos_cursor) ==
                         idx);

        ZETA_SeqCntr_Write(seq_cntr, pos_cursor, cnt_a, src_beg, pos_cursor);

        SeqCntrUtils_Sanitize(seq_cntr);

        ZETA_DebugAssert(ZETA_SeqCntr_Cursor_GetIdx(seq_cntr, pos_cursor) ==
                         idx + cnt_a);

        src_beg += cnt_a;
        idx += (idx + cnt_a) % size;
    }

    if (0 < cnt_b) {
        ZETA_SeqCntr_Access(seq_cntr, 0, pos_cursor, NULL);

        ZETA_DebugAssert(ZETA_SeqCntr_Cursor_GetIdx(seq_cntr, pos_cursor) == 0);

        ZETA_SeqCntr_Write(seq_cntr, pos_cursor, cnt_b, src_beg, pos_cursor);

        SeqCntrUtils_Sanitize(seq_cntr);

        ZETA_DebugAssert(ZETA_SeqCntr_Cursor_GetIdx(seq_cntr, pos_cursor) ==
                         cnt_b);
    }
}

template <typename Value>
void SeqCntrUtils_Insert(Zeta_SeqCntr* seq_cntr, size_t idx,
                         Value const* src_beg, Value const* src_end) {
    void* pos_cursor = ZETA_SeqCntr_AllocaCursor(seq_cntr);

    size_t cnt = src_end - src_beg;

    ZETA_SeqCntr_Access(seq_cntr, idx, pos_cursor, NULL);

    ZETA_DebugAssert(ZETA_SeqCntr_Cursor_GetIdx(seq_cntr, pos_cursor) == idx);

    ZETA_SeqCntr_Insert(seq_cntr, pos_cursor, cnt);

    SeqCntrUtils_Sanitize(seq_cntr);

    ZETA_DebugAssert(ZETA_SeqCntr_Cursor_GetIdx(seq_cntr, pos_cursor) == idx);

    SeqCntrUtils_Write(seq_cntr, idx, src_beg, src_end);
}
