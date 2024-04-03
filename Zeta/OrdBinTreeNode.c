#include "OrdBinTreeNode.h"

void Zeta_OrdBinTreeNode_Init(void* context, void* n_) {
    ZETA_Unused(context);

    Zeta_OrdBinTreeNode* n = n_;
    ZETA_DebugAssert(n != NULL);

    n->p = NULL;
    n->l = NULL;
    n->r = NULL;
}

void* Zeta_OrdBinTreeNode_GetP(void* context, void* n_) {
    ZETA_Unused(context);

    Zeta_OrdBinTreeNode* n = n_;
    ZETA_DebugAssert(n != NULL);

    return n->p;
}

void* Zeta_OrdBinTreeNode_GetL(void* context, void* n_) {
    ZETA_Unused(context);

    Zeta_OrdBinTreeNode* n = n_;
    ZETA_DebugAssert(n != NULL);

    return n->l;
}

void* Zeta_OrdBinTreeNode_GetR(void* context, void* n_) {
    ZETA_Unused(context);

    Zeta_OrdBinTreeNode* n = n_;
    ZETA_DebugAssert(n != NULL);

    return n->r;
}

void Zeta_OrdBinTreeNode_SetP(void* context, void* n, void* m) {
    ZETA_Unused(context);

    Zeta_OrdBinTreeNode* n = n_;
    ZETA_DebugAssert(n != NULL);

    n->p = m;
}

void Zeta_OrdBinTreeNode_SetL(void* context, void* n, void* m) {
    ZETA_Unused(context);

    Zeta_OrdBinTreeNode* n = n_;
    ZETA_DebugAssert(n != NULL);

    n->l = m;
}

void Zeta_OrdBinTreeNode_SetR(void* context, void* n, void* m) {
    ZETA_Unused(context);

    Zeta_OrdBinTreeNode* n = n_;
    ZETA_DebugAssert(n != NULL);

    n->r = m;
}

void Zeta_OrdBinTreeNode_DeployBinTreeNodeOperator(
    void* context, Zeta_BinTreeNodeOperator* btn_opr) {
    ZETA_Unused(context);

    ZETA_DebugAssert(btn_opr != NULL);

    btn_opr->context = NULL;

    btn_opr->GetP = Zeta_OrdBinTreeNode_GetP;
    btn_opr->GetL = Zeta_OrdBinTreeNode_GetL;
    btn_opr->GetR = Zeta_OrdBinTreeNode_GetR;

    btn_opr->SetP = Zeta_OrdBinTreeNode_SetP;
    btn_opr->SetL = Zeta_OrdBinTreeNode_SetL;
    btn_opr->SetR = Zeta_OrdBinTreeNode_SetR;
}
