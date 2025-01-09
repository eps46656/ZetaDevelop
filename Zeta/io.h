#pragma once

#include "pipe.h"

ZETA_ExternC_Beg;

void Zeta_Pipe_WriteUInt(Zeta_Pipe* dst, unsigned long long val, bool_t sign,
                         unsigned long long base, bool_t lower_case,
                         bool_t just_left, size_t just_width,
                         unsigned char just_char);

void Zeta_Pipe_WriteSInt(Zeta_Pipe* dst, long long val, bool_t sign,
                         unsigned long long base, bool_t lower_case,
                         bool_t just_left, size_t just_width,
                         unsigned char just_char);

void Zeta_Pipe_WriteStr(Zeta_Pipe* dst, unsigned char const* str,
                        bool_t just_left, size_t just_width,
                        unsigned char just_char);

ZETA_ExternC_End;
