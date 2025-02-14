#pragma once

#include "define.h"

ZETA_ExternC_Beg;

void Zeta_HelloWorld();

int Zeta_MemCompare(void const* a, void const* b, size_t size);

/**
 * @brief Copy byte array from dst to src.
 *
 * @param dst The pointer points to destination byte array.
 * @param src The pointer points to source byte array.
 * @param size The number of bytes to be copied.
 */
void Zeta_MemCopy(void* dst, void const* src, size_t size);

/**
 * @brief Copy byte array from dst to src.
 *
 * @param dst The pointer points to destination byte array.
 * @param src The pointer points to source byte array.
 * @param size The number of bytes to be moved.
 */
void Zeta_MemMove(void* dst, void const* src, size_t size);

/**
 * @brief Swap byte array between dst and src
 *
 * @param x The pointer points to the first byte array.
 * @param y The pointer points to the second byte array.
 * @param size The number of bytes to be swapped.
 */
void Zeta_MemSwap(void* x, void* y, size_t size);

/**
 * @brief Rotate the bytes in [beg, end) with mid pivot.
 *
 * @details Let #lv and #rv denote the value of [beg, mid) and [mid, end). The
 * initial value of [beg, end) should be #lv + #rv. After rotation, the value of
 * [beg, end) will become #rv + #lv.
 *
 * @param beg The beginning of byte array.
 * @param mid The pivot of rotation in byte array.
 * @param end The end of byte array.
 *
 * @return The new pivot after rotation.
 */
void* Zeta_MemRotate(void* data, size_t l_size, size_t r_size);

void Zeta_MemReverse(void* data, size_t stride, size_t size);

unsigned long long Zeta_MemHash(void const* data, size_t size,
                                unsigned long long salt);

int Zeta_ElemCompare(void const* a, void const* b, size_t width,
                     size_t a_stride, size_t b_stride, size_t size);

void Zeta_ElemCopy(void* dst, void const* src, size_t width, size_t dst_stride,
                   size_t src_stride, size_t size);

void Zeta_ElemMove(void* dst, void const* src, size_t width, size_t dst_stride,
                   size_t src_stride, size_t size);

void* Zeta_ElemRotate(void* data, size_t width, size_t stride, size_t l_size,
                      size_t r_size);

// -----------------------------------------------------------------------------

#define ZETA_BytesToUInt_LE_case_(tmp_ret, tmp_src, len) \
    case (len): tmp_ret = tmp_ret * 256 + tmp_src[(len) - 1]

#define ZETA_BytesToUInt_BE_case_(tmp_ret, tmp_src, len) \
    case (len): tmp_ret = tmp_ret * 256 + tmp_src[-(len)]

