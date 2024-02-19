#include "RelCntRBTreeNode.h"

static Zeta_RelCntRBTreeNode* GetP_(void* n_) {
    Zeta_RelCntRBTreeNode* n = n_;
    ZETA_DebugAssert(n != NULL);

    void* p = (void*)(((uintptr_t)(void*)n + n->p) / 2 * 2);
    return n == p ? NULL : p;
}

static Zeta_RelCntRBTreeNode* GetL_(void* n_) {
    Zeta_RelCntRBTreeNode* n = n_;
    ZETA_DebugAssert(n != NULL);

    void* l = (void*)((uintptr_t)(void*)n + n->l);
    return n == l ? NULL : l;
}

static Zeta_RelCntRBTreeNode* GetR_(void* n_) {
    Zeta_RelCntRBTreeNode* n = n_;
    ZETA_DebugAssert(n != NULL);

    void* r = (void*)((uintptr_t)(void*)n + n->r);
    return n == r ? NULL : r;
}

static int GetC_(void* n_) {
    Zeta_RelCntRBTreeNode* n = n_;
    return n == NULL ? 0 : ((uintptr_t)(void*)n + n->p) % 2;
}

static void SetPC_(void* n_, void* p, int c) {
    Zeta_RelCntRBTreeNode* n = n_;
    ZETA_DebugAssert(n != NULL);

    ZETA_DebugAssert(c == 0 || c == 1);

    n->p = (uintptr_t)(p == NULL ? (void*)n : p) + (uintptr_t)c -
           (uintptr_t)(void*)n;
}

static void SetP_(void* n_, void* p) {
    Zeta_RelCntRBTreeNode* n = n_;
    ZETA_DebugAssert(n != NULL);

    SetPC_(n, p, GetC_(n));
}

static void SetL_(void* n_, void* l) {
    Zeta_RelCntRBTreeNode* n = n_;
    ZETA_DebugAssert(n != NULL);

    n->l = (uintptr_t)(l == NULL ? (void*)n : l) - (uintptr_t)(void*)n;
}

static void SetR_(void* n_, void* r) {
    Zeta_RelCntRBTreeNode* n = n_;
    ZETA_DebugAssert(n != NULL);

    n->r = (uintptr_t)(r == NULL ? (void*)n : r) - (uintptr_t)(void*)n;
}

static void AddDiffSize_(Zeta_RelCntRBTreeNode* n, size_t diff_size) {
    if (diff_size == 0) { return; }
    for (; n != NULL; n = GetP_(n)) { n->acc_size += diff_size; }
}

void Zeta_RelCntRBTreeNode_Init(void* context, void* n_) {
    ZETA_Unused(context);

    Zeta_RelCntRBTreeNode* n = n_;
    ZETA_DebugAssert(n != NULL);

    SetPC_(n, NULL, 0);
    SetL_(n, NULL);
    SetR_(n, NULL);
    n->acc_size = 0;
}

void* Zeta_RelCntRBTreeNode_GetP(void* context, void* n_) {
    ZETA_Unused(context);
    return GetP_(n_);
}

void* Zeta_RelCntRBTreeNode_GetL(void* context, void* n_) {
    ZETA_Unused(context);
    return GetL_(n_);
}

void* Zeta_RelCntRBTreeNode_GetR(void* context, void* n_) {
    ZETA_Unused(context);
    return GetR_(n_);
}

int Zeta_RelCntRBTreeNode_GetColor(void* context, void* n_) {
    ZETA_Unused(context);
    return GetC_(n_);
}

void Zeta_RelCntRBTreeNode_ReverseColor(void* context, void* n_) {
    ZETA_Unused(context);

    Zeta_RelCntRBTreeNode* n = n_;
    ZETA_DebugAssert(n != NULL);

    SetPC_(n, GetP_(n), 1 - GetC_(n));
}

static size_t GetAccSize_(void* n_) {
    Zeta_RelCntRBTreeNode* n = n_;
    return n == NULL ? 0 : n->acc_size;
}

size_t Zeta_RelCntRBTreeNode_GetAccSize(void* context, void* n) {
    ZETA_Unused(context);
    return GetAccSize_(n);
}

static size_t GetSize_(void* n_) {
    Zeta_RelCntRBTreeNode* n = n_;

    return n == NULL
               ? 0
               : n->acc_size - GetAccSize_(GetL_(n)) - GetAccSize_(GetR_(n));
}

