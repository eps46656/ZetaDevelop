#pragma once

#include "define.h"

ZETA_ExternC_Beg;

void Zeta_HelloWorld();

/**
 * @brief Copy byte array from dst to src
 *
 * @param dst The pointer points to destination byte array.
 * @param src The pointer points to source byte array.
 * @param size The number of bytes the byte arrays pointed by dst and src.
 */
void Zeta_MemCopy(void* dst, void const* src, size_t size);

/**
 * @brief Copy byte array from dst to src
 *
 * @param dst The pointer points to destination byte array.
 * @param src The pointer points to source byte array.
 * @param size The number of bytes the byte arrays pointed by dst and src.
 */
void Zeta_MemMove(void* dst, void const* src, size_t size);

/**
 * @brief Swap byte array between dst and src
 *
 * @param x The pointer points to the first byte array.
 * @param y The pointer points to the second byte array.
 * @param size The number of bytes the byte arrays pointed by x and y.
 */
void Zeta_MemSwap(void* x, void* y, size_t size);

/**
 * @brief Rotate the bytes in [beg, end). mid should always in [beg, end].
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
void* Zeta_MemRotate(void* beg, void* mid, void* end);

void Zeta_MemReverse(void* data, size_t stride, size_t size);

unsigned long long Zeta_MemHash(void* data, size_t size);

void Zeta_ElemCopy(void* dst, void const* src, size_t stride, size_t width,
                   size_t size);

void Zeta_ElemMove(void* dst, void const* src, size_t stride, size_t width,
                   size_t size);

#define ZETA_ReadLittleEndian4_(src_tmp, src)                      \
    ({                                                             \
        byte_t const* src_tmp = (void const*)(src);                \
        ZETA_DebugAssert(src_tmp != NULL);                         \
                                                                   \
        ((u32_t)src_tmp[0] << 0) | ((u32_t)src_tmp[1] << 8) |      \
            ((u32_t)src_tmp[2] << 16) | ((u32_t)src_tmp[3] << 24); \
    })

#define ZETA_ReadLittleEndian4(src) ZETA_ReadLittleEndian4_(ZETA_TmpName, (src))

#define ZETA_ReadLittleEndian8_(src_tmp, src)                       \
    ({                                                              \
        byte_t const* src_tmp = (void const*)(src);                 \
        ZETA_DebugAssert(src_tmp != NULL);                          \
                                                                    \
        ((u64_t)src_tmp[0] << 0) | ((u64_t)src_tmp[1] << 8) |       \
            ((u64_t)src_tmp[2] << 16) | ((u64_t)src_tmp[3] << 24) | \
            ((u64_t)src_tmp[4] << 32) | ((u64_t)src_tmp[5] << 40) | \
            ((u64_t)src_tmp[6] << 48) | ((u64_t)src_tmp[7] << 56);  \
    })

#define ZETA_ReadLittleEndian8(src) ZETA_ReadLittleEndian8_(ZETA_TmpName, (src))

#define ZETA_ReadBigEndian4_(src_tmp, src)                       \
    ({                                                           \
        byte_t const* src_tmp = (void const*)(src);              \
        ZETA_DebugAssert(src_tmp != NULL);                       \
                                                                 \
        ((u32_t)src_tmp[0] << 24) | ((u32_t)src_tmp[1] << 16) |  \
            ((u32_t)src_tmp[2] << 8) | ((u32_t)src_tmp[3] << 0); \
    })

#define ZETA_ReadBigEndian4(src) ZETA_ReadBigEndian4_(ZETA_TmpName, (src))

#define ZETA_ReadBigEndian8_(src_tmp, src)                          \
    ({                                                              \
        byte_t const* src_tmp = (void const*)(src);                 \
        ZETA_DebugAssert(src_tmp != NULL);                          \
                                                                    \
        ((u64_t)src_tmp[0] << 56) | ((u64_t)src_tmp[1] << 48) |     \
            ((u64_t)src_tmp[2] << 40) | ((u64_t)src_tmp[3] << 32) | \
            ((u64_t)src_tmp[4] << 24) | ((u64_t)src_tmp[5] << 16) | \
            ((u64_t)src_tmp[6] << 8) | ((u64_t)src_tmp[7] << 0);    \
    })

#define ZETA_ReadBigEndian8(src) ZETA_ReadBigEndian8_(ZETA_TmpName, (src))

/**
 * @brief Read little endian stored length bytes occupying integer from data.
 *
 * @param data The byte array stores integer.
 * @param length The number of bytes the integer occupied.
 *
 * @return The read integer
 */
unsigned long long Zeta_ReadLittleEndianULL(byte_t const* data,
                                            unsigned length);

/**
 * @brief Read little endian stored length bytes occupying integer from data.
 *
 * @param data The byte array stores integer.
 * @param length The number of bytes the integer occupied.
 *
 * @return The read integer
 */
u128_t Zeta_ReadLittleEndian(byte_t const* data, unsigned length);

/**
 * @brief Write an integer to data in little endian occupying length bytes.
 *
 * @param dst The destination byte array.
 * @param length The number of bytes the integer occupied.
 * @param val The target integer.
 *
 * @return The remaining value after
 */
unsigned long long Zeta_WriteLittleEndianULL(byte_t* dst,
                                             unsigned long long val,
                                             unsigned length);

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
 * @param data The byte array stores integer.
 * @param length The number of bytes the integer occupied.
 *
 * @return The read integer
 */
unsigned long long Zeta_ReadBigEndianULL(byte_t const* data, unsigned length);

/**
 * @brief Read big endian stored length bytes occupying integer from data.
 *
 * @param data The byte array stores integer.
 * @param length The number of bytes the integer occupied.
 *
 * @return The read integer
 */
u128_t Zeta_ReadBigEndian(byte_t const* data, unsigned length);

/**
 * @brief Write an integer to data in big endian occupying length bytes.
 *
 * @param dst The destination byte array.
 * @param length The number of bytes the integer occupied.
 * @param val The target integer.
 */
unsigned long long Zeta_WriteBigEndianULL(byte_t* dst, unsigned long long val,
                                          unsigned length);

/**
 * @brief Write an integer to data in big endian occupying length bytes.
 *
 * @param dst The destination byte array.
 * @param length The number of bytes the integer occupied.
 * @param val The target integer.
 */
u128_t Zeta_WriteBigEndian(byte_t* dst, u128_t val, unsigned length);

unsigned long long Zeta_SimpleRandom(unsigned long long x);

unsigned long long Zeta_SimpleRandomRotate(unsigned long long* x);

unsigned long long Zeta_SimpleHash(unsigned long long x);

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
unsigned long long Zeta_Power(unsigned long long base, unsigned long long exp);

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

void* Zeta_GetMostLink(void* context, void* (*GetLink)(void* context, void* n),
                       void* n);

int Zeta_Choose2(bool_t a_cond, bool_t b_cond, unsigned long long* rand_seed);

int Zeta_Choose3(bool_t a_cond, bool_t b_cond, bool_t c_cond,
                 unsigned long long* rand_seed);

ZETA_ExternC_End;
