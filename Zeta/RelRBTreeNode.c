#include "RelRBTreeNode.h"

static Zeta_RelRBTreeNode* GetP_(void* n_) {
    Zeta_RelRBTreeNode* n = n_;
    ZETA_DebugAssert(n != NULL);

    void* p = (void*)(((uintptr_t)(void*)n + n->p) / 2 * 2);
    return n == p ? NULL : p;
}

static Zeta_RelRBTreeNode* GetL_(void* n_) {
    Zeta_RelRBTreeNode* n = n_;
    ZETA_DebugAssert(n != NULL);

    void* l = (void*)((uintptr_t)(void*)n + n->l);
    return n == l ? NULL : l;
}

static Zeta_RelRBTreeNode* GetR_(void* n_) {
    Zeta_RelRBTreeNode* n = n_;
    ZETA_DebugAssert(n != NULL);

    void* r = (void*)((uintptr_t)(void*)n + n->r);
    return n == r ? NULL : r;
}

static int GetC_(void* n_) {
    Zeta_RelRBTreeNode* n = n_;
    return n == NULL ? 0 : ((uintptr_t)(void*)n + n->p) % 2;
}

static void SetPC_(void* n_, void* p, int c) {
    Zeta_RelRBTreeNode* n = n_;
    ZETA_DebugAssert(n != NULL);

    ZETA_DebugAssert(c == 0 || c == 1);

    n->p = (uintptr_t)(p == NULL ? (void*)n : p) + (uintptr_t)c -
           (uintptr_t)(void*)n;

    ZETA_DebugAssert(p == GetP_(n));
    ZETA_DebugAssert(c == GetC_(n));
}

static void SetP_(void* n_, void* p) {
    Zeta_RelRBTreeNode* n = n_;
    ZETA_DebugAssert(n != NULL);

    SetPC_(n, p, GetC_(n));
}

static void SetL_(void* n_, void* l) {
    Zeta_RelRBTreeNode* n = n_;
    ZETA_DebugAssert(n != NULL);

    n->l = (uintptr_t)(l == NULL ? (void*)n : l) - (uintptr_t)(void*)n;

    ZETA_DebugAssert(l == GetL_(n));
}

static void SetR_(void* n_, void* r) {
    Zeta_RelRBTreeNode* n = n_;
    ZETA_DebugAssert(n != NULL);

    n->r = (uintptr_t)(r == NULL ? (void*)n : r) - (uintptr_t)(void*)n;

    ZETA_DebugAssert(r == GetR_(n));
}

void Zeta_RelRBTreeNode_Init(void* context, void* n_) {
    ZETA_Unused(context);

    Zeta_RelRBTreeNode* n = n_;
    ZETA_DebugAssert(n != NULL);

    SetPC_(n, NULL, 0);
    SetL_(n, NULL);
    SetR_(n, NULL);
}

void* Zeta_RelRBTreeNode_GetP(void* context, void* n_) {
    ZETA_Unused(context);
    return GetP_(n_);
}

void* Zeta_RelRBTreeNode_GetL(void* context, void* n_) {
    ZETA_Unused(context);
    return GetL_(n_);
}

void* Zeta_RelRBTreeNode_GetR(void* context, void* n_) {
    ZETA_Unused(context);
    return GetR_(n_);
}

int Zeta_RelRBTreeNode_GetColor(void* context, void* n_) {
    ZETA_Unused(context);
    return GetC_(n_);
}

void Zeta_RelRBTreeNode_ReverseColor(void* context, void* n_) {
    ZETA_Unused(context);

    Zeta_RelRBTreeNode* n = n_;
    ZETA_DebugAssert(n != NULL);

    SetPC_(n, GetP_(n), 1 - GetC_(n));
}

void Zeta_RelRBTreeNode_AttachL(void* context, void* n_, void* m_) {
    ZETA_Unused(context);

    Zeta_RelRBTreeNode* n = n_;
    Zeta_RelRBTreeNode* m = m_;

    ZETA_DebugAssert(n != NULL);
    ZETA_DebugAssert(GetL_(n) == NULL);
    ZETA_DebugAssert(m != NULL);
    ZETA_DebugAssert(GetP_(m) == NULL);

    SetL_(n, m);
    SetP_(m, n);
}

void Zeta_RelRBTreeNode_AttachR(void* context, void* n_, void* m_) {
    ZETA_Unused(context);

    Zeta_RelRBTreeNode* n = n_;
    Zeta_RelRBTreeNode* m = m_;

    ZETA_DebugAssert(n != NULL);
    ZETA_DebugAssert(GetR_(n) == NULL);
    ZETA_DebugAssert(m != NULL);
    ZETA_DebugAssert(GetP_(m) == NULL);

    SetR_(n, m);
    SetP_(m, n);
}

