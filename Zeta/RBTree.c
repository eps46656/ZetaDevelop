#include "RBTree.h"

#include "utils.h"

static void* Insert_(void* context, void* (*GetP)(void* context, void* n),
                     void* (*GetD)(void* context, void* n),
                     void* (*GetE)(void* context, void* n),
                     int (*GetColor)(void* context, void* n),
                     void (*ReverseColor)(void* context, void* n),
                     void (*AttachD)(void* context, void* n, void* m),
                     void (*AttachE)(void* context, void* n, void* m),
                     void (*RotateD)(void* context, void* n),
                     void (*RotateE)(void* context, void* n), void* n,
                     void* m) {
    if (n == NULL) {
        if (GetColor(context, m) == 1) { ReverseColor(context, m); }
        return m;
    }

    if (GetColor(context, m) == 0) { ReverseColor(context, m); }

    void* root = Zeta_GetMostLink(context, GetP, n);

    void* nd = GetD(context, n);

    if (nd == NULL) {
        AttachD(context, n, m);
    } else {
        AttachE(context, Zeta_GetMostLink(context, GetE, nd), m);
    }

    for (n = m;;) {
        void* np = GetP(context, n);

        if (np == NULL) {
            ReverseColor(context, n);
            break;
        }

        if (GetColor(context, np) == 0) { break; }

        void* ng = GetP(context, np);

        if (ng == NULL) {
            ReverseColor(context, np);
            break;
        }

        void* (*GetY)(void* context, void* n);
        void (*RotateX)(void* context, void* n);
        void (*RotateY)(void* context, void* n);

        if (GetD(context, ng) == np) {
            GetY = GetE;
            RotateX = RotateD;
            RotateY = RotateE;
        } else {
            GetY = GetD;
            RotateX = RotateE;
            RotateY = RotateD;
        }

        void* nu = GetY(context, ng);

        if (GetColor(context, nu) == 1) {
            ReverseColor(context, np);
            ReverseColor(context, nu);
            ReverseColor(context, ng);
            n = ng;
            continue;
        }

        if (GetY(context, np) == n) {
            RotateX(context, np);
            ZETA_Swap(n, np);
        }

        ReverseColor(context, np);
        ReverseColor(context, ng);
        RotateY(context, ng);
        break;
    }

    return Zeta_GetMostLink(context, GetP, root);
}

void* Zeta_RBTree_InsertL(const Zeta_RBTreeNodeOpr* rbtn_opr, void* n,
                          void* m) {
    ZETA_DebugAssert(rbtn_opr != NULL);
    ZETA_DebugAssert(rbtn_opr->GetP != NULL);
    ZETA_DebugAssert(rbtn_opr->GetL != NULL);
    ZETA_DebugAssert(rbtn_opr->GetR != NULL);
    ZETA_DebugAssert(rbtn_opr->GetColor != NULL);
    ZETA_DebugAssert(rbtn_opr->ReverseColor != NULL);
    ZETA_DebugAssert(rbtn_opr->AttachL != NULL);
    ZETA_DebugAssert(rbtn_opr->AttachR != NULL);
    ZETA_DebugAssert(rbtn_opr->RotateL != NULL);
    ZETA_DebugAssert(rbtn_opr->RotateR != NULL);
    ZETA_DebugAssert(m != NULL);
    ZETA_DebugAssert(rbtn_opr->GetP(rbtn_opr->context, m) == NULL);
    ZETA_DebugAssert(rbtn_opr->GetL(rbtn_opr->context, m) == NULL);
    ZETA_DebugAssert(rbtn_opr->GetR(rbtn_opr->context, m) == NULL);

    return Insert_(rbtn_opr->context, rbtn_opr->GetP, rbtn_opr->GetL,
                   rbtn_opr->GetR, rbtn_opr->GetColor, rbtn_opr->ReverseColor,
                   rbtn_opr->AttachL, rbtn_opr->AttachR, rbtn_opr->RotateL,
                   rbtn_opr->RotateR, n, m);
}

void* Zeta_RBTree_InsertR(const Zeta_RBTreeNodeOpr* rbtn_opr, void* n,
                          void* m) {
    ZETA_DebugAssert(rbtn_opr != NULL);
    ZETA_DebugAssert(rbtn_opr->GetP != NULL);
    ZETA_DebugAssert(rbtn_opr->GetL != NULL);
    ZETA_DebugAssert(rbtn_opr->GetR != NULL);
    ZETA_DebugAssert(rbtn_opr->GetColor != NULL);
    ZETA_DebugAssert(rbtn_opr->ReverseColor != NULL);
    ZETA_DebugAssert(rbtn_opr->AttachL != NULL);
    ZETA_DebugAssert(rbtn_opr->AttachR != NULL);
    ZETA_DebugAssert(rbtn_opr->RotateL != NULL);
    ZETA_DebugAssert(rbtn_opr->RotateR != NULL);
    ZETA_DebugAssert(m != NULL);
    ZETA_DebugAssert(rbtn_opr->GetP(rbtn_opr->context, m) == NULL);
    ZETA_DebugAssert(rbtn_opr->GetL(rbtn_opr->context, m) == NULL);
    ZETA_DebugAssert(rbtn_opr->GetR(rbtn_opr->context, m) == NULL);

    return Insert_(rbtn_opr->context, rbtn_opr->GetP, rbtn_opr->GetR,
                   rbtn_opr->GetL, rbtn_opr->GetColor, rbtn_opr->ReverseColor,
                   rbtn_opr->AttachR, rbtn_opr->AttachL, rbtn_opr->RotateR,
                   rbtn_opr->RotateL, n, m);
}

