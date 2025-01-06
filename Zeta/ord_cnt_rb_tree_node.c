#include "ord_cnt_rb_tree_node.h"

#include "debugger.h"
#include "ptr_utils.h"

void Zeta_OrdCntRBTreeNode_Init(void const* context, void* n_) {
    ZETA_Unused(context);

    Zeta_OrdCntRBTreeNode* n = n_;
    ZETA_DebugAssert(n != NULL);

    n->p = (void*)n;
    n->l = NULL;
    n->r = NULL;
    n->acc_size = 0;
}

void* Zeta_OrdCntRBTreeNode_GetP(void const* context, void* n_) {
    ZETA_Unused(context);

    Zeta_OrdCntRBTreeNode* n = n_;
    ZETA_DebugAssert(n != NULL);

    void* p = ZETA_ColorPtr_GetPtr(&n->p, alignof(Zeta_OrdCntRBTreeNode));

    return n == p ? NULL : p;
}

void* Zeta_OrdCntRBTreeNode_GetL(void const* context, void* n_) {
    ZETA_Unused(context);

    Zeta_OrdCntRBTreeNode* n = n_;
    ZETA_DebugAssert(n != NULL);

    return n->l;
}

void* Zeta_OrdCntRBTreeNode_GetR(void const* context, void* n_) {
    ZETA_Unused(context);

    Zeta_OrdCntRBTreeNode* n = n_;
    ZETA_DebugAssert(n != NULL);

    return n->r;
}

void Zeta_OrdCntRBTreeNode_SetP(void const* context, void* n_, void* m) {
    ZETA_Unused(context);

    Zeta_OrdCntRBTreeNode* n = n_;
    ZETA_DebugAssert(n != NULL);

    ZETA_ColorPtr_SetPtr(&n->p, alignof(Zeta_OrdCntRBTreeNode),
                         m == NULL ? n : m);
}

void Zeta_OrdCntRBTreeNode_SetL(void const* context, void* n_, void* m) {
    ZETA_Unused(context);

    Zeta_OrdCntRBTreeNode* n = n_;
    ZETA_DebugAssert(n != NULL);

    n->l = m;
}

void Zeta_OrdCntRBTreeNode_SetR(void const* context, void* n_, void* m) {
    ZETA_Unused(context);

    Zeta_OrdCntRBTreeNode* n = n_;
    ZETA_DebugAssert(n != NULL);

    n->r = m;
}

int Zeta_OrdCntRBTreeNode_GetPColor(void const* context, void* n_) {
    ZETA_Unused(context);

    Zeta_OrdCntRBTreeNode* n = n_;

    return n == NULL
               ? 0
               : ZETA_ColorPtr_GetColor(&n->p, alignof(Zeta_OrdCntRBTreeNode));
}

void Zeta_OrdCntRBTreeNode_SetPColor(void const* context, void* n_, int color) {
    ZETA_Unused(context);

    Zeta_OrdCntRBTreeNode* n = n_;
    ZETA_DebugAssert(n != NULL);

    ZETA_ColorPtr_SetColor(&n->p, alignof(Zeta_OrdCntRBTreeNode), color);
}

size_t Zeta_OrdCntRBTreeNode_GetAccSize(void const* context, void* n_) {
    ZETA_Unused(context);

    Zeta_OrdCntRBTreeNode* n = n_;

    return n == NULL ? 0 : n->acc_size;
}

void Zeta_OrdCntRBTreeNode_SetAccSize(void const* context, void* n_,
                                      size_t acc_size) {
    ZETA_Unused(context);

    Zeta_OrdCntRBTreeNode* n = n_;
    ZETA_DebugAssert(n != NULL);

    n->acc_size = acc_size;
}

void Zeta_OrdCntRBTreeNode_DeployBinTreeNodeOperator(
    void const* context, Zeta_BinTreeNodeOperator* btn_opr) {
    ZETA_Unused(context);

    Zeta_BinTree_InitOpr(btn_opr);

    btn_opr->context = NULL;

    btn_opr->GetP = Zeta_OrdCntRBTreeNode_GetP;
    btn_opr->GetL = Zeta_OrdCntRBTreeNode_GetL;
    btn_opr->GetR = Zeta_OrdCntRBTreeNode_GetR;

    btn_opr->SetP = Zeta_OrdCntRBTreeNode_SetP;
    btn_opr->SetL = Zeta_OrdCntRBTreeNode_SetL;
    btn_opr->SetR = Zeta_OrdCntRBTreeNode_SetR;

    btn_opr->GetColor = Zeta_OrdCntRBTreeNode_GetPColor;
    btn_opr->SetColor = Zeta_OrdCntRBTreeNode_SetPColor;

    btn_opr->GetAccSize = Zeta_OrdCntRBTreeNode_GetAccSize;
    btn_opr->SetAccSize = Zeta_OrdCntRBTreeNode_SetAccSize;
}

Zeta_BinTreeNodeOperator const zeta_ord_cnt_rb_tree_node_opr = {
    .context = NULL,  //

    .GetP = Zeta_OrdCntRBTreeNode_GetP,  //
    .GetL = Zeta_OrdCntRBTreeNode_GetL,  //
    .GetR = Zeta_OrdCntRBTreeNode_GetR,  //

    .SetP = Zeta_OrdCntRBTreeNode_SetP,  //
    .SetL = Zeta_OrdCntRBTreeNode_SetL,  //
    .SetR = Zeta_OrdCntRBTreeNode_SetR,  //

    .GetColor = Zeta_OrdCntRBTreeNode_GetPColor,  //
    .SetColor = Zeta_OrdCntRBTreeNode_SetPColor,  //

    .GetAccSize = Zeta_OrdCntRBTreeNode_GetAccSize,  //
    .SetAccSize = Zeta_OrdCntRBTreeNode_SetAccSize,  //
};
