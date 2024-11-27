
#include <lin_space_allocator.h>

#include <ctime>
#include <vector>

#include "random.h"
#include "std_allocator.h"

StdAllocator std_allocator_;
Zeta_Allocator std_allocator;

Zeta_MemRecorder* ns;

size_t LinSpaceAllocator_Allocate(Zeta_LinSpaceAllocator* lsa, size_t size) {
    size_t ret = Zeta_LinSpaceAllocator_Allocate(lsa, size);

    Zeta_LinSpaceAllocator_Sanitize(lsa, ns);
    Zeta_MemCheck_MatchRecords(std_allocator_.mem_recorder, ns);

    return ret;
}

void LinSpaceAllocator_Deallocate(Zeta_LinSpaceAllocator* lsa, size_t idx) {
    Zeta_LinSpaceAllocator_Deallocate(lsa, idx);

    Zeta_LinSpaceAllocator_Sanitize(lsa, ns);
    Zeta_MemCheck_MatchRecords(std_allocator_.mem_recorder, ns);
}

#define RangeLoop(type, var, beg, end, delta) \
    for (type var = (beg); var < (end); var += delta)

void main1() {
    ZETA_PrintCurPos;

    size_t beg = 16 * 1024 * 1024;
    size_t size = 4 * 1024;

    StdAllocator_DeployAllocator(&std_allocator_, &std_allocator);

    Zeta_LinSpaceAllocator lin_space_allocator;
    lin_space_allocator.beg = beg;
    lin_space_allocator.end = beg + size;
    lin_space_allocator.node_allocator = &std_allocator;

    Zeta_LinSpaceAllocator_Init(&lin_space_allocator);

    Zeta_LinSpaceAllocator_Sanitize(&lin_space_allocator, NULL);

    ns = Zeta_MemRecorder_Create();

    std::vector<size_t> idxes;

    size_t K{ 256 };

    RangeLoop(size_t, test_i, 0, 64, 1) {
        ZETA_PrintVar(test_i);

        RangeLoop(size_t, ZETA_TmpName, 0, 64, 1) {
            idxes.push_back(LinSpaceAllocator_Allocate(&lin_space_allocator,
                                                       GetRandomInt(0, K)));
        }

        RangeLoop(size_t, test_j, 0, 64, 1) {
            ZETA_PrintVar(test_j);

            RangeLoop(size_t, ZETA_TmpName, 0, 1024, 1) {
                size_t idx = LinSpaceAllocator_Allocate(&lin_space_allocator,
                                                        GetRandomInt(0, K));

                if (idx != ZETA_RangeMaxOf(size_t)) { idxes.push_back(idx); }
            }

            RangeLoop(size_t, ZETA_TmpName, 0, 1024, 1) {
                if (idxes.empty()) { break; }

                long long target{ GetRandomInt(
                    0, static_cast<long long>(idxes.size()) - 1) };

                LinSpaceAllocator_Deallocate(&lin_space_allocator,
                                             idxes[target]);
                idxes[target] = idxes.back();
                idxes.pop_back();
            }
        }
    }
}

int main() {
    unsigned long long random_seed{ (unsigned long long)time(NULL) };
    unsigned long long fixed_seed{ 0123456 };

    ZETA_Unused(random_seed);
    ZETA_Unused(fixed_seed);

    unsigned long long seed = random_seed;

    SetRandomSeed(seed);

    ZETA_PrintCurPos;

    main1();

    ZETA_PrintCurPos;

    ZETA_PrintVar("ok");

    return 0;
}
