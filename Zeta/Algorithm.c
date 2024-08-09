#include "Algorithm.h"

#include "Debugger.h"

static void Insertion_(unsigned char* data, size_t width, size_t stride,
                       void* cmper_context,
                       int (*cmper)(void* context, void const* x,
                                    void const* y)) {
    unsigned char* pre_data = data - stride;

    if (cmper(cmper_context, pre_data, data) <= 0) { return; }

    void* tmp = __builtin_alloca(width);
    Zeta_MemCopy(tmp, data, width);

    do {
        Zeta_MemCopy(data, pre_data, width);

        data = pre_data;
        pre_data -= stride;
    } while (0 < cmper(cmper_context, pre_data, data));

    Zeta_MemCopy(data, tmp, width);
}

static void InsertionSort_(unsigned char* data, size_t width, size_t stride,
                           size_t size, void* cmper_context,
                           int (*cmper)(void* context, void const* x,
                                        void const* y)) {
    if (size <= 1) { return; }

    unsigned char* min = data;

    for (size_t i = 1; i < size; ++i) {
        if (cmper(cmper_context, data + stride * i, data) < 0) {
            min = data + stride * i;
        }
    }

    Zeta_MemSwap(data, min, width);

    for (size_t i = 2; i < size; ++i) {
        Insertion_(data + stride * i, width, stride, cmper_context, cmper);
    }
}

static size_t const InsertionSort_Threshold = 32;

static unsigned char* Partition_(unsigned char* data, size_t width,
                                 size_t stride, size_t size,
                                 void* cmper_context,
                                 int (*cmper)(void* cmper_context,
                                              void const* x, void const* y)) {
    unsigned char* i = data + stride;
    unsigned char* j = data + stride * size;

    for (;;) {
        while (cmper(cmper_context, i, data) < 0) { i += stride; }

        do { j -= stride; } while (cmper(cmper_context, data, j) < 0);

        if (j <= i) { break; }

        Zeta_MemSwap(i, j, width);

        i += stride;
    }

    i -= stride;

    Zeta_MemSwap(data, i, width);

    return i;
}

static unsigned char* SimplePartition_(
    unsigned char* data, size_t width, size_t stride, size_t size,
    void* cmper_context,
    int (*cmper)(void* cmper_context, void const* x, void const* y)) {
    if (size <= InsertionSort_Threshold) {
        InsertionSort_(data, width, stride, size, cmper_context, cmper);
        return data + stride * (size / 2);
    }

    void* a = data;
    void* b = data + stride;
    void* c = data + stride * (size - 1);

    if (0 < cmper(cmper_context, a, b)) { ZETA_Swap(a, b); }

    if (0 < cmper(cmper_context, b, c)) {
        ZETA_Swap(b, c);
        if (0 < cmper(cmper_context, a, b)) { ZETA_Swap(a, b); }
    }

    Zeta_MemSwap(data, a, width);

    return Partition_(data, width, stride, size, cmper_context, cmper);
}

static void KthElement_(unsigned char* data, size_t width, size_t stride,
                        size_t mid, size_t size, void* cmper_context,
                        int (*cmper)(void* cmper_context, void const* x,
                                     void const* y));

static unsigned char* PrettyPartition_(
    unsigned char* data, size_t width, size_t stride, size_t size,
    void* cmper_context,
    int (*cmper)(void* cmper_context, void const* x, void const* y)) {
    if (size <= InsertionSort_Threshold) {
        InsertionSort_(data, width, stride, size, cmper_context, cmper);
        return data + stride * (size / 2);
    }

    size_t const group_size = 5;

    size_t group_cnt = size / group_size;

    for (size_t i = 0; i < group_cnt; ++i) {
        unsigned char* g_beg = data + stride * i;

        InsertionSort_(g_beg, width, stride * group_cnt, group_size,
                       cmper_context, cmper);
    }

    KthElement_(data, width, stride, group_cnt / 2, group_cnt, cmper_context,
                cmper);

    Zeta_MemSwap(data, data + stride * (group_cnt / 2), width);

    return Partition_(data, width, stride, size, cmper_context, cmper);
}

static void KthElement_(unsigned char* data, size_t width, size_t stride,
                        size_t mid, size_t size, void* cmper_context,
                        int (*cmper)(void* cmper_context, void const* x,
                                     void const* y)) {
    size_t chance = size * 4;

    for (;;) {
        if (size <= InsertionSort_Threshold) {
            InsertionSort_(data, width, stride, size, cmper_context, cmper);
            return;
        }

        size_t pivot;

        if (0 < chance) {
            pivot = (SimplePartition_(data, width, stride, size, cmper_context,
                                      cmper) -
                     data) /
                    stride;
            chance -= size;
        } else {
            pivot = (PrettyPartition_(data, width, stride, size, cmper_context,
                                      cmper) -
                     data) /
                    stride;
        }

        if (pivot < mid) {
            data = data + stride * (pivot + 1);
            mid -= pivot + 1;
            continue;
        }

        if (mid < pivot) {
            size = pivot;
            continue;
        }

        return;
    }
}

void Zeta_KthElement(void* data, size_t width, size_t stride, size_t mid,
                     size_t size, void* cmper_context,
                     int (*cmper)(void* cmper_context, void const* x,
                                  void const* y)) {
    ZETA_DebugAssert(data != NULL);
    ZETA_DebugAssert(0 < width);
    ZETA_DebugAssert(width <= stride);

    ZETA_DebugAssert(mid <= size);

    ZETA_DebugAssert(cmper != NULL);

    if (mid != size) {
        KthElement_(data, width, stride, mid, size, cmper_context, cmper);
    }
}

void Sort_(size_t chance, unsigned char* data, size_t width, size_t stride,
           size_t size, void* cmper_context,
           int (*cmper)(void* cmper_context, void const* x, void const* y)) {
    for (;;) {
        if (size <= InsertionSort_Threshold) {
            InsertionSort_(data, width, stride, size, cmper_context, cmper);
            return;
        }

        size_t pivot;

        if (0 < chance) {
            pivot = (SimplePartition_(data, width, stride, size, cmper_context,
                                      cmper) -
                     data) /
                    stride;
            chance = chance / 2;
        } else {
            pivot = (PrettyPartition_(data, width, stride, size, cmper_context,
                                      cmper) -
                     data) /
                    stride;
        }

        unsigned char* r_data = data + stride * (pivot + 1);

        size_t l_size = pivot;
        size_t r_size = size - 1 - pivot;

        if (l_size <= r_size) {
            Sort_(chance, data, width, stride, l_size, cmper_context, cmper);
            data = r_data;
            size = r_size;
        } else {
            Sort_(chance, r_data, width, stride, r_size, cmper_context, cmper);
            size = l_size;
        }
    }
}

void Zeta_Sort(void* data, size_t width, size_t stride, size_t size,
               void* cmper_context,
               int (*cmper)(void* cmper_context, void const* x,
                            void const* y)) {
    ZETA_DebugAssert(data != NULL);
    ZETA_DebugAssert(0 < width);
    ZETA_DebugAssert(width <= stride);

    Sort_(size * 4, data, width, stride, size, cmper_context, cmper);
}
