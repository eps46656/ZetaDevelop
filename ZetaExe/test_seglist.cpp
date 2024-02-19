#define ZetaDir

#include "../Zeta/DebugDeque.h"
#include "../Zeta/SegList.h"
#include "../Zeta/utils.h"
#include "StdAllocator.h"

typedef unsigned int val_t;

#define WIDTH sizeof(val_t)

void SetVal(void* dst, val_t x) { Zeta_MemCopy(WIDTH, dst, &x); }

void PrintVal(void* x) {
    unsigned int xi;
    Zeta_MemCopy(WIDTH, &xi, x);
    printf("%x\n", xi);
}

StdAllocator node_allocator_;
Zeta_Allocator node_allocator;

StdAllocator cluster_allocator_;
Zeta_Allocator cluster_allocator;

Zeta_SegList sl;
Zeta_DebugDeque dd;

Zeta_Vector v1;
Zeta_Vector v2;

void SetupSegList() {
    sl.width = WIDTH;
    sl.cluster_capacity = 4;
    sl.node_allocator = &node_allocator;
    sl.cluster_allocator = &cluster_allocator;
    Zeta_SegList_Init(&sl);
};

void SetupDebugDeque() {
    dd.width = WIDTH;
    Zeta_DebugDeque_Create(&dd);
}

void* AccessV1(size_t idx) { return v1.Access(v1.context, idx); }
void* InsertV1(size_t idx) { return v1.Insert(v1.context, idx); }
void EraseV1(size_t idx) { return v1.Erase(v1.context, idx); }

void main1() {
    StdAllocator_ToAllocator(&node_allocator_, &node_allocator);
    StdAllocator_ToAllocator(&cluster_allocator_, &cluster_allocator);

    SetupSegList();
    SetupDebugDeque();

    Zeta_SegList_ToVector(&sl, &v1);
    Zeta_DebugDeque_ToVector(&dd, &v2);

    SetVal(v2.Insert(v2.context, 0), 9);
    SetVal(v2.Insert(v2.context, 0), 8);
    SetVal(v2.Insert(v2.context, 0), 7);
    SetVal(v2.Insert(v2.context, 0), 6);
    SetVal(v2.Insert(v2.context, 0), 5);
    SetVal(v2.Insert(v2.context, 0), 4);

    PrintVal(v2.Access(v2.context, 0));
    PrintVal(v2.Access(v2.context, 1));
    PrintVal(v2.Access(v2.context, 2));
    PrintVal(v2.Access(v2.context, 3));
    PrintVal(v2.Access(v2.context, 4));
}

int main() {
    main1();
    printf("ok\n");
    return 0;
}
