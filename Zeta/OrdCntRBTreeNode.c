#include "OrdCntRBTreeNode.h"

#include "Debugger.h"

void Zeta_OrdCntRBTreeNode_Init(void* context, void* n_) {
    ZETA_Unused(context);

    Zeta_OrdCntRBTreeNode* n = n_;
    ZETA_DebugAssert(n != NULL);

    n->p = (void*)n;
    n->l = NULL;
    n->r = NULL;
    n->acc_size = 0;
}

void* Zeta_OrdCntRBTreeNode_GetP(void* context, void* n_) {
    ZETA_Unused(context);

    Zeta_OrdCntRBTreeNode* n = n_;
    ZETA_DebugAssert(n != NULL);

    void* p = __builtin_align_down(n->p, alignof(Zeta_OrdCntRBTreeNode));

    return n == p ? NULL : p;
}

void* Zeta_OrdCntRBTreeNode_GetL(void* context, void* n_) {
    ZETA_Unused(context);

    Zeta_OrdCntRBTreeNode* n = n_;
    ZETA_DebugAssert(n != NULL);

    return n->l;
}

void* Zeta_OrdCntRBTreeNode_GetR(void* context, void* n_) {
    ZETA_Unused(context);

    Zeta_OrdCntRBTreeNode* n = n_;
    ZETA_DebugAssert(n != NULL);

    return n->r;
}

void Zeta_OrdCntRBTreeNode_SetP(void* context, void* n_, void* m) {
    ZETA_Unused(context);

    Zeta_OrdCntRBTreeNode* n = n_;
    ZETA_DebugAssert(n != NULL);

    n->p = (m == NULL ? (unsigned char*)n : (unsigned char*)m) +
           (n->p - __builtin_align_down(n->p, alignof(Zeta_OrdCntRBTreeNode)));
}

void Zeta_OrdCntRBTreeNode_SetL(void* context, void* n_, void* m) {
    ZETA_Unused(context);

    Zeta_OrdCntRBTreeNode* n = n_;
    ZETA_DebugAssert(n != NULL);

    n->l = m;
}

void Zeta_OrdCntRBTreeNode_SetR(void* context, void* n_, void* m) {
    ZETA_Unused(context);

    Zeta_OrdCntRBTreeNode* n = n_;
    ZETA_DebugAssert(n != NULL);

    n->r = m;
}

int Zeta_OrdCntRBTreeNode_GetColor(void* context, void* n_) {
    ZETA_Unused(context);

    Zeta_OrdCntRBTreeNode* n = n_;

    return n == NULL ? 0
                     : n->p - __builtin_align_down(
                                  n->p, alignof(Zeta_OrdCntRBTreeNode));
}

void Zeta_OrdCntRBTreeNode_SetColor(void* context, void* n_, int color) {
    ZETA_Unused(context);

    Zeta_OrdCntRBTreeNode* n = n_;
    ZETA_DebugAssert(n != NULL);

    ZETA_DebugAssert(0 <= color &&
                     (size_t)color < alignof(Zeta_OrdCntRBTreeNode));

    n->p = __builtin_align_down(n->p, alignof(Zeta_OrdCntRBTreeNode)) + color;
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

    Zeta_BinTree_InitOpr(btn_opr);

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
