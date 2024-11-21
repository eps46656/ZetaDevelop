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

void Zeta_CascadeAllocator_Init(void* fba);

size_t Zeta_CascadeAllocator_Query(void* fba, size_t size);

size_t Zeta_CascadeAllocator_GetAlign(void* fba);

void* Zeta_CascadeAllocator_Allocate(void* fba, size_t size);

void Zeta_CascadeAllocator_Deallocate(void* fba, void* ptr);

void Zeta_CascadeAllocator_Check(void* ca);

void Zeta_CascadeAllocator_Sanitize(void* ca, Zeta_MemRecorder* dst_recorder);

void Zeta_CascadeAllocator_DeployAllocator(void* ca, Zeta_Allocator* allocator);

ZETA_ExternC_End;