static void ExtractBalance_(const Zeta_RBTreeNodeOpr* rbtn_opr, void* n) {
    void* context = rbtn_opr->context;

    for (;;) {
        if (rbtn_opr->GetColor(context, n) == 1) {
            rbtn_opr->ReverseColor(context, n);
            break;
        }

        void* np = rbtn_opr->GetP(context, n);
        if (np == NULL) { break; }

        void* (*GetD)(void* context, void* n);
        void* (*GetE)(void* context, void* n);
        void (*RotateD)(void* context, void* n);
        void (*RotateE)(void* context, void* n);

        if (rbtn_opr->GetL(context, np) == n) {
            GetD = rbtn_opr->GetL;
            GetE = rbtn_opr->GetR;
            RotateD = rbtn_opr->RotateL;
            RotateE = rbtn_opr->RotateR;
        } else {
            GetD = rbtn_opr->GetR;
            GetE = rbtn_opr->GetL;
            RotateD = rbtn_opr->RotateR;
            RotateE = rbtn_opr->RotateL;
        }

        void* ns = GetE(context, np);

        if (rbtn_opr->GetColor(context, ns) == 1) {
            rbtn_opr->ReverseColor(context, np);
            rbtn_opr->ReverseColor(context, ns);
            RotateD(context, np);
            ns = GetE(context, np);
        }

        void* nsd = GetD(context, ns);
        void* nse = GetE(context, ns);

        if (rbtn_opr->GetColor(context, nsd) == 0 &&
            rbtn_opr->GetColor(context, nse) == 0) {
            rbtn_opr->ReverseColor(context, ns);
            n = np;
            continue;
        }

        if (rbtn_opr->GetColor(context, nse) == 0) {
            rbtn_opr->ReverseColor(context, ns);
            rbtn_opr->ReverseColor(context, nsd);
            RotateE(context, ns);
            nse = ns;
            ns = nsd;
            nsd = GetD(context, nsd);
        }

        rbtn_opr->ReverseColor(context, nse);

        if (rbtn_opr->GetColor(context, np) == 1) {
            rbtn_opr->ReverseColor(context, np);
            rbtn_opr->ReverseColor(context, ns);
        }

        RotateD(context, np);
        break;
    }
}

void* Zeta_RBTree_Extract(const Zeta_RBTreeNodeOpr* rbtn_opr, void* n) {
    ZETA_DebugAssert(n != NULL);

    void* context = rbtn_opr->context;

    void* root;

    void* nl = rbtn_opr->GetL(context, n);
    void* nr = rbtn_opr->GetR(context, n);

    if (nl != NULL && nr != NULL) {
        int ran =
            Zeta_SimpleHash(ZETA_PTR_TO_UINT(nl) ^ ZETA_PTR_TO_UINT(nr)) % 2;

        ZETA_PrintVar("%d", ran);

        void* m = ran ? Zeta_GetMostLink(context, rbtn_opr->GetL, nr)
                      : Zeta_GetMostLink(context, rbtn_opr->GetR, nl);

        rbtn_opr->Swap(context, n, m);
        root = Zeta_GetMostLink(context, rbtn_opr->GetP, m);
    } else {
        root = Zeta_GetMostLink(context, rbtn_opr->GetP, n);
    }

    if (rbtn_opr->GetColor(context, n) == 0) {
        nl = rbtn_opr->GetL(context, n);
        nr = rbtn_opr->GetR(context, n);

        if (nl != NULL) {
            rbtn_opr->RotateR(context, n);
            rbtn_opr->ReverseColor(context, nl);
        } else if (nr != NULL) {
            rbtn_opr->RotateL(context, n);
            rbtn_opr->ReverseColor(context, nr);
        } else {
            ExtractBalance_(rbtn_opr, n);
        }
    }

    root = Zeta_GetMostLink(context, rbtn_opr->GetP, root);
    if (root == n) { return NULL; }

    rbtn_opr->Detach(context, n);
    return root;
}

static size_t Check_(const Zeta_RBTreeNodeOpr* rbtn_opr, void* n) {
    if (n == NULL) { return 0; }

    void* context = rbtn_opr->context;

    void* nl = rbtn_opr->GetL(context, n);
    void* nr = rbtn_opr->GetR(context, n);

    if (nl != NULL) { ZETA_Assert(rbtn_opr->GetP(context, nl) == n); }
    if (nr != NULL) { ZETA_Assert(rbtn_opr->GetP(context, nr) == n); }

    size_t lbh = Check_(rbtn_opr, nl);
    size_t rbh = Check_(rbtn_opr, nr);

    ZETA_Assert(lbh == rbh);

    int nc = rbtn_opr->GetColor(context, n);

    if (nc == 0) { return lbh + 1; }

    ZETA_Assert(rbtn_opr->GetColor(context, nl) == 0);
    ZETA_Assert(rbtn_opr->GetColor(context, nr) == 0);

    return lbh;
}

void Zeta_RBTree_Check(const Zeta_RBTreeNodeOpr* rbtn_opr, void* n) {
    ZETA_DebugAssert(rbtn_opr != NULL);
    ZETA_DebugAssert(rbtn_opr->GetP != NULL);
    ZETA_DebugAssert(rbtn_opr->GetL != NULL);
    ZETA_DebugAssert(rbtn_opr->GetR != NULL);
    ZETA_DebugAssert(rbtn_opr->GetColor != NULL);

    if (n == NULL) { return; }

    ZETA_DebugAssert(rbtn_opr->GetP(rbtn_opr->context, n) == NULL);

    Check_(rbtn_opr, n);
}
