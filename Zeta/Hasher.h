#pragma once

#include "define.h"

ZETA_ExternC_Beg;

ZETA_DeclareStruct(Zeta_Hasher);

/**
 * @brief An interface of hasher.
 */
struct Zeta_Hasher {
    /**
     * The context of hasher.
     */
    void* context;

    void (*Reset)(void* context);

    size_t (*GetSize)(void* context);

    size_t (*GetResultWidth)(void* context);

    /**
     * @brief Save the hash result to dst.
     *
     * @param context The context of hasher.
     * @param dst The destination of result.
     */
    void (*GetResult)(void* context, byte_t* dst);

    /**
     * @brief Get the number of bytes a hash result occupied.
     *
     * @param context The context of hasher.
     */
    void (*Rotate)(void* context, byte_t const* data, size_t cnt);
};

/**
 * @brief An interface of hasher.
 */
struct Zeta_Hasher {
    void* context;

    unsigned long long (*Hash)(void* context, void const* elem);
};

void Zeta_Hasher_Init(void* hasher);

ZETA_ExternC_End;
