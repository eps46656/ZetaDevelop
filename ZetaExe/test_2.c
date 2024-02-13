#include "../Zeta/define.h"

// #pragma clang section bss = ".ZetaHeap"

int global_x __attribute__((section(".ZetaHeap")));

int main() {
    printf("Hello World.\n");

    //

    printf("%p", &global_x);

    return 0;
}
