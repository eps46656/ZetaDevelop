#pragma once

#include "define.h"

typedef struct Zeta_OrdDoublyLinkedNode Zeta_OrdDoublyLinkedNode;

struct Zeta_OrdDoublyLinkedNode {
    Zeta_OrdDoublyLinkedNode *l;
    Zeta_OrdDoublyLinkedNode *r;
};

EXTERN_C void Zeta_OrdDoublyLinkedNode_Init(void *n);

EXTERN_C void *Zeta_OrdDoublyLinkedNode_GetL(void *n);

EXTERN_C void *Zeta_OrdDoublyLinkedNode_GetR(void *n);

EXTERN_C void Zeta_OrdDoublyLinkedNode_InsertL(void *n, void *m);

EXTERN_C void Zeta_OrdDoublyLinkedNode_InsertR(void *n, void *m);

EXTERN_C void Zeta_OrdDoublyLinkedNode_Extract(void *n);
