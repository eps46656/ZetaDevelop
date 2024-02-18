#pragma once

#include "define.h"

ZETA_extern_c_beg;

typedef struct Zeta_OrdLinkedListNode Zeta_OrdLinkedListNode;

struct Zeta_OrdLinkedListNode {
    Zeta_OrdLinkedListNode* l;
    Zeta_OrdLinkedListNode* r;
};

void Zeta_OrdLinkedListNode_Init(void* n);

void* Zeta_OrdLinkedListNode_GetL(void* n);

void* Zeta_OrdLinkedListNode_GetR(void* n);

void Zeta_OrdLinkedListNode_InsertL(void* n, void* m);

void Zeta_OrdLinkedListNode_InsertR(void* n, void* m);

void Zeta_OrdLinkedListNode_Extract(void* n);

ZETA_extern_c_end;
