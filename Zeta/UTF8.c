#include "UTF8.h"

#include "utils.h"

typedef struct DecodeRet DecodeRet;

struct DecodeRet {
    int state;
    unichar_t dst;
    const byte_t* nxt_src;
};

static byte_t* EncodeChar_(byte_t* dst, unichar_t src_) {
    if (src_ < 0) { return NULL; }

    u32_t src = src_;

    if (0x80000000 <= src) { return NULL; }

    if (src < 0b10000000) {
        dst[0] = (byte_t)src;
        return dst + 1;
    }

    byte_t tmp[6];

    u32_t ub = 0b00100000;
    u32_t mask = 0b111111000000;

    for (int i = 2;; ++i, ub /= 2, mask /= 2) {
        tmp[i - 2] = (byte_t)(0b10000000 + src % 0b01000000);

        src /= 0b01000000;

        if (src < ub) {
            tmp[i - 1] = (byte_t)(mask % 0b100000000 + src);

            for (; 0 <= --i; ++dst) { dst[0] = tmp[i]; }

            return dst;
        }
    }
}

int Zeta_UTF8_Encode(Zeta_Stream* dst, Zeta_Stream* src) {
    ZETA_DebugAssert(dst != NULL);
    ZETA_DebugAssert(src != NULL);

    void* dst_context = dst->context;

    void (*DstWrite)(void* context, const void* data) = dst->Write;
    ZETA_DebugAssert(DstWrite != NULL);

    void* src_context = src->context;

    bool_t (*SrcIsEmpty)(void* context) = src->IsEmpty;
    const void* (*SrcPeek)(void* context) = src->Peek;
    void (*SrcRead)(void* context) = src->Read;

    ZETA_DebugAssert(SrcIsEmpty != NULL);
    ZETA_DebugAssert(SrcPeek != NULL);
    ZETA_DebugAssert(SrcRead != NULL);

    byte_t tmp[6];

    while (!SrcIsEmpty(src_context)) {
        byte_t* tmp_p =
            EncodeChar_(tmp, *(const unichar_t*)SrcPeek(src_context));

        if (tmp_p == NULL) { return 1; }

        SrcRead(src_context);

        for (byte_t* i = tmp; i < tmp_p; ++i) { DstWrite(dst_context, i); }
    }

    return 0;
}

static bool_t DecodeChar_(unichar_t* dst, Zeta_Stream* src) {
    void* src_context = src->context;

    bool_t (*IsEmpty)(void* context) = src->IsEmpty;
    const void* (*Peek)(void* context) = src->Peek;
    void (*Read)(void* context) = src->Read;
    void (*Unread)(void* context, const void* data) = src->Unread;

    ZETA_DebugAssert(IsEmpty != NULL);
    ZETA_DebugAssert(Peek != NULL);
    ZETA_DebugAssert(Read != NULL);
    ZETA_DebugAssert(Unread != NULL);

    unichar_t tmp[6];
    tmp[0] = *(const unichar_t*)Peek(src_context);

    u32_t x = tmp[0];

    if (x < 0) { return FALSE; }

    if (x < 0b10000000) {
        dst[0] = x;
        Read(src_context);
        return TRUE;
    }

    if (x < 0b11000000 || 0b11111101 < x) { return FALSE; }

    size_t width;
    u32_t ret;

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

    Read(src_context);
    unichar_t* tmp_p = tmp + 1;

    for (unsigned int i = 1; i < width; ++i) {
        if (IsEmpty(src_context)) { goto FAILED_RET; }

        *tmp_p = *(const unichar_t*)Peek(src_context);

        u32_t k = *tmp_p;

        if (k < 0 || k / 0b01000000 != 0b10) { goto FAILED_RET; }

        Read(src_context);
        ++tmp_p;

        ret = ret * 0b01000000 + k % 0b01000000;
    }

    *dst = ret;

    return TRUE;

FAILED_RET:;

    while (tmp_p != tmp) {
        --tmp_p;
        Unread(src_context, tmp_p);
    }

    return FALSE;
}

int Zeta_UTF8_Decode(Zeta_Stream* dst, Zeta_Stream* src) {
    ZETA_DebugAssert(dst != NULL);
    ZETA_DebugAssert(src != NULL);

    void* dst_context = dst->context;

    void (*DstWrite)(void* context, const void* data) = dst->Write;
    ZETA_DebugAssert(DstWrite != NULL);

    void* src_context = src->context;

    bool_t (*SrcIsEmpty)(void* context) = src->IsEmpty;
    ZETA_DebugAssert(SrcIsEmpty != NULL);

    unichar_t tmp;

    while (!SrcIsEmpty(src_context)) {
        if (!DecodeChar_(&tmp, src)) { return 1; }
        DstWrite(dst_context, &tmp);
    }

    return 0;
}
