#include "CircularVector.h"
#include "RawVector.h"
#include "stdio.h"
#include "stdlib.h"

#define N (10)

void Print(Zeta_CircularVector *cv) {
    if (cv == NULL) {
        printf("null cv\n");
        return;
    }

    diff_t size = Zeta_CircularVector_GetSize(cv);

    printf("size = %lld\n", size);
    printf("cv = { ");

    for (diff_t i = 0; i < size; ++i) {
        int val = *(int *)Zeta_CircularVector_GetRef(cv, i);
        printf("%d ", val);
    }

    printf("}\n");
}

int main() {
    int data[N];

    struct Zeta_RawVector raw_vector;
    struct Zeta_CircularVector cv;

    ZETA_PRINT_POS;

    Zeta_RawVector_Entrust(&raw_vector, data, sizeof(data[0]), N);

    ZETA_PRINT_POS;

    Zeta_RawVector_GetVectorOpr(&cv.vec_opr);

    ZETA_PRINT_POS;

    Zeta_CircularVector_Entrust(&cv, &raw_vector, 0, 0);

    ZETA_PRINT_POS;

    Print(&cv);

    *(int *)Zeta_CircularVector_PushR(&cv) = 0;
    Zeta_CircularVector_Check(&cv);
    Print(&cv);

    *(int *)Zeta_CircularVector_PushR(&cv) = 1;
    Zeta_CircularVector_Check(&cv);
    Print(&cv);

    *(int *)Zeta_CircularVector_PushR(&cv) = 2;
    Zeta_CircularVector_Check(&cv);
    Print(&cv);

    *(int *)Zeta_CircularVector_PushL(&cv) = -1;
    Zeta_CircularVector_Check(&cv);
    Print(&cv);

    *(int *)Zeta_CircularVector_PushL(&cv) = -2;
    Zeta_CircularVector_Check(&cv);
    Print(&cv);

    //

    return 0;
}
