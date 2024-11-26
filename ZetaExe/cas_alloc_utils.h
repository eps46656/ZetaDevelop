#include <cascade_allocator.h>
#include <memory.h>

#include "std_allocator.h"

#if ZETA_EnableDebug

StdAllocator std_allocator_instance;
Zeta_Allocator std_allocator;

#else

Zeta_CascadeAllocator cas_allocator_instance;
Zeta_Allocator cas_allocator;

unsigned char cas_mem[512 * 1024 * 1024];

#endif

void InitCasAllocator() {
#if ZETA_EnableDebug
    // use standard malloc to be sanitized by asan

    StdAllocator_DeployAllocator(&std_allocator_instance, &std_allocator);

    zeta_cas_allocator = &std_allocator;
#else
    cas_allocator_instance.align = alignof(max_align_t);
    cas_allocator_instance.mem = cas_mem;
    cas_allocator_instance.size = sizeof(cas_mem);

    Zeta_CascadeAllocator_Init(&cas_allocator_instance);

    Zeta_CascadeAllocator_DeployAllocator(&cas_allocator_instance,
                                          &cas_allocator);

    zeta_cas_allocator = &cas_allocator;
#endif
}
