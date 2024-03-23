#include "RelCntRBTreeNode.h"

static Zeta_RelCntRBTreeNode* GetP_(void* n_) {
    Zeta_RelCntRBTreeNode* n = n_;
    ZETA_DebugAssert(n != NULL);

    void* p = ZETA_UINT_TO_PTR((ZETA_PTR_TO_UINT(n) + n->p) / 2 * 2);
    return n == p ? NULL : p;
}

static Zeta_RelCntRBTreeNode* GetL_(void* n_) {
    Zeta_RelCntRBTreeNode* n = n_;
    ZETA_DebugAssert(n != NULL);

    void* l = ZETA_UINT_TO_PTR(ZETA_PTR_TO_UINT(n) + n->l);
    return n == l ? NULL : l;
}

static Zeta_RelCntRBTreeNode* GetR_(void* n_) {
    Zeta_RelCntRBTreeNode* n = n_;
    ZETA_DebugAssert(n != NULL);

    void* r = ZETA_UINT_TO_PTR(ZETA_PTR_TO_UINT(n) + n->r);
    return n == r ? NULL : r;
}

static int GetPColor_(void* n_) {
    Zeta_RelCntRBTreeNode* n = n_;
    return n == NULL ? 0 : (ZETA_PTR_TO_UINT(n) + n->p) % 2;
}

static void SetPC_(void* n_, void* p, int c) {
    Zeta_RelCntRBTreeNode* n = n_;
    ZETA_DebugAssert(n != NULL);

    ZETA_DebugAssert(c == 0 || c == 1);

    n->p = ZETA_PTR_TO_UINT(p == NULL ? (void*)n : p) + (uintptr_t)c -
           ZETA_PTR_TO_UINT(n);
}

static void SetL_(void* n_, void* l) {
    Zeta_RelCntRBTreeNode* n = n_;
    ZETA_DebugAssert(n != NULL);

    n->l = ZETA_PTR_TO_UINT(l == NULL ? (void*)n : l) - ZETA_PTR_TO_UINT(n);
}

static void SetR_(void* n_, void* r) {
    Zeta_RelCntRBTreeNode* n = n_;
    ZETA_DebugAssert(n != NULL);

    n->r = ZETA_PTR_TO_UINT(r == NULL ? (void*)n : r) - ZETA_PTR_TO_UINT(n);
}

void Zeta_RelCntRBTreeNode_Init(void* context, void* n_) {
    ZETA_Unused(context);

    Zeta_RelCntRBTreeNode* n = n_;
    ZETA_DebugAssert(n != NULL);

    SetPC_(n, NULL, 0);
    SetL_(n, NULL);
    SetR_(n, NULL);
    n->acc_size = 0;
}

void* Zeta_RelCntRBTreeNode_GetP(void* context, void* n) {
    ZETA_Unused(context);
    return GetP_(n);
}

void* Zeta_RelCntRBTreeNode_GetL(void* context, void* n) {
    ZETA_Unused(context);
    return GetL_(n);
}

void* Zeta_RelCntRBTreeNode_GetR(void* context, void* n) {
    ZETA_Unused(context);
    return GetR_(n);
}

void Zeta_RelCntRBTreeNode_SetP(void* context, void* n, void* m) {
    ZETA_Unused(context);
    SetPC_(n, m, GetPColor_(n));
}

void Zeta_RelCntRBTreeNode_SetL(void* context, void* n, void* m) {
    ZETA_Unused(context);
    SetL_(n, m);
}

void Zeta_RelCntRBTreeNode_SetR(void* context, void* n, void* m) {
    ZETA_Unused(context);
    SetR_(n, m);
}

int Zeta_RelCntRBTreeNode_GetPColor(void* context, void* n) {
    ZETA_Unused(context);
    return GetPColor_(n);
}

void Zeta_RelCntRBTreeNode_SetPColor(void* context, void* n_, int p_color) {
    ZETA_Unused(context);

    Zeta_RelCntRBTreeNode* n = n_;
    ZETA_DebugAssert(n != NULL);

    SetPC_(n, GetP_(n), p_color);
}

size_t Zeta_RelCntRBTreeNode_GetAccSize(void* context, void* n_) {
    ZETA_Unused(context);

    Zeta_RelCntRBTreeNode* n = n_;

    return n == NULL ? 0 : n->acc_size;
}

void Zeta_RelCntRBTreeNode_SetAccSize(void* context, void* n_,
                                      size_t acc_size) {
    ZETA_Unused(context);

    Zeta_RelCntRBTreeNode* n = n_;
    ZETA_DebugAssert(n != NULL);

    n->acc_size = acc_size;
}

void Zeta_RelCntRBTreeNode_DeployBinTreeNodeOperator(
    void* context, Zeta_BinTreeNodeOperator* btn_opr) {
    ZETA_Unused(context);

    ZETA_DebugAssert(btn_opr != NULL);

    btn_opr->context = NULL;

    btn_opr->GetP = Zeta_RelCntRBTreeNode_GetP;
    btn_opr->GetL = Zeta_RelCntRBTreeNode_GetL;
    btn_opr->GetR = Zeta_RelCntRBTreeNode_GetR;

    btn_opr->SetP = Zeta_RelCntRBTreeNode_SetP;
    btn_opr->SetL = Zeta_RelCntRBTreeNode_SetL;
    btn_opr->SetR = Zeta_RelCntRBTreeNode_SetR;

    btn_opr->GetPColor = Zeta_RelCntRBTreeNode_GetPColor;
    btn_opr->SetPColor = Zeta_RelCntRBTreeNode_SetPColor;

    btn_opr->GetAccSize = Zeta_RelCntRBTreeNode_GetAccSize;
    btn_opr->SetAccSize = Zeta_RelCntRBTreeNode_SetAccSize;
}
