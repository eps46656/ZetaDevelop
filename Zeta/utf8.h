#pragma once

#include "define.h"

ZETA_ExternC_Beg;

ZETA_DeclareStruct(Zeta_UTF8_EncodeRet);
ZETA_DeclareStruct(Zeta_UTF8_DecodeRet);

/**
 * @brief Indicate the encode result.
 */
struct Zeta_UTF8_EncodeRet {
    bool_t success;

    size_t dst_cnt;
    unichar_t const* nxt_src;
};

/**
 * @brief Indicate the decode result.
 */
struct Zeta_UTF8_DecodeRet {
    bool_t success;

    size_t dst_cnt;
    byte_t const* nxt_src;
};

Zeta_UTF8_EncodeRet Zeta_UTF8_Encode(byte_t* dst, size_t dst_size,
                                     unichar_t const* src, size_t src_size);

Zeta_UTF8_DecodeRet Zeta_UTF8_Decode(unichar_t* dst, size_t dst_size,
                                     byte_t const* src, size_t src_size);

ZETA_ExternC_End;
