#pragma once

#include "define.h"

ZETA_ExternC_Beg;

typedef struct Zeta_OrdLinkedListNode Zeta_OrdLinkedListNode;

struct Zeta_OrdLinkedListNode {
    Zeta_OrdLinkedListNode* l;
    Zeta_OrdLinkedListNode* r;
};

void Zeta_OrdLinkedListNode_Init(void* n);

void* Zeta_OrdLinkedListNode_GetL(void* n);
void* Zeta_OrdLinkedListNode_GetR(void* n);

size_t Zeta_OrdLinkedListNode_Count(void* n, void* m);

void Zeta_OrdLinkedListNode_InsertL(void* n, void* m);
void Zeta_OrdLinkedListNode_InsertR(void* n, void* m);

void Zeta_OrdLinkedListNode_InsertSegL(void* n, void* m_beg, void* m_end);
void Zeta_OrdLinkedListNode_InsertSegR(void* n, void* m_beg, void* m_end);

void Zeta_OrdLinkedListNode_Extract(void* n);

ZETA_ExternC_End;
