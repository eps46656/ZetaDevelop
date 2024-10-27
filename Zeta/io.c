#include "io.h"

#include "debugger.h"
#include "utils.h"

static unsigned char lower_chars[] = "0123456789abcdef";

// static unsigned char upper_chars[] = { "0123456789ABCDEF" };

static void Pipe_WriteInt_(Zeta_Pipe* pipe, unsigned long long val,
                           unsigned char sign_char, unsigned base,
                           unsigned char const* chars, int just_dir,
                           size_t just_width, unsigned char just_char) {
    ZETA_DebugAssert(pipe != NULL);

    ZETA_DebugAssert(1 < base);
    ZETA_DebugAssert(base <= 16);

    ZETA_DebugAssert(sign_char == '\0' || sign_char == '-' || sign_char == '+');

    void* context = pipe->context;

    void (*Write)(void* context, size_t cnt, void const* data) = pipe->Write;
    ZETA_DebugAssert(Write != NULL);

    unsigned char buffer[ZETA_WidthOf(unsigned long long)];
    unsigned char* buffer_i = &buffer[0];

    if (val == 0) {
        *(buffer_i++) = chars[0];
    } else {
        for (; 0 < val; val /= base) { *(buffer_i++) = chars[val % base]; }
    }

    if (sign_char != '\0') { *(buffer_i++) = sign_char; }

    size_t width = buffer_i - &buffer[0];

    if (just_dir == ZETA_io_l_just) {
        while (buffer_i-- != &buffer[0]) { Write(context, 1, buffer_i); }
    }

    for (size_t i = width; i < just_width; ++i) {
        Write(context, 1, &just_char);
    }

    if (just_dir == ZETA_io_r_just) {
        while (buffer_i-- != &buffer[0]) { Write(context, 1, buffer_i); }
    }
}

void Zeta_Pipe_WriteUInt(Zeta_Pipe* pipe, unsigned long long val, bool_t sign,
                         unsigned long long base, int just_dir,
                         size_t just_width, unsigned char just_char) {
    Pipe_WriteInt_(pipe, val, sign ? '+' : '\0', base, lower_chars, just_dir,
                   just_width, just_char);
}

void Zeta_Pipe_WriteSInt(Zeta_Pipe* pipe, long long val, bool_t sign,
                         unsigned long long base, int just_dir,
                         size_t just_width, unsigned char just_char) {
    Pipe_WriteInt_(pipe, val < 0 ? -(unsigned long long)val : val,
                   val < 0 ? '-' : (sign ? '+' : '\0'), base, lower_chars,
                   just_dir, just_width, just_char);
}

static size_t CountStrLength_(unsigned char const* str) {
    ZETA_DebugAssert(str != NULL);

    size_t ret = 0;

    for (; *str != '\0'; ++str) { ++ret; }

    return ret;
}

void Zeta_Pipe_WriteStr(Zeta_Pipe* pipe, unsigned char const* str, int just_dir,
                        size_t just_width, unsigned char just_char) {
    ZETA_DebugAssert(pipe != NULL);

    void* context = pipe->context;

    void (*Write)(void* context, size_t cnt, void const* src) = pipe->Write;
    ZETA_DebugAssert(Write != NULL);

    ZETA_DebugAssert(str != NULL);

    size_t str_length = CountStrLength_(str);

    if (just_dir == ZETA_io_l_just) { Write(context, str_length, str); }

    for (size_t i = str_length; i < just_width; ++i) {
        Write(context, 1, &just_char);
    }

    if (just_dir == ZETA_io_r_just) { Write(context, str_length, str); }
}

/*
void Zeta_SIntToBinStr(Zeta_Pipe* pipe, long long val, bool_t sign, int
just_dir, size_t just_width, unsigned char just_char) { IntToStr_(pipe, val, val
< 0 ? '-' : (sign ? '+' : '\0'), 16, lower_chars, just_dir, just_width,
just_char);
}

void Zeta_UIntToBinStr(Zeta_Pipe* pipe, unsigned long long val, bool_t sign,
                       int just_dir, size_t just_width,
                       unsigned char just_char) {
    IntToStr_(pipe, val, sign ? '+' : '\0', 16, lower_chars, just_dir,
              just_width, just_char);
}

void Zeta_SIntToOctStr(Zeta_Pipe* pipe, long long val, bool_t sign, int
just_dir, size_t just_width, unsigned char just_char) { IntToStr_(pipe, val, val
< 0 ? '-' : (sign ? '+' : '\0'), 8, lower_chars, just_dir, just_width,
just_char);
}

void Zeta_UIntToOctStr(Zeta_Pipe* pipe, unsigned long long val, bool_t sign,
                       int just_dir, size_t just_width,
                       unsigned char just_char) {
    IntToStr_(pipe, val, sign ? '+' : '\0', 8, lower_chars, just_dir,
just_width, just_char);
}

void Zeta_SIntToDecStr(Zeta_Pipe* pipe, long long val, bool_t sign, int
just_dir, size_t just_width, unsigned char just_char) { IntToStr_(pipe, val, val
< 0 ? '-' : (sign ? '+' : '\0'), 10, lower_chars, just_dir, just_width,
just_char);
}

void Zeta_UIntToDecStr(Zeta_Pipe* pipe, unsigned long long val, bool_t sign,
                       int just_dir, size_t just_width,
                       unsigned char just_char) {
    IntToStr_(pipe, val, sign ? '+' : '\0', 10, lower_chars, just_dir,
              just_width, just_char);
}

void Zeta_SIntToHexStr(Zeta_Pipe* pipe, long long val, bool_t sign,
                       bool_t uppercase, int just_dir, size_t just_width,
                       unsigned char just_char) {
    IntToStr_(pipe, val, val < 0 ? '-' : (sign ? '+' : '\0'), 16,
              uppercase ? upper_chars : lower_chars, just_dir, just_width,
              just_char);
}

void Zeta_UIntToHexStr(Zeta_Pipe* pipe, unsigned long long val, bool_t sign,
                       bool_t uppercase, int just_dir, size_t just_width,
                       unsigned char just_char) {
    IntToStr_(pipe, val, sign ? '+' : '\0', 16,
              uppercase ? upper_chars : lower_chars, just_dir, just_width,
              just_char);
}

void Zeta_PtrToHexStr(Zeta_Pipe* pipe, void const* val, bool_t uppercase,
                      int just_dir, size_t just_width,
                      unsigned char just_char) {
    return Zeta_UIntToHexStr(pipe, ZETA_PtrToAddr(val), FALSE, uppercase,
                             just_dir, just_width, just_char);
}
*/
