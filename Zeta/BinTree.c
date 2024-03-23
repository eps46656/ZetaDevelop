#include "BinTree.h"

#include "utils.h"

void Zeta_BinTree_InitOpr(Zeta_BinTreeNodeOperator* btn_opr) {
    btn_opr->context = NULL;

    btn_opr->GetP = NULL;
    btn_opr->GetL = NULL;
    btn_opr->GetR = NULL;

    btn_opr->SetP = NULL;
    btn_opr->SetL = NULL;
    btn_opr->SetR = NULL;

    btn_opr->GetPColor = NULL;
    btn_opr->GetLColor = NULL;
    btn_opr->GetRColor = NULL;

    btn_opr->SetPColor = NULL;
    btn_opr->SetLColor = NULL;
    btn_opr->SetRColor = NULL;

    btn_opr->GetAccSize = NULL;
    btn_opr->SetAccSize = NULL;
}

static void AddDiffSize_(Zeta_BinTreeNodeOperator const* btn_opr, void* n,
                         size_t diff_size) {
    if (diff_size == 0) { return; }

    void* context = btn_opr->context;

    void* (*GetP)(void* context, void* n) = btn_opr->GetP;
    ZETA_DebugAssert(GetP != NULL);

    size_t (*GetAccSize)(void* context, void* n) = btn_opr->GetAccSize;
    void (*SetAccSize)(void* context, void* n, size_t acc_size) =
        btn_opr->SetAccSize;

    ZETA_DebugAssert(GetAccSize != NULL);
    ZETA_DebugAssert(SetAccSize != NULL);

    for (; n != NULL; n = GetP(context, n)) {
        SetAccSize(context, n, GetAccSize(context, n) + diff_size);
    }
}

size_t Zeta_BinTree_GetSize(Zeta_BinTreeNodeOperator const* btn_opr, void* n) {
    ZETA_DebugAssert(btn_opr != NULL);

    void* context = btn_opr->context;

    size_t (*GetAccSize)(void* context, void* n) = btn_opr->GetAccSize;

    ZETA_DebugAssert(GetAccSize != NULL);

    size_t n_acc_size = GetAccSize(context, n);

    if (n == NULL) { return n_acc_size; }

    void* (*GetL)(void* context, void* n) = btn_opr->GetL;
    void* (*GetR)(void* context, void* n) = btn_opr->GetR;

    ZETA_DebugAssert(GetL != NULL);
    ZETA_DebugAssert(GetR != NULL);

    return n_acc_size - GetAccSize(context, GetL(context, n)) -
           GetAccSize(context, GetR(context, n));
}

void Zeta_BinTree_SetSize(Zeta_BinTreeNodeOperator const* btn_opr, void* n,
                          size_t size) {
    ZETA_DebugAssert(btn_opr != NULL);
    ZETA_DebugAssert(n != NULL);

    AddDiffSize_(btn_opr, n, size - Zeta_BinTree_GetSize(btn_opr, n));
}

#define Attatch_(D)                                                       \
    ZETA_DebugAssert(btn_opr != NULL);                                    \
                                                                          \
    ZETA_DebugAssert(n != NULL);                                          \
                                                                          \
    void* context = btn_opr->context;                                     \
                                                                          \
    void* (*GetP)(void* context, void* n) = btn_opr->GetP;                \
    void* (*GetD)(void* context, void* n) = btn_opr->Get##D;              \
                                                                          \
    ZETA_DebugAssert(GetP != NULL);                                       \
    ZETA_DebugAssert(GetD != NULL);                                       \
                                                                          \
    void (*SetP)(void* context, void* n, void* m) = btn_opr->SetP;        \
    void (*SetD)(void* context, void* n, void* m) = btn_opr->Set##D;      \
                                                                          \
    ZETA_DebugAssert(SetP != NULL);                                       \
    ZETA_DebugAssert(SetD != NULL);                                       \
                                                                          \
    ZETA_DebugAssert(GetD(context, n) == NULL);                           \
    ZETA_DebugAssert(m == NULL || GetP(context, m) == NULL);              \
                                                                          \
    SetD(context, n, m);                                                  \
    SetP(context, m, n);                                                  \
                                                                          \
    size_t (*GetAccSize)(void* context, void* n) = btn_opr->GetAccSize;   \
                                                                          \
    if (GetAccSize != NULL) {                                             \
        AddDiffSize_(btn_opr, n,                                          \
                     GetAccSize(context, m) - GetAccSize(context, NULL)); \
    }

void Zeta_BinTree_AttatchL(Zeta_BinTreeNodeOperator const* btn_opr, void* n,
                           void* m) {
    Attatch_(L);
}

