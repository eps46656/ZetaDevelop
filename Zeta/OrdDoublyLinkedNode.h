#pragma once

#include "define.h"

ZETA_extern_c_beg;

typedef struct Zeta_OrdDoublyLinkedNode Zeta_OrdDoublyLinkedNode;

struct Zeta_OrdDoublyLinkedNode {
    Zeta_OrdDoublyLinkedNode* l;
    Zeta_OrdDoublyLinkedNode* r;
};

void Zeta_OrdDoublyLinkedNode_Init(void* n);

void* Zeta_OrdDoublyLinkedNode_GetL(void* n);

void* Zeta_OrdDoublyLinkedNode_GetR(void* n);

void Zeta_OrdDoublyLinkedNode_InsertL(void* n, void* m);

void Zeta_OrdDoublyLinkedNode_InsertR(void* n, void* m);

void Zeta_OrdDoublyLinkedNode_Extract(void* n);

ZETA_extern_c_end;
