#include "utf16.h"

#include "debugger.h"
#include "utils.h"

static byte_t* EncodeChar_(byte_t* dst, unichar_t data, int endian) {
    if (data < 0) { return NULL; }

    if (data < 0xD800) {
        ZETA_UIntToBytes(data, dst, 2, endian);
        return dst + 2;
    }

    if (data < 0xE000) { return NULL; }

    if (data < 0x10000) {
        ZETA_UIntToBytes(data, dst, 2, endian);
        return dst + 2;
    }

    if (0x10FFFF < data) { return NULL; }

    data -= 0x10000;

    ZETA_UIntToBytes(data / 0x400 + 0xD800, dst, 2, endian);
    ZETA_UIntToBytes(data % 0x400 + 0xDC00, dst + 2, 2, endian);

    return dst + 4;
}

Zeta_UTF16_EncodeRet Zeta_UTF16_Encode(byte_t* dst, size_t dst_size,
                                       unichar_t const* src, size_t src_size,
                                       int endian) {
    ZETA_DebugAssert(dst != NULL);
    ZETA_DebugAssert(src != NULL);

    ZETA_DebugAssert(endian == ZETA_LittleEndian || endian == ZETA_BigEndian);

    byte_t tmp[8];

    size_t dst_cnt = 0;

    for (; dst_cnt < dst_size && 0 < src_size; ++src, --src_size) {
        byte_t* tmp_p = EncodeChar_(tmp, *src, endian);

        if (tmp_p == NULL) {
            return (Zeta_UTF16_EncodeRet){
                .success = FALSE,
                .dst_cnt = dst_cnt,
                .nxt_src = src,
            };
        }

        size_t tmp_size = tmp_p - tmp;

        if (dst_size - dst_cnt < tmp_size) {
            return (Zeta_UTF16_EncodeRet){
                .success = TRUE,
                .dst_cnt = dst_cnt,
                .nxt_src = src,
            };
        }

        if (dst != NULL) {
            if (endian == ZETA_LittleEndian) {
                for (size_t i = 0; i < tmp_size; ++i) {
                    dst[i] = tmp[tmp_size - 1 - i];
                }
            } else {
                for (size_t i = 0; i < tmp_size; ++i) { dst[i] = tmp[i]; }
            }

            dst += tmp_size;
        }

        dst_cnt += tmp_size;
    }

    return (Zeta_UTF16_EncodeRet){
        .success = TRUE,
        .dst_cnt = dst_cnt,
        .nxt_src = src,
    };
}

static byte_t const* DecodeChar_(unichar_t* dst, byte_t const* src,
                                 size_t src_size, int endian) {
    if (src_size < 2) { return src; }

    unichar_t x = ZETA_BytesToUInt(unichar_t, src, 2, endian);

    if (0xFFFF < x) { return NULL; }

    if (x < 0xD800 || 0xDFFF < x) {
        *dst = x;
        return src + 2;
    }

    if (0xDBFF < x) { return NULL; }

    if (src_size < 4) { return src; }

    unichar_t y = ZETA_BytesToUInt(unichar_t, src + 2, 2, endian);

    if (y < 0xDC00 || 0xDFFF < y) { return NULL; }

    *dst = (x - 0xD800) * 0x400 + (y - 0xDC00) + 0x10000;

    return src + 4;
}

Zeta_UTF16_DecodeRet Zeta_UTF16_Decode(unichar_t* dst, size_t dst_size,
                                       byte_t const* src, size_t src_size,
                                       int endian) {
    ZETA_DebugAssert(dst != NULL);
    ZETA_DebugAssert(src != NULL);

    ZETA_DebugAssert(endian == ZETA_LittleEndian || endian == ZETA_BigEndian);

    size_t dst_cnt = 0;

    while (dst_cnt < dst_size && 0 < src_size) {
        unichar_t tmp;

        byte_t const* nxt_src = DecodeChar_(&tmp, src, src_size, endian);

        if (nxt_src == NULL) {
            return (Zeta_UTF16_DecodeRet){
                .success = FALSE,
                .dst_cnt = dst_cnt,
                .nxt_src = src,
            };
        }

        if (src == nxt_src) { break; }

        if (dst != NULL) { *(dst++) = tmp; }

        ++dst_cnt;

        src_size -= nxt_src - src;
        src = nxt_src;
    }

    return (Zeta_UTF16_DecodeRet){
        .success = TRUE,
        .dst_cnt = dst_cnt,
        .nxt_src = src,
    };
}