void Zeta_BinTree_AttatchR(Zeta_BinTreeNodeOperator const* btn_opr, void* n,
                           void* m) {
    Attatch_(R);
}

void Zeta_BinTree_Detach(Zeta_BinTreeNodeOperator const* btn_opr, void* n) {
    ZETA_DebugAssert(btn_opr != NULL);

    ZETA_DebugAssert(n != NULL);

    void* context = btn_opr->context;

    void* (*GetP)(void* context, void* n) = btn_opr->GetP;
    void* (*GetL)(void* context, void* n) = btn_opr->GetL;

    ZETA_DebugAssert(GetP != NULL);
    ZETA_DebugAssert(GetL != NULL);

    void (*SetP)(void* context, void* n, void* m) = btn_opr->SetP;
    void (*SetL)(void* context, void* n, void* m) = btn_opr->SetL;
    void (*SetR)(void* context, void* n, void* m) = btn_opr->SetR;

    ZETA_DebugAssert(SetP != NULL);
    ZETA_DebugAssert(SetL != NULL);
    ZETA_DebugAssert(SetR != NULL);

    size_t (*GetAccSize)(void* context, void* n) = btn_opr->GetAccSize;
    void (*SetAccSize)(void* context, void* n, size_t acc_size) =
        btn_opr->SetAccSize;

    void* np = GetP(context, n);

    if (np == NULL) { return; }

    if (GetL(context, np) == n) {
        SetL(context, np, NULL);
    } else {
        SetR(context, np, NULL);
    }

    SetP(context, n, NULL);

    if (GetAccSize != NULL && SetAccSize != NULL) {
        AddDiffSize_(btn_opr, np,
                     GetAccSize(context, NULL) - GetAccSize(context, n));
    }
}

void Zeta_BinTree_Swap(Zeta_BinTreeNodeOperator const* btn_opr, void* n,
                       void* m) {
    ZETA_DebugAssert(btn_opr != NULL);

    ZETA_DebugAssert(n != NULL);
    ZETA_DebugAssert(m != NULL);

    if (n == m) { return; }

    void* context = btn_opr->context;

    void* (*GetP)(void* context, void* n) = btn_opr->GetP;
    void* (*GetL)(void* context, void* n) = btn_opr->GetL;
    void* (*GetR)(void* context, void* n) = btn_opr->GetR;

    ZETA_DebugAssert(GetP != NULL);
    ZETA_DebugAssert(GetL != NULL);
    ZETA_DebugAssert(GetR != NULL);

    void (*SetP)(void* context, void* n, void* m) = btn_opr->SetP;
    void (*SetL)(void* context, void* n, void* m) = btn_opr->SetL;
    void (*SetR)(void* context, void* n, void* m) = btn_opr->SetR;

    ZETA_DebugAssert(SetP != NULL);
    ZETA_DebugAssert(SetL != NULL);
    ZETA_DebugAssert(SetR != NULL);

    size_t (*GetAccSize)(void* context, void* n) = btn_opr->GetAccSize;
    void (*SetAccSize)(void* context, void* n, size_t acc_size) =
        btn_opr->SetAccSize;

    void* np = GetP(context, n);
    void* mp = GetP(context, m);

    if (np == m) {
        ZETA_Swap(n, m);
        ZETA_Swap(np, mp);
    }

    size_t n_acc_size;
    size_t m_acc_size;

    size_t n_size;
    size_t m_size;

    void* nl = GetL(context, n);
    void* nr = GetR(context, n);

    void* ml = GetL(context, m);
    void* mr = GetR(context, m);

    if (GetAccSize != NULL && SetAccSize != NULL) {
        n_acc_size = GetAccSize(context, n);
        m_acc_size = GetAccSize(context, m);

        n_size = n_acc_size - GetAccSize(context, nl) - GetAccSize(context, nr);
        m_size = m_acc_size - GetAccSize(context, ml) - GetAccSize(context, mr);
    }

    if (mp == n) {
        if (np != NULL) {
            if (GetL(context, np) == n) {
                SetL(context, np, m);
            } else {
                SetR(context, np, m);
            }
        }

        SetP(context, m, np);

        if (GetL(context, n) == m) {
            SetL(context, m, n);

            SetR(context, m, nr);
            if (nr != NULL) { SetP(context, nr, m); }
        } else {
            SetL(context, m, nl);
            if (nl != NULL) { SetP(context, nl, m); }

            SetR(context, m, n);
        }

        SetP(context, n, m);

        SetL(context, n, ml);
        if (ml != NULL) { SetP(context, ml, n); }

        SetR(context, n, mr);
        if (mr != NULL) { SetP(context, mr, n); }
    } else {
        if (np != NULL) {
            if (GetL(context, np) == n) {
                SetL(context, np, m);
            } else {
                SetR(context, np, m);
            }
        }

        SetP(context, m, np);

        SetL(context, m, nl);
        if (nl != NULL) { SetP(context, nl, m); }

        SetR(context, m, nr);
        if (nr != NULL) { SetP(context, nr, m); }

        if (mp != NULL) {
            if (GetL(context, mp) == m) {
                SetL(context, mp, n);
            } else {
                SetR(context, mp, n);
            }
        }

        SetP(context, n, mp);

        SetL(context, n, ml);
        if (ml != NULL) { SetP(context, ml, n); }

        SetR(context, n, mr);
        if (mr != NULL) { SetP(context, mr, n); }
    }

    if (GetAccSize != NULL && SetAccSize != NULL) {
        SetAccSize(context, n, m_acc_size);
        SetAccSize(context, m, n_acc_size);

        AddDiffSize_(btn_opr, n, n_size - m_size);
        AddDiffSize_(btn_opr, m, m_size - n_size);
    }
}

