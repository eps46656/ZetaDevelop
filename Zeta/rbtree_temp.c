#if !defined(TreeNode)
#error "TreeNode is not defined."
#endif

#include "debugger.h"
#include "utils.h"

#pragma push_macro("Zeta_TreeNode_")
#pragma push_macro("Zeta_BinTree_TreeNode_")
#pragma push_macro("Zeta_RBTree_TreeNode_")
#pragma push_macro("Black")
#pragma push_macro("Red")
#pragma push_macro("Insert_")
#pragma push_macro("F_")
#pragma push_macro("GeneralInsert_")

#define Zeta_TreeNode_(x) ZETA_Concat(ZETA_Concat(Zeta_TreeNode, _), x)

#define Zeta_BinTree_TreeNode_(x) \
    ZETA_Concat(ZETA_Concat(ZETA_Concat(Zeta_BinTree_, TreeNode), _), x)

#define Zeta_RBTree_TreeNode_(x) \
    ZETA_Concat(ZETA_Concat(ZETA_Concat(Zeta_RBTree_, TreeNode), _), x)

#define Black (0)
#define Red (1)

#define Insert_(D, E)                                            \
                                                                 \
    ZETA_DebugAssert(pos != n);                                  \
    ZETA_DebugAssert(n != NULL);                                 \
                                                                 \
    ZETA_DebugAssert(Zeta_TreeNode_(GetP)(n) == NULL);           \
    ZETA_DebugAssert(Zeta_TreeNode_(Get##D)(n) == NULL);         \
    ZETA_DebugAssert(Zeta_TreeNode_(Get##E)(n) == NULL);         \
                                                                 \
    if (pos == NULL) {                                           \
        if (Zeta_TreeNode_(GetPColor)(n) != Black) {             \
            Zeta_TreeNode_(SetPColor)(n, Black);                 \
        }                                                        \
        return n;                                                \
    }                                                            \
                                                                 \
    if (Zeta_TreeNode_(GetPColor)(n) != Red) {                   \
        Zeta_TreeNode_(SetPColor)(n, Red);                       \
    }                                                            \
                                                                 \
    void* pos_d = Zeta_TreeNode_(Get##D)(pos);                   \
                                                                 \
    if (pos_d == NULL) {                                         \
        Zeta_BinTree_TreeNode_(Attatch##D)(pos, n);              \
    } else {                                                     \
        Zeta_BinTree_TreeNode_(Attatch##E)(                      \
            Zeta_GetMostLink(pos_d, Zeta_TreeNode_(Get##E)), n); \
    }                                                            \
                                                                 \
    return Zeta_RBTree_TreeNode_(InsertBalance_)(n);

#define F_(D, E)                                              \
    void* nu = Zeta_TreeNode_(Get##E)(ng);                    \
                                                              \
    if (nu != NULL && Zeta_TreeNode_(GetPColor)(nu) == Red) { \
        Zeta_TreeNode_(SetPColor)(ng, Red);                   \
        Zeta_TreeNode_(SetPColor)(np, Black);                 \
        Zeta_TreeNode_(SetPColor)(nu, Black);                 \
        n = ng;                                               \
        continue;                                             \
    }                                                         \
                                                              \
    if (Zeta_TreeNode_(Get##E)(np) == n) {                    \
        Zeta_BinTree_TreeNode_(Rotate##D)(np);                \
        ZETA_Swap(n, np);                                     \
    }                                                         \
                                                              \
    Zeta_TreeNode_(SetPColor)(ng, Red);                       \
    Zeta_TreeNode_(SetPColor)(np, Black);                     \
    Zeta_BinTree_TreeNode_(Rotate##E)(ng);                    \
    break;

static void* Zeta_RBTree_TreeNode_(InsertBalance_)(void* n) {
    ZETA_DebugAssert(n != NULL);

    for (;;) {
        void* np = Zeta_TreeNode_(GetP)(n);

        if (np == NULL) {
            Zeta_TreeNode_(SetPColor)(n, Black);
            break;
        }

        if (Zeta_TreeNode_(GetPColor)(np) == Black) { break; }

        void* ng = Zeta_TreeNode_(GetP)(np);

        if (ng == NULL) {
            Zeta_TreeNode_(SetPColor)(np, Black);
            break;
        }

        if (Zeta_TreeNode_(GetL)(ng) == np) {
            F_(L, R);
        } else {
            F_(R, L);
        }
    }

    return Zeta_GetMostLink(n, Zeta_TreeNode_(GetP));
}

void* Zeta_RBTree_TreeNode_(InsertL)(void* pos, void* n) { Insert_(L, R); }

void* Zeta_RBTree_TreeNode_(InsertR)(void* pos, void* n) { Insert_(R, L); }

void* Zeta_RBTree_TreeNode_(Insert)(void* pos_l, void* pos_r, void* n) {
    ZETA_DebugAssert(pos_l != n);
    ZETA_DebugAssert(pos_r != n);
    ZETA_DebugAssert(n != NULL);

    ZETA_DebugAssert(Zeta_TreeNode_(GetP)(n) == NULL);
    ZETA_DebugAssert(Zeta_TreeNode_(GetL)(n) == NULL);
    ZETA_DebugAssert(Zeta_TreeNode_(GetR)(n) == NULL);

    ZETA_DebugAssert(pos_l == NULL ||
                     Zeta_BinTree_TreeNode_(StepR)(pos_l) == pos_r);
    ZETA_DebugAssert(pos_r == NULL ||
                     Zeta_BinTree_TreeNode_(StepL)(pos_r) == pos_l);

    if (pos_l == NULL && pos_r == NULL) {
        if (Zeta_TreeNode_(GetPColor)(n) != Black) {
            Zeta_TreeNode_(SetPColor)(n, Black);
        }

        return n;
    }

    if (Zeta_TreeNode_(GetPColor)(n) != Red) {
        Zeta_TreeNode_(SetPColor)(n, Red);
    }

    if (pos_l != NULL && Zeta_TreeNode_(GetR)(pos_l) == NULL) {
        Zeta_BinTree_TreeNode_(AttatchR)(pos_l, n);
    } else {
        Zeta_BinTree_TreeNode_(AttatchL)(pos_r, n);
    }

    return Zeta_RBTree_TreeNode_(InsertBalance_)(n);
}

#define GeneralInsert_(D, E)                                               \
                                                                           \
    if (pos == NULL) {                                                     \
        return Zeta_RBTree_TreeNode_(Insert##E)(                           \
            Zeta_GetMostLink(root, Zeta_TreeNode_(Get##E)), n);            \
    }                                                                      \
                                                                           \
    ZETA_DebugAssert(root == Zeta_GetMostLink(Zeta_TreeNode_(GetP), pos)); \
                                                                           \
    return Zeta_RBTree_TreeNode_(Insert##D)(pos, n);

void* Zeta_RBTree_TreeNode_(GeneralInsertL)(void* root, void* pos, void* n) {
    GeneralInsert_(L, R);
}

void* Zeta_RBTree_TreeNode_(GeneralInsertR)(void* root, void* pos, void* n) {
    GeneralInsert_(R, L);
}

static void Zeta_RBTree_TreeNode_(ExtractBalance_)(void* n) {
    for (;;) {
        if (Zeta_TreeNode_(GetPColor)(n) == Red) {
            Zeta_TreeNode_(SetPColor)(n, Black);
            break;
        }

        void* np = Zeta_TreeNode_(GetP)(n);
        if (np == NULL) { break; }

        void* (*GetD)(void* n);
        void* (*GetE)(void* n);
        void (*RotateD)(void* n);
        void (*RotateE)(void* n);

        if (Zeta_TreeNode_(GetL)(np) == n) {
            GetD = Zeta_TreeNode_(GetL);
            GetE = Zeta_TreeNode_(GetR);
            RotateD = Zeta_BinTree_TreeNode_(RotateL);
            RotateE = Zeta_BinTree_TreeNode_(RotateR);
        } else {
            GetD = Zeta_TreeNode_(GetR);
            GetE = Zeta_TreeNode_(GetL);
            RotateD = Zeta_BinTree_TreeNode_(RotateR);
            RotateE = Zeta_BinTree_TreeNode_(RotateL);
        }

        void* ns = GetE(np);

        if (ns != NULL && Zeta_TreeNode_(GetPColor)(ns) == Red) {
            Zeta_TreeNode_(SetPColor)(np, Red);
            Zeta_TreeNode_(SetPColor)(ns, Black);
            RotateD(np);
            ns = GetE(np);
        }

        void* nsd = GetD(ns);
        void* nse = GetE(ns);

        int nse_color = nse == NULL ? Black : Zeta_TreeNode_(GetPColor)(nse);

        if ((nsd == NULL || Zeta_TreeNode_(GetPColor)(nsd) == Black) &&
            nse_color == Black) {
            Zeta_TreeNode_(SetPColor)(ns, Red);
            n = np;
            continue;
        }

        if (nse_color == Black) {
            Zeta_TreeNode_(SetPColor)(ns, Red);
            Zeta_TreeNode_(SetPColor)(nsd, Black);
            RotateE(ns);
            nse = ns;
            ns = nsd;
            nsd = GetD(nsd);
        }

        Zeta_TreeNode_(SetPColor)(ns, Zeta_TreeNode_(GetPColor)(np));
        Zeta_TreeNode_(SetPColor)(nse, Black);
        Zeta_TreeNode_(SetPColor)(np, Black);
        RotateD(np);

        break;
    }
}

void* Zeta_RBTree_TreeNode_(Extract)(void* pos) {
    ZETA_DebugAssert(pos != NULL);

    void* n = pos;

    void* root;

    void* nl = Zeta_TreeNode_(GetL)(n);
    void* nr = Zeta_TreeNode_(GetR)(n);

    if (nl != NULL && nr != NULL) {
        void* m = Zeta_GetRandom() % 2 == 0
                      ? Zeta_GetMostLink(nr, Zeta_TreeNode_(GetL))
                      : Zeta_GetMostLink(nl, Zeta_TreeNode_(GetR));

        Zeta_BinTree_TreeNode_(Swap)(n, m);

        int nc = Zeta_TreeNode_(GetPColor)(n);
        int mc = Zeta_TreeNode_(GetPColor)(m);

        if (nc != mc) {
            Zeta_TreeNode_(SetPColor)(n, mc);
            Zeta_TreeNode_(SetPColor)(m, nc);
        }

        root = Zeta_GetMostLink(m, Zeta_TreeNode_(GetP));
    } else {
        root = Zeta_GetMostLink(n, Zeta_TreeNode_(GetP));
    }

    if (Zeta_TreeNode_(GetPColor)(n) == Black) {
        nl = Zeta_TreeNode_(GetL)(n);
        nr = Zeta_TreeNode_(GetR)(n);

        if (nl != NULL) {
            Zeta_BinTree_TreeNode_(RotateR)(n);
            Zeta_TreeNode_(SetPColor)(nl, Black);
        } else if (nr != NULL) {
            Zeta_BinTree_TreeNode_(RotateL)(n);
            Zeta_TreeNode_(SetPColor)(nr, Black);
        } else {
            Zeta_RBTree_TreeNode_(ExtractBalance_)(n);
        }
    }

    root = Zeta_GetMostLink(root, Zeta_TreeNode_(GetP));
    if (root == n) { return NULL; }

    Zeta_BinTree_TreeNode_(Detach)(n);
    return root;
}

static size_t Zeta_RBTree_TreeNode_(Sanitize_)(Zeta_MemRecorder* dst_mr,
                                               void* n) {
    if (n == NULL) { return 0; }

    void* nl = Zeta_TreeNode_(GetL)(n);
    void* nr = Zeta_TreeNode_(GetR)(n);

    if (nl != NULL) { ZETA_DebugAssert(Zeta_TreeNode_(GetP)(nl) == n); }
    if (nr != NULL) { ZETA_DebugAssert(Zeta_TreeNode_(GetP)(nr) == n); }

    size_t lbh = Zeta_RBTree_TreeNode_(Sanitize_)(dst_mr, nl);

    if (dst_mr != NULL) { Zeta_MemRecorder_Record(dst_mr, n, sizeof(void*)); }

    size_t rbh = Zeta_RBTree_TreeNode_(Sanitize_)(dst_mr, nr);

    ZETA_DebugAssert(lbh == rbh);

    int nc = Zeta_TreeNode_(GetPColor)(n);

    ZETA_DebugAssert(nc == Black || nc == Red);

    if (nc == Black) { return lbh + 1; }

    ZETA_DebugAssert(nl == NULL || Zeta_TreeNode_(GetPColor)(nl) == Black);
    ZETA_DebugAssert(nr == NULL || Zeta_TreeNode_(GetPColor)(nr) == Black);

    return lbh;
}

void Zeta_RBTree_TreeNode_(Sanitize)(Zeta_MemRecorder* dst_mr, void* root) {
    if (root == NULL) { return; }

    ZETA_DebugAssert(Zeta_TreeNode_(GetP)(root) == NULL);
    ZETA_DebugAssert(Zeta_TreeNode_(GetPColor)(root) == Black);

    Zeta_RBTree_TreeNode_(Sanitize_)(dst_mr, root);
}

#pragma pop_macro("Zeta_TreeNode_")
#pragma pop_macro("Zeta_BinTree_TreeNode_")
#pragma pop_macro("Zeta_RBTree_TreeNode_")
#pragma pop_macro("Black")
#pragma pop_macro("Red")
#pragma pop_macro("Insert_")
#pragma pop_macro("F_")
#pragma pop_macro("GeneralInsert_")
