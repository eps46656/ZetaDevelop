#include "OrdRBLinkedListNode.h"

static void* GetL_(void* n_) {
    Zeta_OrdRBLinkedListNode* n = n_;
    ZETA_DebugAssert(n != NULL);

    return ZETA_GetPtrFromAddr(ZETA_GetAddrFromPtr(n->l) / 2 * 2);
}

static void* GetR_(void* n_) {
    Zeta_OrdRBLinkedListNode* n = n_;
    ZETA_DebugAssert(n != NULL);

    return ZETA_GetPtrFromAddr(ZETA_GetAddrFromPtr(n->r) / 2 * 2);
}

static void SetL_(void* n_, void* l) {
    Zeta_OrdRBLinkedListNode* n = n_;
    ZETA_DebugAssert(n != NULL);

    n->l = ZETA_GetPtrFromAddr(ZETA_GetAddrFromPtr(l) +
                               ZETA_GetAddrFromPtr(n->l) % 2);
}

static void SetR_(void* n_, void* r) {
    Zeta_OrdRBLinkedListNode* n = n_;
    ZETA_DebugAssert(n != NULL);

    n->r = ZETA_GetPtrFromAddr(ZETA_GetAddrFromPtr(r) +
                               ZETA_GetAddrFromPtr(n->r) % 2);
}

void Zeta_OrdRBLinkedListNode_Init(void* n_) {
    Zeta_OrdRBLinkedListNode* n = n_;
    ZETA_DebugAssert(n != NULL);

    n->l = n;
    n->r = n;
}

void* Zeta_OrdRBLinkedListNode_GetL(void* n) { return GetL_(n); }

void* Zeta_OrdRBLinkedListNode_GetR(void* n) { return GetR_(n); }

int Zeta_OrdRBLinkedListNode_GetColor(void* n_) {
    Zeta_OrdRBLinkedListNode* n = n_;
    ZETA_DebugAssert(n != NULL);

    int lc = ZETA_GetAddrFromPtr(n->l) % 2;
    int rc = ZETA_GetAddrFromPtr(n->r) % 2;

    return rc * 2 + lc;
}

void Zeta_OrdRBLinkedListNode_SetColor(void* n_, int color) {
    Zeta_OrdRBLinkedListNode* n = n_;
    ZETA_DebugAssert(n != NULL);

    ZETA_DebugAssert(0 <= color);
    ZETA_DebugAssert(color < 4);

    int lc = color % 2;
    int rc = color / 2;

    n->l =
        ZETA_GetPtrFromAddr(ZETA_GetAddrFromPtr(n->l) / 2 * 2 + (uintptr_t)lc);

    n->r =
        ZETA_GetPtrFromAddr(ZETA_GetAddrFromPtr(n->r) / 2 * 2 + (uintptr_t)rc);
}

void Zeta_OrdRBLinkedListNode_InsertL(void* n, void* m) {
    ZETA_DebugAssert(n != NULL);
    ZETA_DebugAssert(m != NULL);
    ZETA_DebugAssert(GetL_(m) == m);

    void* nl = GetL_(n);

    SetL_(n, m);
    SetR_(m, n);

    SetL_(m, nl);
    SetR_(nl, m);
}

void Zeta_OrdRBLinkedListNode_InsertR(void* n, void* m) {
    ZETA_DebugAssert(n != NULL);
    ZETA_DebugAssert(m != NULL);
    ZETA_DebugAssert(GetL_(m) == m);

    void* nr = GetR_(n);

    SetR_(n, m);
    SetL_(m, n);

    SetR_(m, nr);
    SetL_(nr, m);
}

void Zeta_OrdRBLinkedListNode_Extract(void* n_) {
    Zeta_OrdRBLinkedListNode* n = n_;
    ZETA_DebugAssert(n != NULL);

    ZETA_DebugAssert(n != NULL);

    void* nl = GetL_(n);
    void* nr = GetR_(n);

    SetR_(nl, nr);
    SetL_(nr, nl);

    n->l = n;
    n->r = n;
}
