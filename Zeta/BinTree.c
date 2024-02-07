#include "BinTree.h"

#include "utils.h"

void* StepD_(void* context, void* (*GetP)(void* context, void* n),
             void* (*GetD)(void* context, void* n),
             void* (*GetE)(void* context, void* n), void* n) {
    ZETA_DebugAssert(n != NULL);
    ZETA_DebugAssert(GetP != NULL);
    ZETA_DebugAssert(GetD != NULL);
    ZETA_DebugAssert(GetE != NULL);

    void* nd = GetD(context, n);
    if (nd != NULL) { return Zeta_GetMostLink(context, GetE, nd); }

    for (;;) {
        void* np = GetP(context, n);
        if (np == NULL) { return NULL; }
        if (GetE(context, np) == n) { return np; }
        n = np;
    }
}

void* Zeta_BinTree_StepL(Zeta_BinTreeNodeAccessor* bt_accessor, void* n) {
    ZETA_DebugAssert(bt_accessor != NULL);

    return StepD_(bt_accessor->context, bt_accessor->GetP, bt_accessor->GetL,
                  bt_accessor->GetR, n);
}

void* Zeta_BinTree_StepR(Zeta_BinTreeNodeAccessor* bt_accessor, void* n) {
    ZETA_DebugAssert(bt_accessor != NULL);

    return StepD_(bt_accessor->context, bt_accessor->GetP, bt_accessor->GetR,
                  bt_accessor->GetL, n);
}

void* Zeta_BinTree_FindFirst(Zeta_BinTreeNodeAccessor* bt_accessor, void* root,
                             void* target, void* cmp_context,
                             int (*Compare)(void* cmp_context, void* x,
                                            void* y));
{
    ZETA_DebugAssert(bt_accessor != NULL);
    ZETA_DebugAssert(Compare != NULL);

    while (root != NULL) {
        int cmp_result = Compare(cmp_context, target, root);

        if (cmp_result < 0) {
            //
        }

        if (0 < cmp_result) {
            //
        }

        // return
    }
}