#define ZETA_BytesToUInt_(tmp_ret, tmp_src, tmp_len, tmp_endian, val_type, \
                          src, len, endian)                                \
    ({                                                                     \
        ZETA_StaticAssert(ZETA_WidthOf(val_type) <= 8 * 16);               \
                                                                           \
        ZETA_AutoVar(tmp_src, (src));                                      \
        ZETA_DebugAssert(tmp_src != NULL);                                 \
                                                                           \
        ZETA_AutoVar(tmp_len, (len));                                      \
        ZETA_DebugAssert(0 <= tmp_len);                                    \
        ZETA_DebugAssert(tmp_len <= 8);                                    \
                                                                           \
        ZETA_AutoVar(tmp_endian, (endian));                                \
        ZETA_DebugAssert(tmp_endian == ZETA_LittleEndian ||                \
                         tmp_endian == ZETA_BigEndian);                    \
                                                                           \
        val_type tmp_ret = 0;                                              \
                                                                           \
        if (tmp_endian == ZETA_LittleEndian) {                             \
            switch (tmp_len) {                                             \
                ZETA_BytesToUInt_LE_case_(tmp_ret, tmp_src, 0x10);         \
                ZETA_BytesToUInt_LE_case_(tmp_ret, tmp_src, 0x0f);         \
                ZETA_BytesToUInt_LE_case_(tmp_ret, tmp_src, 0x0e);         \
                ZETA_BytesToUInt_LE_case_(tmp_ret, tmp_src, 0x0d);         \
                ZETA_BytesToUInt_LE_case_(tmp_ret, tmp_src, 0x0c);         \
                ZETA_BytesToUInt_LE_case_(tmp_ret, tmp_src, 0x0b);         \
                ZETA_BytesToUInt_LE_case_(tmp_ret, tmp_src, 0x0a);         \
                ZETA_BytesToUInt_LE_case_(tmp_ret, tmp_src, 0x09);         \
                ZETA_BytesToUInt_LE_case_(tmp_ret, tmp_src, 0x08);         \
                ZETA_BytesToUInt_LE_case_(tmp_ret, tmp_src, 0x07);         \
                ZETA_BytesToUInt_LE_case_(tmp_ret, tmp_src, 0x06);         \
                ZETA_BytesToUInt_LE_case_(tmp_ret, tmp_src, 0x05);         \
                ZETA_BytesToUInt_LE_case_(tmp_ret, tmp_src, 0x04);         \
                ZETA_BytesToUInt_LE_case_(tmp_ret, tmp_src, 0x03);         \
                ZETA_BytesToUInt_LE_case_(tmp_ret, tmp_src, 0x02);         \
                ZETA_BytesToUInt_LE_case_(tmp_ret, tmp_src, 0x01);         \
                case 0x0:                                                  \
            }                                                              \
        } else {                                                           \
            tmp_src += tmp_len;                                            \
                                                                           \
            switch (tmp_len) {                                             \
                ZETA_BytesToUInt_BE_case_(tmp_ret, tmp_src, 0x10);         \
                ZETA_BytesToUInt_BE_case_(tmp_ret, tmp_src, 0x0f);         \
                ZETA_BytesToUInt_BE_case_(tmp_ret, tmp_src, 0x0e);         \
                ZETA_BytesToUInt_BE_case_(tmp_ret, tmp_src, 0x0d);         \
                ZETA_BytesToUInt_BE_case_(tmp_ret, tmp_src, 0x0c);         \
                ZETA_BytesToUInt_BE_case_(tmp_ret, tmp_src, 0x0b);         \
                ZETA_BytesToUInt_BE_case_(tmp_ret, tmp_src, 0x0a);         \
                ZETA_BytesToUInt_BE_case_(tmp_ret, tmp_src, 0x09);         \
                ZETA_BytesToUInt_BE_case_(tmp_ret, tmp_src, 0x08);         \
                ZETA_BytesToUInt_BE_case_(tmp_ret, tmp_src, 0x07);         \
                ZETA_BytesToUInt_BE_case_(tmp_ret, tmp_src, 0x06);         \
                ZETA_BytesToUInt_BE_case_(tmp_ret, tmp_src, 0x05);         \
                ZETA_BytesToUInt_BE_case_(tmp_ret, tmp_src, 0x04);         \
                ZETA_BytesToUInt_BE_case_(tmp_ret, tmp_src, 0x03);         \
                ZETA_BytesToUInt_BE_case_(tmp_ret, tmp_src, 0x02);         \
                ZETA_BytesToUInt_BE_case_(tmp_ret, tmp_src, 0x01);         \
                case 0x0:                                                  \
            }                                                              \
        }                                                                  \
                                                                           \
        tmp_ret;                                                           \
    })

#define ZETA_BytesToUInt(val_type, src, len, endian)                          \
    ZETA_BytesToUInt_(ZETA_TmpName, ZETA_TmpName, ZETA_TmpName, ZETA_TmpName, \
                      val_type, (src), (len), (endian))

// -----------------------------------------------------------------------------

#define ZETA_UIntToBytes_LE_case_(tmp_val, tmp_dst, len) \
    case (len): tmp_dst[-(len)] = tmp_val % 256; tmp_val /= 256

#define ZETA_UIntToBytes_BE_case_(tmp_val, tmp_dst, len) \
    case (len): tmp_dst[(len) - 1] = tmp_val % 256; tmp_val /= 256

