#pragma once

#include "define.h"

ZETA_ExternC_Beg;

/**
 * @brief Copy byte array from dst to src
 *
 * @param dst The pointer points to destination byte array.
 * @param src The pointer points to source byte array.
 * @param size The number of bytes the byte arrays pointed by dst and src.
 */
void Zeta_MemCopy(void* dst, void const* src, size_t size);

/**
 * @brief Swap byte array between dst and src
 *
 * @param dst The pointer points to destination byte array.
 * @param src The pointer points to source byte array.
 * @param size The number of bytes the byte arrays pointed by dst and src.
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

/**
 * @brief Read little endian stored length bytes occupying integer from data.
 *
 * @param data The byte array stores integer.
 * @param length The number of bytes the integer occupied.
 *
 * @return The read integer
 */
u128_t Zeta_ReadLittleEndian(byte_t const* data, unsigned int length);

/**
 * @brief Write an integer to data in little endian occupying length bytes.
 *
 * @param dst The destination byte array.
 * @param length The number of bytes the integer occupied.
 * @param val The target integer.
 *
 * @return The remaining value after
 */
u128_t Zeta_WriteLittleEndian(byte_t* dst, u128_t val, unsigned int length);

/**
 * @brief Read big endian stored length bytes occupying integer from data.
 *
 * @param data The byte array stores integer.
 * @param length The number of bytes the integer occupied.
 *
 * @return The read integer
 */
u128_t Zeta_ReadBigEndian(byte_t const* data, unsigned int length);

/**
 * @brief Write an integer to data in big endian occupying length bytes.
 *
 * @param dst The destination byte array.
 * @param length The number of bytes the integer occupied.
 * @param val The target integer.
 */
u128_t Zeta_WriteBigEndian(byte_t* dst, u128_t val, unsigned int length);

size_t Zeta_SimpleRandom(size_t x);

size_t Zeta_SimpleHash(size_t x);

size_t Zeta_GetGCD(size_t x, size_t y);

size_t Zeta_GetLCM(size_t x, size_t y);

/**
 * @brief Calculate base^exp.
 *
 * @param base The base of base^exp.
 * @param exp The exp of base^exp.
 *
 * @return base^exp.
 */
size_t Zeta_GetPower(size_t base, size_t exp);

/**
 * @brief Calculate base^exp % mod.
 *
 * @param base The base of base^exp % mod.
 * @param exp The exp of base^exp % mod.
 * @param mod The mod of base^exp % mod.
 *
 * @return base^exp % mod.
 */
size_t Zeta_GetPowerMod(size_t base, size_t exp, size_t mod);

int Zeta_GetLogFloor(size_t val, size_t base);

int Zeta_GetLogCeil(size_t val, size_t base);

size_t Zeta_FindNextConMod(size_t beg, size_t target, size_t mod);

int Zeta_GetLSB(size_t x);

int Zeta_GetMSB(size_t x);

void* Zeta_GetMostLink(void* context, void* (*GetLink)(void* context, void* n),
                       void* n);

ZETA_ExternC_End;
