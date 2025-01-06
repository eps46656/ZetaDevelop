#include "ord_bin_tree_node.h"

#include "debugger.h"

void Zeta_OrdBinTreeNode_Init(void const* context, void* n_) {
    ZETA_Unused(context);

    Zeta_OrdBinTreeNode* n = n_;
    ZETA_DebugAssert(n != NULL);

    n->p = NULL;
    n->l = NULL;
    n->r = NULL;
}

void* Zeta_OrdBinTreeNode_GetP(void const* context, void* n_) {
    ZETA_Unused(context);

    Zeta_OrdBinTreeNode* n = n_;
    ZETA_DebugAssert(n != NULL);

    return n->p;
}

void* Zeta_OrdBinTreeNode_GetL(void const* context, void* n_) {
    ZETA_Unused(context);

    Zeta_OrdBinTreeNode* n = n_;
    ZETA_DebugAssert(n != NULL);

    return n->l;
}

void* Zeta_OrdBinTreeNode_GetR(void const* context, void* n_) {
    ZETA_Unused(context);

    Zeta_OrdBinTreeNode* n = n_;
    ZETA_DebugAssert(n != NULL);

    return n->r;
}

void Zeta_OrdBinTreeNode_SetP(void const* context, void* n_, void* m) {
    ZETA_Unused(context);

    Zeta_OrdBinTreeNode* n = n_;
    ZETA_DebugAssert(n != NULL);

    n->p = m;
}

void Zeta_OrdBinTreeNode_SetL(void const* context, void* n_, void* m) {
    ZETA_Unused(context);

    Zeta_OrdBinTreeNode* n = n_;
    ZETA_DebugAssert(n != NULL);

    n->l = m;
}

void Zeta_OrdBinTreeNode_SetR(void const* context, void* n_, void* m) {
    ZETA_Unused(context);

    Zeta_OrdBinTreeNode* n = n_;
    ZETA_DebugAssert(n != NULL);

    n->r = m;
}

void Zeta_OrdBinTreeNode_DeployBinTreeNodeOperator(
    void const* context, Zeta_BinTreeNodeOperator* btn_opr) {
    ZETA_Unused(context);

    Zeta_BinTree_InitOpr(btn_opr);

    btn_opr->context = NULL;

    btn_opr->GetP = Zeta_OrdBinTreeNode_GetP;
    btn_opr->GetL = Zeta_OrdBinTreeNode_GetL;
    btn_opr->GetR = Zeta_OrdBinTreeNode_GetR;

    btn_opr->SetP = Zeta_OrdBinTreeNode_SetP;
    btn_opr->SetL = Zeta_OrdBinTreeNode_SetL;
    btn_opr->SetR = Zeta_OrdBinTreeNode_SetR;
}

Zeta_BinTreeNodeOperator const zeta_ord_bin_tree_node_opr = {
    .context = NULL,  //

    .GetP = Zeta_OrdBinTreeNode_GetP,  //
    .GetL = Zeta_OrdBinTreeNode_GetL,  //
    .GetR = Zeta_OrdBinTreeNode_GetR,  //

    .SetP = Zeta_OrdBinTreeNode_SetP,  //
    .SetL = Zeta_OrdBinTreeNode_SetL,  //
    .SetR = Zeta_OrdBinTreeNode_SetR,  //

    .GetColor = NULL,  //
    .SetColor = NULL,  //

    .GetAccSize = NULL,  //
    .SetAccSize = NULL,  //
};
