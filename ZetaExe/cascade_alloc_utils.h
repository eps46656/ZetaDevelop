#include <cascade_allocator.h>
#include <memory.h>

#include "std_allocator.h"

#if ZETA_EnableDebug

StdAllocator std_allocator;

#else

Zeta_CascadeAllocator cascade_allocator;

unsigned char cascade_mem[512 * 1024 * 1024];

#endif

void InitCascadeAllocator() {
#if ZETA_EnableDebug
    // use standard malloc to be sanitized by asan

    zeta_memory_cascade_allocator = {
        .vtable = &zeta_std_allocator_vtable,
        .context = &std_allocator,
    };
#else

    cascade_allocator.align = alignof(max_align_t);
    cascade_allocator.mem = cascade_mem;
    cascade_allocator.size = sizeof(cascade_mem);

    Zeta_CascadeAllocator_Init(&cascade_allocator);

    zeta_memory_cascade_allocator = {
        .vtable = &zeta_cascade_allocator_vtable,
        .context = &cascade_allocator,
    };
#endif
}
