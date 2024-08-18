#pragma once

#include "MemCheck.h"

ZETA_ExternC_Beg;

ZETA_DeclareStruct(Zeta_Allocator);

struct Zeta_Allocator {
    void* context;

    size_t (*GetAlign)(void* context);

    size_t (*Query)(void* context, size_t size);

    void* (*Allocate)(void* context, size_t size);

    void (*Deallocate)(void* context, void* ptr);

    Zeta_MemRecorder* (*GetRecords)(void* context);
};

void Zeta_Allocator_Init(void* allocator);

#define ZETA_Allocator_SafeAllocate_(tmp_ret, tmp_allocator, tmp_align,    \
                                     tmp_size, allocator, align, size)     \
    ({                                                                     \
        Zeta_Allocator* tmp_allocator = (allocator);                       \
        size_t tmp_align = (align);                                        \
        size_t tmp_size = (size);                                          \
                                                                           \
        ZETA_DebugAssert(0 < tmp_align);                                   \
                                                                           \
        ZETA_DebugAssert(tmp_allocator != NULL);                           \
        ZETA_DebugAssert(tmp_allocator->Allocate != NULL);                 \
                                                                           \
        void* tmp_ret;                                                     \
                                                                           \
        if (size == 0) {                                                   \
            tmp_ret = NULL;                                                \
        } else {                                                           \
            tmp_ret =                                                      \
                tmp_allocator->Allocate(tmp_allocator->context, tmp_size); \
            ZETA_DebugAssert(tmp_ret != NULL);                             \
            ZETA_DebugAssert(__builtin_is_aligned(tmp_ret, tmp_align));    \
        }                                                                  \
                                                                           \
        tmp_ret;                                                           \
    })

#define ZETA_Allocator_SafeAllocate(allocator, align, size)                \
    ZETA_Allocator_SafeAllocate_(ZETA_TmpName, ZETA_TmpName, ZETA_TmpName, \
                                 ZETA_TmpName, allocator, align, size)

ZETA_ExternC_End;
