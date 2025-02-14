// define LListNode

#include "debugger.h"

ZETA_ExternC_Beg;

#if !defined(LListNode)
#error "LListNode is not defined."
#endif

#pragma push_macro("Zeta_LListNode_")

#define Zeta_LListNode_(x) ZETA_Concat(Zeta_, LListNode, _, x)

size_t Zeta_LList_(LListNode, Count)(void* n) {
    if (n == NULL) { return 0; }

    size_t ret = 1;

    for (void* m = Zeta_LListNode_(GetR)(n); m != n;
         m = Zeta_LListNode_(GetR)(m)) {
        ++ret;
    }

    return ret;
}

void Zeta_LList_(LListNode, InsertL)(void* n, void* m) {
    ZETA_DebugAssert(n != NULL);
    ZETA_DebugAssert(m != NULL);

    ZETA_DebugAssert(Zeta_LListNode_(GetL)(m) == m);
    ZETA_DebugAssert(Zeta_LListNode_(GetR)(m) == m);

    void* nl = Zeta_LListNode_(GetL)(n);
    void* nr = n;

    Zeta_LListNode_(SetR)(nl, m);
    Zeta_LListNode_(SetL)(m, nl);

    Zeta_LListNode_(SetR)(m, nr);
    Zeta_LListNode_(SetL)(nr, m);
}

void Zeta_LList_(LListNode, InsertR)(void* n, void* m) {
    ZETA_DebugAssert(n != NULL);
    ZETA_DebugAssert(m != NULL);

    ZETA_DebugAssert(Zeta_LListNode_(GetL)(m) == m);
    ZETA_DebugAssert(Zeta_LListNode_(GetR)(m) == m);

    void* nl = n;
    void* nr = Zeta_LListNode_(GetR)(n);

    Zeta_LListNode_(SetR)(nl, m);
    Zeta_LListNode_(SetL)(m, nl);

    Zeta_LListNode_(SetR)(m, nr);
    Zeta_LListNode_(SetL)(nr, m);
}

void Zeta_LList_(LListNode, Extract)(void* n) {
    ZETA_DebugAssert(n != NULL);

    void* nl = Zeta_LListNode_(GetL)(n);
    void* nr = Zeta_LListNode_(GetR)(n);

    Zeta_LListNode_(SetR)(nl, nr);
    Zeta_LListNode_(SetL)(nr, nl);

    Zeta_LListNode_(SetR)(n, n);
    Zeta_LListNode_(SetL)(n, n);
}

void Zeta_LList_(LListNode, InsertSegL)(void* n, void* m_beg, void* m_end) {
    void* nl = Zeta_LListNode_(GetL)(n);
    void* nr = n;

    void* m_beg_l = Zeta_LListNode_(GetL)(m_beg);
    void* m_end_r = Zeta_LListNode_(GetR)(m_end);

    Zeta_LListNode_(SetR)(m_beg_l, m_end_r);
    Zeta_LListNode_(SetL)(m_end_r, m_beg_l);

    Zeta_LListNode_(SetR)(nl, m_beg);
    Zeta_LListNode_(SetL)(m_beg, nl);

    Zeta_LListNode_(SetR)(m_end, nr);
    Zeta_LListNode_(SetL)(nr, m_end);
}

void Zeta_LList_(LListNode, InsertSegR)(void* n, void* m_beg, void* m_end) {
    void* nl = n;
    void* nr = Zeta_LListNode_(GetR)(n);

    void* m_beg_l = Zeta_LListNode_(GetL)(m_beg);
    void* m_end_r = Zeta_LListNode_(GetR)(m_end);

    Zeta_LListNode_(SetR)(m_beg_l, m_end_r);
    Zeta_LListNode_(SetL)(m_end_r, m_beg_l);

    Zeta_LListNode_(SetR)(nl, m_beg);
    Zeta_LListNode_(SetL)(m_beg, nl);

    Zeta_LListNode_(SetR)(m_end, nr);
    Zeta_LListNode_(SetL)(nr, m_end);
}

void Zeta_LList_(LListNode, ExtractSeg)(void* n_beg, void* n_end) {
    void* n_beg_l = Zeta_LListNode_(GetL)(n_beg);
    void* n_end_r = Zeta_LListNode_(GetR)(n_end);

    Zeta_LListNode_(SetR)(n_beg_l, n_end_r);
    Zeta_LListNode_(SetL)(n_end_r, n_beg_l);

    Zeta_LListNode_(SetR)(n_end, n_beg);
    Zeta_LListNode_(SetL)(n_beg, n_end);
}

#pragma pop_macro("Zeta_LListNode_")

ZETA_ExternC_End;
