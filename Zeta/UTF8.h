#pragma once

#include "define.h"

ZETA_extern_c_beg;

typedef struct Zeta_UTF8_EncodeRet Zeta_UTF8_EncodeRet;

/**
 * @brief Indicate the encode result.
 */
struct Zeta_UTF8_EncodeRet {
    /** If the encoding is succes and no error. */
    bool_t success;

    byte_t* nxt_dst;
    const unichar_t* nxt_data;
};

typedef struct Zeta_UTF8_DecodeRet Zeta_UTF8_DecodeRet;

/**
 * @brief Indicate the decode result.
 */
struct Zeta_UTF8_DecodeRet {
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
 * @param data_size The end of source data.
 */
size_t Zeta_UTF8_GetEncodeSize(const unichar_t* data,
                               const unichar_t* data_end);

Zeta_UTF8_EncodeRet Zeta_UTF8_Encode(byte_t* dst, byte_t* dst_end,
                                     const unichar_t* data,
                                     const unichar_t* data_end);

/**
 * @brief Get the size(unicode character) of data decoded from data. The tailing
 * incomplete bytes in data will be ignored.
 *
 * @param data The source data.
 * @param data_end The end of source data.
 */
size_t Zeta_UTF8_GetDecodeSize(const byte_t* data, const byte_t* data_end);

Zeta_UTF8_DecodeRet Zeta_UTF8_Decode(unichar_t* dst, unichar_t* dst_end,
                                     const byte_t* data,
                                     const byte_t* data_end);

ZETA_extern_c_end;
