#pragma once

#include "BinTree.h"

ZETA_ExternC_Beg;

typedef struct Zeta_BinHeap Zeta_BinHeap;

struct Zeta_BinHeap {
    size_t size;

    void* root;

    Zeta_BinTreeNodeOperator* btn_opr;
};

void Zeta_BinHeap_Init(void* bh);

size_t Zeta_BinHeap_GetSize(void* bh);

void* Zeta_BinHeap_Access(void* bh, size_t idx);

void Zeta_BinHeap_Insert(void* bh, void* n);

void* Zeta_BinHeap_Extract(void* bh, void* n);

void Zeta_BinHeap_UpdateRoot(void* bh);

void Zeta_BinHeap_Check(void* bh);

ZETA_ExternC_End;
