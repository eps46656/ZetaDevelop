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

void* Zeta_BinTree_StepL(const Zeta_BinTreeNodeAccessor* btn_accessor,
                         void* n) {
    ZETA_DebugAssert(btn_accessor != NULL);

    return StepD_(btn_accessor->context, btn_accessor->GetP, btn_accessor->GetL,
                  btn_accessor->GetR, n);
}

void* Zeta_BinTree_StepR(const Zeta_BinTreeNodeAccessor* btn_accessor,
                         void* n) {
    ZETA_DebugAssert(btn_accessor != NULL);

    return StepD_(btn_accessor->context, btn_accessor->GetP, btn_accessor->GetR,
                  btn_accessor->GetL, n);
}
