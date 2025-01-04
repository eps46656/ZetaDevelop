#include "rbtree.h"

#include "debugger.h"
#include "utils.h"

#define Black (0)
#define Red (1)

#define Insert_(D, E)                                                        \
    ZETA_DebugAssert(btn_opr != NULL);                                       \
                                                                             \
    ZETA_DebugAssert(pos != n);                                              \
    ZETA_DebugAssert(n != NULL);                                             \
                                                                             \
    void* context = btn_opr->context;                                        \
                                                                             \
    void* (*GetP)(void* context, void* n) = btn_opr->GetP;                   \
    void* (*GetD)(void* context, void* n) = btn_opr->Get##D;                 \
    void* (*GetE)(void* context, void* n) = btn_opr->Get##E;                 \
                                                                             \
    ZETA_DebugAssert(GetP != NULL);                                          \
    ZETA_DebugAssert(GetD != NULL);                                          \
    ZETA_DebugAssert(GetE != NULL);                                          \
                                                                             \
    ZETA_DebugAssert(GetP(context, n) == NULL);                              \
    ZETA_DebugAssert(GetD(context, n) == NULL);                              \
    ZETA_DebugAssert(GetE(context, n) == NULL);                              \
                                                                             \
    int (*GetColor)(void* context, void* n) = btn_opr->GetColor;             \
    void (*SetColor)(void* context, void* n, int color) = btn_opr->SetColor; \
                                                                             \
    ZETA_DebugAssert(GetColor != NULL);                                      \
    ZETA_DebugAssert(SetColor != NULL);                                      \
                                                                             \
    if (pos == NULL) {                                                       \
        if (GetColor(context, n) != Black) { SetColor(context, n, Black); }  \
        return n;                                                            \
    }                                                                        \
                                                                             \
    if (GetColor(context, n) != Red) { SetColor(context, n, Red); }          \
                                                                             \
    void* pos_d = GetD(context, pos);                                        \
                                                                             \
    if (pos_d == NULL) {                                                     \
        Zeta_BinTree_Attatch##D(btn_opr, pos, n);                            \
    } else {                                                                 \
        Zeta_BinTree_Attatch##E(btn_opr,                                     \
                                Zeta_GetMostLink(context, GetE, pos_d), n);  \
    }                                                                        \
                                                                             \
    return InsertBalance_(btn_opr, n);

static void* InsertBalance_(Zeta_BinTreeNodeOperator const* btn_opr, void* n) {
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
    void (*SetColor)(void* context, void* n, int color) = btn_opr->SetColor;

    ZETA_DebugAssert(GetColor != NULL);
    ZETA_DebugAssert(SetColor != NULL);

    for (;;) {
        void* np = GetP(context, n);

        if (np == NULL) {
            SetColor(context, n, Black);
            break;
        }

        if (GetColor(context, np) == Black) { break; }

        void* ng = GetP(context, np);

        if (ng == NULL) {
            SetColor(context, np, Black);
            break;
        }

        void* (*GetY)(void* context, void* n);
        void (*RotateX)(Zeta_BinTreeNodeOperator const* btn_opr, void* n);
        void (*RotateY)(Zeta_BinTreeNodeOperator const* btn_opr, void* n);

        if (GetL(context, ng) == np) {
            GetY = GetR;
            RotateX = Zeta_BinTree_RotateL;
            RotateY = Zeta_BinTree_RotateR;
        } else {
            GetY = GetL;
            RotateX = Zeta_BinTree_RotateR;
            RotateY = Zeta_BinTree_RotateL;
        }

        void* nu = GetY(context, ng);

        if (GetColor(context, nu) == Red) {
            SetColor(context, ng, Red);
            SetColor(context, np, Black);
            SetColor(context, nu, Black);
            n = ng;
            continue;
        }

        if (GetY(context, np) == n) {
            RotateX(btn_opr, np);
            ZETA_Swap(n, np);
        }

        SetColor(context, ng, Red);
        SetColor(context, np, Black);
        RotateY(btn_opr, ng);
        break;
    }

    return Zeta_GetMostLink(context, GetP, n);
}

