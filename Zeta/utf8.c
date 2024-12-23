#include "utf8.h"

#include "debugger.h"
#include "utils.h"

static byte_t* EncodeChar_(byte_t* dst, unichar_t data) {
    if (data < 0 || 0x7FFFFFFF < data) { return NULL; }

    if (data <= 0b01111111) {
        *dst = data;
        return dst + 1;
    }

    unichar_t ub = 0b00011111;
    unichar_t lead = 0b111111000000;

    byte_t tmp[6];
    byte_t* tmp_p = tmp;

    for (;; ub /= 2, lead /= 2) {
        *(tmp_p++) = 0b10000000 + data % 0b01000000;
        data /= 0b01000000;

        if (data <= ub) {
            *(dst++) = lead % 0b100000000 + data;
            while (tmp < tmp_p) { *(dst++) = *(--tmp_p); }
            return dst;
        }
    }
}

size_t Zeta_UTF8_GetEncodeSize(unichar_t const* src, size_t src_size) {
    ZETA_DebugAssert(src != NULL);

    byte_t tmp[6];

    size_t ret = 0;

    for (; 0 < src_size--; ++src) {
        byte_t* tmp_p = EncodeChar_(tmp, *src);
        if (tmp_p == NULL) { return ZETA_SIZE_MAX; }
        ret += tmp_p - tmp;
    }

    return ret;
}

Zeta_UTF8_EncodeRet Zeta_UTF8_Encode(byte_t* dst, size_t dst_size,
                                     unichar_t const* src, size_t src_size) {
    ZETA_DebugAssert(dst != NULL);
    ZETA_DebugAssert(src != NULL);

    byte_t tmp[6];

    for (; 0 < src_size--; ++src) {
        byte_t* tmp_p = EncodeChar_(tmp, *src);

        if (tmp_p == NULL) {
            return (Zeta_UTF8_EncodeRet){ .ret_code = ZETA_UTF8_RetCode_failed,
                                          .nxt_dst = dst,
                                          .nxt_src = src };
        }

        size_t tmp_size = tmp_p - tmp;

        if (dst_size < tmp_size) {
            return (Zeta_UTF8_EncodeRet){
                .ret_code = ZETA_UTF8_RetCode_insufficient_dst,
                .nxt_dst = dst,
                .nxt_src = src,
            };
        }

        Zeta_MemCopy(dst, tmp, tmp_size);

        dst_size -= tmp_size;
        dst += tmp_size;
    }

    return (Zeta_UTF8_EncodeRet){
        .ret_code = ZETA_UTF8_RetCode_success,
        .nxt_dst = dst,
        .nxt_src = src,
    };
}

static Zeta_UTF8_DecodeRet DecodeChar_(unichar_t* dst, byte_t const* src,
                                       size_t src_size) {
    unichar_t x = *src;

    if (x < 0) {
        return (Zeta_UTF8_DecodeRet){
            .ret_code = ZETA_UTF8_RetCode_failed,
            .nxt_dst = dst,
            .nxt_src = src,
        };
    }

    if (x < 0b10000000) {
        *dst = x;

        return (Zeta_UTF8_DecodeRet){
            .ret_code = ZETA_UTF8_RetCode_success,
            .nxt_dst = dst + 1,
            .nxt_src = src + 1,
        };
    }

    if (x < 0b11000000 || 0b11111101 < x) {
        return (Zeta_UTF8_DecodeRet){
            .ret_code = ZETA_UTF8_RetCode_failed,
            .nxt_dst = dst,
            .nxt_src = src,
        };
    }

    unsigned width;
    unichar_t ret;

    if (0b11111100 <= x) {
        width = 6;
        ret = x % 0b00000010;
    } else if (0b11111000 <= x) {
        width = 5;
        ret = x % 0b00000100;
    } else if (0b11110000 <= x) {
        width = 4;
        ret = x % 0b00001000;
    } else if (0b11100000 <= x) {
        width = 3;
        ret = x % 0b00010000;
    } else {  // 0b11000000 <= x
        width = 2;
        ret = x % 0b00100000;
    }

    if (src_size < width) {
        return (Zeta_UTF8_DecodeRet){
            .ret_code = ZETA_UTF8_RetCode_insufficient_src,
            .nxt_dst = dst,
            .nxt_src = src,
        };
    }

    byte_t const* origin_src = src;

    ++src;

    for (unsigned i = 1; i < width; ++i, ++src) {
        unichar_t k = *src;

        ZETA_DebugAssert(0 <= k);
        ZETA_DebugAssert(k / 0b01000000 == 0b10);

        if (k < 0 || k / 0b01000000 != 0b10) {
            return (Zeta_UTF8_DecodeRet){
                .ret_code = ZETA_UTF8_RetCode_failed,
                .nxt_dst = dst,
                .nxt_src = origin_src,
            };
        }

        ret = ret * 0b01000000 + k % 0b01000000;
    }

    *dst = ret;

    return (Zeta_UTF8_DecodeRet){
        .ret_code = ZETA_UTF8_RetCode_success,
        .nxt_dst = dst + 1,
        .nxt_src = src,
    };
}

size_t Zeta_UTF8_GetDecodeSize(byte_t const* src, size_t src_size) {
    ZETA_DebugAssert(src != NULL);

    size_t ret = 0;

    unichar_t tmp;

    for (;;) {
        Zeta_UTF8_DecodeRet decode_ret = DecodeChar_(&tmp, src, src_size);

        if (decode_ret.ret_code == ZETA_UTF8_RetCode_insufficient_src) {
            return ret;
        }

        if (decode_ret.ret_code == ZETA_UTF8_RetCode_failed) {
            return ZETA_SIZE_MAX;
        }

        src_size -= decode_ret.nxt_src - src;
        src = decode_ret.nxt_src;

        ++ret;
    }
}

Zeta_UTF8_DecodeRet Zeta_UTF8_Decode(unichar_t* dst, size_t dst_size,
                                     byte_t const* src, size_t src_size) {
    ZETA_DebugAssert(dst != NULL);
    ZETA_DebugAssert(src != NULL);

    while (0 < dst_size && 0 < src_size) {
        Zeta_UTF8_DecodeRet decode_ret = DecodeChar_(dst, src, src_size);

        if (decode_ret.ret_code == ZETA_UTF8_RetCode_failed ||
            decode_ret.ret_code == ZETA_UTF8_RetCode_insufficient_src) {
            return (Zeta_UTF8_DecodeRet){
                .ret_code = decode_ret.ret_code,
                .nxt_dst = dst,
                .nxt_src = src,
            };
        }

        --dst_size;
        ++dst;

        src_size -= decode_ret.nxt_src - src;
        src = decode_ret.nxt_src;
    }

    return (Zeta_UTF8_DecodeRet){
        .ret_code = ZETA_UTF8_RetCode_success,
        .nxt_dst = dst,
        .nxt_src = src,
    };
}
