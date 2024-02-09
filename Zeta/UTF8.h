#pragma once

#include "define.h"

ZETA_extern_c_beg;

typedef struct UTF8_NxtEncodePair UTF8_NxtEncodePair;

struct UTF8_NxtEncodePair {
    bool_t success;

    byte_t* nxt_dst;
    const unichar_t* nxt_src;
};

typedef struct UTF8_NxtDecodePair UTF8_NxtDecodePair;

struct UTF8_NxtDecodePair {
    bool_t success;

    unichar_t* nxt_dst;
    const byte_t* nxt_src;
};

size_t Zeta_UTF8_GetEncodeSize(const unichar_t* src, size_t src_size);

UTF8_NxtEncodePair Zeta_UTF8_Encode(byte_t* dst, size_t dst_size,
                                    const unichar_t* src, size_t src_size);

size_t Zeta_UTF8_GetDecodeSize(const byte_t* src, size_t src_size);

UTF8_NxtDecodePair Zeta_UTF8_Decode(unichar_t* dst, size_t dst_size,
                                    const byte_t* src, size_t src_size);

ZETA_extern_c_end;
