#include <unordered_map>

#include "../Zeta/lru_cache_manager.h"

struct StdAllocator {
    std::unordered_map<void*, size_t> ptr_size;
};

size_t StdQuery(void* sa_, size_t size) {
    ZETA_Unused(sa_);
    return 0 < size ? size : 0;
}

void* StdAllocate(void* sa_, size_t size) {
    StdAllocator* sa = (StdAllocator*)sa_;

    if (size == 0) { return NULL; }

    void* ptr = malloc(size);
    sa->ptr_size.insert({ ptr, size });

    return ptr;
}

void StdDeallocate(void* sa_, void* ptr) {
    if (ptr == NULL) { return; }

    StdAllocator* sa = (StdAllocator*)sa_;

    bool_t b = sa->ptr_size.erase(ptr) != 0;
    ZETA_DebugAssert(b);

    free(ptr);
}

StdAllocator lrucm_allocator;

Zeta_Allocator lrucm_allocator_i = {
    .context = &lrucm_allocator,
    .Query = StdQuery,
    .Allocate = StdAllocate,
    .Deallocate = StdDeallocate,
};

void main1() {
    Zeta_LRUCacheManager lrucm;
    lrucm.allocator = &lrucm_allocator_i;
    Zeta_LRUCacheManager_Init(&lrucm);

    Zeta_LRUCacheManager_PrintState(&lrucm);

    Zeta_LRUCacheManager_Push(&lrucm, 456);
    Zeta_LRUCacheManager_Push(&lrucm, 789);
    Zeta_LRUCacheManager_Push(&lrucm, 112);
    Zeta_LRUCacheManager_Push(&lrucm, 365);
    Zeta_LRUCacheManager_PrintState(&lrucm);

    Zeta_LRUCacheManager_Find(&lrucm, 456);
    Zeta_LRUCacheManager_PrintState(&lrucm);

    Zeta_LRUCacheManager_Push(&lrucm, 789);
    Zeta_LRUCacheManager_PrintState(&lrucm);

    Zeta_LRUCacheManager_Pop(&lrucm);
    Zeta_LRUCacheManager_PrintState(&lrucm);

    Zeta_LRUCacheManager_PopWithKey(&lrucm, 456);
    Zeta_LRUCacheManager_PrintState(&lrucm);
}

int main() {
    main1();

    return 0;
}
