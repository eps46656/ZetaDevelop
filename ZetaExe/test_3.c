

#include "../Zeta/utils.h"

int main() {
    int x = 0b1;

    int ans = __builtin_clz(x);

    // printf("%d", ans);

    ZETA_PrintVar(ans);

    return 0;
}
