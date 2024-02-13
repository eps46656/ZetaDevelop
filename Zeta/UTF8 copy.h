#pragma once

#include "define.h"

ZETA_extern_c_beg;

typedef struct UTF8_NxtEncodePair UTF8_NxtEncodePair;

/**
 * @brief Indicate the encode result.
 */
struct UTF8_NxtEncodePair {
    /** If the encoding is succes and no error. */
    bool_t success;

    byte_t* nxt_dst;
    const unichar_t* nxt_src;
};

typedef struct UTF8_NxtDecodePair UTF8_NxtDecodePair;

/**
 * @brief Indicate the decode result.
 */
struct UTF8_NxtDecodePair {
    /** If the decoding is succes and no error. */
    bool_t success;

    unichar_t* nxt_dst;
    const byte_t* nxt_src;
};

/**
 * @brief Get the size(unicode character) of data decoded from src. The tailing
 * incomplete bytes in src will be ignored.
 *
 * @param src The source data.
 * @param src_size The end of source data.
 */
size_t Zeta_UTF8_GetEncodeSize(const unichar_t* src, const unichar_t* src_end);

UTF8_NxtEncodePair Zeta_UTF8_Encode(byte_t* dst, byte_t* dst_end,
                                    const unichar_t* src,
                                    const unichar_t* src_end);

/**
 * @brief Get the size(unicode character) of data decoded from src. The tailing
 * incomplete bytes in src will be ignored.
 *
 * @param src The source data.
 * @param src_end The end of source data.
 */
size_t Zeta_UTF8_GetDecodeSize(const byte_t* src, const byte_t* src_end);

UTF8_NxtDecodePair Zeta_UTF8_Decode(unichar_t* dst, unichar_t* dst_end,
                                    const byte_t* src, const byte_t* src_end);

ZETA_extern_c_end;