void* Zeta_RBTree_InsertL(Zeta_BinTreeNodeOperator const* btn_opr, void* pos,
                          void* n) {
    Insert_(L, R);
}

void* Zeta_RBTree_InsertR(Zeta_BinTreeNodeOperator const* btn_opr, void* pos,
                          void* n) {
    Insert_(R, L);
}

void* Zeta_RBTree_Insert(Zeta_BinTreeNodeOperator const* btn_opr, void* pos_l,
                         void* pos_r, void* n) {
    ZETA_DebugAssert(btn_opr != NULL);

    ZETA_DebugAssert(pos_l != n);
    ZETA_DebugAssert(pos_r != n);
    ZETA_DebugAssert(n != NULL);

    void* context = btn_opr->context;

    void* (*GetP)(void* context, void* n) = btn_opr->GetP;
    void* (*GetL)(void* context, void* n) = btn_opr->GetL;
    void* (*GetR)(void* context, void* n) = btn_opr->GetR;

    ZETA_DebugAssert(GetP != NULL);
    ZETA_DebugAssert(GetL != NULL);
    ZETA_DebugAssert(GetR != NULL);

    ZETA_DebugAssert(GetP(context, n) == NULL);
    ZETA_DebugAssert(GetL(context, n) == NULL);
    ZETA_DebugAssert(GetR(context, n) == NULL);

    int (*GetColor)(void* context, void* n) = btn_opr->GetColor;
    void (*SetColor)(void* context, void* n, int color) = btn_opr->SetColor;

    ZETA_DebugAssert(GetColor != NULL);
    ZETA_DebugAssert(SetColor != NULL);

    void (*AttachL)(Zeta_BinTreeNodeOperator const* btn_opr, void* pos,
                    void* n) = Zeta_BinTree_AttatchL;
    void (*AttachR)(Zeta_BinTreeNodeOperator const* btn_opr, void* pos,
                    void* n) = Zeta_BinTree_AttatchR;

    ZETA_DebugAssert(AttachL != NULL);
    ZETA_DebugAssert(AttachR != NULL);

    ZETA_DebugAssert(pos_l == NULL ||
                     Zeta_BinTree_StepR(btn_opr, pos_l) == pos_r);
    ZETA_DebugAssert(pos_r == NULL ||
                     Zeta_BinTree_StepL(btn_opr, pos_r) == pos_l);

    if (pos_l == NULL && pos_r == NULL) {
        if (GetColor(context, n) != Black) { SetColor(context, n, Black); }
        return n;
    }

    if (GetColor(context, n) != Red) { SetColor(context, n, Red); }

    if (pos_l != NULL && GetR(context, pos_l) == NULL) {
        AttachR(btn_opr, pos_l, n);
    } else {
        AttachL(btn_opr, pos_r, n);
    }

    return InsertBalance_(btn_opr, n);
}

