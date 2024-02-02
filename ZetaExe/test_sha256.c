
#include <stdio.h>
#include "../Zeta/SHA256.h"
#include "../Zeta/utils.h"

typedef unsigned long long ull;

int main() {
    Zeta_SHA256Hasher hasher;
    Zeta_SHA256Hasher_Init(&hasher);

    _BitInt(32) k = 0x59966565679;

    byte_t k_tmp[4];

    Zeta_WriteBigEndian(k_tmp, k, 4);

    _BitInt(32) k_ = Zeta_ReadBigEndian(k_tmp, 4);

    ZETA_PRINT_VAR("%llu", (ull)k);
    ZETA_PRINT_VAR("%llu", (ull)k_);

    byte_t tmp;

    if (1) {
        FILE* f = fopen("test_2.elf", "rb");

        for (;;) {
            tmp = fgetc(f);
            if (feof(f)) { break; }

            Zeta_SHA256Hasher_Rotate(&hasher, &tmp, 1);
        }

        fclose(f);
    } else {
        // tmp = 0x53;
        // Zeta_SHA256Hasher_Rotate(&hasher, &tmp, 1);
    }

    ZETA_PRINT_VAR("%llu", hasher.size);

    byte_t digits[32];

    Zeta_SHA256Hasher_GetDigits(&hasher, digits);

    for (int i = 0; i < 32; ++i) { printf("%02x", digits[i]); }

    printf("\n");

    // b5123644174b7ba1d1101bb2157a5802acc664c037d52c2852aadf3f1dea905a
    // b5123644174b7ba1d1101bb2157a5802acc664c037d52c2852aadf3f1dea905a

    return 0;
}
