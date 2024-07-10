#pragma once

#include "Pipe.h"

ZETA_ExternC_Beg;

void Zeta_UIntToStr(Zeta_Pipe* dst, unsigned long long val, bool_t sign,
                    unsigned long long base, bool_t l_just, size_t just_width,
                    unichar_t just_char);

void Zeta_SIntToStr(Zeta_Pipe* dst, long long val, bool_t sign,
                    unsigned long long base, bool_t l_just, size_t just_width,
                    unichar_t just_char);

//

void Zeta_SIntToBinStr(Zeta_Pipe* dst, long long val, bool_t sign,
                       bool_t l_just, size_t just_width, unichar_t just_char);

void Zeta_UIntToBinStr(Zeta_Pipe* dst, unsigned long long val, bool_t sign,
                       bool_t l_just, size_t just_width, unichar_t just_char);

void Zeta_SIntToOctStr(Zeta_Pipe* dst, long long val, bool_t sign,
                       bool_t l_just, size_t just_width, unichar_t just_char);

void Zeta_UIntToOctStr(Zeta_Pipe* dst, unsigned long long val, bool_t sign,
                       bool_t l_just, size_t just_width, unichar_t just_char);

void Zeta_SIntToDecStr(Zeta_Pipe* dst, long long val, bool_t sign,
                       bool_t l_just, size_t just_width, unichar_t just_char);

void Zeta_UIntToDecStr(Zeta_Pipe* dst, unsigned long long val, bool_t sign,
                       bool_t l_just, size_t just_width, unichar_t just_char);

void Zeta_SIntToHexStr(Zeta_Pipe* dst, long long x, bool_t sign,
                       bool_t uppercase, bool_t l_just, size_t just_width,
                       unichar_t just_char);

void Zeta_UIntToHexStr(Zeta_Pipe* dst, unsigned long long val, bool_t sign,
                       bool_t uppercase, bool_t l_just, size_t just_width,
                       unichar_t just_char);

void Zeta_PtrToHexStr(Zeta_Pipe* dst, void const* val, bool_t uppercase,
                      bool_t l_just, size_t just_width, unichar_t just_char);

ZETA_ExternC_End;
