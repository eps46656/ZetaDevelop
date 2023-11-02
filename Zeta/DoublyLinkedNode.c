#include "DoublyLinkedNode.h"

void Zeta_DoublyLinkedNode_Init(void *n_) {
    Zeta_DoublyLinkedNode *n = n_;
    n->l = n;
    n->r = n;
}

void *Zeta_DoublyLinkedNode_GetL(void *n_) {
    Zeta_DoublyLinkedNode *n = n_;
    ZETA_DEBUG_ASSERT(n != NULL);
    return n->l;
}

void *Zeta_DoublyLinkedNode_GetR(void *n_) {
    Zeta_DoublyLinkedNode *n = n_;
    ZETA_DEBUG_ASSERT(n != NULL);
    return n->r;
}

void Zeta_DoublyLinkedNode_InsertL(void *n_, void *m_) {
    Zeta_DoublyLinkedNode *n = n_;
    Zeta_DoublyLinkedNode *m = m_;

    ZETA_DEBUG_ASSERT(n != NULL);
    ZETA_DEBUG_ASSERT(m != NULL);
    ZETA_DEBUG_ASSERT(m->l == m);

    Zeta_DoublyLinkedNode *nl = n->l;

    n->l = m;
    m->r = n;
    m->l = nl;
    nl->r = m;
}

void Zeta_DoublyLinkedNode_InsertR(void *n_, void *m_) {
    struct Zeta_DoublyLinkedNode *n = n_;
    struct Zeta_DoublyLinkedNode *m = m_;

    ZETA_DEBUG_ASSERT(n != NULL);
    ZETA_DEBUG_ASSERT(m != NULL);
    ZETA_DEBUG_ASSERT(m->l == m);

    Zeta_DoublyLinkedNode *nr = n->r;

    n->r = m;
    m->l = n;
    m->r = nr;
    nr->l = m;
}

void Zeta_DoublyLinkedNode_Extract(void *n_) {
    Zeta_DoublyLinkedNode *n = n_;

    ZETA_DEBUG_ASSERT(n != NULL);

    Zeta_DoublyLinkedNode *nl = n->r;
    Zeta_DoublyLinkedNode *nr = n->r;

    nl->r = nr;
    nr->l = nl;

    n->l = n;
    n->r = n;
}
