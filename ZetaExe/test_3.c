#include <stdio.h>

#include "../Zeta/define.h"

void main3() {
    unsigned char const* str = ZETA_ToUCharStr(":");

    printf("%d\n", str[0]);
    printf("%d\n", str[1]);
    printf("%d\n", str[2]);
    printf("%d\n", str[3]);
    printf("%d\n", str[4]);

    // printf("%llu\n", strlen(str));
}

int main() {
    main3();
    printf("ok\n");
    return 0;
}
