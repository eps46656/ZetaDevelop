#include "UTF8.h"

#include "utils.h"

#define ZETA_DecodeRet_state_success 0
#define ZETA_DecodeRet_state_failed 1
#define ZETA_DecodeRet_state_eof 2

typedef struct DecodeRet DecodeRet;

struct DecodeRet {
    int state;
    unichar_t dst;
    const byte_t* nxt_src;
};

static byte_t* EncodeChar_(byte_t* dst, unichar_t src) {
    if (src < 0 || (unichar_t)0x80000000 <= src) { return NULL; }

    if (src < 0b10000000) {
        dst[0] = (byte_t)src;
        return dst + 1;
    }

    byte_t tmp[6];

    unichar_t ub = 0b00100000;
    unichar_t mask = 0b111111000000;

    for (int i = 2;; ++i, ub /= 2, mask /= 2) {
        tmp[i - 2] =
            (byte_t)((unichar_t)0b10000000 + src % (unichar_t)0b01000000);

        src /= 0b01000000;

        if (src < ub) {
            tmp[i - 1] = (byte_t)(mask % (unichar_t)0b100000000 + src);

            for (; 0 <= --i; ++dst) { dst[0] = tmp[i]; }

            return dst;
        }
    }
}

size_t Zeta_UTF8_GetEncodeSize(const unichar_t* src, size_t src_size) {
    byte_t tmp[6];

    size_t ret = 0;

    for (; 0 < src_size; ++src, --src_size) {
        byte_t* tmp_p = EncodeChar_(tmp, src[0]);
        if (tmp_p == NULL) { return ZETA_maxof(size_t); }
        ret += tmp_p - tmp;
    }

    return ret;
}

UTF8_NxtEncodePair Zeta_UTF8_Encode(byte_t* dst, size_t dst_size,
                                    const unichar_t* src, size_t src_size) {
    byte_t tmp[6];

    for (; 0 < src_size; ++src, --src_size) {
        byte_t* tmp_p = EncodeChar_(tmp, src[0]);

        if (tmp_p == NULL) {
            return (UTF8_NxtEncodePair){ .success = FALSE,
                                         .nxt_dst = dst,
                                         .nxt_src = src };
        }

        size_t tmp_size = tmp_p - tmp;

        if (dst_size < tmp_size) {
            return (UTF8_NxtEncodePair){
                .success = TRUE,
                .nxt_dst = dst,
                .nxt_src = src,
            };
        }

        Zeta_MemCopy(tmp_size, dst, tmp);
        dst += tmp_size;
        dst_size -= tmp_size;
    }

    return (UTF8_NxtEncodePair){
        .success = TRUE,
        .nxt_dst = dst,
        .nxt_src = src,
    };
}

static const byte_t* DecodeChar_(unichar_t* dst, const byte_t* src,
                                 size_t src_size) {
    unichar_t x = src[0];
    if (x < 0) { return NULL; }

    if (x < 0b10000000) {
        dst[0] = x;
        return src + 1;
    }

    if (x < 0b11000000 || 0b11111101 < x) { return NULL; }

    unsigned int width;
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

    if (src_size < width) { return src; }

    ++src;

    for (unsigned int i = 1; i < width; ++i, ++src) {
        unichar_t k = src[0];

        if (k / 0b01000000 != 0b10) { return NULL; }

        ret = ret * 0b01000000 + k % 0b01000000;
    }

    *dst = ret;

    return src;
}

size_t Zeta_UTF8_GetDecodeSize(const byte_t* src, size_t src_size) {
    ZETA_DebugAssert(src != NULL);

    size_t ret = 0;

    unichar_t dst;

    while (src_size != 0) {
        const byte_t* nxt_src = DecodeChar_(&dst, src, src_size);
        if (nxt_src == NULL) { return ZETA_maxof(size_t); }
        src_size -= nxt_src - src;
        src = nxt_src;
        ++ret;
    }

    return ret;
}

UTF8_NxtDecodePair Zeta_UTF8_Decode(unichar_t* dst, size_t dst_size,
                                    const byte_t* src, size_t src_size) {
    ZETA_DebugAssert(dst != NULL);
    ZETA_DebugAssert(0 <= dst_size);

    ZETA_DebugAssert(src != NULL);
    ZETA_DebugAssert(0 <= src_size);

    while (0 < dst_size && 0 < src_size) {
        const byte_t* nxt_src = DecodeChar_(dst, src, src_size);

        if (nxt_src == NULL) {
            return (UTF8_NxtDecodePair){
                .success = FALSE,
                .nxt_dst = dst,
                .nxt_src = src,
            };
        }

        if (src == nxt_src) {
            return (UTF8_NxtDecodePair){
                .success = TRUE,
                .nxt_dst = dst,
                .nxt_src = src,
            };
        }

        ++dst;
        --dst_size;
        src_size -= nxt_src - src;
        src = nxt_src;
    }

    return (UTF8_NxtDecodePair){
        .success = TRUE,
        .nxt_dst = dst,
        .nxt_src = src,
    };
}
