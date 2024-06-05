#pragma once

#include "define.h"

ZETA_ExternC_Beg;

ZETA_DeclareStruct(UTF16_EncodeRet);
ZETA_DeclareStruct(UTF16_DecodeRet);

/**
 * @brief Indicate the encode result.
 */
struct UTF16_EncodeRet {
    /** If the encoding is succes and no error. */
    bool_t success;

    byte_t* nxt_dst;
    unichar_t const* nxt_data;
};

/**
 * @brief Indicate the decode result.
 */
struct UTF16_DecodeRet {
    /** If the decoding is succes and no error. */
    bool_t success;

    unichar_t* nxt_dst;
    byte_t const* nxt_data;
};

/**
 * @brief Get the size(unicode character) of data decoded from data. The tailing
 * incomplete bytes in data will be ignored.
 *
 * @param data The source data.
 * @param data_end The end of source data.
 *
 * @return The size(bytes) of encoded data.
 */
size_t Zeta_UTF16_GetEncodeSize(unichar_t const* data,
                                unichar_t const* data_end,
                                bool_t little_endian);

UTF16_EncodeRet Zeta_UTF16_Encode(byte_t* dst, byte_t* dst_end,
                                  unichar_t const* data,
                                  unichar_t const* data_end,
                                  bool_t little_endian);

/**
 * @brief Get the size(unicode character) of data decoded from data. The tailing
 * incomplete bytes in data will be ignored.
 *
 * @param data The source data.
 * @param data_end The end of source data.
 */
size_t Zeta_UTF16_GetDecodeSize(byte_t const* data, byte_t const* data_end,
                                bool_t little_endian);

UTF16_DecodeRet Zeta_UTF16_Decode(unichar_t* dst, unichar_t* dst_end,
                                  byte_t const* data, byte_t const* data_end,
                                  bool_t little_endian);

ZETA_ExternC_End;