size_t Zeta_RelCntRBTreeNode_GetSize(void* context, void* n) {
    ZETA_Unused(context);
    return GetSize_(n);
}

static void SetSize_(void* n_, size_t size) {
    Zeta_RelCntRBTreeNode* n = n_;
    ZETA_DebugAssert(n != NULL);

    AddDiffSize_(n, size - GetSize_(n));
}

void Zeta_RelCntRBTreeNode_SetSize(void* context, void* n, size_t size) {
    ZETA_Unused(context);
    SetSize_(n, size);
}

void Zeta_RelCntRBTreeNode_AttachL(void* context, void* n_, void* m_) {
    ZETA_Unused(context);

    Zeta_RelCntRBTreeNode* n = n_;
    Zeta_RelCntRBTreeNode* m = m_;

    ZETA_DebugAssert(n != NULL);
    ZETA_DebugAssert(GetL_(n) == NULL);
    ZETA_DebugAssert(m != NULL);
    ZETA_DebugAssert(GetP_(m) == NULL);

    SetL_(n, m);
    SetP_(m, n);
    AddDiffSize_(n, m->acc_size);
}

void Zeta_RelCntRBTreeNode_AttachR(void* context, void* n_, void* m_) {
    ZETA_Unused(context);

    Zeta_RelCntRBTreeNode* n = n_;
    Zeta_RelCntRBTreeNode* m = m_;

    ZETA_DebugAssert(n != NULL);
    ZETA_DebugAssert(GetR_(n) == NULL);
    ZETA_DebugAssert(m != NULL);
    ZETA_DebugAssert(GetP_(m) == NULL);

    SetR_(n, m);
    SetP_(m, n);
    AddDiffSize_(n, m->acc_size);
}

void Zeta_RelCntRBTreeNode_Detach(void* context, void* n_) {
    ZETA_Unused(context);

    Zeta_RelCntRBTreeNode* n = n_;
    ZETA_DebugAssert(n != NULL);

    Zeta_RelCntRBTreeNode* np = GetP_(n);

    if (np == NULL) { return; }

    if (GetL_(np) == n) {
        SetL_(np, NULL);
    } else {
        SetR_(np, NULL);
    }

    SetP_(n, NULL);
    AddDiffSize_(np, -n->acc_size);
}

void Zeta_RelCntRBTreeNode_Swap(void* context, void* n_, void* m_) {
    ZETA_Unused(context);

    Zeta_RelCntRBTreeNode* n = n_;
    Zeta_RelCntRBTreeNode* m = m_;

    ZETA_DebugAssert(n != NULL);
    ZETA_DebugAssert(m != NULL);

    if (n == m) { return; }

    Zeta_RelCntRBTreeNode* np = GetP_(n);
    Zeta_RelCntRBTreeNode* mp = GetP_(m);

    if (np == m) {
        ZETA_Swap(n, m);
        ZETA_Swap(np, mp);
    }

    size_t n_size = GetSize_(n);
    size_t m_size = GetSize_(m);

    Zeta_RelCntRBTreeNode* nl = GetL_(n);
    Zeta_RelCntRBTreeNode* nr = GetR_(n);

    Zeta_RelCntRBTreeNode* ml = GetL_(m);
    Zeta_RelCntRBTreeNode* mr = GetR_(m);

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

    ZETA_Swap(n->acc_size, m->acc_size);

    SetSize_(n, n_size);
    SetSize_(m, m_size);
}

void Zeta_RelCntRBTreeNode_RotateL(void* context, void* n_) {
    ZETA_Unused(context);

    Zeta_RelCntRBTreeNode* n = n_;
    ZETA_DebugAssert(n != NULL);

    Zeta_RelCntRBTreeNode* nr = GetR_(n);
    ZETA_DebugAssert(nr != NULL);

    Zeta_RelCntRBTreeNode* nrl = GetL_(nr);
    Zeta_RelCntRBTreeNode* np = GetP_(n);

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

void Zeta_RelCntRBTreeNode_RotateR(void* context, void* n_) {
    ZETA_Unused(context);

    Zeta_RelCntRBTreeNode* n = n_;
    ZETA_DebugAssert(n != NULL);

    Zeta_RelCntRBTreeNode* nl = GetL_(n);
    ZETA_DebugAssert(nl != NULL);

    Zeta_RelCntRBTreeNode* nlr = GetR_(nl);
    Zeta_RelCntRBTreeNode* np = GetP_(n);

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
