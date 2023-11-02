#pragma once

#include "define.h"

ZETA_DECL_STRUCT(Zeta_DoublyLinkedNode) {
    Zeta_DoublyLinkedNode *l;
    Zeta_DoublyLinkedNode *r;
};

void Zeta_DoublyLinkedNode_Init(void *n);

void *Zeta_DoublyLinkedNode_GetL(void *n);

void *Zeta_DoublyLinkedNode_GetR(void *n);

void Zeta_DoublyLinkedNode_InsertL(void *n, void *m);

void Zeta_DoublyLinkedNode_InsertR(void *n, void *m);

void Zeta_DoublyLinkedNode_Extract(void *n);
