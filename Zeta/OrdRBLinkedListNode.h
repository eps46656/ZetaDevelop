#pragma once

#include "define.h"

ZETA_ExternC_Beg;

typedef struct Zeta_OrdRBLinkedListNode Zeta_OrdRBLinkedListNode;

struct Zeta_OrdRBLinkedListNode {
    void* l;
    void* r;
} __attribute__((aligned(2)));

void Zeta_OrdRBLinkedListNode_Init(void* n);

void* Zeta_OrdRBLinkedListNode_GetL(void* n);

void* Zeta_OrdRBLinkedListNode_GetR(void* n);

int Zeta_OrdRBLinkedListNode_GetColor(void* n);

void Zeta_OrdRBLinkedListNode_SetColor(void* n, int r_color);

void Zeta_OrdRBLinkedListNode_InsertL(void* n, void* m);

void Zeta_OrdRBLinkedListNode_InsertR(void* n, void* m);

void Zeta_OrdRBLinkedListNode_Extract(void* n);

ZETA_ExternC_End;