#include "utf16.h"

#include "debugger.h"
#include "utils.h"

static byte_t* EncodeChar_(byte_t* dst, unichar_t data,
                           unsigned long long (*Write)(byte_t* dst,
                                                       unsigned long long val,
                                                       unsigned length)) {
    if (data < 0) { return NULL; }

    if (data < 0xD800) {
        Write(dst, data, 2);
        return dst + 2;
    }

    if (data < 0xE000) { return NULL; }

    if (data < 0x10000) {
        Write(dst, data, 2);
        return dst + 2;
    }

    if (0x10FFFF < data) { return NULL; }

    data -= 0x10000;

    Write(dst, data / 0x400 + 0xD800, 2);
    Write(dst + 2, data % 0x400 + 0xDC00, 2);

    return dst + 4;
}

size_t Zeta_UTF16_GetEncodeSize(unichar_t const* src, size_t src_size,
                                int endian) {
    ZETA_DebugAssert(src != NULL);
    ZETA_DebugAssert(endian == ZETA_LittleEndian || endian == ZETA_BigEndian);

    size_t ret = 0;

    byte_t tmp[8];

    unsigned long long (*Write)(byte_t* dst, unsigned long long val,
                                unsigned length) =
        endian == ZETA_LittleEndian ? Zeta_WriteLittleEndianULL
                                    : Zeta_WriteBigEndianULL;

    for (; 0 < src_size--; ++src) {
        byte_t* tmp_p = EncodeChar_(tmp, *src, Write);
        if (tmp_p == NULL) { return ZETA_RangeMaxOf(size_t); }
        ret += tmp_p - tmp;
    }

    return ret;
}

Zeta_UTF16_EncodeRet Zeta_UTF16_Encode(byte_t* dst, size_t dst_size,
                                       unichar_t const* src, size_t src_size,
                                       int endian) {
    ZETA_DebugAssert(dst != NULL);
    ZETA_DebugAssert(src != NULL);

    ZETA_DebugAssert(endian == ZETA_LittleEndian || endian == ZETA_BigEndian);

    byte_t tmp[8];

    unsigned long long (*Write)(byte_t* dst, unsigned long long val,
                                unsigned length) =
        endian == ZETA_LittleEndian ? Zeta_WriteLittleEndianULL
                                    : Zeta_WriteBigEndianULL;

    for (; 0 < src_size--; ++src) {
        byte_t* tmp_p = EncodeChar_(tmp, *src, Write);

        if (tmp_p == NULL) {
            return (Zeta_UTF16_EncodeRet){
                .ret_code = ZETA_UTF16_RetCode_failed,
                .nxt_dst = dst,
                .nxt_src = src,
            };
        }

        size_t tmp_size = tmp_p - tmp;

        if (dst_size < tmp_size) {
            return (Zeta_UTF16_EncodeRet){
                .ret_code = ZETA_UTF16_RetCode_insufficient_dst,
                .nxt_dst = dst,
                .nxt_src = src,
            };
        }

        Zeta_MemCopy(dst, tmp, tmp_size);

        dst_size -= tmp_size;
        dst += tmp_size;
    }

    return (Zeta_UTF16_EncodeRet){
        .ret_code = ZETA_UTF16_RetCode_success,
        .nxt_dst = dst,
        .nxt_src = src,
    };
}

