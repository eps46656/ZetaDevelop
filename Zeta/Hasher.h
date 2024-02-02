#pragma

#include "define.h"

typedef struct Zeta_Hasher Zeta_Hasher;

/**
 * @brief An interface of hasher.
 */
struct Zeta_Hasher {
    /**
     * The context of hasher.
     */
    void* context;

    /**
     * @brief Get the number of bytes a hash result occupied.
     *
     * @param context The context of hasher.
     */
    size_t (*GetSize)(void* context);

    /**
     * @brief Save the hash result to dst.
     *
     * @param context The context of hasher.
     * @param dst The destination of result.
     */
    byte_t (*GetResult)(void* context, byte_t* dst);

    /**
     * @brief Get the number of bytes a hash result occupied.
     *
     * @param context The context of hasher.
     */
    void (*Rotate)(void* context, const byte_t* data, size_t size);
};
