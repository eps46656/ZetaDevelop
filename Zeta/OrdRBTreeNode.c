#include "OrdRBTreeNode.h"

static Zeta_OrdRBTreeNode *GetP_(void *n_) {
    Zeta_OrdRBTreeNode *n = n_;
    ZETA_DEBUG_ASSERT(n != NULL);

    void *np = (void *)(uintptr_t)((uintptr_t)n->p / 2 * 2);
    return n == np ? NULL : np;
}

static Zeta_OrdRBTreeNode *GetL_(void *n_) {
    Zeta_OrdRBTreeNode *n = n_;
    ZETA_DEBUG_ASSERT(n != NULL);

    return n == n->l ? NULL : n->l;
}

static Zeta_OrdRBTreeNode *GetR_(void *n_) {
    Zeta_OrdRBTreeNode *n = n_;
    ZETA_DEBUG_ASSERT(n != NULL);

    return n == n->r ? NULL : n->r;
}

static int GetC_(void *n_) {
    Zeta_OrdRBTreeNode *n = n_;
    return n == NULL ? 0 : (uintptr_t)n->p % 2;
}

static void SetPC_(void *n_, void *p, int c) {
    Zeta_OrdRBTreeNode *n = n_;
    ZETA_DEBUG_ASSERT(n != NULL);

    ZETA_DEBUG_ASSERT(c == 0 || c == 1);

    if (p == NULL) { p = n; }

    n->p = (void *)(uintptr_t)((uintptr_t)n->p + c);
}

static void SetP_(void *n_, void *p) {
    Zeta_OrdRBTreeNode *n = n_;
    ZETA_DEBUG_ASSERT(n != NULL);

    SetPC_(n, p, GetC_(n));
}

static void SetL_(void *n_, void *l) {
    Zeta_OrdRBTreeNode *n = n_;
    ZETA_DEBUG_ASSERT(n != NULL);

    n->l = l == NULL ? n : l;
}

static void SetR_(void *n_, void *r) {
    Zeta_OrdRBTreeNode *n = n_;
    ZETA_DEBUG_ASSERT(n != NULL);

    n->r = r == NULL ? n : r;
}

void Zeta_OrdRBTreeNode_Init(void *context, void *n_) {
    ZETA_UNUSED(context);

    Zeta_OrdRBTreeNode *n = n_;
    ZETA_DEBUG_ASSERT(n != NULL);

    SetPC_(n, NULL, 0);
    SetL_(n, NULL);
    SetR_(n, NULL);
}

void *Zeta_OrdRBTreeNode_GetP(void *context, void *n_) {
    ZETA_UNUSED(context);
    return GetP_(n_);
}

void *Zeta_OrdRBTreeNode_GetL(void *context, void *n_) {
    ZETA_UNUSED(context);
    return GetL_(n_);
}

void *Zeta_OrdRBTreeNode_GetR(void *context, void *n_) {
    ZETA_UNUSED(context);
    return GetR_(n_);
}

int Zeta_OrdRBTreeNode_GetColor(void *context, void *n_) {
    ZETA_UNUSED(context);
    return GetC_(n_);
}

void Zeta_OrdRBTreeNode_ReverseColor(void *context, void *n_) {
    ZETA_UNUSED(context);

    Zeta_OrdRBTreeNode *n = n_;
    ZETA_DEBUG_ASSERT(n != NULL);

    SetPC_(n, GetP_(n), 1 - GetC_(n));
}

void Zeta_OrdRBTreeNode_AttachL(void *context, void *n_, void *m_) {
    ZETA_UNUSED(context);

    Zeta_OrdRBTreeNode *n = n_;
    Zeta_OrdRBTreeNode *m = m_;

    ZETA_DEBUG_ASSERT(n != NULL);
    ZETA_DEBUG_ASSERT(GetL_(n) == NULL);
    ZETA_DEBUG_ASSERT(m != NULL);
    ZETA_DEBUG_ASSERT(GetP_(m) == NULL);

    SetL_(n, m);
    SetP_(m, n);
}

void Zeta_OrdRBTreeNode_AttachR(void *context, void *n_, void *m_) {
    ZETA_UNUSED(context);

    Zeta_OrdRBTreeNode *n = n_;
    Zeta_OrdRBTreeNode *m = m_;

    ZETA_DEBUG_ASSERT(n != NULL);
    ZETA_DEBUG_ASSERT(GetR_(n) == NULL);
    ZETA_DEBUG_ASSERT(m != NULL);
    ZETA_DEBUG_ASSERT(GetP_(m) == NULL);

    SetR_(n, m);
    SetP_(m, n);
}

void Zeta_OrdRBTreeNode_Detach(void *context, void *n_) {
    ZETA_UNUSED(context);

    Zeta_OrdRBTreeNode *n = n_;
    ZETA_DEBUG_ASSERT(n != NULL);

    Zeta_OrdRBTreeNode *np = GetP_(n);

    if (np == NULL) { return; }

    if (GetL_(np) == n) {
        SetL_(np, NULL);
    } else {
        SetR_(np, NULL);
    }

    SetP_(n, NULL);
}

void Zeta_OrdRBTreeNode_Swap(void *context, void *n_, void *m_) {
    ZETA_UNUSED(context);

    Zeta_OrdRBTreeNode *n = n_;
    Zeta_OrdRBTreeNode *m = m_;

    ZETA_DEBUG_ASSERT(n != NULL);
    ZETA_DEBUG_ASSERT(m != NULL);

    if (n == m) { return; }

    Zeta_OrdRBTreeNode *np = GetP_(n);
    Zeta_OrdRBTreeNode *mp = GetP_(m);

    if (np == m) {
        Zeta_OrdRBTreeNode *tmp;

        tmp = n;
        n = m;
        m = tmp;

        tmp = np;
        np = mp;
        mp = tmp;
    }

    Zeta_OrdRBTreeNode *nl = GetL_(n);
    Zeta_OrdRBTreeNode *nr = GetR_(n);

    Zeta_OrdRBTreeNode *ml = GetL_(m);
    Zeta_OrdRBTreeNode *mr = GetR_(m);

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

void Zeta_OrdRBTreeNode_RotateL(void *context, void *n_) {
    ZETA_UNUSED(context);

    Zeta_OrdRBTreeNode *n = n_;
    ZETA_DEBUG_ASSERT(n != NULL);

    Zeta_OrdRBTreeNode *nr = GetR_(n);
    ZETA_DEBUG_ASSERT(nr != NULL);

    Zeta_OrdRBTreeNode *nrl = GetL_(nr);
    Zeta_OrdRBTreeNode *np = GetP_(n);

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

void Zeta_OrdRBTreeNode_RotateR(void *context, void *n_) {
    ZETA_UNUSED(context);

    Zeta_OrdRBTreeNode *n = n_;
    ZETA_DEBUG_ASSERT(n != NULL);

    Zeta_OrdRBTreeNode *nl = GetL_(n);
    ZETA_DEBUG_ASSERT(nl != NULL);

    Zeta_OrdRBTreeNode *nlr = GetR_(nl);
    Zeta_OrdRBTreeNode *np = GetP_(n);

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
