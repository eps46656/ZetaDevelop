#pragma once

#include "define.h"

ZETA_ExternC_Beg;

ZETA_DeclareStruct(Zeta_UTF16_EncodeRet);
ZETA_DeclareStruct(Zeta_UTF16_DecodeRet);

struct Zeta_UTF16_EncodeRet {
    bool_t success;

    size_t dst_cnt;
    unichar_t const* nxt_src;
};

struct Zeta_UTF16_DecodeRet {
    bool_t success;

    size_t dst_cnt;
    byte_t const* nxt_src;
};

Zeta_UTF16_EncodeRet Zeta_UTF16_Encode(byte_t* dst, size_t dst_size,
                                       unichar_t const* src, size_t src_size,
                                       int endian);

Zeta_UTF16_DecodeRet Zeta_UTF16_Decode(unichar_t* dst, size_t dst_size,
                                       byte_t const* src, size_t src_size,
                                       int endian);

ZETA_ExternC_End;
