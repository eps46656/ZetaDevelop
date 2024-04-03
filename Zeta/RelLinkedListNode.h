#pragma once

#include "define.h"

ZETA_ExternC_Beg;

typedef struct Zeta_RelLinkedListNode Zeta_RelLinkedListNode;

struct Zeta_RelLinkedListNode {
    uintptr_t l;
    uintptr_t r;
};

void Zeta_RelLinkedListNode_Init(void* n);

void* Zeta_RelLinkedListNode_GetL(void* n);

void* Zeta_RelLinkedListNode_GetR(void* n);

void Zeta_RelLinkedListNode_InsertL(void* n, void* m);

void Zeta_RelLinkedListNode_InsertR(void* n, void* m);

void Zeta_RelLinkedListNode_Extract(void* n);

ZETA_ExternC_End;
