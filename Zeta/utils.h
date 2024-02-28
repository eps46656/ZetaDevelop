#pragma once

#include "define.h"

ZETA_extern_c_beg;

/**
 * @brief Copy byte array from dst to src
 *
 * @param size The number of bytes the byte arrays pointed by dst and src.
 * @param dst The pointer points to destination byte array.
 * @param src The pointer points to source byte array.
 */
void Zeta_MemCopy(size_t size, void* dst, void const* src);

/**
 * @brief Swap byte array between dst and src
 *
 * @param size The number of bytes the byte arrays pointed by dst and src.
 * @param dst The pointer points to destination byte array.
 * @param src The pointer points to source byte array.
 */
void Zeta_MemSwap(size_t size, void* x, void* y);

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

u128_t Zeta_SimpleRandom(u128_t x);

u128_t Zeta_SimpleHash(u128_t x);

u128_t Zeta_GetGCD(u128_t x, u128_t y);

u128_t Zeta_GetLCM(u128_t x, u128_t y);

/**
 * @brief Calculate base^exp.
 *
 * @param base The base of base^exp.
 * @param exp The exp of base^exp.
 *
 * @return base^exp.
 */
u128_t Zeta_GetPower(u128_t base, u128_t exp);

/**
 * @brief Calculate base^exp % mod.
 *
 * @param base The base of base^exp % mod.
 * @param exp The exp of base^exp % mod.
 * @param mod The mod of base^exp % mod.
 *
 * @return base^exp % mod.
 */
u128_t Zeta_GetPowerMod(u128_t base, u128_t exp, u128_t mod);

u128_t Zeta_FindNextConMod(u128_t beg, u128_t target, u128_t mod);

void* Zeta_GetMostLink(void* context, void* (*GetLink)(void* context, void* n),
                       void* n);

ZETA_extern_c_end;
