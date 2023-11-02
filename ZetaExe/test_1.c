#include "../Zeta/CountingBinTree.h"
#include "../Zeta/OrdinaryCountingBinColoredTreeNode.h"
#include "../Zeta/RBTree.h"
#include "../Zeta/random.h"
#include "../Zeta/utils.h"
#include "SimpleVector.h"
#include "test_head.h"

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
    data_t data;
};

ZETA_DECL_STRUCT(GPList) {
    void* root;
    Zeta_OrdinaryCountingBinColoredTreeNode lb;
    Zeta_OrdinaryCountingBinColoredTreeNode rb;
};

void GPList_Init(GPList* gplist) {
    Zeta_OrdinaryCountingBinColoredTreeNode_Init(&gplist->lb);
    Zeta_OrdinaryCountingBinColoredTreeNode_Init(&gplist->rb);
    cbtn_opr.SetSize(&gplist->lb, 1);
    cbtn_opr.SetSize(&gplist->rb, 1);

    void* root = NULL;
    root = Zeta_RBTree_InsertR(&rbtn_opr, root, &gplist->lb);
    root = Zeta_RBTree_InsertR(&rbtn_opr, root, &gplist->rb);
    gplist->root = root;

    ZETA_PRINT_VAR("%lld", root);
}

diff_t GPList_GetSize(GPList* gplist) {
    return cbtn_opr.GetAccSize(gplist->root) - 2;
}

void* GPList_GetNode(GPList* gplist, diff_t idx) {
    diff_t size = GPList_GetSize(gplist);
    ZETA_DEBUG_ASSERT(0 <= idx && idx <= size);

    void* ret;

    Zeta_CountingBinTree_Access(&ret,          // dst_n
                                0,             // dst_idx
                                &cbtn_opr,     // cbtn_opr
                                gplist->root,  // n
                                idx + 1        // idx
    );

    return ret;
}

data_t* GPList_Access(GPList* gplist, diff_t idx) {
    diff_t size = GPList_GetSize(gplist);
    ZETA_DEBUG_ASSERT(0 <= idx && idx < size);
    void* n = GPList_GetNode(gplist, idx);
    return &ZETA_FROM_MEM(GPListNode, n, n)->data;
}

data_t* GPList_Insert(GPList* gplist, void* n) {
    void* root =
        Zeta_GetMostLink(Zeta_OrdinaryCountingBinColoredTreeNode_GetP, n);

    ZETA_DEBUG_ASSERT(gplist->root == root);
    ZETA_DEBUG_ASSERT(&gplist->lb != n);

    GPListNode* gpn = malloc(sizeof(GPListNode));
    Zeta_OrdinaryCountingBinColoredTreeNode_Init(&gpn->n);
    Zeta_OrdinaryCountingBinColoredTreeNode_SetSize(&gpn->n, 1);
    gplist->root = Zeta_RBTree_InsertL(&rbtn_opr, n, &gpn->n);

    return &gpn->data;
}

data_t* GPList_InsertWithIdx(GPList* gplist, diff_t idx) {
    return GPList_Insert(gplist, GPList_GetNode(gplist, idx));
}

void GPList_Erase(GPList* gplist, void* n) {
    void* root =
        Zeta_GetMostLink(Zeta_OrdinaryCountingBinColoredTreeNode_GetP, n);

    ZETA_DEBUG_ASSERT(gplist->root == root);
    ZETA_DEBUG_ASSERT(&gplist->lb != n);
    ZETA_DEBUG_ASSERT(&gplist->rb != n);

    gplist->root = Zeta_RBTree_Extract(&rbtn_opr, n);
    free(ZETA_FROM_MEM(GPListNode, n, n));
}

void GPList_EraseWithIdx(GPList* gplist, diff_t idx) {
    return GPList_Erase(gplist, GPList_GetNode(gplist, idx));
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

void GPList_Check(GPList* gplist) { Check_(gplist->root); }

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

diff_t GetInt(Zeta_LCGRandomGenerator* lcgrg, diff_t lb, diff_t ub) {
    diff_t len = ub - lb + 1;
    ZETA_DEBUG_ASSERT(1 <= len);

    diff_t k = Zeta_LCGRandomGenerator_Fetch(lcgrg);
    Zeta_LCGRandomGenerator_Rotate(lcgrg);
    return k % len + lb;
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

static void Compare(GPList* gplsit) {
    //
}

int main() {
    Zeta_LCGRandomGenerator rg;
    Zeta_LCGRandomGenerator_Set(&rg, 545);

    for (int i = 0; i < 100; ++i) {
        diff_t k = GetInt(&rg, -10, 10);
        printf("%lld ", k);
    }

    printf("\n");

    // -------------------------------------------------------------------------

    SimpleVector sv;
    SimpleVector_Init(&sv);

    *SimpleVector_InsertWithIdx(&sv, 0) = 0;

    SimpleVector_PrintAll(&sv);

    *SimpleVector_InsertWithIdx(&sv, 1) = 1;

    SimpleVector_PrintAll(&sv);

    *SimpleVector_InsertWithIdx(&sv, 2) = 2;

    SimpleVector_PrintAll(&sv);

    // -------------------------------------------------------------------------

    return 0;

    GPList gplist;
    GPList_Init(&gplist);

    ZETA_PRINT_VAR("%lld", GPList_GetSize(&gplist));

    for (diff_t i = 0; i < 1024; ++i) {
        *GPList_InsertWithIdx(&gplist, i) = i;
        GPList_Check(&gplist);
    }

    ZETA_PRINT_VAR("%lld", GPList_GetSize(&gplist));

    /* for (diff_t i = 0; i < 1024; ++i) {
        ZETA_PRINT_VAR("%lld", *GPList_Access(&gplist, i));
    } */

    GPList_EraseWithIdx(&gplist, 1);

    ZETA_PRINT_VAR("%lld", GPList_GetSize(&gplist));

    ZETA_PRINT_VAR("%lld", *GPList_Access(&gplist, 0));
    ZETA_PRINT_VAR("%lld", *GPList_Access(&gplist, 1));

    GPList_Check(&gplist);

    printf("ok\n");

    return 0;
}
