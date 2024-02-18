#pragma once

#include "Stream.h"

ZETA_extern_c_beg;

typedef struct Zeta_UTF8_EncodeRet Zeta_UTF8_EncodeRet;

int Zeta_UTF8_EncodeSingle(byte_t* dst, unichar_t src);

int Zeta_UTF8_Encode(Zeta_Stream* dst, Zeta_Stream* src);

int Zeta_UTF8_DecodeSingle(unichar_t* dst, const byte_t* src);

int Zeta_UTF8_Decode(Zeta_Stream* dst, Zeta_Stream* src);

ZETA_extern_c_end;
