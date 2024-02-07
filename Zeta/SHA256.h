#pragma once

#include "define.h"

ZETA_extern_c_beg;

typedef struct Zeta_SHA256Hasher Zeta_SHA256Hasher;

/**
 * @brief The sha256 hasher.
 */
struct Zeta_SHA256Hasher {
    /**
     * The intermediate state of sha256.
     */
    unsigned _BitInt(32) hs[8];

    /**
     * The size(byte) of last chunk.
     */
    size_t size;

    /**
     * The last chunk of inputted data.
     */
    byte_t last_chunk[64];
};

/**
 * @brief Get hash result of sha256
 *
 * @param dst The destination of hash result.
 * @param data The target data.
 * @param size The number of bytes of data.
 */
void Zeta_SHA256_Hash(byte_t* dst, const byte_t* data, size_t size);

/**
 * @brief Initialize the hasher
 *
 * @param hasher The target hasher.
 */
void Zeta_SHA256Hasher_Init(void* hasher);

/**
 * @brief Get the size(byte) of inputted data.
 *
 * @param hasher The target hasher.
 *
 * @return The size(byte) of inputted data.
 */
size_t Zeta_SHA256Hasher_GetSize(void* hasher);

/**
 * @brief Get the size(byte) of result. Always return 32.
 *
 * @param hasher The target hasher.
 *
 * @return The size(byte) of result. Always return 32.
 */
size_t Zeta_SHA256Hasher_GetResultSize(void* hasher);

/**
 * @brief Get the size(byte) of result. Always return 32.
 *
 * @param hasher The target hasher.
 * @param dst The destination of result.
 */
void Zeta_SHA256Hasher_GetResult(void* hasher, byte_t* dst);

/**
 * @brief Append new data after previous inputted data to hash.
 *
 * @param hasher The target hasher.
 * @param data The target data.
 * @param size The size(byte) of target data.
 */
void Zeta_SHA256Hasher_Rotate(void* hasher, const byte_t* data, size_t size);

ZETA_extern_c_end;
