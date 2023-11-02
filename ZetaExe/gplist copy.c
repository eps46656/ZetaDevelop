#include "../Zeta/CountingBinTree.h"
#include "../Zeta/OrdinaryCountingBinColoredTreeNode.h"
#include "../Zeta/RBTree.h"
#include "../Zeta/utils.h"

typedef unsigned long long int val_t;

static Zeta_RBTreeNodeOpr rbtn_opr = {
    .GetP = Zeta_OrdinaryCountingBinColoredTreeNode_GetP,
    .GetL = Zeta_OrdinaryCountingBinColoredTreeNode_GetL,
    .GetR = Zeta_OrdinaryCountingBinColoredTreeNode_GetR,

    .GetColor = Zeta_OrdinaryCountingBinColoredTreeNode_GetColor,
    .ReverseColor = Zeta_OrdinaryCountingBinColoredTreeNode_ReverseColor,

    .AttachL = Zeta_OrdinaryCountingBinColoredTreeNode_AttachL,
    .AttachR = Zeta_OrdinaryCountingBinColoredTreeNode_AttachR,

    .Detach = Zeta_OrdinaryCountingBinColoredTreeNode_Detach,

    .Swap = Zeta_OrdinaryCountingBinColoredTreeNode_Swap,

    .RotateL = Zeta_OrdinaryCountingBinColoredTreeNode_RotateL,
    .RotateR = Zeta_OrdinaryCountingBinColoredTreeNode_RotateR,
};

static Zeta_CountingBinTreeNodeOpr cbtn_opr = {
    .GetP = Zeta_OrdinaryCountingBinColoredTreeNode_GetP,
    .GetL = Zeta_OrdinaryCountingBinColoredTreeNode_GetL,
    .GetR = Zeta_OrdinaryCountingBinColoredTreeNode_GetR,

    .GetAccSize = Zeta_OrdinaryCountingBinColoredTreeNode_GetAccSize,
    .GetSize = Zeta_OrdinaryCountingBinColoredTreeNode_GetSize,
    .SetSize = Zeta_OrdinaryCountingBinColoredTreeNode_SetSize,
};

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

ZETA_DECL_STRUCT(GPListNode) {
    Zeta_OrdinaryCountingBinColoredTreeNode n;
    val_t val;
};

ZETA_DECL_STRUCT(GPList) {
    void* root;
    Zeta_OrdinaryCountingBinColoredTreeNode lb;
    Zeta_OrdinaryCountingBinColoredTreeNode rb;
};

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

val_t GPListNode_GetVal(void* gpln_) {
    GPListNode* gpln = gpln;
    return &ZETA_FROM_MEM(GPListNode, n, gpln)->val;
}

