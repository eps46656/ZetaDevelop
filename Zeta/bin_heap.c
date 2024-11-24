#include "bin_heap.h"

#include "debugger.h"
#include "utils.h"

void Zeta_BinHeap_Construct(void* data, size_t width, size_t stride,
                            size_t size, void* cmp_context,
                            int (*Compare)(void* cmp_context, void const* x,
                                           void const* y)) {
    ZETA_DebugAssert(data != NULL);

    ZETA_DebugAssert(0 < width);
    ZETA_DebugAssert(width <= stride);

    for (size_t i = size; 0 <= --i;) {
        size_t n_i = i;

        for (;;) {
            size_t l_i = n_i * 2 + 1;

            if (size <= l_i) { break; }

            void* n_ref = data + stride * n_i;

            void* min = n_ref;

            void* l_ref = data + stride * l_i;

            if (Compare(cmp_context, l_ref, min) < 0) { min = l_ref; }

            size_t r_i = n_i * 2 + 2;

            if (r_i < size) {
                void* r_ref = data + stride * r_i;

                if (Compare(cmp_context, r_ref, min) < 0) { min = r_ref; }
            }

            if (n_ref == min) { break; }

            Zeta_MemSwap(n_ref, min, width);

            n_i = min == l_ref ? l_i : r_i;
        }
    }
}

void Zeta_BinHeap_Push(void* data, size_t width, size_t stride, size_t size,
                       void const* src_elem, void* cmp_context,
                       int (*Compare)(void* cmp_context, void const* x,
                                      void const* y)) {
    ZETA_DebugAssert(data != NULL);

    ZETA_DebugAssert(0 < width);
    ZETA_DebugAssert(width <= stride);

    if (size == 0) {
        Zeta_MemCopy(data, src_elem, width);
        return;
    }

    size_t n_i = size;

    while (0 < n_i) {
        size_t p_i = (n_i - 1) / 2;

        void* p_ref = data + stride * p_i;

        if (Compare(cmp_context, p_ref, src_elem) <= 0) { break; }

        Zeta_MemCopy(data + stride * n_i, p_ref, width);

        n_i = p_i;
    }

    Zeta_MemCopy(data + stride * n_i, src_elem, width);
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
