#include "mem_check_utils.h"

#if ZETA_EnableDebug

#include <stdio.h>

#include <map>

#include "debugger.h"
#include "utils.h"

struct Zeta_MemRecorder {
    std::map<unsigned char const*, size_t> records;
};

Zeta_MemRecorder* Zeta_MemRecorder_Create() { return new Zeta_MemRecorder{}; }

void Zeta_MemRecorder_Destroy(Zeta_MemRecorder* mem_recorder) {
    delete mem_recorder;
}

size_t Zeta_MemRecorder_GetSize(Zeta_MemRecorder* mem_recorder) {
    ZETA_DebugAssert(mem_recorder != NULL);

    return mem_recorder->records.size();
}

size_t Zeta_MemRecorder_GetRecordSize(Zeta_MemRecorder* mem_recorder,
                                      void const* ptr_) {
    ZETA_DebugAssert(mem_recorder != NULL);

    unsigned char const* ptr = (unsigned char const*)ptr_;

    auto iter{ mem_recorder->records.find(ptr) };

    return iter == mem_recorder->records.end() ? ZETA_RangeMaxOf(size_t)
                                               : iter->second;
}

bool_t Zeta_MemRecorder_IsRecorded(Zeta_MemRecorder* mem_recorder,
                                   void const* ptr_) {
    ZETA_DebugAssert(mem_recorder != NULL);

    unsigned char const* ptr = (unsigned char const*)ptr_;

    return mem_recorder->records.find(ptr) != mem_recorder->records.end();
}

void Zeta_MemRecorder_Record(Zeta_MemRecorder* mem_recorder, void const* ptr_,
                             size_t size) {
    ZETA_DebugAssert(mem_recorder != NULL);

    unsigned char const* ptr = (unsigned char const*)ptr_;

    auto iter{ mem_recorder->records.lower_bound(ptr) };

    if (iter != mem_recorder->records.end()) {
        ZETA_DebugAssert(iter->first != ptr);
        ZETA_DebugAssert(ptr + size <= iter->first);
    }

    if (iter != mem_recorder->records.begin()) {
        --iter;
        ZETA_DebugAssert(iter->first + iter->second <= ptr);
    }

    mem_recorder->records.insert({ ptr, size });
}

bool_t Zeta_MemRecorder_Unrecord(Zeta_MemRecorder* mem_recorder,
                                 void const* ptr) {
    ZETA_DebugAssert(mem_recorder != NULL);

    return mem_recorder->records.erase((unsigned char const*)ptr) != 0;
}

void Zeta_MemRecorder_Clear(Zeta_MemRecorder* mem_recorder) {
    ZETA_DebugAssert(mem_recorder != NULL);

    mem_recorder->records.clear();
}

void Zeta_MemCheck_MatchRecords(Zeta_MemRecorder const* src_mem_recorder,
                                Zeta_MemRecorder const* dst_mem_recorder) {
    ZETA_DebugAssert(src_mem_recorder != NULL);
    ZETA_DebugAssert(dst_mem_recorder != NULL);

    ZETA_DebugAssert(src_mem_recorder->records.size() ==
                     dst_mem_recorder->records.size());

    for (auto src_iter{ src_mem_recorder->records.begin() },
         src_end{ src_mem_recorder->records.end() },
         dst_iter{ dst_mem_recorder->records.begin() };
         src_iter != src_end; ++src_iter, ++dst_iter) {
        ZETA_DebugAssert(!(src_iter->first < dst_iter->first));
        // memory leek: dst misses allocated memory.

        ZETA_DebugAssert(!(dst_iter->first < src_iter->first));
        // hallucination or use after free: dst uses unallocated memoty.

        ZETA_DebugAssert(dst_iter->second <= src_iter->second);
        // overflow: dst uses more than allocated memory.
    }
}

#endif
