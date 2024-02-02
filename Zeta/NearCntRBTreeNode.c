#include "NearCntRBTreeNode.h"

static Zeta_NearCntRBTreeNode *GetP_(void *n_) {
    Zeta_NearCntRBTreeNode *n = n_;
    ZETA_DEBUG_ASSERT(n != NULL);

    void *np = (void *)(intptr_t)((intptr_t)(void *)n + n->p);
    np = (void *)(uintptr_t)((uintptr_t)np / 2 * 2);

    return n == np ? NULL : np;
}

static Zeta_NearCntRBTreeNode *GetL_(void *n_) {
    Zeta_NearCntRBTreeNode *n = n_;
    ZETA_DEBUG_ASSERT(n != NULL);

    void *nl = (void *)(intptr_t)((intptr_t)(void *)n + n->l);
    return n == nl ? NULL : nl;
}

static Zeta_NearCntRBTreeNode *GetR_(void *n_) {
    Zeta_NearCntRBTreeNode *n = n_;
    ZETA_DEBUG_ASSERT(n != NULL);

    void *nr = (void *)(intptr_t)((intptr_t)(void *)n + n->r);
    return n == nr ? NULL : nr;
}

static int GetC_(void *n_) {
    Zeta_NearCntRBTreeNode *n = n_;

    return n == NULL
               ? 0
               : (uintptr_t)(void *)(intptr_t)((intptr_t)(void *)n + n->p) % 2;
}

static void SetPC_(void *n_, void *p, int c) {
    Zeta_NearCntRBTreeNode *n = n_;
    ZETA_DEBUG_ASSERT(n != NULL);

    ZETA_DEBUG_ASSERT(c == 0 || c == 1);

    void *real_p = p;
    if (p == NULL) { p = n; }

    n->p =
        (intptr_t)(void *)(uintptr_t)((uintptr_t)p + c) - (intptr_t)(void *)n;

    ZETA_DEBUG_ASSERT(GetP_(n) == real_p);
}

static void SetP_(void *n_, void *p) {
    Zeta_NearCntRBTreeNode *n = n_;
    ZETA_DEBUG_ASSERT(n != NULL);

    SetPC_(n, p, GetC_(n));
}

static void SetL_(void *n_, void *l) {
    Zeta_NearCntRBTreeNode *n = n_;
    ZETA_DEBUG_ASSERT(n != NULL);

    void *real_l = l;
    if (l == NULL) { l = n; }

    n->l = (intptr_t)l - (intptr_t)(void *)n;
    ZETA_DEBUG_ASSERT(GetL_(n) == real_l);
}

static void SetR_(void *n_, void *r) {
    Zeta_NearCntRBTreeNode *n = n_;
    ZETA_DEBUG_ASSERT(n != NULL);

    void *real_r = r;
    if (r == NULL) { r = n; }

    n->r = (intptr_t)r - (intptr_t)(void *)n;
    ZETA_DEBUG_ASSERT(GetR_(n) == real_r);
}

static void AddDiffSize_(Zeta_NearCntRBTreeNode *n, size_t diff_size) {
    if (diff_size == 0) { return; }
    for (; n != NULL; n = GetP_(n)) { n->acc_size += diff_size; }
}

void Zeta_NearCntRBTreeNode_Init(void *context, void *n_) {
    ZETA_UNUSED(context);

    Zeta_NearCntRBTreeNode *n = n_;
    ZETA_DEBUG_ASSERT(n != NULL);

    SetPC_(n, NULL, 0);
    SetL_(n, NULL);
    SetR_(n, NULL);
    n->acc_size = 0;
}

void *Zeta_NearCntRBTreeNode_GetP(void *context, void *n_) {
    ZETA_UNUSED(context);
    return GetP_(n_);
}

void *Zeta_NearCntRBTreeNode_GetL(void *context, void *n_) {
    ZETA_UNUSED(context);
    return GetL_(n_);
}

void *Zeta_NearCntRBTreeNode_GetR(void *context, void *n_) {
    ZETA_UNUSED(context);
    return GetR_(n_);
}

int Zeta_NearCntRBTreeNode_GetColor(void *context, void *n_) {
    ZETA_UNUSED(context);
    return GetC_(n_);
}

void Zeta_NearCntRBTreeNode_ReverseColor(void *context, void *n_) {
    ZETA_UNUSED(context);

    Zeta_NearCntRBTreeNode *n = n_;
    ZETA_DEBUG_ASSERT(n != NULL);

    SetPC_(n, GetP_(n), 1 - GetC_(n));
}

static size_t GetAccSize_(void *n_) {
    Zeta_NearCntRBTreeNode *n = n_;
    return n == NULL ? 0 : n->acc_size;
}

size_t Zeta_NearCntRBTreeNode_GetAccSize(void *context, void *n) {
    ZETA_UNUSED(context);
    return GetAccSize_(n);
}

static size_t GetSize_(void *n_) {
    Zeta_NearCntRBTreeNode *n = n_;

    return n == NULL
               ? 0
               : n->acc_size - GetAccSize_(GetL_(n)) - GetAccSize_(GetR_(n));
}

size_t Zeta_NearCntRBTreeNode_GetSize(void *context, void *n) {
    ZETA_UNUSED(context);
    return GetSize_(n);
}

