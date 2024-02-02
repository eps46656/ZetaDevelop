#include "Algorithm.h"

static void Zeta_Insertion_(Zeta_Vector* vec, size_t i, void* cmper_context,
                            int (*cmper)(void* cmper_context, const void* x,
                                         const void* y)) {
    const size_t max_tmp_width = 64;

    void* vec_context = vec->context;

    size_t width = vec->GetWidth(vec_context);

    void* ref = vec->Access(vec_context, i);
    void* prev_ref = vec->Access(vec_context, i - 1);

    if (max_tmp_width < width) {
        while (0 < cmper(cmper_context, prev_ref, ref)) {
            Zeta_MemSwap(width, prev_ref, ref);

            --i;
            ref = prev_ref;
            prev_ref = vec->Access(vec_context, i - 1);
        }

        return;
    }

    if (cmper(cmper_context, prev_ref, ref) <= 0) { return; }

    byte_t tmp[max_tmp_width];

    Zeta_MemCopy(width, tmp, ref);
    Zeta_MemCopy(width, ref, prev_ref);

    for (;;) {
        --i;
        ref = prev_ref;
        prev_ref = vec->Access(vec_context, i - 1);

        if (cmper(cmper_context, prev_ref, tmp) <= 0) {
            Zeta_MemCopy(width, ref, tmp);
            break;
        }

        Zeta_MemCopy(width, ref, prev_ref);
    }
}

static void Zeta_InsertionSort_(Zeta_Vector* vec, size_t beg, size_t end,
                                void* cmper_context,
                                int (*cmper)(void* cmper_context, const void* x,
                                             const void* y)) {
    void* vec_context = vec->context;

    size_t width = vec->GetWidth(vec_context);

    void* min_addr = vec->Access(vec_context, beg);

    for (size_t i = beg + 1; i < end; ++i) {
        void* cur_addr = vec->Access(vec_context, i);

        if (cmper(cmper_context, cur_addr, min_addr) < 0) {
            min_addr = cur_addr;
        }
    }

    Zeta_MemSwap(width, vec->Access(vec_context, beg), min_addr);

    for (size_t i = beg + 2; i < end; ++i) {
        Zeta_Insertion_(vec, i, cmper_context, cmper);
    }
}

static const size_t Zeta_InsertionSort_Threshold = 32;

/*
static void Zeta_ShellSort_(Zeta_Vector* vec,  size_t beg,
                            size_t end, void* cmper_context,
                            int (*cmper)(void* cmper_context, const void* x,
                                         const void* y)) {
    size_t gaps[] = {
        1,
        9,
        34,
        182,
        836,
        4025,
        19001,
        90358,
        428481,
        2034035,
        9651787,
        45806244,
        217378076,
        1031612713,
        4895689348,
        23233383508,
        110258103799,
        523249458736,
        2483172990911,
        11784338112247,
        55924666344750,
        265400424550505,
        1259504788813791,
        5977203382848010,
    };

    size_t size = (end - beg) / stride;

    size_t gap_i;

    {
        size_t lb = 0;
        size_t rb = sizeof(gaps) / sizeof(gaps[0]) - 1;

        while (lb < rb) {
            size_t mb = (lb + rb + 1) / 2;

            if (size * 4 <= gaps[mb]) {
                rb = mb - 1;
            } else {
                lb = mb;
            }
        }

        gap_i = lb;
    }

    for (; 0 <= gap_i; --gap_i) {
        size_t gap = gaps[gap_i];

        for (size_t g = 0; g < gap; ++g) {
            size_t subgroup_end =
                beg +  gap * ((size - g + gap - 1) / gap);

            // ZETA_DEBUG_ASSERT(subgroup_last < end);

            size_t _ = (subgroup_end - end) / stride;

            ZETA_DEBUG_ASSERT(0 <= _);
            ZETA_DEBUG_ASSERT(_ < gap);

            Zeta_InsertionSort_(vec, width,  gap, beg +  g,
                                subgroup_end, cmper_context, cmper);
        }
    }
}
*/

static size_t Zeta_Partition_(Zeta_Vector* vec, size_t beg, size_t end,
                              void* cmper_context,
                              int (*cmper)(void* cmper_context, const void* x,
                                           const void* y)) {
    void* vec_context = vec->context;

    size_t width = vec->GetWidth(vec_context);

    void* pivot_ref = vec->Access(vec_context, beg);

    size_t i = beg + 1;
    size_t j = end;

    for (;;) {
        while (cmper(cmper_context, vec->Access(vec_context, i), pivot_ref) <
               0) {
            ++i;
        }

        do {
            --j;
        } while (cmper(cmper_context, pivot_ref, vec->Access(vec_context, j)) <
                 0);

        if (j <= i) { break; }

        Zeta_MemSwap(width, vec->Access(vec_context, i),
                     vec->Access(vec_context, j));

        ++i;
    }

    --i;

    Zeta_MemSwap(width, vec->Access(vec_context, beg),
                 vec->Access(vec_context, i));

    return i;
}

static size_t Zeta_SimplePartition_(
    Zeta_Vector* vec, size_t beg, size_t end, void* cmper_context,
    int (*cmper)(void* cmper_context, const void* x, const void* y)) {
    size_t size = end - beg;

    if (size <= Zeta_InsertionSort_Threshold) {
        Zeta_InsertionSort_(vec, beg, end, cmper_context, cmper);
        return beg + size / 2;
    }

    void* vec_context = vec->context;

    size_t width = vec->GetWidth(vec_context);

    void* a = vec->Access(vec_context, beg);
    void* b = vec->Access(vec_context, beg + 1);
    void* c = vec->Access(vec_context, end - 1);

    if (0 < cmper(cmper_context, a, b)) { Zeta_MemSwap(width, a, b); }

    if (0 < cmper(cmper_context, b, c)) {
        Zeta_MemSwap(width, b, c);
        if (0 < cmper(cmper_context, a, b)) { Zeta_MemSwap(width, a, b); }
    }

    return Zeta_Partition_(vec, beg + 1, end - 1, cmper_context, cmper);
}

