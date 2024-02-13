#pragma once

#include "Stream.h"

ZETA_extern_c_beg;

typedef struct UTF8_NxtEncodePair UTF8_NxtEncodePair;

int Zeta_UTF8_Encode(Zeta_Stream* dst, Zeta_Stream* src);

int Zeta_UTF8_Decode(Zeta_Stream* dst, Zeta_Stream* src);

ZETA_extern_c_end;
