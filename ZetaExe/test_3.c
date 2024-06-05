

#include "../Zeta/utils.h"

int main() {
    size_t a = 5;
    size_t b = 3;
    size_t c = 7;
    size_t d = 2;

    /*
    size_t ans =
        (size_t tmp1 = a + b, tmp2 = c + d, (tmp1 < tmp2 ? tmp1 : tmp2));
    */

    size_t ans =
        (size_t tmp1 = a + b; size_t tmp1 = c + d; (tmp1 < tmp2 ? tmp1 : tmp2));

    printf("%zu\n", ans);

    return 0;
}