#define Rotate_(D, E)                                                   \
    ZETA_DebugAssert(btn_opr != NULL);                                  \
                                                                        \
    ZETA_DebugAssert(n != NULL);                                        \
                                                                        \
    void* context = btn_opr->context;                                   \
                                                                        \
    void* (*GetP)(void* context, void* n) = btn_opr->GetP;              \
    void* (*GetD)(void* context, void* n) = btn_opr->Get##D;            \
    void* (*GetE)(void* context, void* n) = btn_opr->Get##E;            \
                                                                        \
    ZETA_DebugAssert(GetP != NULL);                                     \
    ZETA_DebugAssert(GetD != NULL);                                     \
    ZETA_DebugAssert(GetE != NULL);                                     \
                                                                        \
    void (*SetP)(void* context, void* n, void* m) = btn_opr->SetP;      \
    void (*SetD)(void* context, void* n, void* m) = btn_opr->Set##D;    \
    void (*SetE)(void* context, void* n, void* m) = btn_opr->Set##E;    \
                                                                        \
    ZETA_DebugAssert(SetP != NULL);                                     \
    ZETA_DebugAssert(SetD != NULL);                                     \
    ZETA_DebugAssert(SetE != NULL);                                     \
                                                                        \
    size_t (*GetAccSize)(void* context, void* n) = btn_opr->GetAccSize; \
    void (*SetAccSize)(void* context, void* n, size_t acc_size) =       \
        btn_opr->SetAccSize;                                            \
                                                                        \
    size_t n_size = Zeta_BinTree_GetSize(btn_opr, n);                   \
                                                                        \
    void* ne = GetE(context, n);                                        \
    ZETA_DebugAssert(ne != NULL);                                       \
                                                                        \
    size_t ne_size = Zeta_BinTree_GetSize(btn_opr, ne);                 \
                                                                        \
    void* ned = GetD(context, ne);                                      \
    void* np = GetP(context, n);                                        \
                                                                        \
    size_t ned_size = Zeta_BinTree_GetSize(btn_opr, ned);               \
                                                                        \
    if (np != NULL) {                                                   \
        if (GetD(context, np) == n) {                                   \
            SetD(context, np, ne);                                      \
        } else {                                                        \
            SetE(context, np, ne);                                      \
        }                                                               \
    }                                                                   \
                                                                        \
    SetP(context, ne, np);                                              \
                                                                        \
    SetD(context, ne, n);                                               \
    SetP(context, n, ne);                                               \
                                                                        \
    SetE(context, n, ned);                                              \
    if (ned != NULL) { SetP(context, ned, n); }                         \
                                                                        \
    if (SetAccSize == NULL) { return; }                                 \
                                                                        \
    size_t n_acc_size = GetAccSize(context, n);                         \
    size_t ne_acc_size = GetAccSize(context, ne);                       \
    size_t ned_acc_size = GetAccSize(context, ned);                     \
                                                                        \
    SetAccSize(context, n, n_acc_size - ne_acc_size + ned_acc_size);    \
    SetAccSize(context, ne, n_acc_size);                                \
                                                                        \
    ZETA_DebugAssert(Zeta_BinTree_GetSize(btn_opr, n) == n_size);       \
    ZETA_DebugAssert(Zeta_BinTree_GetSize(btn_opr, ne) == ne_size);     \
    ZETA_DebugAssert(Zeta_BinTree_GetSize(btn_opr, ned) == ned_size);

void Zeta_BinTree_RotateL(Zeta_BinTreeNodeOperator const* btn_opr, void* n) {
    Rotate_(L, R);
}

void Zeta_BinTree_RotateR(Zeta_BinTreeNodeOperator const* btn_opr, void* n) {
    Rotate_(R, L);
}

