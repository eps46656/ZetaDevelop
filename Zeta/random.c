#include "random.h"

void Zeta_LCGRandomGenerator_Set(void* lcgrg_, size_t seed) {
    Zeta_LCGRandomGenerator* lcgrg = lcgrg_;

    size_t a;
    size_t c;
    size_t m;

    if (48 <= CHAR_BIT * sizeof(size_t)) {
        a = 25214903917;
        c = 11;
        m = (size_t)1 << 48;
    } else {
        a = 1103515245;
        c = 12345;
        m = (size_t)1 << 31;
    }

    while (seed < m) { seed = seed * a + c; }

    lcgrg->a = a;
    lcgrg->c = c;
    lcgrg->m = m;
    lcgrg->seed = seed % m;
}

void Zeta_LCGRandomGenerator_Rotate(void* lcgrg_) {
    Zeta_LCGRandomGenerator* lcgrg = lcgrg_;
    lcgrg->seed = (lcgrg->a * lcgrg->seed + lcgrg->c) % lcgrg->m;
}

size_t Zeta_LCGRandomGenerator_Fetch(void* lcgrg_) {
    Zeta_LCGRandomGenerator* lcgrg = lcgrg_;
    return lcgrg->seed >> 16;
}
