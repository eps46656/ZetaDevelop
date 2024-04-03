#pragma once

#include "define.h"

ZETA_ExternC_Beg;

typedef struct Zeta_RelRBLinkedListNode Zeta_RelRBLinkedListNode;

struct Zeta_RelRBLinkedListNode {
    uintptr_t lc;
    uintptr_t rc;
} __attribute__((aligned(2)));

void Zeta_RelRBLinkedListNode_Init(void* n);

void* Zeta_RelRBLinkedListNode_GetL(void* n);

void* Zeta_RelRBLinkedListNode_GetR(void* n);

int Zeta_RelRBLinkedListNode_GetLColor(void* n);

int Zeta_RelRBLinkedListNode_GetRColor(void* n);

void Zeta_RelRBLinkedListNode_SetLColor(void* n, int l_color);

void Zeta_RelRBLinkedListNode_SetRColor(void* n, int r_color);

void Zeta_RelRBLinkedListNode_InsertL(void* n, void* m);

void Zeta_RelRBLinkedListNode_InsertR(void* n, void* m);

void Zeta_RelRBLinkedListNode_Extract(void* n);

ZETA_ExternC_End;
