
#include "../Zeta/debugger.h"
#include "../Zeta/utf16.h"

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
    bool_t little_endian = TRUE;

    size_t size = 512 * 1024 * 1024 - 1;

    size_t beg = 0xE000;
    size_t end = 0x10000;

    size_t max = 0x10FFFF;

    ZETA_PrintVar(max);

    if (max + 1 < end) { end = max + 1; }

    size = end - beg;

    ZETA_PrintVar(beg);
    ZETA_PrintVar(end);
    ZETA_PrintVar(size);

    unichar_t* data_uni = malloc(sizeof(unichar_t) * size);

    unichar_t* data_uni_re = malloc(sizeof(unichar_t) * size);
    unichar_t* data_uni_re_beg = data_uni_re;
    unichar_t* data_uni_re_end = data_uni_re + size;

    unsigned char* data_utf16 = malloc(sizeof(unsigned char) * (size * 6 + 1));
    unsigned char* data_utf16_beg = data_utf16;
    unsigned char* data_utf16_end = data_utf16 + (size * 6 + 1);

    for (size_t i = 0; i < size; ++i) { data_uni[i] = beg + i; }

    Zeta_UTF16_EncodeRet en_ret = Zeta_UTF16_Encode(
        size * 6 + 1, data_utf16_beg, size, data_uni, little_endian);

    ZETA_DebugAssert(en_ret.ret_code == ZETA_UTF16_RetCode_success);
    ZETA_DebugAssert(en_ret.nxt_dst != data_utf16_end);
    ZETA_DebugAssert(en_ret.nxt_src == data_uni + size);

    ZETA_PrintVar(en_ret.nxt_dst - data_utf16_beg);

    unsigned char* data_utf16_mid = en_ret.nxt_dst;

    Zeta_UTF16_DecodeRet de_ret = Zeta_UTF16_Decode(
        size, data_uni_re_beg, data_utf16_mid - data_utf16_beg, data_utf16_beg,
        little_endian);

    ZETA_DebugAssert(de_ret.ret_code == ZETA_UTF16_RetCode_success);
    ZETA_DebugAssert(de_ret.nxt_dst == data_uni_re_end);
    ZETA_DebugAssert(de_ret.nxt_src == data_utf16_mid);

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
