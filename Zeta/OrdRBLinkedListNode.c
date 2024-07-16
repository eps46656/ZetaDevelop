#include "OrdRBLinkedListNode.h"

#include "Debugger.h"

#define GetL_(n) \
    ((void*)__builtin_align_down(n->l, alignof(Zeta_OrdRBLinkedListNode)))

#define GetR_(n) \
    ((void*)__builtin_align_down(n->r, alignof(Zeta_OrdRBLinkedListNode)))

#define GetLC_(n) (n->l - (unsigned char*)GetL_(n))

#define GetRC_(n) (n->r - (unsigned char*)GetR_(n))

static void SetL_(void* n_, void* l) {
    Zeta_OrdRBLinkedListNode* n = n_;
    ZETA_DebugAssert(n != NULL);

    n->l = (unsigned char*)l + GetLC_(n);
}

static void SetR_(void* n_, void* r) {
    Zeta_OrdRBLinkedListNode* n = n_;
    ZETA_DebugAssert(n != NULL);

    n->r = (unsigned char*)r + GetRC_(n);
}

void Zeta_OrdRBLinkedListNode_Init(void* n_) {
    Zeta_OrdRBLinkedListNode* n = n_;
    ZETA_DebugAssert(n != NULL);

    n->l = (void*)n;
    n->r = (void*)n;
}

void* Zeta_OrdRBLinkedListNode_GetL(void* n_) {
    Zeta_OrdRBLinkedListNode* n = n_;
    ZETA_DebugAssert(n != NULL);

    return GetL_(n);
}

void* Zeta_OrdRBLinkedListNode_GetR(void* n_) {
    Zeta_OrdRBLinkedListNode* n = n_;
    ZETA_DebugAssert(n != NULL);

    return GetR_(n);
}

int Zeta_OrdRBLinkedListNode_GetColor(void* n_) {
    Zeta_OrdRBLinkedListNode* n = n_;
    ZETA_DebugAssert(n != NULL);

    return GetRC_(n) * alignof(Zeta_OrdRBLinkedListNode) + GetLC_(n);
}

void Zeta_OrdRBLinkedListNode_SetColor(void* n_, int color) {
    Zeta_OrdRBLinkedListNode* n = n_;
    ZETA_DebugAssert(n != NULL);

    ZETA_DebugAssert(0 <= color);
    ZETA_DebugAssert((size_t)color < alignof(Zeta_OrdRBLinkedListNode) *
                                         alignof(Zeta_OrdRBLinkedListNode));

    int lc = color % alignof(Zeta_OrdRBLinkedListNode);
    int rc = color / alignof(Zeta_OrdRBLinkedListNode);

    n->l = (unsigned char*)GetL_(n) + lc;
    n->r = (unsigned char*)GetR_(n) + rc;
}

size_t Zeta_OrdRBLinkedListNode_Count(void* n_, void* m_) {
    Zeta_OrdRBLinkedListNode* n = n_;
    Zeta_OrdRBLinkedListNode* m = m_;

    ZETA_DebugAssert(n != NULL);
    ZETA_DebugAssert(m != NULL);

    size_t ret = 1;

    for (; n != m; n = GetR_(n)) { ++ret; }

    return ret;
}

void Zeta_OrdRBLinkedListNode_InsertL(void* n_, void* m_) {
    Zeta_OrdRBLinkedListNode* n = n_;
    Zeta_OrdRBLinkedListNode* m = m_;

    ZETA_DebugAssert(n != NULL);
    ZETA_DebugAssert(m != NULL);

    ZETA_DebugAssert(GetL_(m) == m);
    ZETA_DebugAssert(GetR_(m) == m);

    void* nl = GetL_(n);

    SetL_(n, m);
    SetR_(m, n);

    SetL_(m, nl);
    SetR_(nl, m);
}

void Zeta_OrdRBLinkedListNode_InsertR(void* n_, void* m_) {
    Zeta_OrdRBLinkedListNode* n = n_;
    Zeta_OrdRBLinkedListNode* m = m_;

    ZETA_DebugAssert(n != NULL);
    ZETA_DebugAssert(m != NULL);

    ZETA_DebugAssert(GetL_(m) == m);
    ZETA_DebugAssert(GetR_(m) == m);

    void* nr = GetR_(n);

    SetR_(n, m);
    SetL_(m, n);

    SetR_(m, nr);
    SetL_(nr, m);
}

void Zeta_OrdRBLinkedListNode_InsertSegL(void* n_, void* m_beg_, void* m_end_) {
    Zeta_OrdRBLinkedListNode* n = n_;
    Zeta_OrdRBLinkedListNode* m_beg = m_beg_;
    Zeta_OrdRBLinkedListNode* m_end = m_end_;

    ZETA_DebugAssert(n != NULL);
    ZETA_DebugAssert(m_beg != NULL);
    ZETA_DebugAssert(m_end != NULL);

    void* nl = GetL_(n);

    void* m_beg_l = GetL_(m_beg);
    void* m_end_r = GetR_(m_end);

    SetL_(m_end_r, m_beg_l);
    SetR_(m_beg_l, m_end_r);

    SetL_(n, m_end);
    SetR_(m_end, n);

    SetL_(m_beg, nl);
    SetR_(nl, m_beg);
}

void Zeta_OrdRBLinkedListNode_InsertSegR(void* n_, void* m_beg_, void* m_end_) {
    Zeta_OrdRBLinkedListNode* n = n_;
    Zeta_OrdRBLinkedListNode* m_beg = m_beg_;
    Zeta_OrdRBLinkedListNode* m_end = m_end_;

    ZETA_DebugAssert(n != NULL);
    ZETA_DebugAssert(m_beg != NULL);
    ZETA_DebugAssert(m_end != NULL);

    void* nr = GetR_(n);

    void* m_beg_l = GetL_(m_beg);
    void* m_end_r = GetR_(m_end);

    SetR_(m_beg_l, m_end_r);
    SetL_(m_end_r, m_beg_l);

    SetR_(n, m_beg);
    SetL_(m_beg, n);

    SetR_(m_end, nr);
    SetL_(nr, m_end);
}

void Zeta_OrdRBLinkedListNode_Extract(void* n_) {
    Zeta_OrdRBLinkedListNode* n = n_;
    ZETA_DebugAssert(n != NULL);

    void* nl = GetL_(n);
    void* nr = GetR_(n);

    SetR_(nl, nr);
    SetL_(nr, nl);

    n->l = (unsigned char*)n + GetLC_(n);
    n->r = (unsigned char*)n + GetRC_(n);
}
