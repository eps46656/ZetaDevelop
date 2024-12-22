#include "bin_heap.h"

#include "debugger.h"
#include "utils.h"

static void DownAdjust_(void* data, size_t width, size_t stride, size_t idx,
                        size_t size, void* src_elem, void* cmp_context,
                        int (*Compare)(void* cmp_context, void const* x,
                                       void const* y)) {
    ZETA_DebugAssert(data != NULL);

    ZETA_DebugAssert(0 < width);
    ZETA_DebugAssert(width <= stride);

    ZETA_DebugAssert(idx < size);

    if (size == 1) {
        if (src_elem != NULL) { Zeta_MemCopy(data, src_elem, width); }
        return;
    }

    for (size_t n_i = idx;;) {
        size_t l_i = n_i * 2 + 1;

        if (size <= l_i) { break; }

        void* n_ref = data + stride * n_i;

        void* l_ref = data + stride * l_i;

        void* min = src_elem == NULL ? n_ref : src_elem;

        if (Compare(cmp_context, l_ref, min) < 0) { min = l_ref; }

        size_t r_i = n_i * 2 + 2;

        if (r_i < size) {
            void* r_ref = data + stride * r_i;

            if (Compare(cmp_context, r_ref, min) < 0) { min = r_ref; }
        }

        if (src_elem == NULL) {
            if (min == n_ref) { break; }

            src_elem = __builtin_alloca_with_align(
                width, __CHAR_BIT__ * alignof(max_align_t));

            Zeta_MemCopy(src_elem, n_ref, width);
        }

        Zeta_MemCopy(n_ref, min, width);

        if (min == src_elem) { break; }

        n_i = min == l_ref ? l_i : r_i;
    }
}

void Zeta_BinHeap_Construct(void* data, size_t width, size_t stride,
                            size_t size, void* cmp_context,
                            int (*Compare)(void* cmp_context, void const* x,
                                           void const* y)) {
    ZETA_DebugAssert(data != NULL);

    ZETA_DebugAssert(0 < width);
    ZETA_DebugAssert(width <= stride);

    for (size_t i = size; 0 <= --i;) {
        DownAdjust_(data, width, stride, i, size, NULL, cmp_context, Compare);
    }
}

void Zeta_BinHeap_Push(void* data, size_t width, size_t stride, size_t size,
                       void const* src_elem, void* cmp_context,
                       int (*Compare)(void* cmp_context, void const* x,
                                      void const* y)) {
    ZETA_DebugAssert(data != NULL);

    ZETA_DebugAssert(0 < width);
    ZETA_DebugAssert(width <= stride);

    ZETA_DebugAssert(0 < stride);

    if (size == 1) {
        if (src_elem != NULL) { Zeta_MemCopy(data, src_elem, width); }
        return;
    }

    for (size_t n_i = size - 1;;) {
        if (n_i == 0) {
            if (src_elem != NULL) { Zeta_MemCopy(data, src_elem, width); }
        }

        size_t p_i = (n_i - 1) / 2;

        void* n_ref = data + stride * n_i;

        void* p_ref = data + stride * p_i;

        void const* cur = src_elem == NULL ? n_ref : src_elem;

        if (Compare(cmp_context, p_ref, cur) <= 0) {
            Zeta_MemCopy(n_ref, cur, width);
            break;
        }

        if (src_elem == NULL) {
            void* src_elem_ = __builtin_alloca_with_align(
                width, __CHAR_BIT__ * alignof(max_align_t));

            src_elem = src_elem_;

            Zeta_MemCopy(src_elem_, n_ref, width);
        }

        Zeta_MemCopy(n_ref, p_ref, width);

        n_i = p_i;
    }
}

void Zeta_BinHeap_Pop(void* data, size_t width, size_t stride, size_t size,
                      void* dst_elem, void* cmp_context,
                      int (*Compare)(void* cmp_context, void const* x,
                                     void const* y)) {
    ZETA_DebugAssert(data != NULL);

    ZETA_DebugAssert(0 < width);
    ZETA_DebugAssert(width <= stride);

    if (size <= 1) { return; }

    if (dst_elem != NULL) { Zeta_MemCopy(dst_elem, data, width); }

    void* last_ref = data + stride * (size - 1);

    size_t n_i = 0;

    for (;;) {
        size_t l_i = n_i * 2 + 1;

        if (size <= l_i) { break; }

        void* l_ref = data + stride * l_i;

        size_t r_i = n_i * 2 + 2;

        void* r_ref = data + stride * r_i;

        void* min = r_i < size && Compare(cmp_context, r_ref, l_ref) <= 0
                        ? r_ref
                        : l_ref;

        if (Compare(cmp_context, last_ref, min) <= 0) { break; }

        Zeta_MemSwap(data + stride * n_i, min, width);

        n_i = min == l_ref ? l_i : r_i;
    }

    Zeta_MemSwap(data + stride * n_i, last_ref, width);
}

void Zeta_BinHeap_Adjust(void* data, size_t width, size_t stride, size_t idx,
                         size_t size, void* cmp_context,
                         int (*Compare)(void* cmp_context, void const* x,
                                        void const* y)) {
    ZETA_DebugAssert(data != NULL);

    ZETA_DebugAssert(0 < width);
    ZETA_DebugAssert(width <= stride);

    ZETA_DebugAssert(idx < size);

    if (size == 1) { return; }

    DownAdjust_(data, width, stride, idx, size, NULL, cmp_context, Compare);

    Zeta_BinHeap_Push(data, width, stride, idx + 1, NULL, cmp_context, Compare);
}
