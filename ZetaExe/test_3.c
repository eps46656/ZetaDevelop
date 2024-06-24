#include "../Zeta/define.h"

void main3() {
    long long char_min = ZETA_RangeMinOf(char);
    long long char_max = ZETA_RangeMaxOf(char);
    ZETA_PrintVar(char_min);
    ZETA_PrintVar(char_max);

    // ZETA_PrintVar(ZETA_RangeMaxOf(unsigned char));
    // ZETA_PrintVar(ZETA_RangeMaxOf(signed char));

    unsigned int char_width = ZETA_WidthOf(unsigned char);
    unsigned int short_width = ZETA_WidthOf(unsigned short);

    ZETA_PrintVar(char_width);
    ZETA_PrintVar(short_width);
}

int main() {
    main3();
    printf("ok\n");
    return 0;
}