void Zeta_RelRBTreeNode_Detach(void* context, void* n_) {
    ZETA_Unused(context);

    Zeta_RelRBTreeNode* n = n_;
    ZETA_DebugAssert(n != NULL);

    Zeta_RelRBTreeNode* np = GetP_(n);

    if (np == NULL) { return; }

    if (GetL_(np) == n) {
        SetL_(np, NULL);
    } else {
        SetR_(np, NULL);
    }

    SetP_(n, NULL);
}

void Zeta_RelRBTreeNode_Swap(void* context, void* n_, void* m_) {
    ZETA_Unused(context);

    Zeta_RelRBTreeNode* n = n_;
    Zeta_RelRBTreeNode* m = m_;

    ZETA_DebugAssert(n != NULL);
    ZETA_DebugAssert(m != NULL);

    if (n == m) { return; }

    Zeta_RelRBTreeNode* np = GetP_(n);
    Zeta_RelRBTreeNode* mp = GetP_(m);

    if (np == m) {
        ZETA_Swap(n, m);
        ZETA_Swap(np, mp);
    }

    Zeta_RelRBTreeNode* nl = GetL_(n);
    Zeta_RelRBTreeNode* nr = GetR_(n);

    Zeta_RelRBTreeNode* ml = GetL_(m);
    Zeta_RelRBTreeNode* mr = GetR_(m);

    if (mp == n) {
        if (np != NULL) {
            if (GetL_(np) == n) {
                SetL_(np, m);
            } else {
                SetR_(np, m);
            }
        }

        SetP_(m, np);

        if (GetL_(n) == m) {
            SetL_(m, n);

            SetR_(m, nr);
            if (nr != NULL) { SetP_(nr, m); }
        } else {
            SetL_(m, nl);
            if (nl != NULL) { SetP_(nl, m); }

            SetR_(m, n);
        }

        SetP_(n, m);

        SetL_(n, ml);
        if (ml != NULL) { SetP_(ml, n); }

        SetR_(n, mr);
        if (mr != NULL) { SetP_(mr, n); }
    } else {
        if (np != NULL) {
            if (GetL_(np) == n) {
                SetL_(np, m);
            } else {
                SetR_(np, m);
            }
        }

        SetP_(m, np);

        SetL_(m, nl);
        if (nl != NULL) { SetP_(nl, m); }

        SetR_(m, nr);
        if (nr != NULL) { SetP_(nr, m); }

        if (mp != NULL) {
            if (GetL_(mp) == m) {
                SetL_(mp, n);
            } else {
                SetR_(mp, n);
            }
        }

        SetP_(n, mp);

        SetL_(n, ml);
        if (ml != NULL) { SetP_(ml, n); }

        SetR_(n, mr);
        if (mr != NULL) { SetP_(mr, n); }
    }

    np = GetP_(n);
    mp = GetP_(m);

    int nc = GetC_(n);
    int mc = GetC_(m);

    if (nc != mc) {
        SetPC_(n, np, mc);
        SetPC_(m, mp, nc);
    }
}

void Zeta_RelRBTreeNode_RotateL(void* context, void* n_) {
    ZETA_Unused(context);

    Zeta_RelRBTreeNode* n = n_;
    ZETA_DebugAssert(n != NULL);

    Zeta_RelRBTreeNode* nr = GetR_(n);
    ZETA_DebugAssert(nr != NULL);

    Zeta_RelRBTreeNode* nrl = GetL_(nr);
    Zeta_RelRBTreeNode* np = GetP_(n);

    if (np != NULL) {
        if (GetL_(np) == n) {
            SetL_(np, nr);
        } else {
            SetR_(np, nr);
        }
    }

    SetP_(nr, np);

    SetL_(nr, n);
    SetP_(n, nr);

    SetR_(n, nrl);

    if (nrl != NULL) { SetP_(nrl, n); }
}

void Zeta_RelRBTreeNode_RotateR(void* context, void* n_) {
    ZETA_Unused(context);

    Zeta_RelRBTreeNode* n = n_;
    ZETA_DebugAssert(n != NULL);

    Zeta_RelRBTreeNode* nl = GetL_(n);
    ZETA_DebugAssert(nl != NULL);

    Zeta_RelRBTreeNode* nlr = GetR_(nl);
    Zeta_RelRBTreeNode* np = GetP_(n);

    if (np != NULL) {
        if (GetL_(np) == n) {
            SetL_(np, nl);
        } else {
            SetR_(np, nl);
        }
    }

    SetP_(nl, np);

    SetR_(nl, n);
    SetP_(n, nl);

    SetL_(n, nlr);

    if (nlr != NULL) { SetP_(nlr, n); }
}
