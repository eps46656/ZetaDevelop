
#include "../Zeta/UTF8.h"

#define BIN_PATTERN "%c%c%c%c%c%c%c%c"

#define BIN_VAL(x)                                          \
    ((x) & 0x80 ? '1' : '0'), ((x) & 0x40 ? '1' : '0'),     \
        ((x) & 0x20 ? '1' : '0'), ((x) & 0x10 ? '1' : '0'), \
        ((x) & 0x8 ? '1' : '0'), ((x) & 0x4 ? '1' : '0'),   \
        ((x) & 0x2 ? '1' : '0'), ((x) & 0x1 ? '1' : '0')

typedef unsigned long long ull;

void Print(byte_t* b, size_t size) {
    for (size_t i = 0; i < size; ++i) {
        byte_t x = b[i];
        printf(BIN_PATTERN " ", BIN_VAL(x));
    }

    printf("\n");
}

int main() {
    size_t size = 32 * 1024;

    size_t beg = 2147483648;
    size_t end = beg + size;

    ZETA_PrintVar("%llu", 0x7FFFFFFF);

    printf("%llu\n", 0x7FFFFFFF);

    if (0x7FFFFFFFull + 1 < end) { end = 0x7FFFFFFFull + 1; }

    ZETA_PrintVar("%llu", (ull)end);

    unichar_t* data_uni = malloc(sizeof(u32_t) * size);

    unichar_t* data_uni_re = malloc(sizeof(u32_t) * size);
    unichar_t* data_uni_re_beg = data_uni_re;
    unichar_t* data_uni_re_end = data_uni_re + size;

    byte_t* data_utf8 = malloc(sizeof(byte_t) * (size * 6 + 1));
    byte_t* data_utf8_beg = data_utf8;
    byte_t* data_utf8_end = data_utf8 + (size * 6 + 1);

    for (size_t i = 0; i < size; ++i) { data_uni[i] = beg + i; }

    Zeta_UTF8_EncodeRet en_ret = Zeta_UTF8_Encode(data_utf8_beg, data_utf8_end,
                                                  data_uni, data_uni + size);

    ZETA_DebugAssert(en_ret.success);
    ZETA_DebugAssert(en_ret.nxt_dst != data_utf8_end);
    ZETA_DebugAssert(en_ret.nxt_src == data_uni + size);

    ZETA_PrintVar("%lld", en_ret.nxt_dst - data_utf8_beg);

    byte_t* data_utf8_mid = en_ret.nxt_dst;

    Zeta_UTF8_DecodeRet de_ret = Zeta_UTF8_Decode(
        data_uni_re_beg, data_uni_re_end, data_utf8_beg, data_utf8_mid);

    ZETA_DebugAssert(de_ret.success);
    ZETA_DebugAssert(de_ret.nxt_dst == data_uni_re_end);
    ZETA_DebugAssert(de_ret.nxt_src == data_utf8_mid);

    // printf("size re = %llu\n", (ull)(data_uni_re_iter - data_uni_re));

    /*
    for (size_t i = 0; i < size; ++i) { printf("%llu ", (ull)data_uni_re[i]); }
    */

    for (size_t i = 0; i < size; ++i) {
        ZETA_DebugAssert(data_uni[i] == data_uni_re[i]);
    }

    printf("ok\n");

    return 0;
}
