#pragma once

#include "Stream.h"

ZETA_ExternC_Beg;

void Zeta_SIntToBinStr(Zeta_Stream* dst, s128_t x, bool_t sign, size_t width,
                       unichar_t padding_char);

void Zeta_UIntToBinStr(Zeta_Stream* dst, u128_t x, bool_t sign, size_t width,
                       unichar_t padding_char);

void Zeta_SIntToOctStr(Zeta_Stream* dst, s128_t x, bool_t sign, size_t width,
                       unichar_t padding_char);

void Zeta_UIntToOctStr(Zeta_Stream* dst, u128_t x, bool_t sign, size_t width,
                       unichar_t padding_char);

void Zeta_SIntToDecStr(Zeta_Stream* dst, s128_t x, bool_t sign, size_t width,
                       unichar_t padding_char);

void Zeta_UIntToDecStr(Zeta_Stream* dst, u128_t x, bool_t sign, size_t width,
                       unichar_t padding_char);

void Zeta_SIntToHexStr(Zeta_Stream* dst, s128_t x, bool_t sign,
                       bool_t uppercase, size_t width, unichar_t padding_char);

void Zeta_UIntToHexStr(Zeta_Stream* dst, u128_t x, bool_t sign,
                       bool_t uppercase, size_t width, unichar_t padding_char);

void Zeta_PtrToHexStr(Zeta_Stream* dst, void const* x, bool_t uppercase,
                      size_t width, unichar_t padding_char);

ZETA_ExternC_End;
