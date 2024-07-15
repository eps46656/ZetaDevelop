#pragma once

#include "Pipe.h"

ZETA_ExternC_Beg;

#define ZETA_io_l_just (0)

#define ZETA_io_r_just (1)

void Zeta_Pipe_WriteUInt(Zeta_Pipe* dst, unsigned long long val, bool_t sign,
                         unsigned long long base, int just_dir,
                         size_t just_width, unsigned char just_char);

void Zeta_Pipe_WriteSInt(Zeta_Pipe* dst, long long val, bool_t sign,
                         unsigned long long base, int just_dir,
                         size_t just_width, unsigned char just_char);

void Zeta_Pipe_WriteStr(Zeta_Pipe* dst, unsigned char const* str, int just_dir,
                        size_t just_width, unsigned char just_char);

/*
void Zeta_SIntToBinStr(Zeta_Pipe* dst, long long val, bool_t sign, int just_dir,
                       size_t just_width, unsigned char just_char);

void Zeta_UIntToBinStr(Zeta_Pipe* dst, unsigned long long val, bool_t sign,
                       int just_dir, size_t just_width,
                       unsigned char just_char);

void Zeta_SIntToOctStr(Zeta_Pipe* dst, long long val, bool_t sign, int just_dir,
                       size_t just_width, unsigned char just_char);

void Zeta_UIntToOctStr(Zeta_Pipe* dst, unsigned long long val, bool_t sign,
                       int just_dir, size_t just_width,
                       unsigned char just_char);

void Zeta_SIntToDecStr(Zeta_Pipe* dst, long long val, bool_t sign, int just_dir,
                       size_t just_width, unsigned char just_char);

void Zeta_UIntToDecStr(Zeta_Pipe* dst, unsigned long long val, bool_t sign,
                       int just_dir, size_t just_width,
                       unsigned char just_char);

void Zeta_SIntToHexStr(Zeta_Pipe* dst, long long x, bool_t sign,
                       bool_t uppercase, int just_dir, size_t just_width,
                       unsigned char just_char);

void Zeta_UIntToHexStr(Zeta_Pipe* dst, unsigned long long val, bool_t sign,
                       bool_t uppercase, int just_dir, size_t just_width,
                       unsigned char just_char);

void Zeta_PtrToHexStr(Zeta_Pipe* dst, void const* val, bool_t uppercase,
                      int just_dir, size_t just_width, unsigned char just_char);
*/

ZETA_ExternC_End;
