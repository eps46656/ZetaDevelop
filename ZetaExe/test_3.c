#include "../Zeta/define.h"

#define P 3
#define Q P

int main() {
    // unsigned int x = Zeta_GetPower(unsigned int)(3, 5);

    unsigned x = ZETA_GetRangeMax(unsigned int);

    const char* x_name = ZETA_ToStr(ZETA_GetIntName(unsigned int));
    const char* y_name = ZETA_ToStr(Q);

    unsigned int ZETA_Concat(x, P) = 5;

    ZETA_PrintVar(x);
    ZETA_PrintVar(x_name);
    ZETA_PrintVar(y_name);
    ZETA_PrintVar(x3);
    ZETA_PrintVar(NULL);

    ZETA_Pause;

    ZETA_Print("%d %d", x, x3);

    return 0;
}
