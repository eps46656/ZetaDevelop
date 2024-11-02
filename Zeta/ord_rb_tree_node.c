#include "ord_rb_tree_node.h"

#include "debugger.h"

void Zeta_OrdRBTreeNode_Init(void* context, void* n_) {
    ZETA_Unused(context);

    Zeta_OrdRBTreeNode* n = n_;
    ZETA_DebugAssert(n != NULL);

    n->p = (void*)n;
    n->l = NULL;
    n->r = NULL;
}

void* Zeta_OrdRBTreeNode_GetP(void* context, void* n_) {
    ZETA_Unused(context);

    Zeta_OrdRBTreeNode* n = n_;
    ZETA_DebugAssert(n != NULL);

    void* p = __builtin_align_down(n->p, alignof(Zeta_OrdRBTreeNode));

    return n == p ? NULL : p;
}

void* Zeta_OrdRBTreeNode_GetL(void* context, void* n_) {
    ZETA_Unused(context);

    Zeta_OrdRBTreeNode* n = n_;
    ZETA_DebugAssert(n != NULL);

    return n->l;
}

void* Zeta_OrdRBTreeNode_GetR(void* context, void* n_) {
    ZETA_Unused(context);

    Zeta_OrdRBTreeNode* n = n_;
    ZETA_DebugAssert(n != NULL);

    return n->r;
}

void Zeta_OrdRBTreeNode_SetP(void* context, void* n_, void* m) {
    ZETA_Unused(context);

    Zeta_OrdRBTreeNode* n = n_;
    ZETA_DebugAssert(n != NULL);

    n->p = (m == NULL ? (unsigned char*)n : (unsigned char*)m) +
           (n->p - __builtin_align_down(n->p, alignof(Zeta_OrdRBTreeNode)));
}

void Zeta_OrdRBTreeNode_SetL(void* context, void* n_, void* m) {
    ZETA_Unused(context);

    Zeta_OrdRBTreeNode* n = n_;
    ZETA_DebugAssert(n != NULL);

    n->l = m;
}

void Zeta_OrdRBTreeNode_SetR(void* context, void* n_, void* m) {
    ZETA_Unused(context);

    Zeta_OrdRBTreeNode* n = n_;
    ZETA_DebugAssert(n != NULL);

    n->r = m;
}

int Zeta_OrdRBTreeNode_GetColor(void* context, void* n_) {
    ZETA_Unused(context);

    Zeta_OrdRBTreeNode* n = n_;

    return n == NULL
               ? 0
               : n->p - __builtin_align_down(n->p, alignof(Zeta_OrdRBTreeNode));
}

void Zeta_OrdRBTreeNode_SetColor(void* context, void* n_, int color) {
    ZETA_Unused(context);

    Zeta_OrdRBTreeNode* n = n_;
    ZETA_DebugAssert(n != NULL);

    ZETA_DebugAssert(0 <= color && (size_t)color < alignof(Zeta_OrdRBTreeNode));

    n->p = __builtin_align_down(n->p, alignof(Zeta_OrdRBTreeNode)) + color;
}

void Zeta_OrdRBTreeNode_DeployBinTreeNodeOperator(
    void* context, Zeta_BinTreeNodeOperator* btn_opr) {
    ZETA_Unused(context);

    Zeta_BinTree_InitOpr(btn_opr);

    btn_opr->context = NULL;

    btn_opr->GetP = Zeta_OrdRBTreeNode_GetP;
    btn_opr->GetL = Zeta_OrdRBTreeNode_GetL;
    btn_opr->GetR = Zeta_OrdRBTreeNode_GetR;

    btn_opr->SetP = Zeta_OrdRBTreeNode_SetP;
    btn_opr->SetL = Zeta_OrdRBTreeNode_SetL;
    btn_opr->SetR = Zeta_OrdRBTreeNode_SetR;

    btn_opr->GetColor = Zeta_OrdRBTreeNode_GetColor;
    btn_opr->SetColor = Zeta_OrdRBTreeNode_SetColor;
}

Zeta_BinTreeNodeOperator const zeta_ord_rb_tree_node_opr = {
    .context = NULL,  //

    .GetP = Zeta_OrdRBTreeNode_GetP,  //
    .GetL = Zeta_OrdRBTreeNode_GetL,  //
    .GetR = Zeta_OrdRBTreeNode_GetR,  //

    .SetP = Zeta_OrdRBTreeNode_SetP,  //
    .SetL = Zeta_OrdRBTreeNode_SetL,  //
    .SetR = Zeta_OrdRBTreeNode_SetR,  //

    .GetColor = Zeta_OrdRBTreeNode_GetColor,  //
    .SetColor = Zeta_OrdRBTreeNode_SetColor,  //
}
