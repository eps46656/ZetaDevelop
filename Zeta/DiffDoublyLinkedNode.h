#pragma once

#include "define.h"

ZETA_DECL_STRUCT(Zeta_DiffDoublyLinkedNode) {
    diff_t l;
    diff_t r;
};

void Zeta_DiffDoublyLinkedNode_Init(void *n);

void *Zeta_DiffDoublyLinkedNode_GetL(void *n);

void *Zeta_DiffDoublyLinkedNode_GetR(void *n);

void Zeta_DiffDoublyLinkedNode_InsertL(void *n, void *m);

void Zeta_DiffDoublyLinkedNode_InsertR(void *n, void *m);

void Zeta_DiffDoublyLinkedNode_Extract(void *n);
