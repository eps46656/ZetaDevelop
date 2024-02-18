#pragma once

#include "define.h"

ZETA_extern_c_beg;

typedef struct UTF16_EncodeRet UTF16_EncodeRet;

/**
 * @brief Indicate the encode result.
 */
struct UTF16_EncodeRet {
    /** If the encoding is succes and no error. */
    bool_t success;

    byte_t* nxt_dst;
    const unichar_t* nxt_data;
};

typedef struct UTF16_DecodeRet UTF16_DecodeRet;

/**
 * @brief Indicate the decode result.
 */
struct UTF16_DecodeRet {
    /** If the decoding is succes and no error. */
    bool_t success;

    unichar_t* nxt_dst;
    const byte_t* nxt_data;
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
size_t Zeta_UTF16_GetEncodeSize(const unichar_t* data,
                                const unichar_t* data_end,
                                bool_t little_endian);

UTF16_EncodeRet Zeta_UTF16_Encode(byte_t* dst, byte_t* dst_end,
                                  const unichar_t* data,
                                  const unichar_t* data_end,
                                  bool_t little_endian);

/**
 * @brief Get the size(unicode character) of data decoded from data. The tailing
 * incomplete bytes in data will be ignored.
 *
 * @param data The source data.
 * @param data_end The end of source data.
 */
size_t Zeta_UTF16_GetDecodeSize(const byte_t* data, const byte_t* data_end,
                                bool_t little_endian);

UTF16_DecodeRet Zeta_UTF16_Decode(unichar_t* dst, unichar_t* dst_end,
                                  const byte_t* data, const byte_t* data_end,
                                  bool_t little_endian);

ZETA_extern_c_end;
