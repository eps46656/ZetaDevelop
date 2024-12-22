#pragma once

#include "define.h"

ZETA_ExternC_Beg;

ZETA_DeclareStruct(Zeta_SegTree);

struct Zeta_SegTree {
    size_t n;
    size_t* data;
};

size_t Zeta_SegTree_GetDataSize(size_t n);

void Zeta_SegTree_Init(void* st);

size_t Zeta_SegTree_Query(void* st, size_t lb, size_t rb);

size_t Zeta_SegTree_Set(void* st, size_t idx, size_t val);

size_t Zeta_SegTree_Del(void* st, size_t idx, size_t del);

size_t Zeta_SegTree_Find(void* st, size_t offset, size_t* dst_res_offset);

void Zeta_SegTree_Check(void* st);

void Zeta_SegTree_Sanitize(void* st);

ZETA_ExternC_End;
