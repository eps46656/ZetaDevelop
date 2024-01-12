#include "OrdinaryBinColoredTreeNode.h"

static Zeta_OrdinaryBinColoredTreeNode *GetP_(void *n_) {
    Zeta_OrdinaryBinColoredTreeNode *n = n_;
    ZETA_DEBUG_ASSERT(n != NULL);
    return (intptr_t)n->p & ~(intptr_t)1;
}

static void SetP_(void *n_, void *p) {
    Zeta_OrdinaryBinColoredTreeNode *n = n_;
    ZETA_DEBUG_ASSERT(n != NULL);
    n->p = (intptr_t)p | ((intptr_t)n->p & (intptr_t)1);
}

void *Zeta_OrdinaryBinColoredTreeNode_GetP(void *context, void *n_) {
    ZETA_UNUSED(context);
    return GetP_(n_);
}

void *Zeta_OrdinaryBinColoredTreeNode_GetL(void *context, void *n_) {
    ZETA_UNUSED(context);
    Zeta_OrdinaryBinColoredTreeNode *n = n_;
    ZETA_DEBUG_ASSERT(n != NULL);
    return n->l;
}

void *Zeta_OrdinaryBinColoredTreeNode_GetR(void *context, void *n_) {
    ZETA_UNUSED(context);
    Zeta_OrdinaryBinColoredTreeNode *n = n_;
    ZETA_DEBUG_ASSERT(n != NULL);
    return n->r;
}

int Zeta_OrdinaryBinColoredTreeNode_GetColor(void *context, void *n_) {
    ZETA_UNUSED(context);
    Zeta_OrdinaryBinColoredTreeNode *n = n_;
    ZETA_DEBUG_ASSERT(n != NULL);
    return (int)n->p & 1;
}

void Zeta_OrdinaryBinColoredTreeNode_ReverseColor(void *context, void *n_) {
    ZETA_UNUSED(context);
    Zeta_OrdinaryBinColoredTreeNode *n = n_;
    ZETA_DEBUG_ASSERT(n != NULL);
    n->p = (size_t)n->p ^ (size_t)1;
}

void Zeta_OrdinaryBinColoredTreeNode_AttachL(void *context, void *n_,
                                             void *m_) {
    ZETA_UNUSED(context);

    Zeta_OrdinaryBinColoredTreeNode *n = n_;
    Zeta_OrdinaryBinColoredTreeNode *m = m_;

    ZETA_DEBUG_ASSERT(n != NULL);
    ZETA_DEBUG_ASSERT(n->l == NULL);
    ZETA_DEBUG_ASSERT(m != NULL);
    ZETA_DEBUG_ASSERT(GetP_(m) == NULL);

    n->l = m;
    SetP_(m, n);
}

void Zeta_OrdinaryBinColoredTreeNode_AttachR(void *context, void *n_,
                                             void *m_) {
    ZETA_UNUSED(context);

    Zeta_OrdinaryBinColoredTreeNode *n = n_;
    Zeta_OrdinaryBinColoredTreeNode *m = m_;

    ZETA_DEBUG_ASSERT(n != NULL);
    ZETA_DEBUG_ASSERT(n->r == NULL);
    ZETA_DEBUG_ASSERT(m != NULL);
    ZETA_DEBUG_ASSERT(GetP_(m) == NULL);

    n->r = m;
    SetP_(m, n);
}

void Zeta_OrdinaryBinColoredTreeNode_Detach(void *context, void *n_) {
    ZETA_UNUSED(context);

    Zeta_OrdinaryBinColoredTreeNode *n = n_;
    ZETA_DEBUG_ASSERT(n != NULL);

    Zeta_OrdinaryBinColoredTreeNode *np = GetP_(n);

    if (np == NULL) { return; }

    if (np->l == n) {
        np->l = NULL;
    } else {
        np->r = NULL;
    }

    SetP_(n, NULL);
}

static void Replace_(void *n_, void *m_) {
    Zeta_OrdinaryBinColoredTreeNode *n = n_;
    Zeta_OrdinaryBinColoredTreeNode *m = m_;

    ZETA_DEBUG_ASSERT(n != NULL);
    ZETA_DEBUG_ASSERT(m != NULL);

    Zeta_OrdinaryBinColoredTreeNode *np = GetP_(n);
    Zeta_OrdinaryBinColoredTreeNode *nl = n->l;
    Zeta_OrdinaryBinColoredTreeNode *nr = n->r;

    m->p = n->p;
    m->l = nl;
    m->r = nr;

    if (np != NULL) {
        if (np->l == n) {
            np->l = m;
        } else {
            np->r = m;
        }
    }

    if (nl != NULL) { SetP_(nl, m); }
    if (nr != NULL) { SetP_(nr, m); }
}

void Zeta_OrdinaryBinColoredTreeNode_Swap(void *context, void *n_, void *m_) {
    ZETA_UNUSED(context);

    Zeta_OrdinaryBinColoredTreeNode *n = n_;
    Zeta_OrdinaryBinColoredTreeNode *m = m_;

    if (n == m) { return; }

    Zeta_OrdinaryBinColoredTreeNode tmp;

    Replace_(n, &tmp);
    Replace_(m, n);
    Replace_(&tmp, m);
}

void Zeta_OrdinaryBinColoredTreeNode_RotateL(void *context, void *n_) {
    ZETA_UNUSED(context);

    Zeta_OrdinaryBinColoredTreeNode *n = n_;
    ZETA_DEBUG_ASSERT(n != NULL);

    Zeta_OrdinaryBinColoredTreeNode *nr = n->r;
    ZETA_DEBUG_ASSERT(nr != NULL);

    Zeta_OrdinaryBinColoredTreeNode *nrl = nr->l;

    Zeta_OrdinaryBinColoredTreeNode *np = GetP_(n);

    if (np != NULL) {
        if (np->l == n) {
            np->l = nr;
        } else {
            np->r = nr;
        }
    }

    SetP_(nr, np);

    nr->l = n;
    SetP_(n, nr);

    n->r = nrl;
    if (nrl != NULL) { SetP_(nrl, n); }
}

void Zeta_OrdinaryBinColoredTreeNode_RotateR(void *context, void *n_) {
    ZETA_UNUSED(context);

    Zeta_OrdinaryBinColoredTreeNode *n = n_;
    ZETA_DEBUG_ASSERT(n != NULL);

    Zeta_OrdinaryBinColoredTreeNode *nl = n->l;
    ZETA_DEBUG_ASSERT(nl != NULL);

    Zeta_OrdinaryBinColoredTreeNode *nlr = nl->r;

    Zeta_OrdinaryBinColoredTreeNode *np = GetP_(n);

    if (np != NULL) {
        if (np->l == n) {
            np->l = nl;
        } else {
            np->r = nl;
        }
    }

    SetP_(nl, np);

    nl->r = n;
    SetP_(n, nl);

    n->l = nlr;
    if (nlr != NULL) { SetP_(nlr, n); }
}
