#include "SegUtils.h"

#include "Debugger.h"
#include "utils.h"

void SegShoveL(Zeta_CircularArray* l_ca, Zeta_CircularArray* r_ca,
               size_t rl_cnt, size_t ins_cnt, size_t shove_cnt) {
    ZETA_DebugAssert(l_ca != NULL);
    ZETA_DebugAssert(r_ca != NULL);

    ZETA_DebugAssert(l_ca->width == r_ca->width);
    ZETA_DebugAssert(l_ca->stride == r_ca->stride);

    ZETA_DebugAssert(shove_cnt <= l_ca->capacity - l_ca->size);
    ZETA_DebugAssert(shove_cnt <= r_ca->size + ins_cnt);
    ZETA_DebugAssert(r_ca->size + ins_cnt - shove_cnt <= r_ca->capacity);

    size_t cnt_a = ZETA_GetMinOf(rl_cnt, shove_cnt);
    size_t cnt_b = ZETA_GetMinOf(ins_cnt, shove_cnt - cnt_a);
    size_t cnt_c = shove_cnt - cnt_a - cnt_b;

    size_t l_size = l_ca->size;

    Zeta_CircularArray_PushR(l_ca, shove_cnt, NULL);

    if (0 < cnt_a) { Zeta_CircularArray_Assign(l_ca, r_ca, l_size, 0, cnt_a); }

    if (0 < cnt_c) {
        Zeta_CircularArray_Assign(l_ca, r_ca, l_size + cnt_a + cnt_b, cnt_a,
                                  cnt_c);
    }

    Zeta_CircularArray_PopL(r_ca, cnt_a + cnt_c);

    if (0 < ins_cnt - cnt_b) {
        Zeta_CircularArray_Cursor r_ca_cursor;

        Zeta_CircularArray_Access(r_ca, rl_cnt - cnt_a, &r_ca_cursor, NULL);

        Zeta_CircularArray_Insert(r_ca, &r_ca_cursor, ins_cnt - cnt_b);
    }
}

void SegShoveR(Zeta_CircularArray* l_ca, Zeta_CircularArray* r_ca,
               size_t lr_cnt, size_t ins_cnt, size_t shove_cnt) {
    ZETA_DebugAssert(l_ca != NULL);
    ZETA_DebugAssert(r_ca != NULL);

    ZETA_DebugAssert(l_ca->width == r_ca->width);
    ZETA_DebugAssert(l_ca->stride == r_ca->stride);

    ZETA_DebugAssert(r_ca->size + shove_cnt <= r_ca->capacity);
    ZETA_DebugAssert(shove_cnt <= l_ca->size + ins_cnt);

    size_t cnt_a = ZETA_GetMinOf(lr_cnt, shove_cnt);
    size_t cnt_b = ZETA_GetMinOf(ins_cnt, shove_cnt - cnt_a);
    size_t cnt_c = shove_cnt - cnt_a - cnt_b;

    size_t l_size = l_ca->size;

    Zeta_CircularArray_PushL(r_ca, shove_cnt, NULL);

    if (0 < cnt_c) {
        Zeta_CircularArray_Assign(r_ca, l_ca, 0, l_size - cnt_a - cnt_c, cnt_c);
    }

    if (0 < cnt_a) {
        Zeta_CircularArray_Assign(r_ca, l_ca, cnt_c + cnt_b, l_size - cnt_a,
                                  cnt_a);
    }

    Zeta_CircularArray_PopR(l_ca, cnt_c + cnt_a);

    if (0 < ins_cnt - cnt_b) {
        Zeta_CircularArray_Cursor l_ca_cursor;

        Zeta_CircularArray_Access(l_ca, l_size - lr_cnt, &l_ca_cursor, NULL);

        Zeta_CircularArray_Insert(l_ca, &l_ca_cursor, ins_cnt - cnt_b);
    }
}
