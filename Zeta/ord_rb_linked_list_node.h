#pragma once

#include "define.h"

ZETA_ExternC_Beg;

ZETA_DeclareStruct(Zeta_OrdRBLinkedListNode);

struct Zeta_OrdRBLinkedListNode {
    unsigned char* l;
    unsigned char* r;
} __attribute__((aligned(2)));

void Zeta_OrdRBLinkedListNode_Init(void* n);

void* Zeta_OrdRBLinkedListNode_GetL(void* n);
void* Zeta_OrdRBLinkedListNode_GetR(void* n);

int Zeta_OrdRBLinkedListNode_GetColor(void* n);
void Zeta_OrdRBLinkedListNode_SetColor(void* n, int color);

size_t Zeta_OrdRBLinkedListNode_Count(void* n, void* m);

void Zeta_OrdRBLinkedListNode_InsertL(void* n, void* m);
void Zeta_OrdRBLinkedListNode_InsertR(void* n, void* m);

void Zeta_OrdRBLinkedListNode_InsertSegL(void* n, void* m_beg, void* m_end);
void Zeta_OrdRBLinkedListNode_InsertSegR(void* n, void* m_beg, void* m_end);

void Zeta_OrdRBLinkedListNode_Extract(void* n);

ZETA_ExternC_End;