static Zeta_UTF16_DecodeRet DecodeChar_(
    unichar_t* dst, byte_t const* src, size_t src_size,
    unsigned long long (*Read)(byte_t const* data, unsigned length)) {
    if (src_size < 2) {
        return (Zeta_UTF16_DecodeRet){
            .ret_code = ZETA_UTF16_RetCode_insufficient_src,
            .nxt_dst = dst,
            .nxt_src = src,
        };
    }

    unichar_t x = Read(src, 2);

    if (0xFFFF < x) {
        return (Zeta_UTF16_DecodeRet){
            .ret_code = ZETA_UTF16_RetCode_failed,
            .nxt_dst = dst,
            .nxt_src = src,
        };
    }

    if (x < 0xD800 || 0xDFFF < x) {
        *dst = x;

        return (Zeta_UTF16_DecodeRet){
            .ret_code = ZETA_UTF16_RetCode_success,
            .nxt_dst = dst + 1,
            .nxt_src = src + 2,
        };
    }

    if (0xDBFF < x) {
        return (Zeta_UTF16_DecodeRet){
            .ret_code = ZETA_UTF16_RetCode_failed,
            .nxt_dst = dst,
            .nxt_src = src,
        };
    }

    if (src_size < 4) {
        return (Zeta_UTF16_DecodeRet){
            .ret_code = ZETA_UTF16_RetCode_insufficient_src,
            .nxt_dst = dst,
            .nxt_src = src,
        };
    }

    unichar_t y = Read(src + 2, 2);

    if (y < 0xDC00 || 0xDFFF < y) {
        return (Zeta_UTF16_DecodeRet){
            .ret_code = ZETA_UTF16_RetCode_failed,
            .nxt_dst = dst,
            .nxt_src = src,
        };
    }

    *dst = (x - 0xD800) * 0x400 + (y - 0xDC00) + 0x10000;

    return (Zeta_UTF16_DecodeRet){
        .ret_code = ZETA_UTF16_RetCode_success,
        .nxt_dst = dst + 1,
        .nxt_src = src + 4,
    };
}

size_t Zeta_UTF16_GetDecodeSize(byte_t const* src, size_t src_size,
                                int endian) {
    ZETA_DebugAssert(src != NULL);

    ZETA_DebugAssert(endian == ZETA_LittleEndian || endian == ZETA_BigEndian);

    size_t ret = 0;

    unichar_t tmp;

    unsigned long long (*Read)(byte_t const* data, unsigned length) =
        endian == ZETA_LittleEndian ? Zeta_ReadLittleEndianULL
                                    : Zeta_ReadBigEndianULL;

    for (;;) {
        Zeta_UTF16_DecodeRet decode_ret =
            DecodeChar_(&tmp, src, src_size, Read);

        if (decode_ret.ret_code == ZETA_UTF16_RetCode_insufficient_src) {
            return ret;
        }

        if (decode_ret.ret_code == ZETA_UTF16_RetCode_failed) {
            return ZETA_RangeMaxOf(size_t);
        }

        src_size -= decode_ret.nxt_src - src;
        src = decode_ret.nxt_src;

        ++ret;
    }

    return ret;
}

Zeta_UTF16_DecodeRet Zeta_UTF16_Decode(unichar_t* dst, size_t dst_size,
                                       byte_t const* src, size_t src_size,
                                       int endian) {
    ZETA_DebugAssert(dst != NULL);
    ZETA_DebugAssert(src != NULL);

    ZETA_DebugAssert(endian == ZETA_LittleEndian || endian == ZETA_BigEndian);

    unsigned long long (*Read)(byte_t const* data, unsigned length) =
        endian == ZETA_LittleEndian ? Zeta_ReadLittleEndianULL
                                    : Zeta_ReadBigEndianULL;

    while (0 < dst_size && 0 < src_size) {
        Zeta_UTF16_DecodeRet decode_ret = DecodeChar_(dst, src, src_size, Read);

        if (decode_ret.ret_code == ZETA_UTF16_RetCode_failed ||
            decode_ret.ret_code == ZETA_UTF16_RetCode_insufficient_src) {
            return (Zeta_UTF16_DecodeRet){
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

    return (Zeta_UTF16_DecodeRet){
        .ret_code = src_size == 0 ? ZETA_UTF16_RetCode_success
                                  : ZETA_UTF16_RetCode_insufficient_dst,
        .nxt_dst = dst,
        .nxt_src = src,
    };
}