#define GeneralInsert_(D, E)                                                  \
    ZETA_DebugAssert(btn_opr != NULL);                                        \
                                                                              \
    if (pos == NULL) {                                                        \
        return Zeta_RBTree_Insert##E(                                         \
            btn_opr,                                                          \
            Zeta_GetMostLink(btn_opr->context, btn_opr->Get##E, root), n);    \
    }                                                                         \
                                                                              \
    ZETA_DebugAssert(root ==                                                  \
                     Zeta_GetMostLink(btn_opr->context, btn_opr->GetP, pos)); \
                                                                              \
    return Zeta_RBTree_Insert##D(btn_opr, pos, n);

void* Zeta_RBTree_GeneralInsertL(Zeta_BinTreeNodeOperator const* btn_opr,
                                 void* root, void* pos, void* n) {
    GeneralInsert_(L, R);
}

void* Zeta_RBTree_GeneralInsertR(Zeta_BinTreeNodeOperator const* btn_opr,
                                 void* root, void* pos, void* n) {
    GeneralInsert_(R, L);
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
    void (*SetColor)(void* context, void* n, int color) = btn_opr->SetColor;

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

void* Zeta_RBTree_Extract(Zeta_BinTreeNodeOperator const* btn_opr, void* pos) {
    ZETA_DebugAssert(btn_opr != NULL);

    ZETA_DebugAssert(pos != NULL);

    void* n = pos;

    void* context = btn_opr->context;

    void* (*GetP)(void* context, void* n) = btn_opr->GetP;
    void* (*GetL)(void* context, void* n) = btn_opr->GetL;
    void* (*GetR)(void* context, void* n) = btn_opr->GetR;

    ZETA_DebugAssert(GetP != NULL);
    ZETA_DebugAssert(GetL != NULL);
    ZETA_DebugAssert(GetR != NULL);

    int (*GetColor)(void* context, void* n) = btn_opr->GetColor;
    void (*SetColor)(void* context, void* n, int color) = btn_opr->SetColor;

    ZETA_DebugAssert(GetColor != NULL);
    ZETA_DebugAssert(SetColor != NULL);

    void* root;

    void* nl = GetL(context, n);
    void* nr = GetR(context, n);

    if (nl != NULL && nr != NULL) {
        unsigned long long rand_seed = ZETA_PtrToAddr(nl) + ZETA_PtrToAddr(nr);

        void* m = Zeta_SimpleRandomRotate(&rand_seed) % 2 == 0
                      ? Zeta_GetMostLink(context, GetL, nr)
                      : Zeta_GetMostLink(context, GetR, nl);

        Zeta_BinTree_Swap(btn_opr, n, m);

        int nc = GetColor(context, n);
        int mc = GetColor(context, m);

        if (nc != mc) {
            SetColor(context, n, mc);
            SetColor(context, m, nc);
        }

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

static size_t Sanitize_(Zeta_BinTreeNodeOperator const* btn_opr,
                        Zeta_MemRecorder* dst_mr, void* n) {
    if (n == NULL) { return 0; }

    void* context = btn_opr->context;

    void* nl = btn_opr->GetL(context, n);
    void* nr = btn_opr->GetR(context, n);

    if (nl != NULL) { ZETA_DebugAssert(btn_opr->GetP(context, nl) == n); }
    if (nr != NULL) { ZETA_DebugAssert(btn_opr->GetP(context, nr) == n); }

    size_t lbh = Sanitize_(btn_opr, dst_mr, nl);

    if (dst_mr != NULL) { Zeta_MemRecorder_Record(dst_mr, n, sizeof(void*)); }

    size_t rbh = Sanitize_(btn_opr, dst_mr, nr);

    ZETA_DebugAssert(lbh == rbh);

    int nc = btn_opr->GetColor(context, n);

    ZETA_DebugAssert(nc == Black || nc == Red);

    if (nc == Black) { return lbh + 1; }

    ZETA_DebugAssert(btn_opr->GetColor(context, nl) == Black);
    ZETA_DebugAssert(btn_opr->GetColor(context, nr) == Black);

    return lbh;
}

void Zeta_RBTree_Sanitize(Zeta_BinTreeNodeOperator const* btn_opr,
                          Zeta_MemRecorder* dst_mr, void* root) {
    ZETA_DebugAssert(btn_opr != NULL);
    ZETA_DebugAssert(btn_opr->GetP != NULL);
    ZETA_DebugAssert(btn_opr->GetL != NULL);
    ZETA_DebugAssert(btn_opr->GetR != NULL);
    ZETA_DebugAssert(btn_opr->GetColor != NULL);

    if (root == NULL) { return; }

    ZETA_DebugAssert(ZETA_CallMemberFunc(btn_opr, GetP, root) == NULL);
    ZETA_DebugAssert(ZETA_CallMemberFunc(btn_opr, GetColor, root) == Black);

    Sanitize_(btn_opr, dst_mr, root);
}
