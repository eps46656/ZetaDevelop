#pragma once

#include "circular_array.h"

ZETA_ExternC_Beg;

void SegShoveL(Zeta_CircularArray* l_ca, Zeta_CircularArray* r_ca,
               size_t rl_cnt, size_t ins_cnt, size_t shove_cnt);

void SegShoveR(Zeta_CircularArray* l_ca, Zeta_CircularArray* r_ca,
               size_t lr_cnt, size_t ins_cnt, size_t shove_cnt);

ZETA_ExternC_End;
