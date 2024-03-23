#include "OrdCntRBTreeNode.h"

static Zeta_OrdCntRBTreeNode* GetP_(void* n_) {
    Zeta_OrdCntRBTreeNode* n = n_;
    ZETA_DebugAssert(n != NULL);

    void* p = ZETA_UINT_TO_PTR(n->p / 2 * 2);
    return n == p ? NULL : p;
}

static Zeta_OrdCntRBTreeNode* GetL_(void* n_) {
    Zeta_OrdCntRBTreeNode* n = n_;
    ZETA_DebugAssert(n != NULL);

    void* l = ZETA_UINT_TO_PTR(n->l);
    return n == l ? NULL : l;
}

static Zeta_OrdCntRBTreeNode* GetR_(void* n_) {
    Zeta_OrdCntRBTreeNode* n = n_;
    ZETA_DebugAssert(n != NULL);

    void* r = ZETA_UINT_TO_PTR(n->r);
    return n == r ? NULL : r;
}

static int GetColor_(void* n_) {
    Zeta_OrdCntRBTreeNode* n = n_;
    return n == NULL ? 0 : n->p % 2;
}

static void SetPC_(void* n_, void* p, int c) {
    Zeta_OrdCntRBTreeNode* n = n_;
    ZETA_DebugAssert(n != NULL);

    ZETA_DebugAssert(c == 0 || c == 1);

    n->p = ZETA_PTR_TO_UINT(p == NULL ? (void*)n : p) + (uintptr_t)c;
}

static void SetL_(void* n_, void* l) {
    Zeta_OrdCntRBTreeNode* n = n_;
    ZETA_DebugAssert(n != NULL);

    n->l = ZETA_PTR_TO_UINT(l == NULL ? (void*)n : l);
}

static void SetR_(void* n_, void* r) {
    Zeta_OrdCntRBTreeNode* n = n_;
    ZETA_DebugAssert(n != NULL);

    n->r = ZETA_PTR_TO_UINT(r == NULL ? (void*)n : r);
}

void Zeta_OrdCntRBTreeNode_Init(void* context, void* n_) {
    ZETA_Unused(context);

    Zeta_OrdCntRBTreeNode* n = n_;
    ZETA_DebugAssert(n != NULL);

    SetPC_(n, NULL, 0);
    SetL_(n, NULL);
    SetR_(n, NULL);
    n->acc_size = 0;
}

void* Zeta_OrdCntRBTreeNode_GetP(void* context, void* n_) {
    ZETA_Unused(context);
    return GetP_(n_);
}

void* Zeta_OrdCntRBTreeNode_GetL(void* context, void* n_) {
    ZETA_Unused(context);
    return GetL_(n_);
}

void* Zeta_OrdCntRBTreeNode_GetR(void* context, void* n_) {
    ZETA_Unused(context);
    return GetR_(n_);
}

void Zeta_OrdCntRBTreeNode_SetP(void* context, void* n, void* m) {
    ZETA_Unused(context);
    SetPC_(n, m, GetColor_(n));
}

void Zeta_OrdCntRBTreeNode_SetL(void* context, void* n, void* m) {
    ZETA_Unused(context);
    SetL_(n, m);
}

void Zeta_OrdCntRBTreeNode_SetR(void* context, void* n, void* m) {
    ZETA_Unused(context);
    SetR_(n, m);
}

int Zeta_OrdCntRBTreeNode_GetColor(void* context, void* n_) {
    ZETA_Unused(context);
    return GetColor_(n_);
}

void Zeta_OrdCntRBTreeNode_SetColor(void* context, void* n_, int p_color) {
    ZETA_Unused(context);

    Zeta_OrdCntRBTreeNode* n = n_;
    ZETA_DebugAssert(n != NULL);

    SetPC_(n, GetP_(n), p_color);
}

size_t Zeta_OrdCntRBTreeNode_GetAccSize(void* context, void* n_) {
    ZETA_Unused(context);

    Zeta_OrdCntRBTreeNode* n = n_;

    return n == NULL ? 0 : n->acc_size;
}

void Zeta_OrdCntRBTreeNode_SetAccSize(void* context, void* n_,
                                      size_t acc_size) {
    ZETA_Unused(context);

    Zeta_OrdCntRBTreeNode* n = n_;
    ZETA_DebugAssert(n != NULL);

    n->acc_size = acc_size;
}

void Zeta_OrdCntRBTreeNode_DeployBinTreeNodeOperator(
    void* context, Zeta_BinTreeNodeOperator* btn_opr) {
    ZETA_Unused(context);

    ZETA_DebugAssert(btn_opr != NULL);

    btn_opr->context = NULL;

    btn_opr->GetP = Zeta_OrdCntRBTreeNode_GetP;
    btn_opr->GetL = Zeta_OrdCntRBTreeNode_GetL;
    btn_opr->GetR = Zeta_OrdCntRBTreeNode_GetR;

    btn_opr->SetP = Zeta_OrdCntRBTreeNode_SetP;
    btn_opr->SetL = Zeta_OrdCntRBTreeNode_SetL;
    btn_opr->SetR = Zeta_OrdCntRBTreeNode_SetR;

    btn_opr->GetColor = Zeta_OrdCntRBTreeNode_GetColor;
    btn_opr->SetColor = Zeta_OrdCntRBTreeNode_SetColor;

    btn_opr->GetAccSize = Zeta_OrdCntRBTreeNode_GetAccSize;
    btn_opr->SetAccSize = Zeta_OrdCntRBTreeNode_SetAccSize;
}
