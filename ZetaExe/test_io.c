#include "../Zeta/define.h"

//

extern void __stdcall syscall__write_(unsigned int fd, const char* data,
                                      size_t data_len);

int main() {
    const char str[] = "Hello World\n";

    syscall__write_(fileno(stdout), str, sizeof(str) / sizeof(char));

    printf("fileno(stdout): %d\n", fileno(stdout));
    printf("hakdow\n");

    return 0;
}
