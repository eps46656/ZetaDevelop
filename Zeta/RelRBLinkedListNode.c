#include "RelRBLinkedListNode.h"

static void* GetL_(void* n_) {
    Zeta_RelRBLinkedListNode* n = n_;
    ZETA_DebugAssert(n != NULL);

    return ZETA_UINT_TO_PTR((ZETA_PTR_TO_UINT(n) + n->lc) / 2 * 2);
}

static void* GetR_(void* n_) {
    Zeta_RelRBLinkedListNode* n = n_;
    ZETA_DebugAssert(n != NULL);

    return ZETA_UINT_TO_PTR((ZETA_PTR_TO_UINT(n) + n->rc) / 2 * 2);
}

static int GetLColor_(void* n_) {
    Zeta_RelRBLinkedListNode* n = n_;
    ZETA_DebugAssert(n != NULL);

    return (ZETA_PTR_TO_UINT(n) + n->lc) % 2;
}

static int GetRColor_(void* n_) {
    Zeta_RelRBLinkedListNode* n = n_;
    ZETA_DebugAssert(n != NULL);

    return (ZETA_PTR_TO_UINT(n) + n->rc) % 2;
}

static void SetLC_(void* n_, void* l, int l_color) {
    Zeta_RelRBLinkedListNode* n = n_;
    ZETA_DebugAssert(n != NULL);

    ZETA_DebugAssert(l_color == 0 || l_color == 1);

    n->lc = (ZETA_PTR_TO_UINT(l) + l_color - ZETA_PTR_TO_UINT(n));
}

static void SetRC_(void* n_, void* r, int r_color) {
    Zeta_RelRBLinkedListNode* n = n_;
    ZETA_DebugAssert(n != NULL);

    ZETA_DebugAssert(r_color == 0 || r_color == 1);

    n->rc = (ZETA_PTR_TO_UINT(r) + r_color - ZETA_PTR_TO_UINT(n));
}

static void SetL_(void* n_, void* l) {
    Zeta_RelRBLinkedListNode* n = n_;
    ZETA_DebugAssert(n != NULL);

    SetLC_(n, l, GetLColor_(l));
}

static void SetR_(void* n_, void* r) {
    Zeta_RelRBLinkedListNode* n = n_;
    ZETA_DebugAssert(n != NULL);

    SetLC_(n, r, GetRColor_(r));
}

void Zeta_RelRBLinkedListNode_Init(void* n) {
    SetLC_(n, n, 0);
    SetRC_(n, n, 0);
}

void* Zeta_RelRBLinkedListNode_GetL(void* n) { return GetL_(n); }

void* Zeta_RelRBLinkedListNode_GetR(void* n) { return GetR_(n); }

int Zeta_RelRBLinkedListNode_GetLColor(void* n) { return GetLColor_(n); }

int Zeta_RelRBLinkedListNode_GetRColor(void* n) { return GetRColor_(n); }

void Zeta_RelRBLinkedListNode_SetLColor(void* n, int l_color) {
    SetRC_(n, GetL_(n), l_color);
}

void Zeta_RelRBLinkedListNode_SetRColor(void* n, int r_color) {
    SetRC_(n, GetR_(n), r_color);
}

void Zeta_RelRBLinkedListNode_InsertL(void* n, void* m) {
    ZETA_DebugAssert(n != NULL);
    ZETA_DebugAssert(m != NULL);
    ZETA_DebugAssert(GetL_(m) == m);

    void* nl = GetL_(n);

    SetL_(n, m);
    SetR_(m, n);

    SetL_(m, nl);
    SetR_(nl, m);
}

void Zeta_RelRBLinkedListNode_InsertR(void* n, void* m) {
    ZETA_DebugAssert(n != NULL);
    ZETA_DebugAssert(m != NULL);
    ZETA_DebugAssert(GetL_(m) == m);

    void* nr = GetR_(n);

    SetR_(n, m);
    SetL_(m, n);

    SetR_(m, nr);
    SetL_(nr, m);
}

void Zeta_RelRBLinkedListNode_Extract(void* n) {
    ZETA_DebugAssert(n != NULL);

    void* nl = GetL_(n);
    void* nr = GetR_(n);

    SetR_(nl, nr);
    SetL_(nr, nl);

    SetL_(n, n);
    SetR_(n, n);
}
