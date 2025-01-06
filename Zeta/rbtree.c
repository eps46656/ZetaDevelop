#include "rbtree.h"

#include "debugger.h"
#include "utils.h"

#define Black (0)
#define Red (1)

#define Insert_(D, E)                                                       \
    ZETA_DebugAssert(btn_opr != NULL);                                      \
                                                                            \
    ZETA_DebugAssert(pos != n);                                             \
    ZETA_DebugAssert(n != NULL);                                            \
                                                                            \
    ZETA_DebugAssert(ZETA_BinTreeNodeOperator_GetP(btn_opr, n) == NULL);    \
    ZETA_DebugAssert(ZETA_BinTreeNodeOperator_Get##D(btn_opr, n) == NULL);  \
    ZETA_DebugAssert(ZETA_BinTreeNodeOperator_Get##E(btn_opr, n) == NULL);  \
                                                                            \
    if (pos == NULL) {                                                      \
        if (ZETA_BinTreeNodeOperator_GetColor(btn_opr, n) != Black) {       \
            ZETA_BinTreeNodeOperator_SetColor(btn_opr, n, Black);           \
        }                                                                   \
        return n;                                                           \
    }                                                                       \
                                                                            \
    if (ZETA_BinTreeNodeOperator_GetColor(btn_opr, n) != Red) {             \
        ZETA_BinTreeNodeOperator_SetColor(btn_opr, n, Red);                 \
    }                                                                       \
                                                                            \
    void* pos_d = ZETA_BinTreeNodeOperator_Get##D(btn_opr, pos);            \
                                                                            \
    if (pos_d == NULL) {                                                    \
        Zeta_BinTree_Attatch##D(btn_opr, pos, n);                           \
    } else {                                                                \
        Zeta_BinTree_Attatch##E(                                            \
            btn_opr,                                                        \
            Zeta_GetMostLink(btn_opr->context, btn_opr->Get##E, pos_d), n); \
    }                                                                       \
                                                                            \
    return InsertBalance_(btn_opr, n);

static void* InsertBalance_(Zeta_BinTreeNodeOperator const* btn_opr, void* n) {
    ZETA_DebugAssert(btn_opr != NULL);

    ZETA_DebugAssert(n != NULL);

    for (;;) {
        void* np = ZETA_BinTreeNodeOperator_GetP(btn_opr, n);

        if (np == NULL) {
            ZETA_BinTreeNodeOperator_SetColor(btn_opr, n, Black);
            break;
        }

        if (ZETA_BinTreeNodeOperator_GetColor(btn_opr, np) == Black) { break; }

        void* ng = ZETA_BinTreeNodeOperator_GetP(btn_opr, np);

        if (ng == NULL) {
            ZETA_BinTreeNodeOperator_SetColor(btn_opr, np, Black);
            break;
        }

        void* (*GetY)(void const* context, void* n);
        void (*RotateX)(Zeta_BinTreeNodeOperator const* btn_opr, void* n);
        void (*RotateY)(Zeta_BinTreeNodeOperator const* btn_opr, void* n);

        if (ZETA_BinTreeNodeOperator_GetL(btn_opr, ng) == np) {
            GetY = btn_opr->GetR;
            RotateX = Zeta_BinTree_RotateL;
            RotateY = Zeta_BinTree_RotateR;
        } else {
            GetY = btn_opr->GetL;
            RotateX = Zeta_BinTree_RotateR;
            RotateY = Zeta_BinTree_RotateL;
        }

        void* nu = GetY(btn_opr->context, ng);

        if (ZETA_BinTreeNodeOperator_GetColor(btn_opr, nu) == Red) {
            ZETA_BinTreeNodeOperator_SetColor(btn_opr, ng, Red);
            ZETA_BinTreeNodeOperator_SetColor(btn_opr, np, Black);
            ZETA_BinTreeNodeOperator_SetColor(btn_opr, nu, Black);
            n = ng;
            continue;
        }

        if (GetY(btn_opr->context, np) == n) {
            RotateX(btn_opr, np);
            ZETA_Swap(n, np);
        }

        ZETA_BinTreeNodeOperator_SetColor(btn_opr, ng, Red);
        ZETA_BinTreeNodeOperator_SetColor(btn_opr, np, Black);
        RotateY(btn_opr, ng);
        break;
    }

    return Zeta_GetMostLink(btn_opr->context, btn_opr->GetP, n);
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

    ZETA_DebugAssert(ZETA_BinTreeNodeOperator_GetP(btn_opr, n) == NULL);
    ZETA_DebugAssert(ZETA_BinTreeNodeOperator_GetL(btn_opr, n) == NULL);
    ZETA_DebugAssert(ZETA_BinTreeNodeOperator_GetR(btn_opr, n) == NULL);

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
        if (ZETA_BinTreeNodeOperator_GetColor(btn_opr, n) != Black) {
            ZETA_BinTreeNodeOperator_SetColor(btn_opr, n, Black);
        }
        return n;
    }

    if (ZETA_BinTreeNodeOperator_GetColor(btn_opr, n) != Red) {
        ZETA_BinTreeNodeOperator_SetColor(btn_opr, n, Red);
    }

    if (pos_l != NULL &&
        ZETA_BinTreeNodeOperator_GetR(btn_opr, pos_l) == NULL) {
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
    void const* context = btn_opr->context;

    for (;;) {
        if (ZETA_BinTreeNodeOperator_GetColor(btn_opr, n) == Red) {
            ZETA_BinTreeNodeOperator_SetColor(btn_opr, n, Black);
            break;
        }

        void* np = ZETA_BinTreeNodeOperator_GetP(btn_opr, n);
        if (np == NULL) { break; }

        void* (*GetD)(void const* context, void* n);
        void* (*GetE)(void const* context, void* n);
        void (*RotateD)(Zeta_BinTreeNodeOperator const* btn_opr, void* n);
        void (*RotateE)(Zeta_BinTreeNodeOperator const* btn_opr, void* n);

        if (ZETA_BinTreeNodeOperator_GetL(btn_opr, np) == n) {
            GetD = btn_opr->GetL;
            GetE = btn_opr->GetR;
            RotateD = Zeta_BinTree_RotateL;
            RotateE = Zeta_BinTree_RotateR;
        } else {
            GetD = btn_opr->GetR;
            GetE = btn_opr->GetL;
            RotateD = Zeta_BinTree_RotateR;
            RotateE = Zeta_BinTree_RotateL;
        }

        void* ns = GetE(context, np);

        if (ZETA_BinTreeNodeOperator_GetColor(btn_opr, ns) == Red) {
            ZETA_BinTreeNodeOperator_SetColor(btn_opr, np, Red);
            ZETA_BinTreeNodeOperator_SetColor(btn_opr, ns, Black);
            RotateD(btn_opr, np);
            ns = GetE(context, np);
        }

        void* nsd = GetD(context, ns);
        void* nse = GetE(context, ns);

        if (ZETA_BinTreeNodeOperator_GetColor(btn_opr, nsd) == Black &&
            ZETA_BinTreeNodeOperator_GetColor(btn_opr, nse) == Black) {
            ZETA_BinTreeNodeOperator_SetColor(btn_opr, ns, Red);
            n = np;
            continue;
        }

        if (ZETA_BinTreeNodeOperator_GetColor(btn_opr, nse) == Black) {
            ZETA_BinTreeNodeOperator_SetColor(btn_opr, ns, Red);
            ZETA_BinTreeNodeOperator_SetColor(btn_opr, nsd, Black);
            RotateE(btn_opr, ns);
            nse = ns;
            ns = nsd;
            nsd = GetD(context, nsd);
        }

        ZETA_BinTreeNodeOperator_SetColor(
            btn_opr, ns, ZETA_BinTreeNodeOperator_GetColor(btn_opr, np));
        ZETA_BinTreeNodeOperator_SetColor(btn_opr, nse, Black);
        ZETA_BinTreeNodeOperator_SetColor(btn_opr, np, Black);
        RotateD(btn_opr, np);

        break;
    }
}

void* Zeta_RBTree_Extract(Zeta_BinTreeNodeOperator const* btn_opr, void* pos) {
    ZETA_DebugAssert(btn_opr != NULL);

    ZETA_DebugAssert(pos != NULL);

    void* n = pos;

    void* context = btn_opr->context;

    void* root;

    void* nl = ZETA_BinTreeNodeOperator_GetL(btn_opr, n);
    void* nr = ZETA_BinTreeNodeOperator_GetR(btn_opr, n);

    if (nl != NULL && nr != NULL) {
        unsigned long long rand_seed = ZETA_PtrToAddr(nl) + ZETA_PtrToAddr(nr);

        void* m = Zeta_SimpleRandomRotate(&rand_seed) % 2 == 0
                      ? Zeta_GetMostLink(context, btn_opr->GetL, nr)
                      : Zeta_GetMostLink(context, btn_opr->GetR, nl);

        Zeta_BinTree_Swap(btn_opr, n, m);

        int nc = ZETA_BinTreeNodeOperator_GetColor(btn_opr, n);
        int mc = ZETA_BinTreeNodeOperator_GetColor(btn_opr, m);

        if (nc != mc) {
            ZETA_BinTreeNodeOperator_SetColor(btn_opr, n, mc);
            ZETA_BinTreeNodeOperator_SetColor(btn_opr, m, nc);
        }

        root = Zeta_GetMostLink(context, btn_opr->GetP, m);
    } else {
        root = Zeta_GetMostLink(context, btn_opr->GetP, n);
    }

    if (ZETA_BinTreeNodeOperator_GetColor(btn_opr, n) == Black) {
        nl = ZETA_BinTreeNodeOperator_GetL(btn_opr, n);
        nr = ZETA_BinTreeNodeOperator_GetR(btn_opr, n);

        if (nl != NULL) {
            Zeta_BinTree_RotateR(btn_opr, n);
            ZETA_BinTreeNodeOperator_SetColor(btn_opr, nl, Black);
        } else if (nr != NULL) {
            Zeta_BinTree_RotateL(btn_opr, n);
            ZETA_BinTreeNodeOperator_SetColor(btn_opr, nr, Black);
        } else {
            ExtractBalance_(btn_opr, n);
        }
    }

    root = Zeta_GetMostLink(context, btn_opr->GetP, root);
    if (root == n) { return NULL; }

    Zeta_BinTree_Detach(btn_opr, n);
    return root;
}

static size_t Sanitize_(Zeta_BinTreeNodeOperator const* btn_opr,
                        Zeta_MemRecorder* dst_mr, void* n) {
    if (n == NULL) { return 0; }

    void* nl = ZETA_BinTreeNodeOperator_GetL(btn_opr, n);
    void* nr = ZETA_BinTreeNodeOperator_GetR(btn_opr, n);

    if (nl != NULL) {
        ZETA_DebugAssert(ZETA_BinTreeNodeOperator_GetP(btn_opr, nl) == n);
    }
    if (nr != NULL) {
        ZETA_DebugAssert(ZETA_BinTreeNodeOperator_GetP(btn_opr, nr) == n);
    }

    size_t lbh = Sanitize_(btn_opr, dst_mr, nl);

    if (dst_mr != NULL) { Zeta_MemRecorder_Record(dst_mr, n, sizeof(void*)); }

    size_t rbh = Sanitize_(btn_opr, dst_mr, nr);

    ZETA_DebugAssert(lbh == rbh);

    int nc = ZETA_BinTreeNodeOperator_GetColor(btn_opr, n);

    ZETA_DebugAssert(nc == Black || nc == Red);

    if (nc == Black) { return lbh + 1; }

    ZETA_DebugAssert(ZETA_BinTreeNodeOperator_GetColor(btn_opr, nl) == Black);
    ZETA_DebugAssert(ZETA_BinTreeNodeOperator_GetColor(btn_opr, nr) == Black);

    return lbh;
}

void Zeta_RBTree_Sanitize(Zeta_BinTreeNodeOperator const* btn_opr,
                          Zeta_MemRecorder* dst_mr, void* root) {
    ZETA_DebugAssert(btn_opr != NULL);

    if (root == NULL) { return; }

    ZETA_DebugAssert(ZETA_BinTreeNodeOperator_GetP(btn_opr, root) == NULL);
    ZETA_DebugAssert(ZETA_BinTreeNodeOperator_GetColor(btn_opr, root) == Black);

    Sanitize_(btn_opr, dst_mr, root);
}
