#include <stddef.h>

#include "../Zeta/Algorithm.h"
#include "../Zeta/random.h"

typedef long long int lli;

int Comparer(void* context, void const* x, void const* y) {
    ZETA_Unused(context);

    lli x_val = *(lli const*)(x);
    lli y_val = *(lli const*)(y);

    if (x_val < y_val) { return -1; }
    if (y_val < x_val) { return 1; }
    return 0;
}

void Print(int* beg, int* end) {
    for (; beg != end; ++beg) { printf("%d ", *beg); }
    printf("\n");
}

void CMySort(void* arr_, lli size, lli beg, lli end) {
    lli* arr = arr_;

    ZETA_DebugAssert(0 <= beg);
    ZETA_DebugAssert(beg <= end);
    ZETA_DebugAssert(end <= size);

    Zeta_RawVector lli_raw_vector = {
        .data = arr,

        .stride = sizeof(lli),

        .size = size,
    };

    Zeta_Vector lli_vector;
    Zeta_RawVector_GetVector(&lli_raw_vector, &lli_vector);

    Zeta_Sort(&lli_vector,  // vec
              sizeof(lli),  // width
              beg,          // beg
              end,          // end
              NULL,         // cmper_context
              Comparer      // cmper
    );
}

lli Fetch(lli x, lli lb, lli rb) { return x % (rb - lb + 1) + lb; }

int main() {
    int size = 10000000;

    lli* arr = malloc(sizeof(lli) * size);

    Zeta_RawVector int_raw_vector = {
        .data = arr,

        .stride = sizeof(arr[0]),

        .size = size,
    };

    Zeta_Vector int_vector;
    Zeta_RawVector_GetVector(&int_raw_vector, &int_vector);

    lli lb = -(1LL << 60);
    lli rb = (1LL << 60);

    size_t seed = (size_t)arr;

    Zeta_LCGRandomGenerator lcgrg;
    Zeta_LCGRandomGenerator_Set(&lcgrg, seed);

    printf("seed = %zu\n", seed);

    for (int test_i = 0; test_i < 32; ++test_i) {
        printf("test %d\n", test_i);

        for (int i = 0; i < size; ++i) {
            arr[i] = Fetch(Zeta_LCGRandomGenerator_Fetch(&lcgrg), lb, rb);
            Zeta_LCGRandomGenerator_Rotate(&lcgrg);
        }

        size_t beg = Fetch(Zeta_LCGRandomGenerator_Fetch(&lcgrg), 0, size - 2);
        size_t mid =
            Fetch(Zeta_LCGRandomGenerator_Fetch(&lcgrg), beg + 1, size - 1);
        size_t end =
            Fetch(Zeta_LCGRandomGenerator_Fetch(&lcgrg), mid + 1, size);

        Zeta_KthElement(&int_vector,     // vec
                        sizeof(arr[0]),  // width
                        beg,             // beg
                        mid,             // mid
                        end,             // end
                        NULL,            // cmper_context
                        Comparer         // cmper
        );

        lli pivot = arr[mid];

        for (size_t i = beg; i < mid; ++i) { ZETA_Assert(arr[i] <= pivot); }

        for (size_t i = mid + 1; i < end; ++i) { ZETA_Assert(pivot <= arr[i]); }

        printf("ok %d\n", test_i);
    }

    return 0;
}
