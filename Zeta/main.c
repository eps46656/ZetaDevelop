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
        int val = *(int *)Zeta_CircularVector_Access(cv, i);
        printf("%d ", val);
    }

    printf("}\n");
}

int main() {
    int data[N];

    Zeta_RawVector rv;
    rv.data = data;
    rv.stride = sizeof(data[0]);
    rv.size = N;

    Zeta_Vector rv_vector;
    rv_vector.context = &rv;
    rv_vector.GetSize = Zeta_RawVector_GetSize;
    rv_vector.Access = Zeta_RawVector_Access;

    ZETA_PRINT_POS;

    Zeta_CircularVector cv;

    cv.vec = &rv_vector;
    cv.offset = 0;
    cv.size = 0;

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
