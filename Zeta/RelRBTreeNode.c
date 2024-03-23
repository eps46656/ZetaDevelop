#include "RelRBTreeNode.h"

static Zeta_RelRBTreeNode* GetP_(void* n_) {
    Zeta_RelRBTreeNode* n = n_;
    ZETA_DebugAssert(n != NULL);

    void* p = ZETA_UINT_TO_PTR((ZETA_PTR_TO_UINT(n) + n->p) / 2 * 2);
    return n == p ? NULL : p;
}

static Zeta_RelRBTreeNode* GetL_(void* n_) {
    Zeta_RelRBTreeNode* n = n_;
    ZETA_DebugAssert(n != NULL);

    void* l = ZETA_UINT_TO_PTR(ZETA_PTR_TO_UINT(n) + n->l);
    return n == l ? NULL : l;
}

static Zeta_RelRBTreeNode* GetR_(void* n_) {
    Zeta_RelRBTreeNode* n = n_;
    ZETA_DebugAssert(n != NULL);

    void* r = ZETA_UINT_TO_PTR(ZETA_PTR_TO_UINT(n) + n->r);
    return n == r ? NULL : r;
}

static int GetColor_(void* n_) {
    Zeta_RelRBTreeNode* n = n_;
    return n == NULL ? 0 : (ZETA_PTR_TO_UINT(n) + n->p) % 2;
}

static void SetPC_(void* n_, void* p, int c) {
    Zeta_RelRBTreeNode* n = n_;
    ZETA_DebugAssert(n != NULL);

    ZETA_DebugAssert(c == 0 || c == 1);

    n->p = ZETA_PTR_TO_UINT(p == NULL ? (void*)n : p) + (uintptr_t)c -
           ZETA_PTR_TO_UINT(n);
}

static void SetL_(void* n_, void* l) {
    Zeta_RelRBTreeNode* n = n_;
    ZETA_DebugAssert(n != NULL);

    n->l = ZETA_PTR_TO_UINT(l == NULL ? (void*)n : l) - ZETA_PTR_TO_UINT(n);
}

static void SetR_(void* n_, void* r) {
    Zeta_RelRBTreeNode* n = n_;
    ZETA_DebugAssert(n != NULL);

    n->r = ZETA_PTR_TO_UINT(r == NULL ? (void*)n : r) - ZETA_PTR_TO_UINT(n);
}

void Zeta_RelRBTreeNode_Init(void* context, void* n_) {
    ZETA_Unused(context);

    Zeta_RelRBTreeNode* n = n_;
    ZETA_DebugAssert(n != NULL);

    SetPC_(n, NULL, 0);
    SetL_(n, NULL);
    SetR_(n, NULL);
}

void* Zeta_RelRBTreeNode_GetP(void* context, void* n) {
    ZETA_Unused(context);
    return GetP_(n);
}

void* Zeta_RelRBTreeNode_GetL(void* context, void* n) {
    ZETA_Unused(context);
    return GetL_(n);
}

void* Zeta_RelRBTreeNode_GetR(void* context, void* n) {
    ZETA_Unused(context);
    return GetR_(n);
}

void Zeta_RelRBTreeNode_SetP(void* context, void* n, void* m) {
    ZETA_Unused(context);
    SetPC_(n, m, GetColor_(n));
}

void Zeta_RelRBTreeNode_SetL(void* context, void* n, void* m) {
    ZETA_Unused(context);
    SetL_(n, m);
}

void Zeta_RelRBTreeNode_SetR(void* context, void* n, void* m) {
    ZETA_Unused(context);
    SetR_(n, m);
}

int Zeta_RelRBTreeNode_GetColor(void* context, void* n) {
    ZETA_Unused(context);
    return GetColor_(n);
}

void Zeta_RelRBTreeNode_SetColor(void* context, void* n_, int p_color) {
    ZETA_Unused(context);

    Zeta_RelRBTreeNode* n = n_;
    ZETA_DebugAssert(n != NULL);

    SetPC_(n, GetP_(n), p_color);
}

void Zeta_RelRBTreeNode_DeployBinTreeNodeOperator(
    void* context, Zeta_BinTreeNodeOperator* btn_opr) {
    ZETA_Unused(context);

    ZETA_DebugAssert(btn_opr != NULL);

    btn_opr->context = NULL;

    btn_opr->GetP = Zeta_RelRBTreeNode_GetP;
    btn_opr->GetL = Zeta_RelRBTreeNode_GetL;
    btn_opr->GetR = Zeta_RelRBTreeNode_GetR;

    btn_opr->SetP = Zeta_RelRBTreeNode_SetP;
    btn_opr->SetL = Zeta_RelRBTreeNode_SetL;
    btn_opr->SetR = Zeta_RelRBTreeNode_SetR;

    btn_opr->GetColor = Zeta_RelRBTreeNode_GetColor;
    btn_opr->SetColor = Zeta_RelRBTreeNode_SetColor;
}
