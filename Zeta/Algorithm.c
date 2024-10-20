#include "Algorithm.h"

#include "Debugger.h"

#define INSERTION_TH (8)

static void Insertion_(unsigned char* data, size_t width, size_t stride,
                       void* cmp_context,
                       int (*Compare)(void* context, void const* x,
                                      void const* y)) {
    unsigned char* pre_data = data - stride;

    if (Compare(cmp_context, pre_data, data) <= 0) { return; }

    void* tmp =
        __builtin_alloca_with_align(width, CHAR_BIT * alignof(max_align_t));

    Zeta_MemCopy(tmp, data, width);

    do {
        Zeta_MemCopy(data, pre_data, width);

        data = pre_data;
        pre_data -= stride;
    } while (Compare(cmp_context, tmp, pre_data) < 0);

    Zeta_MemCopy(data, tmp, width);
}

static void InsertionSort_(unsigned char* data, size_t width, size_t stride,
                           size_t size, void* cmp_context,
                           int (*Compare)(void* context, void const* x,
                                          void const* y)) {
    if (size <= 1) { return; }

    unsigned char* end = data + stride * size;

    unsigned char* min = data;

    for (unsigned char* i = data + stride; i < end; i += stride) {
        if (Compare(cmp_context, i, min) < 0) { min = i; }
    }

    Zeta_MemSwap(data, min, width);

    for (unsigned char* i = data + stride * 2; i < end; i += stride) {
        Insertion_(i, width, stride, cmp_context, Compare);
    }
}

static unsigned char* Partition_(unsigned char* data, size_t width,
                                 size_t stride, size_t size, void* cmp_context,
                                 int (*Compare)(void* cmp_context,
                                                void const* x, void const* y)) {
    unsigned char* i = data;
    unsigned char* j = data + stride * size;

    int i_cmp = -1;
    int j_cmp = 1;

    for (;;) {
        while (i_cmp < 0) { i_cmp = Compare(cmp_context, i += stride, data); }
        while (0 < j_cmp) { j_cmp = Compare(cmp_context, j -= stride, data); }

        if (j <= i) { break; }

        if (i_cmp != 0 || j_cmp != 0) { Zeta_MemSwap(i, j, width); }

        i_cmp = Compare(cmp_context, i += stride, data);
        j_cmp = Compare(cmp_context, j -= stride, data);
    }

    Zeta_MemSwap(data, j, width);

    return j;
}

static unsigned char* SimplePartition_(
    unsigned char* data, size_t width, size_t stride, size_t size,
    void* cmp_context,
    int (*Compare)(void* cmp_context, void const* x, void const* y)) {
    unsigned char* a = data + stride * (size * 1 / 4);
    unsigned char* b = data + stride * (size * 2 / 4);
    unsigned char* c = data + stride * (size * 3 / 4);

    if (Compare(cmp_context, b, a) < 0) { ZETA_Swap(a, b); }

    if (Compare(cmp_context, c, b) < 0) {
        ZETA_Swap(b, c);
        if (Compare(cmp_context, b, a) < 0) { ZETA_Swap(a, b); }
    }

    ZETA_DebugAssert(Compare(cmp_context, a, b) <= 0);
    ZETA_DebugAssert(Compare(cmp_context, b, c) <= 0);

    Zeta_MemSwap(data, b, width);

    return Partition_(data, width, stride, size, cmp_context, Compare);
}

static void PrettyKthElement_(unsigned char* data, size_t width, size_t stride,
                              size_t mid, size_t size, void* cmp_context,
                              int (*Compare)(void* cmp_context, void const* x,
                                             void const* y));

static unsigned char* PrettyPartition_(
    unsigned char* data, size_t width, size_t stride, size_t size,
    void* cmp_context,
    int (*Compare)(void* cmp_context, void const* x, void const* y)) {
    if (size <= INSERTION_TH) {
        InsertionSort_(data, width, stride, size, cmp_context, Compare);
        return data + stride * (size / 2);
    }

#define GROUP_SIZE 7

    size_t group_cnt = size / GROUP_SIZE;

    for (size_t i = 0; i < group_cnt; ++i) {
        InsertionSort_(data + stride * i, width, stride * group_cnt, GROUP_SIZE,
                       cmp_context, Compare);
    }

    unsigned char* mid = data + stride * group_cnt * (GROUP_SIZE / 2);

    PrettyKthElement_(mid, width, stride, group_cnt / 2, group_cnt, cmp_context,
                      Compare);

    Zeta_MemSwap(data, mid + stride * (group_cnt / 2), width);

    return Partition_(data, width, stride, size, cmp_context, Compare);
}

