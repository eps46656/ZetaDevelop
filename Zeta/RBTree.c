#include "RBTree.h"

#include "utils.h"

#define Black (0)
#define Red (1)

#define Insert_(D, E)                                                          \
    ZETA_DebugAssert(btn_opr != NULL);                                         \
                                                                               \
    ZETA_DebugAssert(m != NULL);                                               \
                                                                               \
    void* context = btn_opr->context;                                          \
                                                                               \
    void* (*GetP)(void* context, void* n) = btn_opr->GetP;                     \
    void* (*GetD)(void* context, void* n) = btn_opr->Get##D;                   \
    void* (*GetE)(void* context, void* n) = btn_opr->Get##E;                   \
                                                                               \
    ZETA_DebugAssert(GetP != NULL);                                            \
    ZETA_DebugAssert(GetD != NULL);                                            \
    ZETA_DebugAssert(GetE != NULL);                                            \
                                                                               \
    ZETA_DebugAssert(GetP(context, m) == NULL);                                \
    ZETA_DebugAssert(GetD(context, m) == NULL);                                \
    ZETA_DebugAssert(GetE(context, m) == NULL);                                \
                                                                               \
    int (*GetColor)(void* context, void* n) = btn_opr->GetColor;               \
    void (*SetColor)(void* context, void* n, int p_color) = btn_opr->SetColor; \
                                                                               \
    ZETA_DebugAssert(GetColor != NULL);                                        \
    ZETA_DebugAssert(SetColor != NULL);                                        \
                                                                               \
    void (*AttachD)(Zeta_BinTreeNodeOperator const* btn_opr, void* n,          \
                    void* m) = Zeta_BinTree_Attatch##D;                        \
    void (*AttachE)(Zeta_BinTreeNodeOperator const* btn_opr, void* n,          \
                    void* m) = Zeta_BinTree_Attatch##E;                        \
                                                                               \
    void (*RotateD)(Zeta_BinTreeNodeOperator const* btn_opr, void* n) =        \
        Zeta_BinTree_Rotate##D;                                                \
    void (*RotateE)(Zeta_BinTreeNodeOperator const* btn_opr, void* n) =        \
        Zeta_BinTree_Rotate##E;                                                \
                                                                               \
    if (n == NULL) {                                                           \
        if (GetColor(context, m) != Black) { SetColor(context, m, Black); }    \
        return m;                                                              \
    }                                                                          \
                                                                               \
    if (GetColor(context, m) != Red) { SetColor(context, m, Red); }            \
                                                                               \
    void* root = Zeta_GetMostLink(context, GetP, n);                           \
                                                                               \
    void* nd = GetD(context, n);                                               \
                                                                               \
    if (nd == NULL) {                                                          \
        AttachD(btn_opr, n, m);                                                \
    } else {                                                                   \
        AttachE(btn_opr, Zeta_GetMostLink(context, GetE, nd), m);              \
    }                                                                          \
                                                                               \
    for (n = m;;) {                                                            \
        void* np = GetP(context, n);                                           \
                                                                               \
        if (np == NULL) {                                                      \
            SetColor(context, n, Black);                                       \
            break;                                                             \
        }                                                                      \
                                                                               \
        if (GetColor(context, np) == Black) { break; }                         \
                                                                               \
        void* ng = GetP(context, np);                                          \
                                                                               \
        if (ng == NULL) {                                                      \
            SetColor(context, np, Black);                                      \
            break;                                                             \
        }                                                                      \
                                                                               \
        void* (*GetY)(void* context, void* n);                                 \
        void (*RotateX)(Zeta_BinTreeNodeOperator const* btn_opr, void* n);     \
        void (*RotateY)(Zeta_BinTreeNodeOperator const* btn_opr, void* n);     \
                                                                               \
        if (GetD(context, ng) == np) {                                         \
            GetY = GetE;                                                       \
            RotateX = RotateD;                                                 \
            RotateY = RotateE;                                                 \
        } else {                                                               \
            GetY = GetD;                                                       \
            RotateX = RotateE;                                                 \
            RotateY = RotateD;                                                 \
        }                                                                      \
                                                                               \
        void* nu = GetY(context, ng);                                          \
                                                                               \
        if (GetColor(context, nu) == Red) {                                    \
            SetColor(context, ng, Red);                                        \
            SetColor(context, np, Black);                                      \
            SetColor(context, nu, Black);                                      \
            n = ng;                                                            \
            continue;                                                          \
        }                                                                      \
                                                                               \
        if (GetY(context, np) == n) {                                          \
            RotateX(btn_opr, np);                                              \
            ZETA_Swap(n, np);                                                  \
        }                                                                      \
                                                                               \
        SetColor(context, ng, Red);                                            \
        SetColor(context, np, Black);                                          \
        RotateY(btn_opr, ng);                                                  \
        break;                                                                 \
    }                                                                          \
                                                                               \
    return Zeta_GetMostLink(context, GetP, root);

