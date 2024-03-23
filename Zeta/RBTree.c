#include "RBTree.h"

#include "utils.h"

#define Black (0)
#define Red (1)

#define Insert_(D, E)                                                         \
    ZETA_DebugAssert(btn_opr != NULL);                                        \
                                                                              \
    ZETA_DebugAssert(m != NULL);                                              \
                                                                              \
    void* context = btn_opr->context;                                         \
                                                                              \
    void* (*GetP)(void* context, void* n) = btn_opr->GetP;                    \
    void* (*GetD)(void* context, void* n) = btn_opr->Get##D;                  \
    void* (*GetE)(void* context, void* n) = btn_opr->Get##E;                  \
                                                                              \
    ZETA_DebugAssert(GetP != NULL);                                           \
    ZETA_DebugAssert(GetD != NULL);                                           \
    ZETA_DebugAssert(GetE != NULL);                                           \
                                                                              \
    ZETA_DebugAssert(GetP(context, m) == NULL);                               \
    ZETA_DebugAssert(GetD(context, m) == NULL);                               \
    ZETA_DebugAssert(GetE(context, m) == NULL);                               \
                                                                              \
    int (*GetPColor)(void* context, void* n) = btn_opr->GetPColor;            \
    void (*SetPColor)(void* context, void* n, int p_color) =                  \
        btn_opr->SetPColor;                                                   \
                                                                              \
    ZETA_DebugAssert(GetPColor != NULL);                                      \
    ZETA_DebugAssert(SetPColor != NULL);                                      \
                                                                              \
    void (*AttachD)(Zeta_BinTreeNodeOperator const* btn_opr, void* n,         \
                    void* m) = Zeta_BinTree_Attatch##D;                       \
    void (*AttachE)(Zeta_BinTreeNodeOperator const* btn_opr, void* n,         \
                    void* m) = Zeta_BinTree_Attatch##E;                       \
                                                                              \
    void (*RotateD)(Zeta_BinTreeNodeOperator const* btn_opr, void* n) =       \
        Zeta_BinTree_Rotate##D;                                               \
    void (*RotateE)(Zeta_BinTreeNodeOperator const* btn_opr, void* n) =       \
        Zeta_BinTree_Rotate##E;                                               \
                                                                              \
    if (n == NULL) {                                                          \
        if (GetPColor(context, m) != Black) { SetPColor(context, m, Black); } \
        return m;                                                             \
    }                                                                         \
                                                                              \
    if (GetPColor(context, m) != Red) { SetPColor(context, m, Red); }         \
                                                                              \
    void* root = Zeta_GetMostLink(context, GetP, n);                          \
                                                                              \
    void* nd = GetD(context, n);                                              \
                                                                              \
    if (nd == NULL) {                                                         \
        AttachD(btn_opr, n, m);                                               \
    } else {                                                                  \
        AttachE(btn_opr, Zeta_GetMostLink(context, GetE, nd), m);             \
    }                                                                         \
                                                                              \
    for (n = m;;) {                                                           \
        void* np = GetP(context, n);                                          \
                                                                              \
        if (np == NULL) {                                                     \
            SetPColor(context, n, Black);                                     \
            break;                                                            \
        }                                                                     \
                                                                              \
        if (GetPColor(context, np) == Black) { break; }                       \
                                                                              \
        void* ng = GetP(context, np);                                         \
                                                                              \
        if (ng == NULL) {                                                     \
            SetPColor(context, np, Black);                                    \
            break;                                                            \
        }                                                                     \
                                                                              \
        void* (*GetY)(void* context, void* n);                                \
        void (*RotateX)(Zeta_BinTreeNodeOperator const* btn_opr, void* n);    \
        void (*RotateY)(Zeta_BinTreeNodeOperator const* btn_opr, void* n);    \
                                                                              \
        if (GetD(context, ng) == np) {                                        \
            GetY = GetE;                                                      \
            RotateX = RotateD;                                                \
            RotateY = RotateE;                                                \
        } else {                                                              \
            GetY = GetD;                                                      \
            RotateX = RotateE;                                                \
            RotateY = RotateD;                                                \
        }                                                                     \
                                                                              \
        void* nu = GetY(context, ng);                                         \
                                                                              \
        if (GetPColor(context, nu) == Red) {                                  \
            SetPColor(context, ng, Red);                                      \
            SetPColor(context, np, Black);                                    \
            SetPColor(context, nu, Black);                                    \
            n = ng;                                                           \
            continue;                                                         \
        }                                                                     \
                                                                              \
        if (GetY(context, np) == n) {                                         \
            RotateX(btn_opr, np);                                             \
            ZETA_Swap(n, np);                                                 \
        }                                                                     \
                                                                              \
        SetPColor(context, ng, Red);                                          \
        SetPColor(context, np, Black);                                        \
        RotateY(btn_opr, ng);                                                 \
        break;                                                                \
    }                                                                         \
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

    int (*GetPColor)(void* context, void* n) = btn_opr->GetPColor;
    void (*SetPColor)(void* context, void* n, int p_color) = btn_opr->SetPColor;

    ZETA_DebugAssert(GetPColor != NULL);
    ZETA_DebugAssert(SetPColor != NULL);

    for (;;) {
        if (GetPColor(context, n) == Red) {
            SetPColor(context, n, Black);
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

        if (GetPColor(context, ns) == Red) {
            SetPColor(context, np, Red);
            SetPColor(context, ns, Black);
            RotateD(btn_opr, np);
            ns = GetE(context, np);
        }

        void* nsd = GetD(context, ns);
        void* nse = GetE(context, ns);

        if (GetPColor(context, nsd) == Black &&
            GetPColor(context, nse) == Black) {
            SetPColor(context, ns, Red);
            n = np;
            continue;
        }

        if (GetPColor(context, nse) == Black) {
            SetPColor(context, ns, Red);
            SetPColor(context, nsd, Black);
            RotateE(btn_opr, ns);
            nse = ns;
            ns = nsd;
            nsd = GetD(context, nsd);
        }

        SetPColor(context, ns, GetPColor(context, np));
        SetPColor(context, nse, Black);
        SetPColor(context, np, Black);
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

    int (*GetPColor)(void* context, void* n) = btn_opr->GetPColor;
    void (*SetPColor)(void* context, void* n, int p_color) = btn_opr->SetPColor;

    void* root;

    void* nl = GetL(context, n);
    void* nr = GetR(context, n);

    if (nl != NULL && nr != NULL) {
        int ran =
            Zeta_SimpleHash(ZETA_PTR_TO_UINT(nl) ^ ZETA_PTR_TO_UINT(nr)) % 2;

        void* m = ran ? Zeta_GetMostLink(context, GetL, nr)
                      : Zeta_GetMostLink(context, GetR, nl);

        Zeta_BinTree_Swap(btn_opr, n, m);

        int nc = GetPColor(context, n);
        int mc = GetPColor(context, m);

        SetPColor(context, n, mc);
        SetPColor(context, m, nc);

        root = Zeta_GetMostLink(context, GetP, m);
    } else {
        root = Zeta_GetMostLink(context, GetP, n);
    }

    if (GetPColor(context, n) == Black) {
        nl = GetL(context, n);
        nr = GetR(context, n);

        if (nl != NULL) {
            Zeta_BinTree_RotateR(btn_opr, n);
            SetPColor(context, nl, Black);
        } else if (nr != NULL) {
            Zeta_BinTree_RotateL(btn_opr, n);
            SetPColor(context, nr, Black);
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

    int nc = btn_opr->GetPColor(context, n);

    if (nc == 0) { return lbh + 1; }

    ZETA_Assert(btn_opr->GetPColor(context, nl) == Black);
    ZETA_Assert(btn_opr->GetPColor(context, nr) == Black);

    return lbh;
}

void Zeta_RBTree_Check(Zeta_BinTreeNodeOperator const* btn_opr, void* n) {
    ZETA_DebugAssert(btn_opr != NULL);
    ZETA_DebugAssert(btn_opr->GetP != NULL);
    ZETA_DebugAssert(btn_opr->GetL != NULL);
    ZETA_DebugAssert(btn_opr->GetR != NULL);
    ZETA_DebugAssert(btn_opr->GetPColor != NULL);

    if (n == NULL) { return; }

    ZETA_DebugAssert(btn_opr->GetP(btn_opr->context, n) == NULL);

    Check_(btn_opr, n);
}
