#pragma once

#include "define.h"

ZETA_ExternC_Beg;

ZETA_DeclareStruct(Zeta_UTF16_EncodeRet);
ZETA_DeclareStruct(Zeta_UTF16_DecodeRet);

#define ZETA_UTF16_RetCode_success (0)
#define ZETA_UTF16_RetCode_failed (1)
#define ZETA_UTF16_RetCode_insufficient_dst (2)
#define ZETA_UTF16_RetCode_insufficient_src (3)

/**
 * @brief Indicate the encode result.
 */
struct Zeta_UTF16_EncodeRet {
    /** If the encoding is succes and no error. */
    int ret_code;

    byte_t* nxt_dst;
    unichar_t const* nxt_src;
};

/**
 * @brief Indicate the decode result.
 */
struct Zeta_UTF16_DecodeRet {
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
size_t Zeta_UTF16_GetEncodeSize(unichar_t const* src, size_t src_size,
                                int endian);

Zeta_UTF16_EncodeRet Zeta_UTF16_Encode(byte_t* dst, size_t dst_size,
                                       unichar_t const* src, size_t src_size,
                                       int endian);

/**
 * @brief Get the size(unicode character) of data decoded from data. The tailing
 * incomplete bytes in data will be ignored.
 *
 * @param data The source data.
 * @param data_end The end of source data.
 */
size_t Zeta_UTF16_GetDecodeSize(byte_t const* src, size_t src_size, int endian);

Zeta_UTF16_DecodeRet Zeta_UTF16_Decode(unichar_t* dst, size_t dst_size,
                                       byte_t const* src, size_t src_size,
                                       int endian);

ZETA_ExternC_End;
