#include "UTF8.h"

#include "utils.h"

static byte_t* EncodeChar_(byte_t* dst, unichar_t data_) {
    if (data_ < 0) { return NULL; }

    u32_t data = data_;

    if (0x7FFFFFFF < data) { return NULL; }

    if (data <= 0b01111111) {
        *dst = data;
        return dst + 1;
    }

    u32_t ub = 0b00011111;
    u32_t lead = 0b111111000000;

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

size_t Zeta_UTF8_GetEncodeSize(unichar_t const* data,
                               unichar_t const* data_end) {
    ZETA_DebugAssert(data != NULL);
    ZETA_DebugAssert(data_end != NULL);
    ZETA_DebugAssert(data <= data_end);

    byte_t tmp[6];

    size_t ret = 0;

    for (; data < data_end; ++data) {
        byte_t* tmp_p = EncodeChar_(tmp, data[0]);
        if (tmp_p == NULL) { return ZETA_RangeMaxOf(size_t); }
        ret += tmp_p - tmp;
    }

    return ret;
}

Zeta_UTF8_EncodeRet Zeta_UTF8_Encode(byte_t* dst, byte_t* dst_end,
                                     unichar_t const* data,
                                     unichar_t const* data_end) {
    ZETA_DebugAssert(dst != NULL);
    ZETA_DebugAssert(dst_end != NULL);
    ZETA_DebugAssert(dst <= dst_end);

    ZETA_DebugAssert(data != NULL);
    ZETA_DebugAssert(data_end != NULL);
    ZETA_DebugAssert(data <= data_end);

    byte_t tmp[6];

    for (; data < data_end; ++data) {
        byte_t* tmp_p = EncodeChar_(tmp, data[0]);

        if (tmp_p == NULL) {
            return (Zeta_UTF8_EncodeRet){ .success = FALSE,
                                          .nxt_dst = dst,
                                          .nxt_data = data };
        }

        diff_t tmp_size = tmp_p - tmp;

        if (dst_end - dst < tmp_size) {
            return (Zeta_UTF8_EncodeRet){
                .success = TRUE,
                .nxt_dst = dst,
                .nxt_data = data,
            };
        }

        Zeta_MemCopy(dst, tmp, tmp_size);
        dst += tmp_size;
    }

    return (Zeta_UTF8_EncodeRet){
        .success = TRUE,
        .nxt_dst = dst,
        .nxt_data = data,
    };
}

static byte_t const* DecodeChar_(unichar_t* dst, byte_t const* data,
                                 byte_t const* data_end) {
    unichar_t x = data[0];

    if (data[0] < 0) {
        ZETA_DebugAssert(FALSE);
        return NULL;
    }

    if (x < 0b10000000) {
        dst[0] = x;
        return data + 1;
    }

    if (x < 0b11000000 || 0b11111101 < x) {
        ZETA_DebugAssert(FALSE);
        return NULL;
    }

    int width;
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

    if (data_end - data < width) { return data; }

    ++data;

    for (int i = 1; i < width; ++i, ++data) {
        unichar_t k = data[0];

        ZETA_DebugAssert(0 <= k);
        ZETA_DebugAssert(k / 0b01000000 == 0b10);

        if (k < 0 || k / 0b01000000 != 0b10) {
            ZETA_DebugAssert(FALSE);
            return NULL;
        }

        ret = ret * 0b01000000 + k % 0b01000000;
    }

    *dst = ret;

    return data;
}

size_t Zeta_UTF8_GetDecodeSize(byte_t const* data, byte_t const* data_end) {
    ZETA_DebugAssert(data != NULL);
    ZETA_DebugAssert(data_end != NULL);
    ZETA_DebugAssert(data <= data_end);

    size_t ret = 0;

    unichar_t dst;

    while (data < data_end) {
        byte_t const* nxt_data = DecodeChar_(&dst, data, data_end);
        if (nxt_data == NULL) { return ZETA_RangeMaxOf(size_t); }
        data = nxt_data;
        ++ret;
    }

    return ret;
}

Zeta_UTF8_DecodeRet Zeta_UTF8_Decode(unichar_t* dst, unichar_t* dst_end,
                                     byte_t const* data,
                                     byte_t const* data_end) {
    ZETA_DebugAssert(dst != NULL);
    ZETA_DebugAssert(dst_end != NULL);
    ZETA_DebugAssert(dst <= dst_end);

    ZETA_DebugAssert(data != NULL);
    ZETA_DebugAssert(data_end != NULL);
    ZETA_DebugAssert(data <= data_end);

    while (dst < dst_end && data < data_end) {
        byte_t const* nxt_data = DecodeChar_(dst, data, data_end);

        if (nxt_data == NULL) {
            return (Zeta_UTF8_DecodeRet){
                .success = FALSE,
                .nxt_dst = dst,
                .nxt_data = data,
            };
        }

        if (data == nxt_data) {
            return (Zeta_UTF8_DecodeRet){
                .success = TRUE,
                .nxt_dst = dst,
                .nxt_data = data,
            };
        }

        ++dst;
        data = nxt_data;
    }

    return (Zeta_UTF8_DecodeRet){
        .success = TRUE,
        .nxt_dst = dst,
        .nxt_data = data,
    };
}
