#include "SegUtils.h"

#include "utils.h"

#if ZETA_IsDebug
#include "Debugger.h"
#endif

void SegShoveL(Zeta_CircularVector* l_cv, Zeta_CircularVector* r_cv,
               size_t rl_cnt, size_t ins_cnt, size_t shove_cnt) {
    ZETA_DebugAssert(l_cv != NULL);
    ZETA_DebugAssert(r_cv != NULL);

    ZETA_DebugAssert(shove_cnt <= l_cv->capacity - l_cv->size);
    ZETA_DebugAssert(shove_cnt <= r_cv->size + ins_cnt);
    ZETA_DebugAssert(r_cv->size + ins_cnt - shove_cnt <= r_cv->capacity);

    size_t width = l_cv->width;
    ZETA_DebugAssert(width == r_cv->width);

    size_t cnt_a = ZETA_GetMinOf(rl_cnt, shove_cnt);
    size_t cnt_b = ZETA_GetMinOf(ins_cnt, shove_cnt - cnt_a);
    size_t cnt_c = shove_cnt - cnt_a - cnt_b;

    size_t l_beg = l_cv->size;
    size_t r_beg = 0;

    Zeta_CircularVector_Cursor l_cv_cursor;
    Zeta_CircularVector_Cursor r_cv_cursor;

    Zeta_CircularVector_GetRBCursor(l_cv, &l_cv_cursor);

    Zeta_CircularVector_Insert(l_cv, &l_cv_cursor, shove_cnt);

    Zeta_CircularVector_Assign(l_cv, r_cv, l_beg, r_beg, cnt_a);

    Zeta_CircularVector_Assign(l_cv, r_cv, l_beg + cnt_a + cnt_b, r_beg + cnt_a,
                               cnt_c);

    Zeta_CircularVector_PeekL(r_cv, &r_cv_cursor, NULL);

    Zeta_CircularVector_Erase(r_cv, &r_cv_cursor, cnt_a + cnt_c);

    Zeta_CircularVector_Cursor_AdvanceR(r_cv, &r_cv_cursor, rl_cnt - cnt_a);

    Zeta_CircularVector_Insert(r_cv, &r_cv_cursor, ins_cnt - cnt_b);
}

void SegShoveR(Zeta_CircularVector* l_cv, Zeta_CircularVector* r_cv,
               size_t lr_cnt, size_t ins_cnt, size_t shove_cnt) {
    ZETA_DebugAssert(l_cv != NULL);
    ZETA_DebugAssert(r_cv != NULL);

    ZETA_DebugAssert(r_cv->size + shove_cnt <= r_cv->capacity);
    ZETA_DebugAssert(shove_cnt <= l_cv->size + ins_cnt);

    size_t width = l_cv->width;
    ZETA_DebugAssert(width == r_cv->width);

    size_t cnt_a = ZETA_GetMinOf(lr_cnt, shove_cnt);
    size_t cnt_b = ZETA_GetMinOf(ins_cnt, shove_cnt - cnt_a);
    size_t cnt_c = shove_cnt - cnt_a - cnt_b;

    size_t l_beg = l_cv->size - cnt_c - cnt_a;
    size_t r_beg = 0;

    Zeta_CircularVector_Cursor l_cv_cursor;
    Zeta_CircularVector_Cursor r_cv_cursor;

    Zeta_CircularVector_PeekL(r_cv, &r_cv_cursor, NULL);

    Zeta_CircularVector_Insert(r_cv, &r_cv_cursor, shove_cnt);

    Zeta_CircularVector_Assign(r_cv, l_cv, r_beg, l_beg, cnt_c);

    Zeta_CircularVector_Assign(r_cv, l_cv, r_beg + cnt_c + cnt_b, l_beg + cnt_c,
                               cnt_a);

    Zeta_CircularVector_Access(l_cv, &l_cv_cursor, NULL,
                               l_cv->size - cnt_a - cnt_c);

    Zeta_CircularVector_Erase(l_cv, &l_cv_cursor, cnt_a + cnt_c);

    Zeta_CircularVector_Cursor_AdvanceL(l_cv, &l_cv_cursor, lr_cnt - cnt_a);

    Zeta_CircularVector_Insert(l_cv, &l_cv_cursor, ins_cnt - cnt_b);
}
