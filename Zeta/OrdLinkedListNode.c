#include "OrdLinkedListNode.h"

static void* GetL_(void* n_) {
    Zeta_OrdLinkedListNode* n = n_;
    ZETA_DebugAssert(n != NULL);

    return n->l;
}

static void* GetR_(void* n_) {
    Zeta_OrdLinkedListNode* n = n_;
    ZETA_DebugAssert(n != NULL);

    return n->r;
}

static void SetL_(void* n_, void* l) {
    Zeta_OrdLinkedListNode* n = n_;
    ZETA_DebugAssert(n != NULL);

    n->l = l;
}

static void SetR_(void* n_, void* r) {
    Zeta_OrdLinkedListNode* n = n_;
    ZETA_DebugAssert(n != NULL);

    n->r = r;
}

void Zeta_OrdLinkedListNode_Init(void* n) {
    SetL_(n, n);
    SetR_(n, n);
}

void* Zeta_OrdLinkedListNode_GetL(void* n) { return GetL_(n); }

void* Zeta_OrdLinkedListNode_GetR(void* n) { return GetR_(n); }

void Zeta_OrdLinkedListNode_InsertL(void* n, void* m) {
    ZETA_DebugAssert(n != NULL);
    ZETA_DebugAssert(m != NULL);
    ZETA_DebugAssert(GetL_(m) == m);

    void* nl = GetL_(n);

    SetL_(n, m);
    SetR_(m, n);

    SetL_(m, nl);
    SetR_(nl, m);
}

void Zeta_OrdLinkedListNode_InsertR(void* n, void* m) {
    ZETA_DebugAssert(n != NULL);
    ZETA_DebugAssert(m != NULL);
    ZETA_DebugAssert(GetL_(m) == m);

    void* nr = GetR_(n);

    SetR_(n, m);
    SetL_(m, n);

    SetR_(m, nr);
    SetL_(nr, m);
}

void Zeta_OrdLinkedListNode_Extract(void* n) {
    ZETA_DebugAssert(n != NULL);

    void* nl = GetL_(n);
    void* nr = GetR_(n);

    SetR_(nl, nr);
    SetL_(nr, nl);

    SetL_(n, n);
    SetR_(n, n);
}
