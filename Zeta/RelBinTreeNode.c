#include "RelBinTreeNode.h"

static Zeta_RelBinTreeNode* GetP_(void* n_) {
    Zeta_RelBinTreeNode* n = n_;
    ZETA_DebugAssert(n != NULL);

    void* p = (void*)((uintptr_t)(void*)n + n->p);
    return n == p ? NULL : p;
}

static Zeta_RelBinTreeNode* GetL_(void* n_) {
    Zeta_RelBinTreeNode* n = n_;
    ZETA_DebugAssert(n != NULL);

    void* l = (void*)((uintptr_t)(void*)n + n->l);
    return n == l ? NULL : l;
}

static Zeta_RelBinTreeNode* GetR_(void* n_) {
    Zeta_RelBinTreeNode* n = n_;
    ZETA_DebugAssert(n != NULL);

    void* r = (void*)((uintptr_t)(void*)n + n->r);
    return n == r ? NULL : r;
}

static void SetP_(void* n_, void* p) {
    Zeta_RelBinTreeNode* n = n_;
    ZETA_DebugAssert(n != NULL);

    n->p = (uintptr_t)(p == NULL ? (void*)n : p) - (uintptr_t)(void*)n;

    ZETA_DebugAssert(p == GetP_(n));
}

static void SetL_(void* n_, void* l) {
    Zeta_RelBinTreeNode* n = n_;
    ZETA_DebugAssert(n != NULL);

    n->l = (uintptr_t)(l == NULL ? (void*)n : l) - (uintptr_t)(void*)n;

    ZETA_DebugAssert(l == GetL_(n));
}

static void SetR_(void* n_, void* r) {
    Zeta_RelBinTreeNode* n = n_;
    ZETA_DebugAssert(n != NULL);

    n->r = (uintptr_t)(r == NULL ? (void*)n : r) - (uintptr_t)(void*)n;

    ZETA_DebugAssert(r == GetR_(n));
}

void Zeta_RelBinTreeNode_Init(void* context, void* n_) {
    ZETA_Unused(context);

    Zeta_RelBinTreeNode* n = n_;
    ZETA_DebugAssert(n != NULL);

    SetP_(n, NULL);
    SetL_(n, NULL);
    SetR_(n, NULL);
}

void* Zeta_RelBinTreeNode_GetP(void* context, void* n_) {
    ZETA_Unused(context);
    return GetP_(n_);
}

void* Zeta_RelBinTreeNode_GetL(void* context, void* n_) {
    ZETA_Unused(context);
    return GetL_(n_);
}

void* Zeta_RelBinTreeNode_GetR(void* context, void* n_) {
    ZETA_Unused(context);
    return GetR_(n_);
}

void Zeta_RelBinTreeNode_SetP(void* context, void* n, void* m) {
    ZETA_Unused(context);
    SetP_(n, m);
}

void Zeta_RelBinTreeNode_SetL(void* context, void* n, void* m) {
    ZETA_Unused(context);
    SetL_(n, m);
}

void Zeta_RelBinTreeNode_SetR(void* context, void* n, void* m) {
    ZETA_Unused(context);
    SetR_(n, m);
}

void Zeta_RelBinTreeNode_DeployBinTreeNodeOperator(
    void* context, Zeta_BinTreeNodeOperator* btn_opr) {
    ZETA_Unused(context);

    ZETA_DebugAssert(btn_opr != NULL);

    btn_opr->context = NULL;

    btn_opr->GetP = Zeta_RelBinTreeNode_GetP;
    btn_opr->GetL = Zeta_RelBinTreeNode_GetL;
    btn_opr->GetR = Zeta_RelBinTreeNode_GetR;

    btn_opr->SetP = Zeta_RelBinTreeNode_SetP;
    btn_opr->SetL = Zeta_RelBinTreeNode_SetL;
    btn_opr->SetR = Zeta_RelBinTreeNode_SetR;
}
