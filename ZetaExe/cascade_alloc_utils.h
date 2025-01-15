#include <cascade_allocator.h>
#include <memory.h>

#include "std_allocator.h"

#if ZETA_EnableDebug

StdAllocator std_allocator_instance;
Zeta_Allocator std_allocator;

#else

Zeta_CascadeAllocator cascade_allocator_instance;
Zeta_Allocator cascade_allocator;

unsigned char cascade_mem[512 * 1024 * 1024];

#endif

void InitCascadeAllocator() {
#if ZETA_EnableDebug
    // use standard malloc to be sanitized by asan

    StdAllocator_DeployAllocator(&std_allocator_instance, &std_allocator);

    zeta_cascade_allocator = &std_allocator;
#else
    cascade_allocator_instance.align = alignof(max_align_t);
    cascade_allocator_instance.mem = cascade_mem;
    cascade_allocator_instance.size = sizeof(cascade_mem);

    Zeta_CascadeAllocator_Init(&cascade_allocator_instance);

    Zeta_CascadeAllocator_DeployAllocator(&cascade_allocator_instance,
                                          &cascade_allocator);

    zeta_cascade_allocator = &cascade_allocator;
#endif
}
