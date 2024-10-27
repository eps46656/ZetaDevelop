
#include "../Zeta/debugger.h"
#include "../Zeta/utf8.h"

#define BIN_PATTERN "%c%c%c%c%c%c%c%c"

#define BIN_VAL(x)                                          \
    ((x) & 0x80 ? '1' : '0'), ((x) & 0x40 ? '1' : '0'),     \
        ((x) & 0x20 ? '1' : '0'), ((x) & 0x10 ? '1' : '0'), \
        ((x) & 0x8 ? '1' : '0'), ((x) & 0x4 ? '1' : '0'),   \
        ((x) & 0x2 ? '1' : '0'), ((x) & 0x1 ? '1' : '0')

typedef unsigned long long ull;

void Print(unsigned char* b, size_t size) {
    for (size_t i = 0; i < size; ++i) {
        unsigned char x = b[i];
        printf(BIN_PATTERN " ", BIN_VAL(x));
    }

    printf("\n");
}

int main() {
    size_t size = 512 * 1024 * 1024 - 1;

    size_t beg = 1677721600;
    size_t end = beg + size;

    ZETA_PrintVar(0x7FFFFFFF);

    if (0x7FFFFFFFull + 1 < end) { end = 0x7FFFFFFFull + 1; }

    size = end - beg;

    ZETA_PrintVar((ull)end);

    unichar_t* data_uni = malloc(sizeof(unichar_t) * size);

    unichar_t* data_uni_re = malloc(sizeof(unichar_t) * size);
    unichar_t* data_uni_re_beg = data_uni_re;
    unichar_t* data_uni_re_end = data_uni_re + size;

    unsigned char* data_utf8 = malloc(sizeof(unsigned char) * (size * 6 + 1));
    unsigned char* data_utf8_beg = data_utf8;
    unsigned char* data_utf8_end = data_utf8 + (size * 6 + 1);

    for (size_t i = 0; i < size; ++i) { data_uni[i] = beg + i; }

    Zeta_UTF8_EncodeRet en_ret =
        Zeta_UTF8_Encode(size * 6 + 1, data_utf8_beg, size, data_uni);

    ZETA_DebugAssert(en_ret.ret_code == ZETA_UTF8_RetCode_success);
    ZETA_DebugAssert(en_ret.nxt_dst != data_utf8_end);
    ZETA_DebugAssert(en_ret.nxt_src == data_uni + size);

    ZETA_PrintVar(en_ret.nxt_dst - data_utf8_beg);

    unsigned char* data_utf8_mid = en_ret.nxt_dst;

    Zeta_UTF8_DecodeRet de_ret = Zeta_UTF8_Decode(
        size, data_uni_re_beg, en_ret.nxt_dst - data_utf8_beg, data_utf8_beg);

    ZETA_DebugAssert(de_ret.ret_code == ZETA_UTF8_RetCode_success);
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
