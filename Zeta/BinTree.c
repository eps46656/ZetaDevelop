#include "BinTree.h"
#include "utils.h"

void *StepD_(void *(*GetP)(void *n), void *(*GetD)(void *n),
             void *(*GetE)(void *n), void *n) {
    ZETA_DEBUG_ASSERT(n != NULL);
    ZETA_DEBUG_ASSERT(GetP != NULL);
    ZETA_DEBUG_ASSERT(GetD != NULL);
    ZETA_DEBUG_ASSERT(GetE != NULL);

    void *nd = GetD(n);
    if (nd != NULL) { return Zeta_GetMostLink(GetE, nd); }

    for (;;) {
        void *np = GetP(n);
        if (np == NULL) { return NULL; }
        if (GetE(np) == n) { return np; }
        n = np;
    }
}

void *Zeta_BinTree_StepL(Zeta_BinTreeNodeAccessor *bt_accessor, void *n) {
    return StepD_(bt_accessor->GetP, bt_accessor->GetL, bt_accessor->GetR, n);
}

void *Zeta_BinTree_StepR(Zeta_BinTreeNodeAccessor *bt_accessor, void *n) {
    return StepD_(bt_accessor->GetP, bt_accessor->GetR, bt_accessor->GetL, n);
}