#define ZETA_UIntToBytes_(tmp_ret, tmp_src, tmp_len, tmp_endian, val, dst, \
                          len, endian)                                     \
    ({                                                                     \
        ZETA_AutoVar(tmp_val, (val));                                      \
                                                                           \
        ZETA_AutoVar(tmp_dst, (dst));                                      \
        ZETA_DebugAssert(tmp_dst != NULL);                                 \
                                                                           \
        ZETA_AutoVar(tmp_len, (len));                                      \
        ZETA_DebugAssert(0 <= tmp_len);                                    \
        ZETA_DebugAssert(tmp_len <= 16);                                   \
                                                                           \
        ZETA_AutoVar(tmp_endian, (endian));                                \
        ZETA_DebugAssert(tmp_endian == ZETA_LittleEndian ||                \
                         tmp_endian == ZETA_BigEndian);                    \
                                                                           \
        if (tmp_endian == ZETA_LittleEndian) {                             \
            tmp_dst += tmp_len;                                            \
                                                                           \
            switch (tmp_len) {                                             \
                ZETA_UIntToBytes_LE_case_(tmp_val, tmp_dst, 0x10);         \
                ZETA_UIntToBytes_LE_case_(tmp_val, tmp_dst, 0x0f);         \
                ZETA_UIntToBytes_LE_case_(tmp_val, tmp_dst, 0x0e);         \
                ZETA_UIntToBytes_LE_case_(tmp_val, tmp_dst, 0x0d);         \
                ZETA_UIntToBytes_LE_case_(tmp_val, tmp_dst, 0x0c);         \
                ZETA_UIntToBytes_LE_case_(tmp_val, tmp_dst, 0x0b);         \
                ZETA_UIntToBytes_LE_case_(tmp_val, tmp_dst, 0x0a);         \
                ZETA_UIntToBytes_LE_case_(tmp_val, tmp_dst, 0x09);         \
                ZETA_UIntToBytes_LE_case_(tmp_val, tmp_dst, 0x08);         \
                ZETA_UIntToBytes_LE_case_(tmp_val, tmp_dst, 0x07);         \
                ZETA_UIntToBytes_LE_case_(tmp_val, tmp_dst, 0x06);         \
                ZETA_UIntToBytes_LE_case_(tmp_val, tmp_dst, 0x05);         \
                ZETA_UIntToBytes_LE_case_(tmp_val, tmp_dst, 0x04);         \
                ZETA_UIntToBytes_LE_case_(tmp_val, tmp_dst, 0x03);         \
                ZETA_UIntToBytes_LE_case_(tmp_val, tmp_dst, 0x02);         \
                ZETA_UIntToBytes_LE_case_(tmp_val, tmp_dst, 0x01);         \
                case 0x0:                                                  \
            }                                                              \
        } else {                                                           \
            switch (tmp_len) {                                             \
                ZETA_UIntToBytes_BE_case_(tmp_val, tmp_dst, 0x10);         \
                ZETA_UIntToBytes_BE_case_(tmp_val, tmp_dst, 0x0f);         \
                ZETA_UIntToBytes_BE_case_(tmp_val, tmp_dst, 0x0e);         \
                ZETA_UIntToBytes_BE_case_(tmp_val, tmp_dst, 0x0d);         \
                ZETA_UIntToBytes_BE_case_(tmp_val, tmp_dst, 0x0c);         \
                ZETA_UIntToBytes_BE_case_(tmp_val, tmp_dst, 0x0b);         \
                ZETA_UIntToBytes_BE_case_(tmp_val, tmp_dst, 0x0a);         \
                ZETA_UIntToBytes_BE_case_(tmp_val, tmp_dst, 0x09);         \
                ZETA_UIntToBytes_BE_case_(tmp_val, tmp_dst, 0x08);         \
                ZETA_UIntToBytes_BE_case_(tmp_val, tmp_dst, 0x07);         \
                ZETA_UIntToBytes_BE_case_(tmp_val, tmp_dst, 0x06);         \
                ZETA_UIntToBytes_BE_case_(tmp_val, tmp_dst, 0x05);         \
                ZETA_UIntToBytes_BE_case_(tmp_val, tmp_dst, 0x04);         \
                ZETA_UIntToBytes_BE_case_(tmp_val, tmp_dst, 0x03);         \
                ZETA_UIntToBytes_BE_case_(tmp_val, tmp_dst, 0x02);         \
                ZETA_UIntToBytes_BE_case_(tmp_val, tmp_dst, 0x01);         \
                case 0x0:                                                  \
            }                                                              \
        }                                                                  \
    })