void GPListNode_SetVal(void* gpln, val_t val) {
    GPListNode* gpln = gpln;
    ZETA_FROM_MEM(GPListNode, n, gpln)->val = val;
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

void GPList_Init(void* gpl_) {
    GPList* gpl = gpl_;

    Zeta_OrdinaryCountingBinColoredTreeNode_Init(&gpl->lb);
    Zeta_OrdinaryCountingBinColoredTreeNode_Init(&gpl->rb);
    cbtn_opr.SetSize(&gpl->lb, 1);
    cbtn_opr.SetSize(&gpl->rb, 1);

    void* root = NULL;
    root = Zeta_RBTree_InsertR(&rbtn_opr, root, &gpl->lb);
    root = Zeta_RBTree_InsertR(&rbtn_opr, root, &gpl->rb);
    gpl->root = root;
}

void* GPList_Create() {
    GPList* gpl = malloc(sizeof(GPList));
    GPList_Init(gpl);
    return gpl;
}

diff_t GPList_GetSize(void* gpl_) {
    GPList* gpl = gpl_;
    return cbtn_opr.GetAccSize(gpl->root) - 2;
}

void* GPList_GetNode(void* gpl_, diff_t idx) {
    GPList* gpl = gpl_;

    diff_t size = GPList_GetSize(gpl);
    ZETA_DEBUG_ASSERT(-1 <= idx && idx <= size);

    void* ret;

    Zeta_CountingBinTree_Access(&ret,       // dst_n
                                0,          // dst_idx
                                &cbtn_opr,  // cbtn_opr
                                gpl->root,  // n
                                idx + 1     // idx
    );

    diff_t dst_l_acc_size;
    diff_t dst_r_acc_size;

    Zeta_CountingBinTree_GetAccSize(&dst_l_acc_size,  // dst_l_acc_size
                                    &dst_r_acc_size,  // dst_r_acc_size
                                    &cbtn_opr,        // cbtn_opr
                                    ret               // n
    );

    ZETA_DEBUG_ASSERT(dst_l_acc_size == idx + 1);
    ZETA_DEBUG_ASSERT(dst_r_acc_size == size - idx);

    return ret;
}

val_t* GPList_Access(void* gpl_, diff_t idx) {
    GPList* gpl = gpl_;

    diff_t size = GPList_GetSize(gpl);
    ZETA_DEBUG_ASSERT(0 <= idx && idx < size);

    void* n = GPList_GetNode(gpl, idx);

    ZETA_DEBUG_ASSERT(
        gpl->root ==
        Zeta_GetMostLink(Zeta_OrdinaryCountingBinColoredTreeNode_GetP, n));
    ZETA_DEBUG_ASSERT(&gpl->lb != n);
    ZETA_DEBUG_ASSERT(&gpl->rb != n);

    return &ZETA_FROM_MEM(GPListNode, n, n)->val;
}

val_t GPList_GetValWithIdx(void* gpl, diff_t idx) {
    return *GPList_Access(gpl, idx);
}

void GPList_SetValWithIdx(void* gpl, diff_t idx, val_t val) {
    *GPList_Access(gpl, idx) = val;
}

val_t* GPList_InsertL(void* gpl_, void* n) {
    GPList* gpl = gpl_;

    void* root =
        Zeta_GetMostLink(Zeta_OrdinaryCountingBinColoredTreeNode_GetP, n);

    ZETA_DEBUG_ASSERT(gpl->root == root);
    ZETA_DEBUG_ASSERT(&gpl->lb != n);

    GPListNode* gpn = malloc(sizeof(GPListNode));
    Zeta_OrdinaryCountingBinColoredTreeNode_Init(&gpn->n);
    Zeta_OrdinaryCountingBinColoredTreeNode_SetSize(&gpn->n, 1);
    gpl->root = Zeta_RBTree_InsertL(&rbtn_opr, n, &gpn->n);

    return &gpn->val;
}

val_t* GPList_InsertLWithIdx(void* gpl_, diff_t idx) {
    GPList* gpl = gpl_;
    return GPList_InsertL(gpl, GPList_GetNode(gpl, idx));
}

void GPList_InsertLWithIdxWithVal(void* gpl, diff_t idx, val_t val) {
    *GPList_InsertLWithIdx(gpl, idx) = val;
}

val_t* GPList_InsertR(void* gpl_, void* n) {
    GPList* gpl = gpl_;

    void* root =
        Zeta_GetMostLink(Zeta_OrdinaryCountingBinColoredTreeNode_GetP, n);

    ZETA_DEBUG_ASSERT(gpl->root == root);
    ZETA_DEBUG_ASSERT(&gpl->rb != n);

    GPListNode* gpn = malloc(sizeof(GPListNode));
    Zeta_OrdinaryCountingBinColoredTreeNode_Init(&gpn->n);
    Zeta_OrdinaryCountingBinColoredTreeNode_SetSize(&gpn->n, 1);
    gpl->root = Zeta_RBTree_InsertR(&rbtn_opr, n, &gpn->n);

    return &gpn->val;
}

val_t* GPList_InsertRWithIdx(void* gpl_, diff_t idx) {
    GPList* gpl = gpl_;

    return GPList_InsertR(gpl, GPList_GetNode(gpl, idx));
}

void GPList_InsertRWithIdxWithVal(void* gpl, diff_t idx, val_t val) {
    *GPList_InsertRWithIdx(gpl, idx) = val;
}

void GPList_Erase(void* gpl_, void* n) {
    GPList* gpl = gpl_;

    void* root =
        Zeta_GetMostLink(Zeta_OrdinaryCountingBinColoredTreeNode_GetP, n);

    ZETA_DEBUG_ASSERT(gpl->root == root);
    ZETA_DEBUG_ASSERT(&gpl->lb != n);
    ZETA_DEBUG_ASSERT(&gpl->rb != n);

    gpl->root = Zeta_RBTree_Extract(&rbtn_opr, n);
    free(ZETA_FROM_MEM(GPListNode, n, n));
}

void GPList_EraseWithIdx(void* gpl_, diff_t idx) {
    GPList* gpl = gpl_;

    return GPList_Erase(gpl, GPList_GetNode(gpl, idx));
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

static diff_t Check_(void* n) {
    if (n == NULL) { return 0; }

    ZETA_DEBUG_ASSERT(Zeta_OrdinaryCountingBinColoredTreeNode_GetSize(n) == 1);

    void* nl = Zeta_OrdinaryCountingBinColoredTreeNode_GetL(n);
    void* nr = Zeta_OrdinaryCountingBinColoredTreeNode_GetR(n);

    if (nl != NULL) {
        ZETA_DEBUG_ASSERT(Zeta_OrdinaryCountingBinColoredTreeNode_GetP(nl) ==
                          n);
    }

    if (nr != NULL) {
        ZETA_DEBUG_ASSERT(Zeta_OrdinaryCountingBinColoredTreeNode_GetP(nr) ==
                          n);
    }

    int nc = Zeta_OrdinaryCountingBinColoredTreeNode_GetColor(n);

    if (nc == 1) {
        ZETA_DEBUG_ASSERT(
            Zeta_OrdinaryCountingBinColoredTreeNode_GetColor(nl) == 0);
        ZETA_DEBUG_ASSERT(
            Zeta_OrdinaryCountingBinColoredTreeNode_GetColor(nr) == 0);
    }

    diff_t l_bh = Check_(nl);
    diff_t r_bh = Check_(nr);

    ZETA_DEBUG_ASSERT(l_bh == r_bh);

    return nc == 1 ? l_bh : 1 + l_bh;
}

void GPList_Check(GPList* gpl) { Check_(gpl->root); }
