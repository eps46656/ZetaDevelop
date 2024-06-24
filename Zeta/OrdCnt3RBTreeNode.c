#include "OrdCnt3RBTreeNode.h"

void Zeta_OrdCnt3RBTreeNode_Init(void* context, void* n_) {
    ZETA_Unused(context);

    Zeta_OrdCnt3RBTreeNode* n = n_;
    ZETA_DebugAssert(n != NULL);

    n->p = ZETA_PtrToAddr(n);
    n->l = ZETA_PtrToAddr(n);
    n->r = ZETA_PtrToAddr(n);
    n->acc_size = 0;
}

void* Zeta_OrdCnt3RBTreeNode_GetP(void* context, void* n_) {
    ZETA_Unused(context);

    Zeta_OrdCnt3RBTreeNode* n = n_;
    ZETA_DebugAssert(n != NULL);

    void* p = ZETA_AddrToPtr(n->p / 2 * 2);

    return n == p ? NULL : p;
}

void* Zeta_OrdCnt3RBTreeNode_GetL(void* context, void* n_) {
    ZETA_Unused(context);

    Zeta_OrdCnt3RBTreeNode* n = n_;
    ZETA_DebugAssert(n != NULL);

    void* l = ZETA_AddrToPtr(n->l / 2 * 2);

    return n == l ? NULL : l;
}

void* Zeta_OrdCnt3RBTreeNode_GetR(void* context, void* n_) {
    ZETA_Unused(context);

    Zeta_OrdCnt3RBTreeNode* n = n_;
    ZETA_DebugAssert(n != NULL);

    void* r = ZETA_AddrToPtr(n->r / 2 * 2);

    return n == r ? NULL : r;
}

void Zeta_OrdCnt3RBTreeNode_SetP(void* context, void* n_, void* m) {
    ZETA_Unused(context);

    Zeta_OrdCnt3RBTreeNode* n = n_;
    ZETA_DebugAssert(n != NULL);

    n->p = ZETA_PtrToAddr(m == NULL ? (void*)n : (void*)m) + n->p % 2;
}

void Zeta_OrdCnt3RBTreeNode_SetL(void* context, void* n_, void* m) {
    ZETA_Unused(context);

    Zeta_OrdCnt3RBTreeNode* n = n_;
    ZETA_DebugAssert(n != NULL);

    n->l = ZETA_PtrToAddr(m == NULL ? (void*)n : (void*)m) + n->l % 2;
}

void Zeta_OrdCnt3RBTreeNode_SetR(void* context, void* n_, void* m) {
    ZETA_Unused(context);

    Zeta_OrdCnt3RBTreeNode* n = n_;
    ZETA_DebugAssert(n != NULL);

    n->r = ZETA_PtrToAddr(m == NULL ? (void*)n : (void*)m) + n->r % 2;
}

int Zeta_OrdCnt3RBTreeNode_GetPColor(void* context, void* n_) {
    ZETA_Unused(context);

    Zeta_OrdCnt3RBTreeNode* n = n_;

    return n == NULL ? 0 : n->p % 2;
}

int Zeta_OrdCnt3RBTreeNode_GetLColor(void* context, void* n_) {
    ZETA_Unused(context);

    Zeta_OrdCnt3RBTreeNode* n = n_;

    return n == NULL ? 0 : n->l % 2;
}

int Zeta_OrdCnt3RBTreeNode_GetRColor(void* context, void* n_) {
    ZETA_Unused(context);

    Zeta_OrdCnt3RBTreeNode* n = n_;

    return n == NULL ? 0 : n->r % 2;
}

void Zeta_OrdCnt3RBTreeNode_SetPColor(void* context, void* n_, int color) {
    ZETA_Unused(context);

    Zeta_OrdCnt3RBTreeNode* n = n_;
    ZETA_DebugAssert(n != NULL);

    ZETA_DebugAssert(color == 0 || color == 1);

    n->p = n->p / 2 * 2 + (uintptr_t)color;
}

void Zeta_OrdCnt3RBTreeNode_SetLColor(void* context, void* n_, int color) {
    ZETA_Unused(context);

    Zeta_OrdCnt3RBTreeNode* n = n_;
    ZETA_DebugAssert(n != NULL);

    ZETA_DebugAssert(color == 0 || color == 1);

    n->l = n->l / 2 * 2 + (uintptr_t)color;
}

void Zeta_OrdCnt3RBTreeNode_SetRColor(void* context, void* n_, int color) {
    ZETA_Unused(context);

    Zeta_OrdCnt3RBTreeNode* n = n_;
    ZETA_DebugAssert(n != NULL);

    ZETA_DebugAssert(color == 0 || color == 1);

    n->r = n->r / 2 * 2 + (uintptr_t)color;
}

size_t Zeta_OrdCnt3RBTreeNode_GetAccSize(void* context, void* n_) {
    ZETA_Unused(context);

    Zeta_OrdCnt3RBTreeNode* n = n_;

    return n == NULL ? 0 : n->acc_size;
}

void Zeta_OrdCnt3RBTreeNode_SetAccSize(void* context, void* n_,
                                       size_t acc_size) {
    ZETA_Unused(context);

    Zeta_OrdCnt3RBTreeNode* n = n_;
    ZETA_DebugAssert(n != NULL);

    n->acc_size = acc_size;
}

void Zeta_OrdCnt3RBTreeNode_DeployBinTreeNodeOperator(
    void* context, Zeta_BinTreeNodeOperator* btn_opr) {
    ZETA_Unused(context);

    ZETA_DebugAssert(btn_opr != NULL);

    btn_opr->context = NULL;

    btn_opr->GetP = Zeta_OrdCnt3RBTreeNode_GetP;
    btn_opr->GetL = Zeta_OrdCnt3RBTreeNode_GetL;
    btn_opr->GetR = Zeta_OrdCnt3RBTreeNode_GetR;

    btn_opr->SetP = Zeta_OrdCnt3RBTreeNode_SetP;
    btn_opr->SetL = Zeta_OrdCnt3RBTreeNode_SetL;
    btn_opr->SetR = Zeta_OrdCnt3RBTreeNode_SetR;

    btn_opr->GetColor = Zeta_OrdCnt3RBTreeNode_GetPColor;
    btn_opr->SetColor = Zeta_OrdCnt3RBTreeNode_SetPColor;

    btn_opr->GetAccSize = Zeta_OrdCnt3RBTreeNode_GetAccSize;
    btn_opr->SetAccSize = Zeta_OrdCnt3RBTreeNode_SetAccSize;
}