static void PrettyKthElement_(unsigned char* data, size_t width, size_t stride,
                              size_t mid, size_t size, void* cmp_context,
                              int (*Compare)(void* cmp_context, void const* x,
                                             void const* y)) {
    for (;;) {
        if (size <= INSERTION_TH) {
            InsertionSort_(data, width, stride, size, cmp_context, Compare);
            return;
        }

        unsigned char* pivot =
            PrettyPartition_(data, width, stride, size, cmp_context, Compare);

        size_t pivot_idx = (pivot - data) / stride;

        if (pivot_idx < mid) {
            data = pivot + stride;
            mid -= pivot_idx + 1;
            size -= pivot_idx + 1;
            continue;
        }

        if (mid < pivot_idx) {
            size = pivot_idx;
            continue;
        }

        return;
    }
}

static void KthElement_(unsigned char* data, size_t width, size_t stride,
                        size_t mid, size_t size, void* cmp_context,
                        int (*Compare)(void* cmp_context, void const* x,
                                       void const* y)) {
    size_t chance = size * 4;

    for (;;) {
        if (size <= INSERTION_TH) {
            InsertionSort_(data, width, stride, size, cmp_context, Compare);
            return;
        }

        unsigned char* pivot;

        if (0 < chance) {
            pivot = SimplePartition_(data, width, stride, size, cmp_context,
                                     Compare);
            chance -= size;
        } else {
            pivot = PrettyPartition_(data, width, stride, size, cmp_context,
                                     Compare);
        }

        size_t pivot_idx = (pivot - data) / stride;

        if (pivot_idx < mid) {
            data = pivot + stride;
            mid -= pivot_idx + 1;
            continue;
        }

        if (mid < pivot_idx) {
            size = pivot_idx;
            continue;
        }

        return;
    }
}

void Zeta_KthElement(void* data, size_t width, size_t stride, size_t mid,
                     size_t size, void* cmp_context,
                     int (*Compare)(void* cmp_context, void const* x,
                                    void const* y)) {
    ZETA_DebugAssert(data != NULL);
    ZETA_DebugAssert(0 < width);
    ZETA_DebugAssert(width <= stride);

    ZETA_DebugAssert(mid <= size);

    ZETA_DebugAssert(Compare != NULL);

    if (mid != size) {
        KthElement_(data, width, stride, mid, size, cmp_context, Compare);
    }
}

void Sort_(size_t credit, unsigned char* data, size_t width, size_t stride,
           size_t size, void* cmp_context,
           int (*Compare)(void* cmp_context, void const* x, void const* y)) {
    for (;;) {
        if (size <= INSERTION_TH) {
            InsertionSort_(data, width, stride, size, cmp_context, Compare);
            return;
        }

        unsigned char* pivot;

        if (0 < credit) {
            pivot = SimplePartition_(data, width, stride, size, cmp_context,
                                     Compare);
            credit = credit / 2;
        } else {
            pivot = PrettyPartition_(data, width, stride, size, cmp_context,
                                     Compare);
        }

        unsigned char* r_data = pivot + stride;

        size_t l_size = (pivot - data) / stride;
        size_t r_size = size - 1 - l_size;

        if (l_size < r_size) {
            Sort_(credit, data, width, stride, l_size, cmp_context, Compare);
            data = r_data;
            size = r_size;
        } else {
            Sort_(credit, r_data, width, stride, r_size, cmp_context, Compare);
            size = l_size;
        }
    }
}

void Zeta_Sort(void* data, size_t width, size_t stride, size_t size,
               void* cmp_context,
               int (*Compare)(void* cmp_context, void const* x,
                              void const* y)) {
    ZETA_DebugAssert(data != NULL);
    ZETA_DebugAssert(0 < width);
    ZETA_DebugAssert(width <= stride);

    Sort_(size, data, width, stride, size, cmp_context, Compare);
}
