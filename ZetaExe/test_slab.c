#include <stddef.h>
#include "../Zeta/Algorithm.h"
#include "../Zeta/PoolAllocator.h"
#include "../Zeta/RawVector.h"
#include "../Zeta/SlabAllocator.h"
#include "../Zeta/random.h"

#define lengthof(arr) (sizeof(arr) / sizeof(arr[0]))

int main() {
    diff_t page_size = 4 * 1024;

    diff_t divs[] = {2, 3, 4, 6, 8, 12, 16, 24, 32, 48, 64};

    Zeta_PoolAllocator pa_;
    Zeta_Allocator pa;
    Zeta_PoolAllocator_DeployAllocator(&pa_, &pa);

    for (int i = 0; i < 1024; ++i) {
        Zeta_PoolAllocator_Deallocate(&pa_, malloc(page_size));
    }

    Zeta_SlabAllocator sa_;
    Zeta_Allocator sa;
    Zeta_SlabAllocator_DeployAllocator(&sa_, &sa);

    Zeta_SlabAllocator_Entrust(&sa_, page_size, lengthof(divs), divs, &pa);

    printf("div\tsize\n");

    for (int i = 0; i < lengthof(divs); ++i) {
        printf("%d\t%d\n", divs[i], Zeta_SlabAllocator_GetSize(&sa_, i));
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