#define Step_(D, E)                                                 \
    ZETA_DebugAssert(btn_opr != NULL);                              \
                                                                    \
    ZETA_DebugAssert(n != NULL);                                    \
                                                                    \
    void* context = btn_opr->context;                               \
                                                                    \
    void* (*GetP)(void* context, void* n) = btn_opr->GetP;          \
    void* (*GetD)(void* context, void* n) = btn_opr->Get##D;        \
    void* (*GetE)(void* context, void* n) = btn_opr->Get##E;        \
                                                                    \
    ZETA_DebugAssert(GetP != NULL);                                 \
    ZETA_DebugAssert(GetD != NULL);                                 \
    ZETA_DebugAssert(GetE != NULL);                                 \
                                                                    \
    void* nd = GetD(context, n);                                    \
    if (nd != NULL) { return Zeta_GetMostLink(context, GetE, nd); } \
                                                                    \
    for (;;) {                                                      \
        void* np = GetP(context, n);                                \
        if (np == NULL) { return NULL; }                            \
        if (GetE(context, np) == n) { return np; }                  \
        n = np;                                                     \
    }

void* Zeta_BinTree_StepL(Zeta_BinTreeNodeOperator const* btn_opr, void* n) {
    Step_(L, R);
}

void* Zeta_BinTree_StepR(Zeta_BinTreeNodeOperator const* btn_opr, void* n) {
    Step_(R, L);
}

void Zeta_BinTree_Access(void** dst_n, size_t* dst_tail_idx,
                         Zeta_BinTreeNodeOperator const* btn_opr, void* n,
                         size_t idx) {
    if (dst_n == NULL && dst_tail_idx == NULL) { return; }

    ZETA_DebugAssert(btn_opr != NULL);

    void* context = btn_opr->context;

    void* (*GetP)(void* context, void* n) = btn_opr->GetP;
    void* (*GetL)(void* context, void* n) = btn_opr->GetL;
    void* (*GetR)(void* context, void* n) = btn_opr->GetR;

    ZETA_DebugAssert(GetP != NULL);
    ZETA_DebugAssert(GetL != NULL);
    ZETA_DebugAssert(GetR != NULL);

    size_t (*GetAccSize)(void* context, void* n) = btn_opr->GetAccSize;

    ZETA_DebugAssert(GetAccSize != NULL);

    size_t n_acc_size = GetAccSize(context, n);

    if (n_acc_size <= idx) {
        if (dst_n != NULL) { *dst_n = NULL; }
        if (dst_tail_idx != NULL) { *dst_tail_idx = idx - n_acc_size; }
        return;
    }

    while (n != NULL) {
        void* nl = GetL(context, n);
        size_t nl_acc_size = GetAccSize(context, nl);

        if (idx < nl_acc_size) {
            n = nl;
            continue;
        }

        idx -= nl_acc_size;
        size_t n_size = Zeta_BinTree_GetSize(btn_opr, n);

        if (idx < n_size) { break; }

        n = GetR(context, n);
        idx -= n_size;
    }

    if (dst_n != NULL) { *dst_n = n; }
    if (dst_tail_idx != NULL) { *dst_tail_idx = idx; }
}

void Zeta_BinTree_GetAccSize(size_t* dst_l_acc_size, size_t* dst_r_acc_size,
                             Zeta_BinTreeNodeOperator const* btn_opr, void* n) {
    ZETA_DebugAssert(btn_opr != NULL);

    ZETA_DebugAssert(n != NULL);

    void* context = btn_opr->context;

    void* (*GetP)(void* context, void* n) = btn_opr->GetP;
    void* (*GetL)(void* context, void* n) = btn_opr->GetL;
    void* (*GetR)(void* context, void* n) = btn_opr->GetR;

    ZETA_DebugAssert(GetP != NULL);
    ZETA_DebugAssert(GetL != NULL);
    ZETA_DebugAssert(GetR != NULL);

    size_t (*GetAccSize)(void* context, void* n) = btn_opr->GetAccSize;

    ZETA_DebugAssert(GetAccSize != NULL);

    if (dst_l_acc_size == NULL && dst_r_acc_size == NULL) { return; }

    size_t l_acc_size = GetAccSize(context, GetL(context, n));
    size_t r_acc_size = GetAccSize(context, GetR(context, n));

    for (;;) {
        void* np = GetP(context, n);
        if (np == NULL) { break; }

        size_t k = GetAccSize(context, np) - GetAccSize(context, n);

        if (GetL(context, np) == n) {
            r_acc_size += k;
        } else {
            l_acc_size += k;
        }

        n = np;
    }

    if (dst_l_acc_size != NULL) { *dst_l_acc_size = l_acc_size; }
    if (dst_r_acc_size != NULL) { *dst_r_acc_size = r_acc_size; }
}
