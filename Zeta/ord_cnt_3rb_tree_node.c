#include "ord_cnt_3rb_tree_node.h"

#include "debugger.h"
#include "ptr_utils.h"

void Zeta_OrdCnt3RBTreeNode_Init(void const* context, void* n_) {
    ZETA_Unused(context);

    Zeta_OrdCnt3RBTreeNode* n = n_;
    ZETA_DebugAssert(n != NULL);

    n->p = (void*)n;
    n->l = (void*)n;
    n->r = (void*)n;
    n->acc_size = 0;
}

void* Zeta_OrdCnt3RBTreeNode_GetP(void const* context, void* n_) {
    ZETA_Unused(context);

    Zeta_OrdCnt3RBTreeNode* n = n_;
    ZETA_DebugAssert(n != NULL);

    void* p = ZETA_ColorPtr_GetPtr(&n->p, alignof(Zeta_OrdCnt3RBTreeNode));

    return n == p ? NULL : p;
}

void* Zeta_OrdCnt3RBTreeNode_GetL(void const* context, void* n_) {
    ZETA_Unused(context);

    Zeta_OrdCnt3RBTreeNode* n = n_;
    ZETA_DebugAssert(n != NULL);

    void* l = ZETA_ColorPtr_GetPtr(&n->l, alignof(Zeta_OrdCnt3RBTreeNode));

    return n == l ? NULL : l;
}

void* Zeta_OrdCnt3RBTreeNode_GetR(void const* context, void* n_) {
    ZETA_Unused(context);

    Zeta_OrdCnt3RBTreeNode* n = n_;
    ZETA_DebugAssert(n != NULL);

    void* r = ZETA_ColorPtr_GetPtr(&n->r, alignof(Zeta_OrdCnt3RBTreeNode));

    return n == r ? NULL : r;
}

void Zeta_OrdCnt3RBTreeNode_SetP(void const* context, void* n_, void* m) {
    ZETA_Unused(context);

    Zeta_OrdCnt3RBTreeNode* n = n_;
    ZETA_DebugAssert(n != NULL);

    ZETA_ColorPtr_SetPtr(&n->p, alignof(Zeta_OrdCnt3RBTreeNode),
                         m == NULL ? n : m);
}

void Zeta_OrdCnt3RBTreeNode_SetL(void const* context, void* n_, void* m) {
    ZETA_Unused(context);

    Zeta_OrdCnt3RBTreeNode* n = n_;
    ZETA_DebugAssert(n != NULL);

    ZETA_ColorPtr_SetPtr(&n->l, alignof(Zeta_OrdCnt3RBTreeNode),
                         m == NULL ? n : m);
}

void Zeta_OrdCnt3RBTreeNode_SetR(void const* context, void* n_, void* m) {
    ZETA_Unused(context);

    Zeta_OrdCnt3RBTreeNode* n = n_;
    ZETA_DebugAssert(n != NULL);

    ZETA_ColorPtr_SetPtr(&n->r, alignof(Zeta_OrdCnt3RBTreeNode),
                         m == NULL ? n : m);
}

int Zeta_OrdCnt3RBTreeNode_GetPColor(void const* context, void* n_) {
    ZETA_Unused(context);

    Zeta_OrdCnt3RBTreeNode* n = n_;

    return n == NULL
               ? 0
               : ZETA_ColorPtr_GetColor(&n->p, alignof(Zeta_OrdCnt3RBTreeNode));
}

int Zeta_OrdCnt3RBTreeNode_GetLColor(void const* context, void* n_) {
    ZETA_Unused(context);

    Zeta_OrdCnt3RBTreeNode* n = n_;

    return n == NULL
               ? 0
               : ZETA_ColorPtr_GetColor(&n->l, alignof(Zeta_OrdCnt3RBTreeNode));
}

int Zeta_OrdCnt3RBTreeNode_GetRColor(void const* context, void* n_) {
    ZETA_Unused(context);

    Zeta_OrdCnt3RBTreeNode* n = n_;

    return n == NULL
               ? 0
               : ZETA_ColorPtr_GetColor(&n->r, alignof(Zeta_OrdCnt3RBTreeNode));
}

void Zeta_OrdCnt3RBTreeNode_SetPColor(void const* context, void* n_,
                                      int color) {
    ZETA_Unused(context);

    Zeta_OrdCnt3RBTreeNode* n = n_;
    ZETA_DebugAssert(n != NULL);

    ZETA_ColorPtr_SetColor(&n->p, alignof(Zeta_OrdCnt3RBTreeNode), color);
}

void Zeta_OrdCnt3RBTreeNode_SetLColor(void const* context, void* n_,
                                      int color) {
    ZETA_Unused(context);

    Zeta_OrdCnt3RBTreeNode* n = n_;
    ZETA_DebugAssert(n != NULL);

    ZETA_ColorPtr_SetColor(&n->l, alignof(Zeta_OrdCnt3RBTreeNode), color);
}

void Zeta_OrdCnt3RBTreeNode_SetRColor(void const* context, void* n_,
                                      int color) {
    ZETA_Unused(context);

    Zeta_OrdCnt3RBTreeNode* n = n_;
    ZETA_DebugAssert(n != NULL);

    ZETA_ColorPtr_SetColor(&n->r, alignof(Zeta_OrdCnt3RBTreeNode), color);
}

size_t Zeta_OrdCnt3RBTreeNode_GetAccSize(void const* context, void* n_) {
    ZETA_Unused(context);

    Zeta_OrdCnt3RBTreeNode* n = n_;

    return n == NULL ? 0 : n->acc_size;
}

void Zeta_OrdCnt3RBTreeNode_SetAccSize(void const* context, void* n_,
                                       size_t acc_size) {
    ZETA_Unused(context);

    Zeta_OrdCnt3RBTreeNode* n = n_;
    ZETA_DebugAssert(n != NULL);

    n->acc_size = acc_size;
}

void Zeta_OrdCnt3RBTreeNode_DeployBinTreeNodeOperator(
    void const* context, Zeta_BinTreeNodeOperator* btn_opr) {
    ZETA_Unused(context);

    Zeta_BinTree_InitOpr(btn_opr);

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

Zeta_BinTreeNodeOperator const zeta_ord_cnt_3rb_tree_node_opr = {
    .context = NULL,  //

    .GetP = Zeta_OrdCnt3RBTreeNode_GetP,  //
    .GetL = Zeta_OrdCnt3RBTreeNode_GetL,  //
    .GetR = Zeta_OrdCnt3RBTreeNode_GetR,  //

    .SetP = Zeta_OrdCnt3RBTreeNode_SetP,  //
    .SetL = Zeta_OrdCnt3RBTreeNode_SetL,  //
    .SetR = Zeta_OrdCnt3RBTreeNode_SetR,  //

    .GetColor = Zeta_OrdCnt3RBTreeNode_GetPColor,  //
    .SetColor = Zeta_OrdCnt3RBTreeNode_SetPColor,  //

    .GetAccSize = Zeta_OrdCnt3RBTreeNode_GetAccSize,  //
    .SetAccSize = Zeta_OrdCnt3RBTreeNode_SetAccSize,  //
};
