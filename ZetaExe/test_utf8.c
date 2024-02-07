
#include "../Zeta/define.h"

#define BIN_PATTERN "%c%c%c%c%c%c%c%c"

#define BIN_VAL(x)                                          \
    ((x) & 0x80 ? '1' : '0'), ((x) & 0x40 ? '1' : '0'),     \
        ((x) & 0x20 ? '1' : '0'), ((x) & 0x10 ? '1' : '0'), \
        ((x) & 0x8 ? '1' : '0'), ((x) & 0x4 ? '1' : '0'),   \
        ((x) & 0x2 ? '1' : '0'), ((x) & 0x1 ? '1' : '0')

typedef unsigned long long ull;

byte_t* Zeta_UTF8_Encode(byte_t* dst, u32_t c);
byte_t* Zeta_UTF8_Decode(u32_t* dst, byte_t* data, size_t length);

byte_t* Zeta_UTF8_Encode(byte_t* dst, u32_t c) {
    ZETA_DebugAssert(dst != NULL);

    if ((u32_t)0x80000000 <= c) { return NULL; }

    if (c <= (u32_t)0x7F) {
        dst[0] = (byte_t)c;
        return dst + 1;
    }

    byte_t tmp[6];

    for (int i = 2;; ++i) {
        tmp[i - 2] = (byte_t)((u32_t)0b10000000 | (c & (u32_t)0b00111111));

        c >>= 6;

        if (c < ((u32_t)1 << (7 - i))) {
            tmp[i - 1] = (byte_t)((((u32_t)0xFF00 >> i) & (u32_t)0xFF) | c);

            for (; 0 <= --i; ++dst) { dst[0] = tmp[i]; }

            return dst;
        }
    }
}

byte_t* Zeta_UTF8_Decode(u32_t* dst, byte_t* data, size_t size) {
    ZETA_DebugAssert(dst != NULL);
    ZETA_DebugAssert(data != NULL);

    if (size == 0) { return NULL; }

    byte_t* data_end = data + size;

    u32_t x = data[0];

    if (x < (u32_t)0x80) {
        dst[0] = x;
        return data + 1;
    }

    if (x < 0b11000000) { return NULL; }

    u32_t ret = 0;

    ++data;

    for (int i = 2;; ++i) {
        if (data == data_end) { return NULL; }

        u32_t k = data[0];
        ++data;

        if ((k & (u32_t)0b11000000) != 0b10000000) { return NULL; }

        ret = (ret << 6) | (k & (u32_t)0b00111111);

        if (x <= (((u32_t)0xFF7F >> i) & (u32_t)0xFF)) {
            ret |= (x & ((u32_t)0x7F >> i)) << (6 * (i - 1));
            break;
        }
    }

    *dst = ret;

    return data;
}

void Print(byte_t* b, size_t size) {
    for (size_t i = 0; i < size; ++i) {
        byte_t x = b[i];
        printf(BIN_PATTERN " ", BIN_VAL(x));
    }

    printf("\n");
}

int main() {
    size_t size = 1;

    size_t beg = 2147483648;
    size_t end = beg + size;

    u32_t* data_uni = malloc(sizeof(u32_t) * size);

    u32_t* data_uni_re = malloc(sizeof(u32_t) * size);
    u32_t* data_uni_re_iter = data_uni_re;

    byte_t* data_utf8 = malloc(sizeof(byte_t) * size * 6);
    byte_t* data_utf8_iter = data_utf8;

    printf("end = %llu\n", (ull)end);

    for (size_t i = 0; i < size; ++i) { data_uni[i] = beg + i; }

    /*
    for (size_t i = 0; i < size; ++i) {
        printf("%llu ", (unsigned long long)data_uni[i]);
    }

    printf("\n");
    */

    for (size_t i = 0; i < size; ++i) {
        data_utf8_iter = Zeta_UTF8_Encode(data_utf8_iter, data_uni[i]);
    }

    // Print(data_utf8, data_utf8_iter - data_utf8);

    byte_t* data_utf8_end = data_utf8_iter;
    data_utf8_iter = data_utf8;

    while (data_utf8_iter != data_utf8_end) {
        data_utf8_iter = Zeta_UTF8_Decode(data_uni_re_iter, data_utf8_iter,
                                          data_utf8_end - data_utf8_iter);
        ++data_uni_re_iter;
    }

    // printf("size re = %llu\n", (ull)(data_uni_re_iter - data_uni_re));

    /*
    for (size_t i = 0; i < size; ++i) { printf("%llu ", (ull)data_uni_re[i]); }
    */

    ZETA_DebugAssert(data_uni_re + size == data_uni_re_iter);
    ZETA_DebugAssert(0 == 0);

    for (size_t i = 0; i < size; ++i) {
        ZETA_DebugAssert(data_uni[i] == data_uni_re[i]);
    }

    printf("ok\n");

    return 0;
}
