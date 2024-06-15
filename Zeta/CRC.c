#include "CRC.h"

void Zeta_CRCHasher_Init(void* hasher_, unsigned _BitInt(64) div,
                         unsigned _BitInt(64) reg) {
    Zeta_CRCHasher* hasher = hasher_;
    ZETA_DebugAssert(hasher != NULL);

    hasher->div = div;
    hasher->reg = reg;
}

unsigned _BitInt(64) Zeta_CRCHasher_GetResult(void* hasher_) {
    Zeta_CRCHasher* hasher = hasher_;
    ZETA_DebugAssert(hasher != NULL);
    return hasher->reg;
}

static byte_t ByteReverse_(byte_t x) {
    byte_t ret = 0;
    for (int i = 0; i < 8; ++i, x /= 2) { ret = ret * 2 + x % 2; }
    return ret;
}

void Zeta_CRCHasher_Rotate(void* hasher_, byte_t const* data,
                           byte_t const* data_end, int reverse) {
    Zeta_CRCHasher* hasher = hasher_;
    ZETA_DebugAssert(hasher != NULL);

    ZETA_DebugAssert(data != NULL);
    ZETA_DebugAssert(data_end != NULL);
    ZETA_DebugAssert(data <= data_end);

    unsigned _BitInt(64) div = hasher->div;
    unsigned _BitInt(64) reg = hasher->reg;

    for (; data < data_end; ++data) {
        byte_t x = data[0];
        ZETA_DebugAssert(0 <= x);
        ZETA_DebugAssert(x <= 255);

        if (reverse) {
            reg ^= ByteReverse_(x);
        } else {
            reg ^= x;
        }

        for (int i = 0; i < 8; ++i) {
            if ((reg & 1) == 0) {
                reg >>= 1;
            } else {
                reg = (reg >> 1) ^ div;
            }
        }
    }

    hasher->reg = reg;
}
