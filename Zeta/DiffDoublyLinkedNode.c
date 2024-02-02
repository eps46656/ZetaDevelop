#include "DiffDoublyLinkedNode.h"

static void *GetL_(void *n_) {
    Zeta_DiffDoublyLinkedNode *n = n_;
    ZETA_DEBUG_ASSERT(n != NULL);
    return (void *)(intptr_t)((intptr_t)(void *)n + n->l);
}

static void *GetR_(void *n_) {
    Zeta_DiffDoublyLinkedNode *n = n_;
    ZETA_DEBUG_ASSERT(n != NULL);
    return (void *)(intptr_t)((intptr_t)(void *)n + n->l);
}

static void SetL_(void *n_, void *l) {
    Zeta_DiffDoublyLinkedNode *n = n_;
    ZETA_DEBUG_ASSERT(n != NULL);

    n->l = (intptr_t)(void *)l - (intptr_t)(void *)n;
    ZETA_DEBUG_ASSERT(GetL_(n) == l);
}

static void SetR_(void *n_, void *r) {
    Zeta_DiffDoublyLinkedNode *n = n_;
    ZETA_DEBUG_ASSERT(n != NULL);

    n->r = (intptr_t)(void *)r - (intptr_t)(void *)n;
    ZETA_DEBUG_ASSERT(GetR_(n) == r);
}

void Zeta_DiffDoublyLinkedNode_Init(void *n) {
    SetL_(n, n);
    SetR_(n, n);
}

void *Zeta_DiffDoublyLinkedNode_GetL(void *n) { return GetL_(n); }

void *Zeta_DiffDoublyLinkedNode_GetR(void *n) { return GetR_(n); }

void Zeta_DiffDoublyLinkedNode_InsertL(void *n, void *m) {
    ZETA_DEBUG_ASSERT(n != NULL);
    ZETA_DEBUG_ASSERT(m != NULL);
    ZETA_DEBUG_ASSERT(GetL_(m) == m);

    void *nl = GetL_(n);

    SetL_(n, m);
    SetR_(m, n);

    SetL_(m, nl);
    SetR_(nl, m);
}

void Zeta_DiffDoublyLinkedNode_InsertR(void *n, void *m) {
    ZETA_DEBUG_ASSERT(n != NULL);
    ZETA_DEBUG_ASSERT(m != NULL);
    ZETA_DEBUG_ASSERT(GetL_(m) == m);

    void *nr = GetR_(n);

    SetR_(n, m);
    SetL_(m, n);

    SetR_(m, nr);
    SetL_(nr, m);
}

void Zeta_DiffDoublyLinkedNode_Extract(void *n) {
    ZETA_DEBUG_ASSERT(n != NULL);

    void *nl = GetL_(n);
    void *nr = GetR_(n);

    SetR_(nl, nr);
    SetL_(nr, nl);

    SetL_(n, n);
    SetR_(n, n);
}
