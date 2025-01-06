#pragma once

#include "allocator.h"
#include "mem_check_utils.h"

ZETA_ExternC_Beg;

ZETA_DeclareStruct(Zeta_CascadeAllocator);

struct Zeta_CascadeAllocator {
    size_t align;

    void* mem;
    size_t size;

    void* first_node;
};

void Zeta_CascadeAllocator_Init(void* ca);

size_t Zeta_CascadeAllocator_GetAlign(void const* ca);

size_t Zeta_CascadeAllocator_Query(void const* ca, size_t size);

void* Zeta_CascadeAllocator_Allocate(void* ca, size_t size);

void Zeta_CascadeAllocator_Deallocate(void* ca, void* ptr);

void Zeta_CascadeAllocator_Check(void const* ca);

void Zeta_CascadeAllocator_Sanitize(void const* ca,
                                    Zeta_MemRecorder* dst_recorder);

void Zeta_CascadeAllocator_DeployAllocator(void* ca, Zeta_Allocator* allocator);

ZETA_ExternC_End;
