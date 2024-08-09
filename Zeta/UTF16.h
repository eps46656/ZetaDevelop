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

    unsigned char* nxt_dst;
    unichar_t const* nxt_src;
};

/**
 * @brief Indicate the decode result.
 */
struct Zeta_UTF16_DecodeRet {
    /** If the decoding is succes and no error. */
    int ret_code;

    unichar_t* nxt_dst;
    unsigned char const* nxt_src;
};

/**
 * @brief Get the size(unicode character) of data decoded from data. The tailing
 * incomplete bytes in data will be ignored.
 *
 * @param src_size The length of src.
 * @param src The source data.
 */
size_t Zeta_UTF16_GetEncodeSize(size_t src_size, unichar_t const* src,
                                bool_t little_endian);

Zeta_UTF16_EncodeRet Zeta_UTF16_Encode(size_t dst_size, unsigned char* dst,
                                       size_t src_size, unichar_t const* src,
                                       bool_t little_endian);

/**
 * @brief Get the size(unicode character) of data decoded from data. The tailing
 * incomplete bytes in data will be ignored.
 *
 * @param data The source data.
 * @param data_end The end of source data.
 */
size_t Zeta_UTF16_GetDecodeSize(size_t src_size, unsigned char const* src,
                                bool_t little_endian);

Zeta_UTF16_DecodeRet Zeta_UTF16_Decode(size_t dst_size, unichar_t* dst,
                                       size_t src_size,
                                       unsigned char const* src,
                                       bool_t little_endian);

ZETA_ExternC_End;