#define ZETA_UIntToBytes(val, dst, len, endian)                               \
    ZETA_UIntToBytes_(ZETA_TmpName, ZETA_TmpName, ZETA_TmpName, ZETA_TmpName, \
                      (val), (dst), (len), (endian))

// -----------------------------------------------------------------------------

/**
 * @brief Read little endian stored length bytes occupying integer from data.
 *
 * @param src The byte array stores integer.
 * @param length The number of bytes the integer occupied.
 *
 * @return The read integer
 */
u128_t Zeta_ReadLittleEndian(byte_t const* src, unsigned length);

/**
 * @brief Write an integer to data in little endian occupying length bytes.
 *
 * @param dst The destination byte array.
 * @param length The number of bytes the integer occupied.
 * @param val The target integer.
 *
 * @return The remaining value after
 */
u128_t Zeta_WriteLittleEndian(byte_t* dst, u128_t val, unsigned length);

/**
 * @brief Read big endian stored length bytes occupying integer from data.
 *
 * @param src The byte array stores integer.
 * @param length The number of bytes the integer occupied.
 *
 * @return The read integer
 */
u128_t Zeta_ReadBigEndian(byte_t const* src, unsigned length);

/**
 * @brief Write an integer to data in big endian occupying length bytes.
 *
 * @param dst The destination byte array.
 * @param length The number of bytes the integer occupied.
 * @param val The target integer.
 */
u128_t Zeta_WriteBigEndian(byte_t* dst, u128_t val, unsigned length);

byte_t const* Zeta_ReadStr(byte_t const* src, size_t src_size);

unsigned long long Zeta_ULLHash(unsigned long long x, unsigned long long salt);

unsigned long long Zeta_LLHash(long long x, unsigned long long salt);

unsigned long long Zeta_GetRandom();

unsigned long long Zeta_SimpleRandom(unsigned long long x);

unsigned long long Zeta_SimpleRandomRotate(unsigned long long* x);

unsigned long long Zeta_GCD(unsigned long long x, unsigned long long y);

unsigned long long Zeta_LCM(unsigned long long x, unsigned long long y);

/**
 * @brief Calculate base^exp.
 *
 * @param base The base of base^exp.
 * @param exp The exp of base^exp.
 *
 * @return base^exp.
 */
unsigned long long Zeta_Power(unsigned long long base, unsigned exp);

/**
 * @brief Calculate base^exp % mod.
 *
 * @param base The base of base^exp % mod.
 * @param exp The exp of base^exp % mod.
 * @param mod The mod of base^exp % mod.
 *
 * @return base^exp % mod.
 */
unsigned long long Zeta_PowerMod(unsigned long long base,
                                 unsigned long long exp,
                                 unsigned long long mod);

unsigned long long Zeta_FixedPoint2Power(long long val);

unsigned Zeta_FloorLog(unsigned long long val, unsigned long long base);

unsigned Zeta_CeilLog(unsigned long long val, unsigned long long base);

long long Zeta_FixedPointLog2(unsigned long long val);

unsigned long long Zeta_FloorSqrt(unsigned long long val);

unsigned long long Zeta_CeilSqrt(unsigned long long val);

unsigned long long Zeta_FixedPointSqrt(unsigned long long val);

unsigned long long Zeta_FindNextConMod(unsigned long long beg,
                                       unsigned long long target,
                                       unsigned long long mod);

void* Zeta_GetMostLink(void* n, void* (*GetLink)(void* n));

int Zeta_Choose2(bool_t a_cond, bool_t b_cond, unsigned long long* random_seed);

int Zeta_Choose3(bool_t a_cond, bool_t b_cond, bool_t c_cond,
                 unsigned long long* random_seed);

ZETA_ExternC_End;
