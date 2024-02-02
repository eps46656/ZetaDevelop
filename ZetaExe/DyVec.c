
#include "../Zeta/DynamicVector.h"

typedef unsigned long long data_t;

ZETA_DECL_STRUCT(DyVec) { Zeta_DynamicVector dv; };

void* StdAllocate(void* context, size_t size) {
    ZETA_UNUSED(context);
    return malloc(size);
}

void StdDeallocate(void* context, void* ptr) {
    ZETA_UNUSED(context);
    free(ptr);
}

Zeta_Allocator std_allocator = {
    .context = NULL,

    .Allocate = StdAllocate,

    .Deallocate = StdDeallocate,
};

void DyVec_Init(void* dy_vec_) {
    DyVec* dyvec = dy_vec_;
    ZETA_DEBUG_ASSERT(dyvec != NULL);

    dyvec->dv.width = sizeof(data_t);

    dyvec->dv.cluster_capacity = 64 / sizeof(data_t);

    dyvec->dv.offset = 0;
    dyvec->dv.size = 0;

    dyvec->dv.mlet.level = 5;

    dyvec->dv.mlet.branch_nums[0] = 64;
    dyvec->dv.mlet.branch_nums[1] = 256;
    dyvec->dv.mlet.branch_nums[2] = 256;
    dyvec->dv.mlet.branch_nums[3] = 512;
    dyvec->dv.mlet.branch_nums[4] = 512;

    dyvec->dv.mlet.root = NULL;

    dyvec->dv.mlet.allocator = &std_allocator;

    dyvec->dv.allocator = &std_allocator;
}

void* DyVec_Create() {
    DyVec* dyvec = malloc(sizeof(DyVec));
    DyVec_Init(dyvec);
    return dyvec;
}

size_t DyVec_GetSize(void* dy_vec_) {
    DyVec* dyvec = dy_vec_;
    ZETA_DEBUG_ASSERT(dyvec != NULL);

    return dyvec->dv.size;
}

data_t DyVec_GetVal(void* dy_vec_, size_t idx) {
    DyVec* dyvec = dy_vec_;
    ZETA_DEBUG_ASSERT(dyvec != NULL);

    return *(data_t*)Zeta_DynamicVector_Access(&dyvec->dv, idx);
}

void DyVec_SetVal(void* dy_vec_, size_t idx, data_t val) {
    DyVec* dyvec = dy_vec_;
    ZETA_DEBUG_ASSERT(dyvec != NULL);

    *(data_t*)Zeta_DynamicVector_Access(&dyvec->dv, idx) = val;
}

void DyVec_Insert(void* dy_vec_, size_t idx, data_t val) {
    DyVec* dyvec = dy_vec_;
    ZETA_DEBUG_ASSERT(dyvec != NULL);

    *(data_t*)Zeta_DynamicVector_Insert(&dyvec->dv, idx) = val;
}

void DyVec_Erase(void* dy_vec_, size_t idx) {
    DyVec* dyvec = dy_vec_;
    ZETA_DEBUG_ASSERT(dyvec != NULL);

    Zeta_DynamicVector_Erase(&dyvec->dv, idx);
}

void DyVec_Check(void* dy_vec_) { ZETA_UNUSED(dy_vec_); }
