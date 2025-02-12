// define LListNode

#include "debugger.h"

ZETA_ExternC_Beg;

#if !defined(LListNode)
#error "LListNode is not defined."
#endif

#pragma push_macro("Zeta_LListNode")
#pragma push_macro("Zeta_LListNode_")
#pragma push_macro("Zeta_LListNode_LListNode_")

#define Zeta_LListNode ZETA_Concat(Zeta_, LListNode)

#define Zeta_LListNode_(x) ZETA_Concat(ZETA_Concat(Zeta_LListNode, _), x)

#define Zeta_LListNode_LListNode_(x) \
    ZETA_Concat(ZETA_Concat(ZETA_Concat(Zeta_LList_, LListNode), _), x)

size_t Zeta_LListNode_LListNode_(Count)(void* n) {
    if (n == NULL) { return 0; }

    size_t ret = 1;

    for (Zeta_LListNode* m = Zeta_LListNode_(GetR)(n); m != n;
         m = Zeta_LListNode_(GetR)(m)) {
        ++ret;
    }

    return ret;
}

void Zeta_LListNode_LListNode_(InsertL)(void* n, void* m) {
    ZETA_DebugAssert(n != NULL);
    ZETA_DebugAssert(m != NULL);

    ZETA_DebugAssert(Zeta_LListNode_(GetL)(m) == m);
    ZETA_DebugAssert(Zeta_LListNode_(GetR)(m) == m);

    Zeta_LListNode* nl = Zeta_LListNode_(GetL)(n);
    Zeta_LListNode* nr = n;

    Zeta_LListNode_(SetR)(nl, m);
    Zeta_LListNode_(SetL)(m, nl);

    Zeta_LListNode_(SetR)(m, nr);
    Zeta_LListNode_(SetL)(nr, m);
}

void Zeta_LListNode_LListNode_(InsertR)(void* n, void* m) {
    ZETA_DebugAssert(n != NULL);
    ZETA_DebugAssert(m != NULL);

    ZETA_DebugAssert(Zeta_LListNode_(GetL)(m) == m);
    ZETA_DebugAssert(Zeta_LListNode_(GetR)(m) == m);

    Zeta_LListNode* nl = n;
    Zeta_LListNode* nr = Zeta_LListNode_(GetR)(n);

    Zeta_LListNode_(SetR)(nl, m);
    Zeta_LListNode_(SetL)(m, nl);

    Zeta_LListNode_(SetR)(m, nr);
    Zeta_LListNode_(SetL)(nr, m);
}

void Zeta_LListNode_LListNode_(Extract)(void* n) {
    ZETA_DebugAssert(n != NULL);

    Zeta_LListNode* nl = Zeta_LListNode_(GetL)(n);
    Zeta_LListNode* nr = Zeta_LListNode_(GetR)(n);

    Zeta_LListNode_(SetR)(nl, nr);
    Zeta_LListNode_(SetL)(nr, nl);
}

void Zeta_LListNode_LListNode_(InsertSegL)(void* n, void* m_beg, void* m_end) {
    Zeta_LListNode* nl = Zeta_LListNode_(GetL)(n);
    Zeta_LListNode* nr = n;

    Zeta_LListNode* m_beg_l = Zeta_LListNode_(GetL)(m_beg);
    Zeta_LListNode* m_end_r = Zeta_LListNode_(GetR)(m_end);

    Zeta_LListNode_(SetR)(m_beg_l, m_end_r);
    Zeta_LListNode_(SetL)(m_end_r, m_beg_l);

    Zeta_LListNode_(SetR)(nl, m_beg);
    Zeta_LListNode_(SetL)(m_beg, nl);

    Zeta_LListNode_(SetR)(m_end, nr);
    Zeta_LListNode_(SetL)(nr, m_end);
}

void Zeta_LListNode_LListNode_(InsertSegR)(void* n, void* m_beg, void* m_end) {
    Zeta_LListNode* nl = n;
    Zeta_LListNode* nr = Zeta_LListNode_(GetR)(n);

    Zeta_LListNode* m_beg_l = Zeta_LListNode_(GetL)(m_beg);
    Zeta_LListNode* m_end_r = Zeta_LListNode_(GetR)(m_end);

    Zeta_LListNode_(SetR)(m_beg_l, m_end_r);
    Zeta_LListNode_(SetL)(m_end_r, m_beg_l);

    Zeta_LListNode_(SetR)(nl, m_beg);
    Zeta_LListNode_(SetL)(m_beg, nl);

    Zeta_LListNode_(SetR)(m_end, nr);
    Zeta_LListNode_(SetL)(nr, m_end);
}

void Zeta_LListNode_LListNode_(ExtractSeg)(void* n_beg, void* n_end) {
    Zeta_LListNode* n_beg_l = Zeta_LListNode_(GetL)(n_beg);
    Zeta_LListNode* n_end_r = Zeta_LListNode_(GetR)(n_end);

    Zeta_LListNode_(SetR)(n_beg_l, n_end_r);
    Zeta_LListNode_(SetL)(n_end_r, n_beg_l);
}

#pragma pop_macro("Zeta_LListNode")
#pragma pop_macro("Zeta_LListNode_")
#pragma pop_macro("Zeta_LListNode_LListNode_")

ZETA_ExternC_End;
