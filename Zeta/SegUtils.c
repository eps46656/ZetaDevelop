#include "SegUtils.h"

#include "Debugger.h"
#include "utils.h"

void SegShoveL(Zeta_CircularArray* l_ca, Zeta_CircularArray* r_ca,
               size_t rl_cnt, size_t ins_cnt, size_t shove_cnt) {
    ZETA_DebugAssert(l_ca != NULL);
    ZETA_DebugAssert(r_ca != NULL);

    ZETA_DebugAssert(shove_cnt <= l_ca->capacity - l_ca->size);
    ZETA_DebugAssert(shove_cnt <= r_ca->size + ins_cnt);
    ZETA_DebugAssert(r_ca->size + ins_cnt - shove_cnt <= r_ca->capacity);

    size_t width = l_ca->width;
    ZETA_DebugAssert(width == r_ca->width);

    size_t cnt_a = ZETA_GetMinOf(rl_cnt, shove_cnt);
    size_t cnt_b = ZETA_GetMinOf(ins_cnt, shove_cnt - cnt_a);
    size_t cnt_c = shove_cnt - cnt_a - cnt_b;

    size_t l_beg = l_ca->size;
    size_t r_beg = 0;

    Zeta_CircularArray_Cursor l_ca_cursor;
    Zeta_CircularArray_Cursor r_ca_cursor;

    Zeta_CircularArray_GetRBCursor(l_ca, &l_ca_cursor);

    Zeta_CircularArray_Insert(l_ca, &l_ca_cursor, shove_cnt);

    Zeta_CircularArray_Assign(l_ca, r_ca, l_beg, r_beg, cnt_a);

    Zeta_CircularArray_Assign(l_ca, r_ca, l_beg + cnt_a + cnt_b, r_beg + cnt_a,
                              cnt_c);

    Zeta_CircularArray_PeekL(r_ca, &r_ca_cursor, NULL);

    Zeta_CircularArray_Erase(r_ca, &r_ca_cursor, cnt_a + cnt_c);

    Zeta_CircularArray_Cursor_AdvanceR(r_ca, &r_ca_cursor, rl_cnt - cnt_a);

    Zeta_CircularArray_Insert(r_ca, &r_ca_cursor, ins_cnt - cnt_b);
}

void SegShoveR(Zeta_CircularArray* l_ca, Zeta_CircularArray* r_ca,
               size_t lr_cnt, size_t ins_cnt, size_t shove_cnt) {
    ZETA_DebugAssert(l_ca != NULL);
    ZETA_DebugAssert(r_ca != NULL);

    ZETA_DebugAssert(r_ca->size + shove_cnt <= r_ca->capacity);
    ZETA_DebugAssert(shove_cnt <= l_ca->size + ins_cnt);

    size_t width = l_ca->width;
    ZETA_DebugAssert(width == r_ca->width);

    size_t cnt_a = ZETA_GetMinOf(lr_cnt, shove_cnt);
    size_t cnt_b = ZETA_GetMinOf(ins_cnt, shove_cnt - cnt_a);
    size_t cnt_c = shove_cnt - cnt_a - cnt_b;

    size_t l_beg = l_ca->size - cnt_c - cnt_a;
    size_t r_beg = 0;

    Zeta_CircularArray_Cursor l_ca_cursor;
    Zeta_CircularArray_Cursor r_ca_cursor;

    Zeta_CircularArray_PeekL(r_ca, &r_ca_cursor, NULL);

    Zeta_CircularArray_Insert(r_ca, &r_ca_cursor, shove_cnt);

    Zeta_CircularArray_Assign(r_ca, l_ca, r_beg, l_beg, cnt_c);

    Zeta_CircularArray_Assign(r_ca, l_ca, r_beg + cnt_c + cnt_b, l_beg + cnt_c,
                              cnt_a);

    Zeta_CircularArray_Access(l_ca, &l_ca_cursor, NULL,
                              l_ca->size - cnt_a - cnt_c);

    Zeta_CircularArray_Erase(l_ca, &l_ca_cursor, cnt_a + cnt_c);

    Zeta_CircularArray_Cursor_AdvanceL(l_ca, &l_ca_cursor, lr_cnt - cnt_a);

    Zeta_CircularArray_Insert(l_ca, &l_ca_cursor, ins_cnt - cnt_b);
}
