#include "OrdRBTreeNode.h"

static Zeta_OrdRBTreeNode* GetP_(void* n_) {
    Zeta_OrdRBTreeNode* n = n_;
    ZETA_DebugAssert(n != NULL);

    void* p = ZETA_UINT_TO_PTR(n->p / 2 * 2);
    return n == p ? NULL : p;
}

static Zeta_OrdRBTreeNode* GetL_(void* n_) {
    Zeta_OrdRBTreeNode* n = n_;
    ZETA_DebugAssert(n != NULL);

    void* l = ZETA_UINT_TO_PTR(n->l);
    return n == l ? NULL : l;
}

static Zeta_OrdRBTreeNode* GetR_(void* n_) {
    Zeta_OrdRBTreeNode* n = n_;
    ZETA_DebugAssert(n != NULL);

    void* r = ZETA_UINT_TO_PTR(n->r);
    return n == r ? NULL : r;
}

static int GetColor_(void* n_) {
    Zeta_OrdRBTreeNode* n = n_;
    return n == NULL ? 0 : n->p % 2;
}

static void SetPC_(void* n_, void* p, int c) {
    Zeta_OrdRBTreeNode* n = n_;
    ZETA_DebugAssert(n != NULL);

    ZETA_DebugAssert(c == 0 || c == 1);

    n->p = ZETA_PTR_TO_UINT(p == NULL ? (void*)n : p) + (uintptr_t)c;
}

static void SetL_(void* n_, void* l) {
    Zeta_OrdRBTreeNode* n = n_;
    ZETA_DebugAssert(n != NULL);

    n->l = ZETA_PTR_TO_UINT(l == NULL ? (void*)n : l);
}

static void SetR_(void* n_, void* r) {
    Zeta_OrdRBTreeNode* n = n_;
    ZETA_DebugAssert(n != NULL);

    n->r = ZETA_PTR_TO_UINT(r == NULL ? (void*)n : r);
}

void Zeta_OrdRBTreeNode_Init(void* context, void* n_) {
    ZETA_Unused(context);

    Zeta_OrdRBTreeNode* n = n_;
    ZETA_DebugAssert(n != NULL);

    SetPC_(n, NULL, 0);
    SetL_(n, NULL);
    SetR_(n, NULL);
}

void* Zeta_OrdRBTreeNode_GetP(void* context, void* n_) {
    ZETA_Unused(context);
    return GetP_(n_);
}

void* Zeta_OrdRBTreeNode_GetL(void* context, void* n_) {
    ZETA_Unused(context);
    return GetL_(n_);
}

void* Zeta_OrdRBTreeNode_GetR(void* context, void* n_) {
    ZETA_Unused(context);
    return GetR_(n_);
}

void Zeta_OrdRBTreeNode_SetP(void* context, void* n, void* m) {
    ZETA_Unused(context);
    SetPC_(n, m, GetColor_(n));
}

void Zeta_OrdRBTreeNode_SetL(void* context, void* n, void* m) {
    ZETA_Unused(context);
    SetL_(n, m);
}

void Zeta_OrdRBTreeNode_SetR(void* context, void* n, void* m) {
    ZETA_Unused(context);
    SetR_(n, m);
}

int Zeta_OrdRBTreeNode_GetColor(void* context, void* n_) {
    ZETA_Unused(context);
    return GetColor_(n_);
}

void Zeta_OrdRBTreeNode_SetColor(void* context, void* n_, int p_color) {
    ZETA_Unused(context);

    Zeta_OrdRBTreeNode* n = n_;
    ZETA_DebugAssert(n != NULL);

    SetPC_(n, GetP_(n), p_color);
}

void Zeta_OrdRBTreeNode_DeployBinTreeNodeOperator(
    void* context, Zeta_BinTreeNodeOperator* btn_opr) {
    ZETA_Unused(context);

    ZETA_DebugAssert(btn_opr != NULL);

    btn_opr->context = NULL;

    btn_opr->GetP = Zeta_OrdRBTreeNode_GetP;
    btn_opr->GetL = Zeta_OrdRBTreeNode_GetL;
    btn_opr->GetR = Zeta_OrdRBTreeNode_GetR;

    btn_opr->SetP = Zeta_OrdRBTreeNode_SetP;
    btn_opr->SetL = Zeta_OrdRBTreeNode_SetL;
    btn_opr->SetR = Zeta_OrdRBTreeNode_SetR;

    btn_opr->GetColor = Zeta_OrdRBTreeNode_GetColor;
    btn_opr->SetColor = Zeta_OrdRBTreeNode_SetColor;
}
