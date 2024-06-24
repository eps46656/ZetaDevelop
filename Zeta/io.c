#include "io.h"

#include "utils.h"

static unichar_t lower_chars[] = { '0', '1', '2', '3', '4', '5', '6', '7',
                                   '8', '9', 'a', 'b', 'c', 'd', 'e', 'f' };

static unichar_t upper_chars[] = { '0', '1', '2', '3', '4', '5', '6', '7',
                                   '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };

static int EstDigitWidth_(unsigned long long val, unsigned int base) {
    return val == 0 ? 1 : Zeta_GetFloorLog(val, base) + 1;
}

static void IntToStr_(Zeta_Stream* dst, unsigned long long val,
                      unichar_t sign_char, unsigned int base,
                      unichar_t const* chars, bool_t l_just, size_t just_width,
                      unichar_t just_char) {
    ZETA_DebugAssert(dst != NULL);

    void* dst_context = dst->context;

    void (*Write)(void* context, void const* dsta) = dst->Write;

    size_t cur_width = 0;

    unichar_t c;

    if (!l_just) {
        int sign_width = (sign_char == '-' || sign_char == '+');
        int digit_width = EstDigitWidth_(val, base);

        for (size_t i = sign_width + digit_width; i < just_width; ++i) {
            Write(dst_context, &just_char);
        }
    }

    if (sign_char == '-' || sign_char == '+') {
        Write(dst_context, &sign_char);
        ++cur_width;
    }

    if (val == 0) {
        Write(dst_context, chars);
        ++cur_width;
    } else {
        for (; 0 < val; val /= base, ++cur_width) {
            unsigned long long k = val % base;
            Write(dst_context, chars + k);
        }
    }

    for (; cur_width < just_width; ++cur_width) {
        Write(dst_context, &just_char);
    }
}

void Zeta_UIntToStr(Zeta_Stream* dst, unsigned long long val, bool_t sign,
                    unsigned long long base, bool_t l_just, size_t just_width,
                    unichar_t just_char) {
    ZETA_DebugAssert(1 < base);
    ZETA_DebugAssert(base <= 16);

    IntToStr_(dst, val, sign ? '+' : '\0', base, lower_chars, l_just,
              just_width, just_char);
}

void Zeta_SIntToStr(Zeta_Stream* dst, long long val, bool_t sign,
                    unsigned long long base, bool_t l_just, size_t just_width,
                    unichar_t just_char) {
    ZETA_DebugAssert(1 < base);
    ZETA_DebugAssert(base <= 16);

    IntToStr_(dst, val, val < 0 ? '-' : (sign ? '+' : '\0'), base, lower_chars,
              l_just, just_width, just_char);
}

void Zeta_SIntToBinStr(Zeta_Stream* dst, long long val, bool_t sign,
                       bool_t l_just, size_t just_width, unichar_t just_char) {
    IntToStr_(dst, val, val < 0 ? '-' : (sign ? '+' : '\0'), 16, lower_chars,
              l_just, just_width, just_char);
}

void Zeta_UIntToBinStr(Zeta_Stream* dst, unsigned long long val, bool_t sign,
                       bool_t l_just, size_t just_width, unichar_t just_char) {
    IntToStr_(dst, val, sign ? '+' : '\0', 16, lower_chars, l_just, just_width,
              just_char);
}

void Zeta_SIntToOctStr(Zeta_Stream* dst, long long val, bool_t sign,
                       bool_t l_just, size_t just_width, unichar_t just_char) {
    IntToStr_(dst, val, val < 0 ? '-' : (sign ? '+' : '\0'), 8, lower_chars,
              l_just, just_width, just_char);
}

void Zeta_UIntToOctStr(Zeta_Stream* dst, unsigned long long val, bool_t sign,
                       bool_t l_just, size_t just_width, unichar_t just_char) {
    IntToStr_(dst, val, sign ? '+' : '\0', 8, lower_chars, l_just, just_width,
              just_char);
}

void Zeta_SIntToDecStr(Zeta_Stream* dst, long long val, bool_t sign,
                       bool_t l_just, size_t just_width, unichar_t just_char) {
    IntToStr_(dst, val, val < 0 ? '-' : (sign ? '+' : '\0'), 10, lower_chars,
              l_just, just_width, just_char);
}

void Zeta_UIntToDecStr(Zeta_Stream* dst, unsigned long long val, bool_t sign,
                       bool_t l_just, size_t just_width, unichar_t just_char) {
    IntToStr_(dst, val, sign ? '+' : '\0', 10, lower_chars, l_just, just_width,
              just_char);
}

void Zeta_SIntToHexStr(Zeta_Stream* dst, long long val, bool_t sign,
                       bool_t uppercase, bool_t l_just, size_t just_width,
                       unichar_t just_char) {
    IntToStr_(dst, val, val < 0 ? '-' : (sign ? '+' : '\0'), 16,
              uppercase ? upper_chars : lower_chars, l_just, just_width,
              just_char);
}

void Zeta_UIntToHexStr(Zeta_Stream* dst, unsigned long long val, bool_t sign,
                       bool_t uppercase, bool_t l_just, size_t just_width,
                       unichar_t just_char) {
    IntToStr_(dst, val, sign ? '+' : '\0', 16,
              uppercase ? upper_chars : lower_chars, l_just, just_width,
              just_char);
}

void Zeta_PtrToHexStr(Zeta_Stream* dst, void const* val, bool_t uppercase,
                      bool_t l_just, size_t just_width, unichar_t just_char) {
    return Zeta_UIntToHexStr(dst, ZETA_PtrToAddr(val), FALSE, uppercase, l_just,
                             just_width, just_char);
}
