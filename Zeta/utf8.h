#pragma once

#include "define.h"

ZETA_ExternC_Beg;

ZETA_DeclareStruct(Zeta_UTF8_EncodeRet);
ZETA_DeclareStruct(Zeta_UTF8_DecodeRet);

#define ZETA_UTF8_RetCode_success (0)
#define ZETA_UTF8_RetCode_failed (1)
#define ZETA_UTF8_RetCode_insufficient_dst (2)
#define ZETA_UTF8_RetCode_insufficient_src (3)

/**
 * @brief Indicate the encode result.
 */
struct Zeta_UTF8_EncodeRet {
    /** If the encoding is succes and no error. */
    int ret_code;

    byte_t* nxt_dst;
    unichar_t const* nxt_src;
};

/**
 * @brief Indicate the decode result.
 */
struct Zeta_UTF8_DecodeRet {
    /** If the decoding is succes and no error. */
    int ret_code;

    unichar_t* nxt_dst;
    byte_t const* nxt_src;
};

/**
 * @brief Get the size(unicode character) of data decoded from data. The tailing
 * incomplete bytes in data will be ignored.
 *
 * @param src_size The length of src.
 * @param src The source data.
 */
size_t Zeta_UTF8_GetEncodeSize(unichar_t const* src, size_t src_size);

Zeta_UTF8_EncodeRet Zeta_UTF8_Encode(byte_t* dst, size_t dst_size,
                                     unichar_t const* src, size_t src_size);

/**
 * @brief Get the size(unicode character) of data decoded from data. The tailing
 * incomplete bytes in data will be ignored.
 *
 * @param data The source data.
 * @param data_end The end of source data.
 */
size_t Zeta_UTF8_GetDecodeSize(byte_t const* src, size_t src_size);

Zeta_UTF8_DecodeRet Zeta_UTF8_Decode(unichar_t* dst, size_t dst_size,
                                     byte_t const* src, size_t src_size);

ZETA_ExternC_End;
