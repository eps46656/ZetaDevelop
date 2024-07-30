#include <stdio.h>

#include "../Zeta/define.h"

void main3() {
    int a = ({ 5 + 6; });

    printf("%d\n", a);
}

int main() {
    main3();
    printf("ok\n");
    return 0;
}
