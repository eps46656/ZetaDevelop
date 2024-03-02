#include <iostream>
#include <map>
#include <random>

#include "../Zeta/DebugTreeMap.h"
#include "../Zeta/SlabAllocator.h"
#include "StdAllocator.h"

typedef long long unsigned llu;

StdAllocator std_allocator_;
Zeta_Allocator std_allocator;

Zeta_SlabAllocator allocator_;
Zeta_Allocator allocator;

std::map<size_t, size_t> req_ptr_size_tm;

void main1() {
    StdAllocator_ToAllocator(&std_allocator_, &std_allocator);

    allocator_.align = 16;
    allocator_.width = sizeof(uintptr_t) * 3 + sizeof(size_t);
    allocator_.num = 32;
    allocator_.allocator = &std_allocator;

    Zeta_SlabAllocator_DeployAllocator(&allocator_, &allocator);

    void* ptr = allocator.Allocate(allocator.context, 32);

    ZETA_PrintVar("%p", ptr);
}

int main() {
    main1();

    printf("ok\n");

    return 0;
}
