#include "memory.h"

Zeta_Allocator zeta_memory_cascade_allocator = {
    .vtable = NULL,
    .context = NULL,
};

Zeta_Allocator zeta_memory_tree_allocator = {
    .vtable = NULL,
    .context = NULL,
};
