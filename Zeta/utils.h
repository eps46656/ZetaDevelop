#pragma once

#include "define.h"

/**
 * @brief Copy byte array from dst to src
 *
 * @param size The number of bytes the byte arrays pointed by dst and src.
 * @param dst The pointer points to destination byte array.
 * @param src The pointer points to source byte array.
 */
EXTERN_C void Zeta_MemCopy(size_t size, byte_t* dst, byte_t* src);

/**
 * @brief Swap byte array between dst and src
 *
 * @param size The number of bytes the byte arrays pointed by dst and src.
 * @param dst The pointer points to destination byte array.
 * @param src The pointer points to source byte array.
 */
EXTERN_C void Zeta_MemSwap(size_t size, byte_t* x, byte_t* y);

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
EXTERN_C byte_t* Zeta_MemRotate(byte_t* beg, byte_t* mid, byte_t* end);

EXTERN_C void Zeta_Swap(diff_t* a, diff_t* b);

/**
 * @brief Calculate base^exp % mod.
 *
 * @param base The base of base^exp % mod.
 * @param exp The exp of base^exp % mod.
 * @param mod The mod of base^exp % mod.
 *
 * @return base^exp % mod.
 */
EXTERN_C size_t Zeta_PowerM(size_t base, size_t exp, size_t mod);

/**
 * @brief Read little endian stored length bytes occupying integer from data.
 *
 * @param data The byte array stores integer.
 * @param length The number of bytes the integer occupied.
 *
 * @return The read integer
 */
EXTERN_C size_t Zeta_ReadLittleEndian(const byte_t* data, int length);

/**
 * @brief Write an integer to data in little endian occupying length bytes.
 *
 * @param dst The destination byte array.
 * @param x The target integer.
 * @param length The number of bytes the integer occupied.
 */
EXTERN_C void Zeta_WriteLittleEndian(byte_t* dst, size_t x, int length);

/**
 * @brief Read big endian stored length bytes occupying integer from data.
 *
 * @param data The byte array stores integer.
 * @param length The number of bytes the integer occupied.
 *
 * @return The read integer
 */
EXTERN_C size_t Zeta_ReadBigEndian(const byte_t* data, int length);

/**
 * @brief Write an integer to data in big endian occupying length bytes.
 *
 * @param dst The destination byte array.
 * @param x The target integer.
 * @param length The number of bytes the integer occupied.
 */
EXTERN_C void Zeta_WriteBigEndian(byte_t* dst, size_t x, int length);

EXTERN_C size_t Zeta_SimpleRandom(size_t x);

EXTERN_C size_t Zeta_SimpleHash(size_t x);

EXTERN_C size_t GetGCD(size_t x, size_t y);

EXTERN_C size_t GetLCM(size_t x, size_t y);

EXTERN_C size_t FindNextConMod(size_t beg, size_t target, size_t mod);

EXTERN_C void* Zeta_GetMostLink(void* context,
                                void* (*GetLink)(void* context, void* n),
                                void* n);
