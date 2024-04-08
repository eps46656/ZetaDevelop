#include "OrdLinkedListNode.h"

void Zeta_OrdLinkedListNode_Init(void* n_) {
    Zeta_OrdLinkedListNode* n = n_;
    ZETA_DebugAssert(n != NULL);

    n->l = n;
    n->r = n;
}

void* Zeta_OrdLinkedListNode_GetL(void* n_) {
    Zeta_OrdLinkedListNode* n = n_;
    ZETA_DebugAssert(n != NULL);

    return n->l;
}

void* Zeta_OrdLinkedListNode_GetR(void* n_) {
    Zeta_OrdLinkedListNode* n = n_;
    ZETA_DebugAssert(n != NULL);

    return n->r;
}

void Zeta_OrdLinkedListNode_InsertL(void* n_, void* m_) {
    Zeta_OrdLinkedListNode* n = n_;
    Zeta_OrdLinkedListNode* m = m_;

    ZETA_DebugAssert(n != NULL);
    ZETA_DebugAssert(m != NULL);
    ZETA_DebugAssert(m->l == m);

    Zeta_OrdLinkedListNode* nl = n->l;

    n->l = m;
    m->r = n;

    m->l = nl;
    nl->r = m;
}

void Zeta_OrdLinkedListNode_InsertR(void* n_, void* m_) {
    Zeta_OrdLinkedListNode* n = n_;
    Zeta_OrdLinkedListNode* m = m_;

    ZETA_DebugAssert(n != NULL);
    ZETA_DebugAssert(m != NULL);
    ZETA_DebugAssert(m->l == m);

    Zeta_OrdLinkedListNode* nr = n->r;

    n->r = m;
    m->l = n;

    m->r = nr;
    nr->l = m;
}

void Zeta_OrdLinkedListNode_Extract(void* n_) {
    Zeta_OrdLinkedListNode* n = n_;
    ZETA_DebugAssert(n != NULL);

    Zeta_OrdLinkedListNode* nl = n->l;
    Zeta_OrdLinkedListNode* nr = n->r;

    nl->r = nr;
    nr->l = nl;

    n->l = n;
    n->r = n;
}
