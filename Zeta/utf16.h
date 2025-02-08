#pragma once

#include "define.h"

ZETA_ExternC_Beg;

ZETA_DeclareStruct(Zeta_UTF16_Result);

struct Zeta_UTF16_Result {
    bool_t success;

    size_t dst_cnt;
    size_t src_cnt;
};

Zeta_UTF16_Result Zeta_UTF16_Encode(byte_t* dst, size_t dst_size,
                                    unichar_t const* src, size_t src_size,
                                    int endian);

Zeta_UTF16_Result Zeta_UTF16_Decode(unichar_t* dst, size_t dst_size,
                                    byte_t const* src, size_t src_size,
                                    int endian);

ZETA_ExternC_End;
