#include <math.h>
#include <utils.h>

void main1() {
    double base = ZETA_2Power(ZETA_FixedPoint_BaseOrder);

    double max_err = 0;
    double max_rel_err = 0;

    for (double x = -10; x <= 10; x += 0.01) {
        double zetay = Zeta_FixedPoint2Power(x * base) / base;

        double stdy = pow(2, x);
        // double stdy = log(x) / log(2);

        double err = fabs(zetay - stdy);
        double rel_err = fabs(err / stdy);

        max_err = max(max_err, err);
        max_rel_err = max(max_rel_err, rel_err);

        if (1 / (1024.0 * 8) <= rel_err) {
            printf("x: %f\n", x);
            printf("\t\trel err: %f\n", rel_err);
        }
    }

    printf("max err: %e\n", max_err);
    printf("max rel err: %e\n", max_rel_err);
}

void main2() {
    double base = ZETA_2Power(ZETA_FixedPoint_BaseOrder);

    double max_err = 0;
    double max_rel_err = 0;

    for (double x = 0.0001; x <= 2; x += 0.00000001) {
        double zetay = Zeta_FixedPointLog2(x * base) / base;

        // double stdy = pow(2, x);
        double stdy = log(x) / log(2);

        double err = fabs(zetay - stdy);
        double rel_err = fabs(err / stdy);

        max_err = max(max_err, err);
        max_rel_err = max(max_rel_err, rel_err);

        /*
        if (1 / (1024.0 * 8) <= rel_err) {
            printf("x: %e\n", x);
            printf("err: %e\n", err);
            printf("rel err: %e\n", rel_err);
        }
        */
    }

    printf("max err: %e\n", max_err);
    printf("max rel err: %e\n", max_rel_err);
}

void main3() {
    double base = ZETA_2Power(ZETA_FixedPoint_BaseOrder);

    double max_err = 0;
    double max_rel_err = 0;

    for (double x = 1024 * 1024; x <= 1024 * 1024 * 2; x += 0.01) {
        double zetay = Zeta_FixedPointSqrt(x * base) / base;

        // double stdy = pow(2, x);
        double stdy = sqrt(x);

        double err = fabs(zetay - stdy);
        double rel_err = fabs(err / stdy);

        max_err = max(max_err, err);
        max_rel_err = max(max_rel_err, rel_err);

        if (1 / (1024.0 * 8) <= rel_err) {
            printf("x: %e\n", x);
            printf("rel err: %e\n", rel_err);
        }
    }

    printf("max err: %e\n", max_err);
    printf("max rel err: %e\n", max_rel_err);
}

int main() {
    printf("main start\n");
    main2();
    printf("ok\n");
    return 0;
}
