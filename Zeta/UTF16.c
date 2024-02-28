#include "UTF16.h"

#include "utils.h"

static byte_t* EncodeChar_(byte_t* dst, unichar_t data_,
                           u128_t (*Write)(byte_t* dst, u128_t val,
                                           unsigned int length)) {
    if (data_ < 0) { return NULL; }

    u32_t data = data_;

    if (data < 0xFFFF) {
        Write(dst, 2, data);
        return dst + 2;
    }

    if (0x10FFFF < data) { return NULL; }

    Write(dst, data / 0x400 + 0xD800, 2);
    Write(dst + 2, data % 0x400 + 0xDC00, 2);

    return dst + 4;
}

size_t Zeta_UTF16_GetEncodeSize(unichar_t const* data,
                                unichar_t const* data_end,
                                bool_t little_endian) {
    ZETA_DebugAssert(data != NULL);
    ZETA_DebugAssert(data_end != NULL);
    ZETA_DebugAssert(data <= data_end);

    size_t ret = 0;

    byte_t tmp[8];

    u128_t (*Write)(byte_t* dst, u128_t val, unsigned int length) =
        little_endian ? Zeta_WriteLittleEndian : Zeta_WriteBigEndian;

    for (; data < data_end; ++data) {
        byte_t* tmp_p = EncodeChar_(tmp, data[0], Write);
        if (tmp_p == NULL) { return ZETA_maxof(size_t); }
        ret += tmp_p - tmp;
    }

    return ret;
}

UTF16_EncodeRet Zeta_UTF16_Encode(byte_t* dst, byte_t* dst_end,
                                  unichar_t const* data,
                                  unichar_t const* data_end,
                                  bool_t little_endian) {
    ZETA_DebugAssert(dst != NULL);
    ZETA_DebugAssert(dst_end != NULL);
    ZETA_DebugAssert(dst <= dst_end);

    ZETA_DebugAssert(data != NULL);
    ZETA_DebugAssert(data_end != NULL);
    ZETA_DebugAssert(data <= data_end);

    byte_t tmp[8];

    u128_t (*Write)(byte_t* dst, u128_t val, unsigned int length) =
        little_endian ? Zeta_WriteLittleEndian : Zeta_WriteBigEndian;

    for (; data < data_end; ++data) {
        byte_t* tmp_p = EncodeChar_(tmp, data[0], Write);

        if (tmp_p == NULL) {
            return (UTF16_EncodeRet){ .success = FALSE,
                                      .nxt_dst = dst,
                                      .nxt_data = data };
        }

        diff_t tmp_size = tmp_p - tmp;

        if (dst_end - dst < tmp_size) {
            return (UTF16_EncodeRet){
                .success = TRUE,
                .nxt_dst = dst,
                .nxt_data = data,
            };
        }

        Zeta_MemCopy(tmp_size, dst, tmp);
        dst += tmp_size;
    }

    return (UTF16_EncodeRet){
        .success = TRUE,
        .nxt_dst = dst,
        .nxt_data = data,
    };
}

static byte_t const* DecodeChar_(unichar_t* dst, byte_t const* data,
                                 byte_t const* data_end,
                                 u128_t (*Read)(byte_t const* data,
                                                unsigned int length)) {
    if (data_end - data < 2) { return data; }

    u32_t x = Read(data, 2);

    if (x <= 0xFFFF) {
        *dst = x;
        return data + 2;
    }

    if (x < 0xD800 || 0xD8FF < x) { return NULL; }

    if (data_end - data < 4) { return data; }

    u32_t y = Read(data + 2, 2);
    if (y < 0xDC00 || 0xDFFF < y) { return NULL; }

    *dst = (x - 0xD800) * 0x400 + (y - 0xDC00);

    return data + 4;
}

size_t Zeta_UTF16_GetDecodeSize(byte_t const* data, byte_t const* data_end,
                                bool_t little_endian) {
    ZETA_DebugAssert(data != NULL);
    ZETA_DebugAssert(data_end != NULL);
    ZETA_DebugAssert(data <= data_end);

    size_t ret = 0;

    unichar_t dst;

    u128_t (*Read)(byte_t const* data, unsigned int length) =
        little_endian ? Zeta_ReadLittleEndian : Zeta_ReadBigEndian;

    while (data < data_end) {
        byte_t const* nxt_data = DecodeChar_(&dst, data, data_end, Read);
        if (nxt_data == NULL) { return ZETA_maxof(size_t); }
        data = nxt_data;
        ++ret;
    }

    return ret;
}

UTF16_DecodeRet Zeta_UTF16_Decode(unichar_t* dst, unichar_t* dst_end,
                                  byte_t const* data, byte_t const* data_end,
                                  bool_t little_endian) {
    ZETA_DebugAssert(dst != NULL);
    ZETA_DebugAssert(dst_end != NULL);
    ZETA_DebugAssert(dst <= dst_end);

    ZETA_DebugAssert(data != NULL);
    ZETA_DebugAssert(data_end != NULL);
    ZETA_DebugAssert(data <= data_end);

    u128_t (*Read)(byte_t const* data, unsigned int length) =
        little_endian ? Zeta_ReadLittleEndian : Zeta_ReadBigEndian;

    while (dst < dst_end && data < data_end) {
        byte_t const* nxt_data = DecodeChar_(dst, data, data_end, Read);

        if (nxt_data == NULL) {
            return (UTF16_DecodeRet){
                .success = FALSE,
                .nxt_dst = dst,
                .nxt_data = data,
            };
        }

        if (data == nxt_data) {
            return (UTF16_DecodeRet){
                .success = TRUE,
                .nxt_dst = dst,
                .nxt_data = data,
            };
        }

        ++dst;
        data = nxt_data;
    }

    return (UTF16_DecodeRet){
        .success = TRUE,
        .nxt_dst = dst,
        .nxt_data = data,
    };
}
