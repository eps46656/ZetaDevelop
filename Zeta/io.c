#include "io.h"

static void IntToStr_(Zeta_Stream* dst, u128_t x, unichar_t sign_char, int base,
                      unichar_t const* chars, size_t width,
                      unichar_t padding_char) {
    int const TMP_SIZE = 128 + 32;

    unichar_t tmp[TMP_SIZE];
    unichar_t* tmp_p = tmp;

    for (; 0 < x; x /= base) {
        int k = x % base;
        ZETA_DebugAssert(tmp_p != tmp + TMP_SIZE);
        *tmp_p = chars[k];
        ++tmp_p;
    }

    if (sign_char == '-' || sign_char == '+') {
        ZETA_DebugAssert(tmp_p < tmp);
        *tmp_p = sign_char;
        ++tmp_p;
    }

    ZETA_DebugAssert(dst != NULL);

    void* dst_context = dst->context;

    void (*Write)(void* context, void const* data) = dst->Write;
    ZETA_DebugAssert(Write != NULL);

    for (size_t cur_width = tmp_p - tmp; cur_width < width; ++cur_width) {
        Write(dst_context, &padding_char);
    }

    while (tmp_p != tmp) {
        --tmp_p;
        Write(dst_context, tmp_p);
    }
}

void Zeta_SIntToBinStr(Zeta_Stream* dst, s128_t x, bool_t sign, size_t width,
                       unichar_t padding_char) {
    unichar_t chars[] = { '0', '1' };

    IntToStr_(dst, x, x < 0 ? '-' : (sign ? '+' : '\0'), 16, chars, width,
              padding_char);
}

void Zeta_UIntToBinStr(Zeta_Stream* dst, u128_t x, bool_t sign, size_t width,
                       unichar_t padding_char) {
    unichar_t chars[] = { '0', '1' };

    IntToStr_(dst, x, sign ? '+' : '\0', 16, chars, width, padding_char);
}

void Zeta_SIntToOctStr(Zeta_Stream* dst, s128_t x, bool_t sign, size_t width,
                       unichar_t padding_char) {
    unichar_t chars[] = { '0', '1', '2', '3', '4', '5', '6', '7' };

    IntToStr_(dst, x, x < 0 ? '-' : (sign ? '+' : '\0'), 8, chars, width,
              padding_char);
}

void Zeta_UIntToOctStr(Zeta_Stream* dst, u128_t x, bool_t sign, size_t width,
                       unichar_t padding_char) {
    unichar_t chars[] = { '0', '1', '2', '3', '4', '5', '6', '7' };

    IntToStr_(dst, x, sign ? '+' : '\0', 8, chars, width, padding_char);
}

void Zeta_SIntToDecStr(Zeta_Stream* dst, s128_t x, bool_t sign, size_t width,
                       unichar_t padding_char) {
    unichar_t chars[] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9' };

    IntToStr_(dst, x, x < 0 ? '-' : (sign ? '+' : '\0'), 10, chars, width,
              padding_char);
}

void Zeta_UIntToDecStr(Zeta_Stream* dst, u128_t x, bool_t sign, size_t width,
                       unichar_t padding_char) {
    unichar_t chars[] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9' };

    IntToStr_(dst, x, sign ? '+' : '\0', 10, chars, width, padding_char);
}

void Zeta_SIntToHexStr(Zeta_Stream* dst, s128_t x, bool_t sign,
                       bool_t uppercase, size_t width, unichar_t padding_char) {
    unichar_t u_chars[] = { '0', '1', '2', '3', '4', '5', '6', '7',
                            '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };

    unichar_t l_chars[] = { '0', '1', '2', '3', '4', '5', '6', '7',
                            '8', '9', 'a', 'b', 'c', 'd', 'e', 'f' };

    IntToStr_(dst, x, x < 0 ? '-' : (sign ? '+' : '\0'), 16,
              uppercase ? u_chars : l_chars, width, padding_char);
}

void Zeta_UIntToHexStr(Zeta_Stream* dst, u128_t x, bool_t sign,
                       bool_t uppercase, size_t width, unichar_t padding_char) {
    unichar_t u_chars[] = { '0', '1', '2', '3', '4', '5', '6', '7',
                            '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };

    unichar_t l_chars[] = { '0', '1', '2', '3', '4', '5', '6', '7',
                            '8', '9', 'a', 'b', 'c', 'd', 'e', 'f' };

    IntToStr_(dst, x, sign ? '+' : '\0', 16, uppercase ? u_chars : l_chars,
              width, padding_char);
}

void Zeta_PtrToHexStr(Zeta_Stream* dst, void const* x, bool_t uppercase,
                      size_t width, unichar_t padding_char) {
    return Zeta_UIntToHexStr(dst, (uintptr_t)x, FALSE, uppercase, width,
                             padding_char);
}
