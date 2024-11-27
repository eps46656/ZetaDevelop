#include "ord_linked_list_node.h"

#include "debugger.h"

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

size_t Zeta_OrdLinkedListNode_Count(void* n_, void* m_) {
    Zeta_OrdLinkedListNode* n = n_;
    Zeta_OrdLinkedListNode* m = m_;

    ZETA_DebugAssert(n != NULL);
    ZETA_DebugAssert(m != NULL);

    size_t ret = 1;

    for (; n != m; n = n->r) { ++ret; }

    return ret;
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

void Zeta_OrdLinkedListNode_InsertSegL(void* n_, void* m_beg_, void* m_end_) {
    Zeta_OrdLinkedListNode* n = n_;
    Zeta_OrdLinkedListNode* m_beg = m_beg_;
    Zeta_OrdLinkedListNode* m_end = m_end_;

    ZETA_DebugAssert(n != NULL);
    ZETA_DebugAssert(m_beg != NULL);
    ZETA_DebugAssert(m_end != NULL);

    Zeta_OrdLinkedListNode* nl = n->l;

    Zeta_OrdLinkedListNode* m_beg_l = m_beg->l;
    Zeta_OrdLinkedListNode* m_end_r = m_end->r;

    m_end_r->l = m_beg_l;
    m_beg_l->r = m_end_r;

    n->l = m_end;
    m_end->r = n;

    m_beg->l = nl;
    nl->r = m_beg;
}

void Zeta_OrdLinkedListNode_InsertSegR(void* n_, void* m_beg_, void* m_end_) {
    Zeta_OrdLinkedListNode* n = n_;
    Zeta_OrdLinkedListNode* m_beg = m_beg_;
    Zeta_OrdLinkedListNode* m_end = m_end_;

    ZETA_DebugAssert(n != NULL);
    ZETA_DebugAssert(m_beg != NULL);
    ZETA_DebugAssert(m_end != NULL);

    Zeta_OrdLinkedListNode* nr = n->r;

    Zeta_OrdLinkedListNode* m_beg_l = m_beg->l;
    Zeta_OrdLinkedListNode* m_end_r = m_end->r;

    m_beg_l->r = m_end_r;
    m_end_r->l = m_beg_l;

    n->r = m_beg;
    m_beg->l = n;

    m_end->r = nr;
    nr->l = m_end;
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