void* Zeta_RBTree_InsertL(Zeta_BinTreeNodeOperator const* btn_opr, void* n,
                          void* m) {
    Insert_(L, R);
}

void* Zeta_RBTree_InsertR(Zeta_BinTreeNodeOperator const* btn_opr, void* n,
                          void* m) {
    Insert_(R, L);
}

static void ExtractBalance_(Zeta_BinTreeNodeOperator const* btn_opr, void* n) {
    void* context = btn_opr->context;

    void* (*GetP)(void* context, void* n) = btn_opr->GetP;
    void* (*GetL)(void* context, void* n) = btn_opr->GetL;
    void* (*GetR)(void* context, void* n) = btn_opr->GetR;

    ZETA_DebugAssert(GetP != NULL);
    ZETA_DebugAssert(GetL != NULL);
    ZETA_DebugAssert(GetR != NULL);

    int (*GetColor)(void* context, void* n) = btn_opr->GetColor;
    void (*SetColor)(void* context, void* n, int p_color) = btn_opr->SetColor;

    ZETA_DebugAssert(GetColor != NULL);
    ZETA_DebugAssert(SetColor != NULL);

    for (;;) {
        if (GetColor(context, n) == Red) {
            SetColor(context, n, Black);
            break;
        }

        void* np = GetP(context, n);
        if (np == NULL) { break; }

        void* (*GetD)(void* context, void* n);
        void* (*GetE)(void* context, void* n);
        void (*RotateD)(Zeta_BinTreeNodeOperator const* btn_opr, void* n);
        void (*RotateE)(Zeta_BinTreeNodeOperator const* btn_opr, void* n);

        if (GetL(context, np) == n) {
            GetD = GetL;
            GetE = GetR;
            RotateD = Zeta_BinTree_RotateL;
            RotateE = Zeta_BinTree_RotateR;
        } else {
            GetD = GetR;
            GetE = GetL;
            RotateD = Zeta_BinTree_RotateR;
            RotateE = Zeta_BinTree_RotateL;
        }

        void* ns = GetE(context, np);

        if (GetColor(context, ns) == Red) {
            SetColor(context, np, Red);
            SetColor(context, ns, Black);
            RotateD(btn_opr, np);
            ns = GetE(context, np);
        }

        void* nsd = GetD(context, ns);
        void* nse = GetE(context, ns);

        if (GetColor(context, nsd) == Black &&
            GetColor(context, nse) == Black) {
            SetColor(context, ns, Red);
            n = np;
            continue;
        }

        if (GetColor(context, nse) == Black) {
            SetColor(context, ns, Red);
            SetColor(context, nsd, Black);
            RotateE(btn_opr, ns);
            nse = ns;
            ns = nsd;
            nsd = GetD(context, nsd);
        }

        SetColor(context, ns, GetColor(context, np));
        SetColor(context, nse, Black);
        SetColor(context, np, Black);
        RotateD(btn_opr, np);

        break;
    }
}