static void SetSize_(void *n_, size_t size) {
    Zeta_NearCntRBTreeNode *n = n_;
    ZETA_DEBUG_ASSERT(n != NULL);
    AddDiffSize_(n, size - GetSize_(n));
}

void Zeta_NearCntRBTreeNode_SetSize(void *context, void *n, size_t size) {
    ZETA_UNUSED(context);
    SetSize_(n, size);
}

void Zeta_NearCntRBTreeNode_AttachL(void *context, void *n_, void *m_) {
    ZETA_UNUSED(context);

    Zeta_NearCntRBTreeNode *n = n_;
    Zeta_NearCntRBTreeNode *m = m_;

    ZETA_DEBUG_ASSERT(n != NULL);
    ZETA_DEBUG_ASSERT(GetL_(n) == NULL);
    ZETA_DEBUG_ASSERT(m != NULL);
    ZETA_DEBUG_ASSERT(GetP_(m) == NULL);

    SetL_(n, m);
    SetP_(m, n);
    AddDiffSize_(n, m->acc_size);
}

void Zeta_NearCntRBTreeNode_AttachR(void *context, void *n_, void *m_) {
    ZETA_UNUSED(context);

    Zeta_NearCntRBTreeNode *n = n_;
    Zeta_NearCntRBTreeNode *m = m_;

    ZETA_DEBUG_ASSERT(n != NULL);
    ZETA_DEBUG_ASSERT(GetR_(n) == NULL);
    ZETA_DEBUG_ASSERT(m != NULL);
    ZETA_DEBUG_ASSERT(GetP_(m) == NULL);

    SetR_(n, m);
    SetP_(m, n);
    AddDiffSize_(n, m->acc_size);
}

void Zeta_NearCntRBTreeNode_Detach(void *context, void *n_) {
    ZETA_UNUSED(context);

    Zeta_NearCntRBTreeNode *n = n_;
    ZETA_DEBUG_ASSERT(n != NULL);

    Zeta_NearCntRBTreeNode *np = GetP_(n);

    if (np == NULL) { return; }

    if (GetL_(np) == n) {
        SetL_(np, NULL);
    } else {
        SetR_(np, NULL);
    }

    SetP_(n, NULL);
    AddDiffSize_(np, -n->acc_size);
}

void Zeta_NearCntRBTreeNode_Swap(void *context, void *n_, void *m_) {
    ZETA_UNUSED(context);

    Zeta_NearCntRBTreeNode *n = n_;
    Zeta_NearCntRBTreeNode *m = m_;

    ZETA_DEBUG_ASSERT(n != NULL);
    ZETA_DEBUG_ASSERT(m != NULL);

    if (n == m) { return; }

    Zeta_NearCntRBTreeNode *np = GetP_(n);
    Zeta_NearCntRBTreeNode *mp = GetP_(m);

    if (np == m) {
        Zeta_NearCntRBTreeNode *tmp;

        tmp = n;
        n = m;
        m = tmp;

        tmp = np;
        np = mp;
        mp = tmp;
    }

    size_t n_size = GetSize_(n);
    size_t m_size = GetSize_(m);

    Zeta_NearCntRBTreeNode *nl = GetL_(n);
    Zeta_NearCntRBTreeNode *nr = GetR_(n);

    Zeta_NearCntRBTreeNode *ml = GetL_(m);
    Zeta_NearCntRBTreeNode *mr = GetR_(m);

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

    size_t acc_size_tmp = n->acc_size;
    n->acc_size = m->acc_size;
    m->acc_size = acc_size_tmp;

    if (np != NULL) { SetSize_(np, n_size); }
    if (mp != NULL) { SetSize_(mp, m_size); }
}

void Zeta_NearCntRBTreeNode_RotateL(void *context, void *n_) {
    ZETA_UNUSED(context);

    Zeta_NearCntRBTreeNode *n = n_;
    ZETA_DEBUG_ASSERT(n != NULL);

    Zeta_NearCntRBTreeNode *nr = GetR_(n);
    ZETA_DEBUG_ASSERT(nr != NULL);

    Zeta_NearCntRBTreeNode *nrl = GetL_(nr);
    Zeta_NearCntRBTreeNode *np = GetP_(n);

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

    size_t n_acc_size = n->acc_size;
    size_t new_n_acc_size = n_acc_size - nr->acc_size;
    nr->acc_size = n_acc_size;

    SetR_(n, nrl);

    if (nrl != NULL) {
        SetP_(nrl, n);
        new_n_acc_size += nrl->acc_size;
    }

    n->acc_size = new_n_acc_size;
}

void Zeta_NearCntRBTreeNode_RotateR(void *context, void *n_) {
    ZETA_UNUSED(context);

    Zeta_NearCntRBTreeNode *n = n_;
    ZETA_DEBUG_ASSERT(n != NULL);

    Zeta_NearCntRBTreeNode *nl = GetL_(n);
    ZETA_DEBUG_ASSERT(nl != NULL);

    Zeta_NearCntRBTreeNode *nlr = GetR_(nl);
    Zeta_NearCntRBTreeNode *np = GetP_(n);

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

    size_t n_acc_size = n->acc_size;
    size_t new_n_acc_size = n_acc_size - nl->acc_size;
    nl->acc_size = n_acc_size;

    SetL_(n, nlr);

    if (nlr != NULL) {
        SetP_(nlr, n);
        new_n_acc_size += nlr->acc_size;
    }

    n->acc_size = new_n_acc_size;
}
