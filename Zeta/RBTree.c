#include "RBTree.h"
#include "utils.h"

static void *Insert_(void *(*GetP)(void *n), void *(*GetD)(void *n),
                     void *(*GetE)(void *n), int (*GetColor)(void *n),
                     void (*ReverseColor)(void *n),
                     void (*AttachD)(void *n, void *m),
                     void (*AttachE)(void *n, void *m),
                     void (*RotateD)(void *n), void (*RotateE)(void *n),
                     void *n, void *m) {
    ZETA_DEBUG_ASSERT(GetP != NULL);
    ZETA_DEBUG_ASSERT(GetD != NULL);
    ZETA_DEBUG_ASSERT(GetE != NULL);
    ZETA_DEBUG_ASSERT(GetColor != NULL);
    ZETA_DEBUG_ASSERT(ReverseColor != NULL);
    ZETA_DEBUG_ASSERT(AttachD != NULL);
    ZETA_DEBUG_ASSERT(AttachE != NULL);
    ZETA_DEBUG_ASSERT(RotateD != NULL);
    ZETA_DEBUG_ASSERT(RotateE != NULL);
    ZETA_DEBUG_ASSERT(m != NULL);
    ZETA_DEBUG_ASSERT(GetP(m) == NULL);
    ZETA_DEBUG_ASSERT(GetD(m) == NULL);
    ZETA_DEBUG_ASSERT(GetE(m) == NULL);

    if (n == NULL) {
        if (GetColor(m) == 1) { ReverseColor(m); }
        return m;
    }

    if (GetColor(m) == 0) { ReverseColor(m); }

    void *root = Zeta_GetMostLink(GetP, n);
    void *nd = GetD(n);

    if (nd == NULL) {
        AttachD(n, m);
    } else {
        AttachE(Zeta_GetMostLink(GetE, nd), m);
    }

    for (n = m;;) {
        void *np = GetP(n);

        if (np == NULL) {
            ReverseColor(n);
            break;
        }

        if (GetColor(np) == 0) { break; }

        void *ng = GetP(np);

        if (ng == NULL) {
            ReverseColor(np);
            break;
        }

        void *(*GetY)(void *n);
        void (*RotateX)(void *n);
        void (*RotateY)(void *n);

        if (GetD(ng) == np) {
            GetY = GetE;
            RotateX = RotateD;
            RotateY = RotateE;
        } else {
            GetY = GetD;
            RotateX = RotateE;
            RotateY = RotateD;
        }

        void *nu = GetY(ng);

        if (GetColor(nu) == 1) {
            ReverseColor(np);
            ReverseColor(nu);
            ReverseColor(ng);
            n = ng;
            continue;
        }

        if (GetY(np) == n) {
            RotateX(np);
            ZETA_SWAP(n, np);
        }

        ReverseColor(np);
        ReverseColor(ng);
        RotateY(ng);
        break;
    }

    return Zeta_GetMostLink(GetP, root);
}

void *Zeta_RBTree_InsertL(Zeta_RBTreeNodeOpr *rbtn_opr, void *n, void *m) {
    ZETA_DEBUG_ASSERT(rbtn_opr != NULL);

    return Insert_(rbtn_opr->GetP, rbtn_opr->GetL, rbtn_opr->GetR,
                   rbtn_opr->GetColor, rbtn_opr->ReverseColor,
                   rbtn_opr->AttachL, rbtn_opr->AttachR, rbtn_opr->RotateL,
                   rbtn_opr->RotateR, n, m);
}

void *Zeta_RBTree_InsertR(Zeta_RBTreeNodeOpr *rbtn_opr, void *n, void *m) {
    ZETA_DEBUG_ASSERT(rbtn_opr != NULL);

    return Insert_(rbtn_opr->GetP, rbtn_opr->GetR, rbtn_opr->GetL,
                   rbtn_opr->GetColor, rbtn_opr->ReverseColor,
                   rbtn_opr->AttachR, rbtn_opr->AttachL, rbtn_opr->RotateR,
                   rbtn_opr->RotateL, n, m);
}

static void ExtractBalance_(Zeta_RBTreeNodeOpr *rbtn_opr, void *n) {
    for (;;) {
        if (rbtn_opr->GetColor(n) == 1) {
            rbtn_opr->ReverseColor(n);
            break;
        }

        void *np = rbtn_opr->GetP(n);
        if (np == NULL) { break; }

        void *(*GetD)(void *n);
        void *(*GetE)(void *n);
        void (*RotateD)(void *n);
        void (*RotateE)(void *n);

        if (rbtn_opr->GetL(np) == n) {
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

        void *ns = GetE(np);

        if (rbtn_opr->GetColor(ns) == 1) {
            rbtn_opr->ReverseColor(np);
            rbtn_opr->ReverseColor(ns);
            RotateD(np);
            ns = GetE(np);
        }

        void *nsd = GetD(ns);
        void *nse = GetE(ns);

        if (rbtn_opr->GetColor(nsd) == 0 && rbtn_opr->GetColor(nse) == 0) {
            rbtn_opr->ReverseColor(ns);
            n = np;
            continue;
        }

        if (rbtn_opr->GetColor(nse) == 0) {
            rbtn_opr->ReverseColor(ns);
            rbtn_opr->ReverseColor(nsd);
            RotateE(ns);
            nse = ns;
            ns = nsd;
            nsd = GetD(nsd);
        }

        rbtn_opr->ReverseColor(nse);

        if (rbtn_opr->GetColor(np) == 1) {
            rbtn_opr->ReverseColor(np);
            rbtn_opr->ReverseColor(ns);
        }

        RotateD(np);
        break;
    }
}

void *Zeta_RBTree_Extract(Zeta_RBTreeNodeOpr *rbtn_opr, void *n) {
    ZETA_DEBUG_ASSERT(n != NULL);

    void *root;

    void *nl = rbtn_opr->GetL(n);
    void *nr = rbtn_opr->GetR(n);

    if (nl != NULL && nr != NULL) {
        int ran = Zeta_SimpleHash((size_t)nl ^ (size_t)nr) % 2;

        void *m = ran ? Zeta_GetMostLink(rbtn_opr->GetL, nr)
                      : Zeta_GetMostLink(rbtn_opr->GetR, nl);

        rbtn_opr->Swap(n, m);
        root = Zeta_GetMostLink(rbtn_opr->GetP, m);
    } else {
        root = Zeta_GetMostLink(rbtn_opr->GetP, n);
    }

    if (rbtn_opr->GetColor(n) == 0) {
        nl = rbtn_opr->GetL(n);
        nr = rbtn_opr->GetR(n);

        if (nl != NULL) {
            rbtn_opr->RotateR(n);
            rbtn_opr->ReverseColor(nl);
        } else if (nr != NULL) {
            rbtn_opr->RotateL(n);
            rbtn_opr->ReverseColor(nr);
        } else {
            ExtractBalance_(rbtn_opr, n);
        }
    }

    root = Zeta_GetMostLink(rbtn_opr->GetP, root);
    if (root == n) { return NULL; }

    rbtn_opr->Detach(n);
    return root;
}

struct Zeta_RBTreeOpr {
    void (*InsertL)(Zeta_RBTreeNodeOpr *rbtn_opr, void *n, void *m);
    void (*InsertR)(Zeta_RBTreeNodeOpr *rbtn_opr, void *n, void *m);
    void (*Extract)(Zeta_RBTreeNodeOpr *rbtn_opr, void *n);
};