void* Zeta_RBTree_Extract(Zeta_BinTreeNodeOperator const* btn_opr, void* n) {
    ZETA_DebugAssert(btn_opr != NULL);

    ZETA_DebugAssert(n != NULL);

    void* context = btn_opr->context;

    void* (*GetP)(void* context, void* n) = btn_opr->GetP;
    void* (*GetL)(void* context, void* n) = btn_opr->GetL;
    void* (*GetR)(void* context, void* n) = btn_opr->GetR;

    ZETA_DebugAssert(GetP != NULL);
    ZETA_DebugAssert(GetL != NULL);
    ZETA_DebugAssert(GetR != NULL);

    int (*GetColor)(void* context, void* n) = btn_opr->GetColor;
    void (*SetColor)(void* context, void* n, int p_color) = btn_opr->SetColor;

    void* root;

    void* nl = GetL(context, n);
    void* nr = GetR(context, n);

    if (nl != NULL && nr != NULL) {
        int ran =
            Zeta_SimpleHash(ZETA_PTR_TO_UINT(nl) ^ ZETA_PTR_TO_UINT(nr)) % 2;

        void* m = ran ? Zeta_GetMostLink(context, GetL, nr)
                      : Zeta_GetMostLink(context, GetR, nl);

        Zeta_BinTree_Swap(btn_opr, n, m);

        int nc = GetColor(context, n);
        int mc = GetColor(context, m);

        SetColor(context, n, mc);
        SetColor(context, m, nc);

        root = Zeta_GetMostLink(context, GetP, m);
    } else {
        root = Zeta_GetMostLink(context, GetP, n);
    }

    if (GetColor(context, n) == Black) {
        nl = GetL(context, n);
        nr = GetR(context, n);

        if (nl != NULL) {
            Zeta_BinTree_RotateR(btn_opr, n);
            SetColor(context, nl, Black);
        } else if (nr != NULL) {
            Zeta_BinTree_RotateL(btn_opr, n);
            SetColor(context, nr, Black);
        } else {
            ExtractBalance_(btn_opr, n);
        }
    }

    root = Zeta_GetMostLink(context, GetP, root);
    if (root == n) { return NULL; }

    Zeta_BinTree_Detach(btn_opr, n);
    return root;
}

static size_t Check_(Zeta_BinTreeNodeOperator const* btn_opr, void* n) {
    if (n == NULL) { return 0; }

    void* context = btn_opr->context;

    void* nl = btn_opr->GetL(context, n);
    void* nr = btn_opr->GetR(context, n);

    if (nl != NULL) { ZETA_Assert(btn_opr->GetP(context, nl) == n); }
    if (nr != NULL) { ZETA_Assert(btn_opr->GetP(context, nr) == n); }

    size_t lbh = Check_(btn_opr, nl);
    size_t rbh = Check_(btn_opr, nr);

    ZETA_Assert(lbh == rbh);

    int nc = btn_opr->GetColor(context, n);

    if (nc == 0) { return lbh + 1; }

    ZETA_Assert(btn_opr->GetColor(context, nl) == Black);
    ZETA_Assert(btn_opr->GetColor(context, nr) == Black);

    return lbh;
}

void Zeta_RBTree_Check(Zeta_BinTreeNodeOperator const* btn_opr, void* n) {
    ZETA_DebugAssert(btn_opr != NULL);
    ZETA_DebugAssert(btn_opr->GetP != NULL);
    ZETA_DebugAssert(btn_opr->GetL != NULL);
    ZETA_DebugAssert(btn_opr->GetR != NULL);
    ZETA_DebugAssert(btn_opr->GetColor != NULL);

    if (n == NULL) { return; }

    ZETA_DebugAssert(btn_opr->GetP(btn_opr->context, n) == NULL);

    Check_(btn_opr, n);
}