static void Zeta_KthElement_(Zeta_Vector* vec, size_t beg, size_t mid,
                             size_t end, void* cmper_context,
                             int (*cmper)(void* cmper_context, const void* x,
                                          const void* y));

static size_t Zeta_PrettyPartition_(
    Zeta_Vector* vec, size_t beg, size_t end, void* cmper_context,
    int (*cmper)(void* cmper_context, const void* x, const void* y)) {
    size_t size = end - beg;

    if (size <= Zeta_InsertionSort_Threshold) {
        Zeta_InsertionSort_(vec, beg, end, cmper_context, cmper);
        return beg + size / 2;
    }

    void* vec_context = vec->context;

    size_t width = vec->GetWidth(vec_context);

    const size_t group_size = 5;

    size_t group_num = size / group_size;

    for (size_t i = 0; i < group_num; ++i) {
        size_t g_beg = beg + group_size * i;

        Zeta_InsertionSort_(vec, g_beg, g_beg + group_size, cmper_context,
                            cmper);

        Zeta_MemSwap(width, vec->Access(vec_context, beg + i),
                     vec->Access(vec_context, g_beg + group_size / 2));
    }

    size_t pivot = beg + group_num / 2;

    Zeta_KthElement_(vec, beg, pivot, beg + group_num, cmper_context, cmper);

    Zeta_MemSwap(width, vec->Access(vec_context, beg),
                 vec->Access(vec_context, pivot));

    return Zeta_Partition_(vec, beg, end, cmper_context, cmper);
}

static void Zeta_KthElement_(Zeta_Vector* vec, size_t beg, size_t mid,
                             size_t end, void* cmper_context,
                             int (*cmper)(void* cmper_context, const void* x,
                                          const void* y)) {
    size_t size = end - beg;
    size_t chance = size * 4;

    for (;;) {
        size = end - beg;

        if (size <= Zeta_InsertionSort_Threshold) {
            Zeta_InsertionSort_(vec, beg, end, cmper_context, cmper);
            return;
        }

        size_t pivot;

        if (0 < chance) {
            pivot = Zeta_SimplePartition_(vec, beg, end, cmper_context, cmper);
            chance -= size;
        } else {
            pivot = Zeta_PrettyPartition_(vec, beg, end, cmper_context, cmper);
        }

        if (pivot < mid) {
            beg = pivot + 1;
            continue;
        }

        if (mid < pivot) {
            end = pivot;
            continue;
        }

        return;
    }
}

void Zeta_KthElement(Zeta_Vector* vec, size_t beg, size_t mid, size_t end,
                     void* cmper_context,
                     int (*cmper)(void* cmper_context, const void* x,
                                  const void* y)) {
    ZETA_DEBUG_ASSERT(vec != NULL);
    ZETA_DEBUG_ASSERT(vec->GetSize != NULL);
    ZETA_DEBUG_ASSERT(vec->Access != NULL);

    void* vec_context = vec->context;

    size_t width = vec->GetWidth(vec_context);
    ZETA_DEBUG_ASSERT(0 < width);

    size_t size = vec->GetSize(vec_context);

    ZETA_DEBUG_ASSERT(0 <= beg);
    ZETA_DEBUG_ASSERT(beg <= mid);
    ZETA_DEBUG_ASSERT(mid <= end);
    ZETA_DEBUG_ASSERT(end <= size);

    if (mid != end) {
        Zeta_KthElement_(vec, beg, mid, end, cmper_context, cmper);
    }
}

void Zeta_Sort_(size_t chance, Zeta_Vector* vec, size_t beg, size_t end,
                void* cmper_context,
                int (*cmper)(void* cmper_context, const void* x,
                             const void* y)) {
    for (;;) {
        size_t size = end - beg;

        if (size <= Zeta_InsertionSort_Threshold) {
            Zeta_InsertionSort_(vec, beg, end, cmper_context, cmper);
            return;
        }

        size_t pivot;

        if (0 < chance) {
            pivot = Zeta_SimplePartition_(vec, beg, end, cmper_context, cmper);
            chance = chance / 2;
        } else {
            pivot = Zeta_PrettyPartition_(vec, beg, end, cmper_context, cmper);
        }

        size_t size_l = pivot - beg;
        size_t size_r = end - pivot - 1;

        if (size_l <= size_r) {
            Zeta_Sort_(chance, vec, beg, pivot, cmper_context, cmper);
            beg = pivot + 1;
        } else {
            Zeta_Sort_(chance, vec, pivot + 1, end, cmper_context, cmper);
            end = pivot;
        }
    }
}

void Zeta_Sort(Zeta_Vector* vec, size_t beg, size_t end, void* cmper_context,
               int (*cmper)(void* cmper_context, const void* x,
                            const void* y)) {
    ZETA_DEBUG_ASSERT(vec != NULL);
    ZETA_DEBUG_ASSERT(vec->GetSize != NULL);
    ZETA_DEBUG_ASSERT(vec->Access != NULL);

    void* vec_context = vec->context;

    size_t width = vec->GetWidth(vec_context);
    ZETA_DEBUG_ASSERT(0 < width);

    size_t size = vec->GetSize(vec_context);

    ZETA_DEBUG_ASSERT(0 <= beg)
    ZETA_DEBUG_ASSERT(beg <= end);
    ZETA_DEBUG_ASSERT(end <= size);

    Zeta_Sort_(size * 4, vec, beg, end, cmper_context, cmper);
}
