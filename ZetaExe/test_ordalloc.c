#include "../Zeta/OrdAllocator.h"

typedef long long unsigned llu;

byte_t mem[16 * 1024 * 1024];

int main() {
    Zeta_OrdAllocator allocator;
    allocator.ptr = mem;
    allocator.align = 16;
    allocator.size = sizeof(mem);
    allocator.num_of_slab_level = -1;

    Zeta_OrdAllocator_Init(&allocator);

    int num_of_slab_level = allocator.num_of_slab_level;

    ZETA_PRINT_VAR("%llu", (llu)num_of_slab_level);

    for (int i = 0; i < num_of_slab_level; ++i) {
        size_t cur_width = allocator.slab_widths[i];
        size_t cur_size = allocator.slab_sizes[i];

        ZETA_PRINT_VAR("%llu", (llu)cur_width);
        ZETA_PRINT_VAR("%llu", (llu)cur_size);
    }

    void* ptr_a = Zeta_OrdAllocator_Allocate(&allocator, 600);
    void* ptr_b = Zeta_OrdAllocator_Allocate(&allocator, 64);

    ZETA_PRINT_VAR("%p", ptr_a);
    ZETA_PRINT_VAR("%p", ptr_b);
    ZETA_PRINT_VAR("%llu", (llu)((uintptr_t)ptr_b - (uintptr_t)ptr_a));

    return 0;
}
