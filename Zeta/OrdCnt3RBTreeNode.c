#include "OrdCnt3RBTreeNode.h"

#include "Debugger.h"

void Zeta_OrdCnt3RBTreeNode_Init(void* context, void* n_) {
    ZETA_Unused(context);

    Zeta_OrdCnt3RBTreeNode* n = n_;
    ZETA_DebugAssert(n != NULL);

    n->p = (void*)n;
    n->l = (void*)n;
    n->r = (void*)n;
    n->acc_size = 0;
}

void* Zeta_OrdCnt3RBTreeNode_GetP(void* context, void* n_) {
    ZETA_Unused(context);

    Zeta_OrdCnt3RBTreeNode* n = n_;
    ZETA_DebugAssert(n != NULL);

    void* p = __builtin_align_down(n->p, alignof(Zeta_OrdCnt3RBTreeNode));

    return n == p ? NULL : p;
}

void* Zeta_OrdCnt3RBTreeNode_GetL(void* context, void* n_) {
    ZETA_Unused(context);

    Zeta_OrdCnt3RBTreeNode* n = n_;
    ZETA_DebugAssert(n != NULL);

    void* l = __builtin_align_down(n->l, alignof(Zeta_OrdCnt3RBTreeNode));

    return n == l ? NULL : l;
}

void* Zeta_OrdCnt3RBTreeNode_GetR(void* context, void* n_) {
    ZETA_Unused(context);

    Zeta_OrdCnt3RBTreeNode* n = n_;
    ZETA_DebugAssert(n != NULL);

    void* r = __builtin_align_down(n->r, alignof(Zeta_OrdCnt3RBTreeNode));

    return n == r ? NULL : r;
}

void Zeta_OrdCnt3RBTreeNode_SetP(void* context, void* n_, void* m) {
    ZETA_Unused(context);

    Zeta_OrdCnt3RBTreeNode* n = n_;
    ZETA_DebugAssert(n != NULL);

    n->p = (m == NULL ? (unsigned char*)n : (unsigned char*)m) +
           (n->p - __builtin_align_down(n->p, alignof(Zeta_OrdCnt3RBTreeNode)));
}

void Zeta_OrdCnt3RBTreeNode_SetL(void* context, void* n_, void* m) {
    ZETA_Unused(context);

    Zeta_OrdCnt3RBTreeNode* n = n_;
    ZETA_DebugAssert(n != NULL);

    n->l = (m == NULL ? (unsigned char*)n : (unsigned char*)m) +
           (n->l - __builtin_align_down(n->l, alignof(Zeta_OrdCnt3RBTreeNode)));
}

void Zeta_OrdCnt3RBTreeNode_SetR(void* context, void* n_, void* m) {
    ZETA_Unused(context);

    Zeta_OrdCnt3RBTreeNode* n = n_;
    ZETA_DebugAssert(n != NULL);

    n->r = (m == NULL ? (unsigned char*)n : (unsigned char*)m) +
           (n->r - __builtin_align_down(n->r, alignof(Zeta_OrdCnt3RBTreeNode)));
}

int Zeta_OrdCnt3RBTreeNode_GetPColor(void* context, void* n_) {
    ZETA_Unused(context);

    Zeta_OrdCnt3RBTreeNode* n = n_;

    return n == NULL ? 0
                     : n->p - __builtin_align_down(
                                  n->p, alignof(Zeta_OrdCnt3RBTreeNode));
}

int Zeta_OrdCnt3RBTreeNode_GetLColor(void* context, void* n_) {
    ZETA_Unused(context);

    Zeta_OrdCnt3RBTreeNode* n = n_;

    return n == NULL ? 0
                     : n->l - __builtin_align_down(
                                  n->l, alignof(Zeta_OrdCnt3RBTreeNode));
}

int Zeta_OrdCnt3RBTreeNode_GetRColor(void* context, void* n_) {
    ZETA_Unused(context);

    Zeta_OrdCnt3RBTreeNode* n = n_;

    return n == NULL ? 0
                     : n->r - __builtin_align_down(
                                  n->r, alignof(Zeta_OrdCnt3RBTreeNode));
}

void Zeta_OrdCnt3RBTreeNode_SetPColor(void* context, void* n_, int color) {
    ZETA_Unused(context);

    Zeta_OrdCnt3RBTreeNode* n = n_;
    ZETA_DebugAssert(n != NULL);

    ZETA_DebugAssert(0 <= color &&
                     (size_t)color < alignof(Zeta_OrdCnt3RBTreeNode));

    n->p = __builtin_align_down(n->p, alignof(Zeta_OrdCnt3RBTreeNode)) + color;
}

void Zeta_OrdCnt3RBTreeNode_SetLColor(void* context, void* n_, int color) {
    ZETA_Unused(context);

    Zeta_OrdCnt3RBTreeNode* n = n_;
    ZETA_DebugAssert(n != NULL);

    ZETA_DebugAssert(0 <= color &&
                     (size_t)color < alignof(Zeta_OrdCnt3RBTreeNode));

    n->l = __builtin_align_down(n->l, alignof(Zeta_OrdCnt3RBTreeNode)) + color;
}

void Zeta_OrdCnt3RBTreeNode_SetRColor(void* context, void* n_, int color) {
    ZETA_Unused(context);

    Zeta_OrdCnt3RBTreeNode* n = n_;
    ZETA_DebugAssert(n != NULL);

    ZETA_DebugAssert(0 <= color &&
                     (size_t)color < alignof(Zeta_OrdCnt3RBTreeNode));

    n->r = __builtin_align_down(n->r, alignof(Zeta_OrdCnt3RBTreeNode)) + color;
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
