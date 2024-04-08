#include <stddef.h>

#include "../Zeta/Algorithm.h"
#include "../Zeta/PoolAllocator.h"
#include "../Zeta/RawVector.h"
#include "../Zeta/SlabAllocator.h"
#include "../Zeta/random.h"

#define lengthof(arr) (sizeof(arr) / sizeof(arr[0]))

int main() {
    size_t page_size = 4 * 1024;

    int const max_type = Zeta_SlabAllocator_max_num_of_types;

    int sizes[max_type];
    int nums[max_type];

    sizes[0] = 48;
    sizes[1] = sizes[0] * 12 / 10;  // 1.2
    sizes[2] = sizes[0] * 15 / 10;  // 1.5

    int type_i = 3;

    for (int i = 0; i < type_i; ++i) {
        nums[i] = Zeta_SlabAllocator_GetMaxNum(page_size, sizes[i]);
    }

    for (; type_i < max_type; ++type_i) {
        sizes[type_i] = sizes[type_i - 2] * 2;
        nums[type_i] = Zeta_SlabAllocator_GetMaxNum(page_size, sizes[type_i]);

        if (nums[type_i] == 0) { break; }
    }

    int num_of_types = type_i;
    ZETA_PrintVar(num_of_types);

    Zeta_PoolAllocator pa_;
    Zeta_Allocator pa;
    Zeta_PoolAllocator_DeployAllocator(&pa_, &pa);

    for (int i = 0; i < 1024; ++i) {
        Zeta_PoolAllocator_Deallocate(&pa_, malloc(page_size));
    }

    Zeta_SlabAllocator sa_;
    Zeta_Allocator sa;
    Zeta_SlabAllocator_DeployAllocator(&sa_, &sa);

    Zeta_SlabAllocator_Entrust(&sa_, num_of_types, sizes, nums, &pa);

    printf("type_i\tsize\tnum\tpage size\n");

    for (int i = 0; i < num_of_types; ++i) {
        printf("%d\t%d\t%d\t%d\n", i, sizes[i], nums[i],
               Zeta_SlabAllocator_GetPageSize(sizes[i], nums[i]));
    }

    void* ptr_a = Zeta_SlabAllocator_Allocate(&sa_, 32);
    void* ptr_b = Zeta_SlabAllocator_Allocate(&sa_, 32);
    void* ptr_c = Zeta_SlabAllocator_Allocate(&sa_, 32);

    printf("ptr_a = %p\n", ptr_a);
    printf("ptr_b = %p\n", ptr_b);
    printf("ptr_c = %p\n", ptr_c);

    Zeta_SlabAllocator_Deallocate(&sa_, ptr_a);
    Zeta_SlabAllocator_Deallocate(&sa_, ptr_b);

    void* ptr_d = Zeta_SlabAllocator_Allocate(&sa_, 32);
    void* ptr_e = Zeta_SlabAllocator_Allocate(&sa_, 32);

    printf("ptr_d = %p\n", ptr_d);
    printf("ptr_e = %p\n", ptr_e);

    void* ptr_f = Zeta_SlabAllocator_Allocate(&sa_, 128);
    void* ptr_h = Zeta_SlabAllocator_Allocate(&sa_, 96);

    printf("ptr_f = %p\n", ptr_f);
    printf("ptr_h = %p\n", ptr_h);

    return 0;
}
