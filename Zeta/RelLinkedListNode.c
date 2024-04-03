#include "RelLinkedListNode.h"

static void* GetL_(void* n_) {
    Zeta_RelLinkedListNode* n = n_;
    ZETA_DebugAssert(n != NULL);

    return ZETA_GetPtrFromAddr(ZETA_GetAddrFromPtr(n) + n->l);
}

static void* GetR_(void* n_) {
    Zeta_RelLinkedListNode* n = n_;
    ZETA_DebugAssert(n != NULL);

    return ZETA_GetPtrFromAddr(ZETA_GetAddrFromPtr(n) + n->r);
}

static void SetL_(void* n_, void* l) {
    Zeta_RelLinkedListNode* n = n_;
    ZETA_DebugAssert(n != NULL);

    n->l = ZETA_GetAddrFromPtr(l) - ZETA_GetAddrFromPtr(n);
}

static void SetR_(void* n_, void* r) {
    Zeta_RelLinkedListNode* n = n_;
    ZETA_DebugAssert(n != NULL);

    n->r = ZETA_GetAddrFromPtr(r) - ZETA_GetAddrFromPtr(n);
}

void Zeta_RelLinkedListNode_Init(void* n) {
    SetL_(n, n);
    SetR_(n, n);
}

void* Zeta_RelLinkedListNode_GetL(void* n) { return GetL_(n); }

void* Zeta_RelLinkedListNode_GetR(void* n) { return GetR_(n); }

void Zeta_RelLinkedListNode_InsertL(void* n, void* m) {
    ZETA_DebugAssert(n != NULL);
    ZETA_DebugAssert(m != NULL);
    ZETA_DebugAssert(GetL_(m) == m);

    void* nl = GetL_(n);

    SetL_(n, m);
    SetR_(m, n);

    SetL_(m, nl);
    SetR_(nl, m);
}

void Zeta_RelLinkedListNode_InsertR(void* n, void* m) {
    ZETA_DebugAssert(n != NULL);
    ZETA_DebugAssert(m != NULL);
    ZETA_DebugAssert(GetL_(m) == m);

    void* nr = GetR_(n);

    SetR_(n, m);
    SetL_(m, n);

    SetR_(m, nr);
    SetL_(nr, m);
}

void Zeta_RelLinkedListNode_Extract(void* n) {
    ZETA_DebugAssert(n != NULL);

    void* nl = GetL_(n);
    void* nr = GetR_(n);

    SetR_(nl, nr);
    SetL_(nr, nl);

    SetL_(n, n);
    SetR_(n, n);
}
