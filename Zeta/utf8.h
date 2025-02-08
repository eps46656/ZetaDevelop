#pragma once

#include "define.h"

ZETA_ExternC_Beg;

ZETA_DeclareStruct(Zeta_UTF8_Result);

/**
 * @brief Indicate the encode result.
 */
struct Zeta_UTF8_Result {
    bool_t success;

    size_t dst_cnt;
    size_t src_cnt;
};

Zeta_UTF8_Result Zeta_UTF8_Encode(byte_t* dst, size_t dst_size,
                                  unichar_t const* src, size_t src_size);

Zeta_UTF8_Result Zeta_UTF8_Decode(unichar_t* dst, size_t dst_size,
                                  byte_t const* src, size_t src_size);

ZETA_ExternC_End;
