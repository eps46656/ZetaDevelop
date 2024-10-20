#include <math.h>
#include <utils.h>

void main1() {
    ZETA_PrintCurPos;

    double base = ZETA_2Power(ZETA_FixedPoint_BaseOrder);

    double max_k_err = 0;

    for (double x = -34; x <= 34; x += 0.01) {
        double zetay = Zeta_FixedPoint2Power(x * base) / base;

        double stdy = pow(2, x);
        // double stdy = log(x) / log(2);

        double err = fabs(zetay - stdy);
        double rel_err = fabs(err / stdy);

        double k_err = min(err, rel_err);

        max_k_err = max(max_k_err, k_err);

        if (1e-4 <= max_k_err) {
            printf("x: %f\n", x);
            printf("\t\tk err: %f\n", k_err);
        }
    }

    printf("max k err: %e\n", max_k_err);
}

void main2() {
    ZETA_PrintCurPos;

    double base = ZETA_2Power(ZETA_FixedPoint_BaseOrder);

    double max_k_err = 0;

    for (double x = 0.0001; x <= 20; x += 0.0000001) {
        double zetay = Zeta_FixedPointLog2(x * base) / base;

        // double stdy = pow(2, x);
        double stdy = log(x) / log(2);

        double err = fabs(zetay - stdy);
        double rel_err = fabs(err / stdy);

        double k_err = min(err, rel_err);

        max_k_err = max(max_k_err, k_err);

        if (1e-3 <= k_err) {
            printf("x: %e\n", x);
            printf("\t\t\t\tk err: %e\n", k_err);
        }
    }

    printf("max k err: %e\n", max_k_err);
}

void main3() {
    ZETA_PrintCurPos;

    double base = ZETA_2Power(ZETA_FixedPoint_BaseOrder);

    double max_k_err = 0;

    for (double x = 0; x <= 1024; x += 0.0001) {
        double zetay = Zeta_FixedPointSqrt(x * base) / base;

        // double stdy = pow(2, x);
        double stdy = sqrt(x);

        double err = fabs(zetay - stdy);
        double rel_err = fabs(err / stdy);

        double k_err = min(err, rel_err);

        max_k_err = max(max_k_err, k_err);

        if (1e-3 <= k_err) {
            printf("x: %e\n", x);
            printf("k err: %e\n", k_err);
        }
    }

    printf("max k err: %e\n", max_k_err);
}

int main() {
    printf("main start\n");
    main1();
    printf("ok\n");
    return 0;
}
