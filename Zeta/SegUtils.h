#pragma once

#include "CircularVector.h"

ZETA_ExternC_Beg;

void SegShoveL(Zeta_CircularVector* l_cv, Zeta_CircularVector* r_cv,
               size_t rl_cnt, size_t ins_cnt, size_t shove_cnt);

void SegShoveR(Zeta_CircularVector* l_cv, Zeta_CircularVector* r_cv,
               size_t lr_cnt, size_t ins_cnt, size_t shove_cnt);

ZETA_ExternC_End;
