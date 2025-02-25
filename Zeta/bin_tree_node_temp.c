// defined TreeNode

// defined PColor
// defined LColor
// defined RColor

#include "debugger.h"
#include "ptr_utils.h"

#if !defined(TreeNode)
#error "TreeNode is not defined."
#endif

#if !defined(PColor)
#error "PColor is not defined."
#endif

#if !defined(LColor)
#error "LColor is not defined."
#endif

#if !defined(RColor)
#error "RColor is not defined."
#endif

#pragma push_macro("Zeta_TreeNode")
#pragma push_macro("Zeta_TreeNode_")

#define Zeta_TreeNode ZETA_Concat(Zeta_, TreeNode)
#define Zeta_TreeNode_(x) ZETA_Concat(Zeta_TreeNode, _, x)

void Zeta_TreeNode_(Init)(void* n_) {
    Zeta_TreeNode* n = n_;
    ZETA_DebugAssert(n != NULL);

#if defined(RelLinkType) && PColor
    ZETA_RelColorPtr_Set(&n->p, alignof(Zeta_TreeNode), n, n, 0);
#elif defined(RelLinkType) && !PColor
    ZETA_RelPtr_SetPtr(&n->p, n, n);
#elif !defined(RelLinkType) && PColor
    ZETA_ColorPtr_SetPtr(&n->p, alignof(Zeta_TreeNode), n);
#else
    n->p = NULL;
#endif

#if defined(RelLinkType) && LColor
    ZETA_RelColorPtr_Set(&n->l, alignof(Zeta_TreeNode), n, n, 0);
#elif defined(RelLinkType) && !LColor
    ZETA_RelPtr_SetPtr(&n->l, n, n);
#elif !defined(RelLinkType) && LColor
    ZETA_ColorPtr_SetPtr(&n->l, alignof(Zeta_TreeNode), n);
#else
    n->l = NULL;
#endif

#if defined(RelLinkType) && RColor
    ZETA_RelColorPtr_Set(&n->r, alignof(Zeta_TreeNode), n, n, 0);
#elif defined(RelLinkType) && !RColor
    ZETA_RelPtr_SetPtr(&n->r, n, n);
#elif !defined(RelLinkType) && RColor
    ZETA_ColorPtr_SetPtr(&n->r, alignof(Zeta_TreeNode), n);
#else
    n->r = NULL;
#endif

#if defined(AccSizeType)
    n->acc_size = 0;
#endif
}

void* Zeta_TreeNode_(GetP)(void* n_) {
    Zeta_TreeNode* n = n_;
    ZETA_DebugAssert(n != NULL);

#if defined(RelLinkType) && PColor
    void* m = ZETA_RelColorPtr_GetPtr(&n->p, alignof(Zeta_TreeNode), n);
    return m == n ? NULL : m;
#elif defined(RelLinkType) && !PColor
    void* m = ZETA_RelPtr_GetPtr(&n->p, n);
    return m == n ? NULL : m;
#elif !defined(RelLinkType) && PColor
    void* m = ZETA_ColorPtr_GetPtr(&n->p, alignof(Zeta_TreeNode));
    return m == n ? NULL : m;
#else
    return n->p;
#endif
}

void* Zeta_TreeNode_(GetL)(void* n_) {
    Zeta_TreeNode* n = n_;
    ZETA_DebugAssert(n != NULL);

#if defined(RelLinkType) && LColor
    void* m = ZETA_RelColorPtr_GetPtr(&n->l, alignof(Zeta_TreeNode), n);
    return m == n ? NULL : m;
#elif defined(RelLinkType) && !LColor
    void* m = ZETA_RelPtr_GetPtr(&n->l, n);
    return m == n ? NULL : m;
#elif !defined(RelLinkType) && LColor
    void* m = ZETA_ColorPtr_GetPtr(&n->l, alignof(Zeta_TreeNode));
    return m == n ? NULL : m;
#else
    return n->l;
#endif
}

void* Zeta_TreeNode_(GetR)(void* n_) {
    Zeta_TreeNode* n = n_;
    ZETA_DebugAssert(n != NULL);

#if defined(RelLinkType) && RColor
    void* m = ZETA_RelColorPtr_GetPtr(&n->r, alignof(Zeta_TreeNode), n);
    return m == n ? NULL : m;
#elif defined(RelLinkType) && !RColor
    void* m = ZETA_RelPtr_GetPtr(&n->r, n);
    return m == n ? NULL : m;
#elif !defined(RelLinkType) && RColor
    void* m = ZETA_ColorPtr_GetPtr(&n->r, alignof(Zeta_TreeNode));
    return m == n ? NULL : m;
#else
    return n->r;
#endif
}

void Zeta_TreeNode_(SetP)(void* n_, void* m) {
    Zeta_TreeNode* n = n_;
    ZETA_DebugAssert(n != NULL);

#if defined(RelLinkType) && PColor
    ZETA_RelColorPtr_SetPtr(&n->p, alignof(Zeta_TreeNode), n,
                            m == NULL ? n : m);
#elif defined(RelLinkType) && !PColor
    ZETA_RelPtr_SetPtr(&n->p, n, m == NULL ? n : m);
#elif !defined(RelLinkType) && PColor
    ZETA_ColorPtr_SetPtr(&n->p, alignof(Zeta_TreeNode), m == NULL ? n : m);
#else
    n->p = m;
#endif
}

void Zeta_TreeNode_(SetL)(void* n_, void* m) {
    Zeta_TreeNode* n = n_;
    ZETA_DebugAssert(n != NULL);

#if defined(RelLinkType) && LColor
    ZETA_RelColorPtr_SetPtr(&n->l, alignof(Zeta_TreeNode), n,
                            m == NULL ? n : m);
#elif defined(RelLinkType) && !LColor
    ZETA_RelPtr_SetPtr(&n->l, n, m == NULL ? n : m);
#elif !defined(RelLinkType) && LColor
    ZETA_ColorPtr_SetPtr(&n->l, alignof(Zeta_TreeNode), m == NULL ? n : m);
#else
    n->l = m;
#endif
}

void Zeta_TreeNode_(SetR)(void* n_, void* m) {
    Zeta_TreeNode* n = n_;
    ZETA_DebugAssert(n != NULL);

#if defined(RelLinkType) && RColor
    ZETA_RelColorPtr_SetPtr(&n->r, alignof(Zeta_TreeNode), n,
                            m == NULL ? n : m);
#elif defined(RelLinkType) && !RColor
    ZETA_RelPtr_SetPtr(&n->r, n, m == NULL ? n : m);
#elif !defined(RelLinkType) && RColor
    ZETA_ColorPtr_SetPtr(&n->r, alignof(Zeta_TreeNode), m == NULL ? n : m);
#else
    n->r = m;
#endif
}

#if PColor

int Zeta_TreeNode_(GetPColor)(void* n_) {
    Zeta_TreeNode* n = n_;
    ZETA_DebugAssert(n != NULL);

#if defined(RelLinkType)
    return ZETA_RelColorPtr_GetColor(&n->p, alignof(Zeta_TreeNode), n);
#else
    return ZETA_ColorPtr_GetColor(&n->p, alignof(Zeta_TreeNode));
#endif
}

void Zeta_TreeNode_(SetPColor)(void* n_, int color) {
    Zeta_TreeNode* n = n_;
    ZETA_DebugAssert(n != NULL);

#if defined(RelLinkType)
    ZETA_RelColorPtr_SetColor(&n->p, alignof(Zeta_TreeNode), n, color);
#else
    ZETA_ColorPtr_SetColor(&n->p, alignof(Zeta_TreeNode), color);
#endif
}

#endif

#if LColor

int Zeta_TreeNode_(GetLColor)(void* n_) {
    Zeta_TreeNode* n = n_;
    ZETA_DebugAssert(n != NULL);

#if defined(RelLinkType)
    return ZETA_RelColorPtr_GetColor(&n->l, alignof(Zeta_TreeNode), n);
#else
    return ZETA_ColorPtr_GetColor(&n->l, alignof(Zeta_TreeNode));
#endif
}

void Zeta_TreeNode_(SetLColor)(void* n_, int color) {
    Zeta_TreeNode* n = n_;
    ZETA_DebugAssert(n != NULL);

#if defined(RelLinkType)
    ZETA_RelColorPtr_SetColor(&n->l, alignof(Zeta_TreeNode), n, color);
#else
    ZETA_ColorPtr_SetColor(&n->l, alignof(Zeta_TreeNode), color);
#endif
}

#endif

#if RColor

int Zeta_TreeNode_(GetRColor)(void* n_) {
    Zeta_TreeNode* n = n_;
    ZETA_DebugAssert(n != NULL);

#if defined(RelLinkType)
    return ZETA_RelColorPtr_GetColor(&n->r, alignof(Zeta_TreeNode), n);
#else
    return ZETA_ColorPtr_GetColor(&n->r, alignof(Zeta_TreeNode));
#endif
}

void Zeta_TreeNode_(SetRColor)(void* n_, int color) {
    Zeta_TreeNode* n = n_;
    ZETA_DebugAssert(n != NULL);

#if defined(RelLinkType)
    ZETA_RelColorPtr_SetColor(&n->r, alignof(Zeta_TreeNode), n, color);
#else
    ZETA_ColorPtr_SetColor(&n->r, alignof(Zeta_TreeNode), color);
#endif
}

#endif

#if defined(AccSizeType)

size_t Zeta_TreeNode_(GetAccSize)(void* n_) {
    Zeta_TreeNode* n = n_;

    return n == NULL ? 0 : n->acc_size;
}

void Zeta_TreeNode_(SetAccSize)(void* n_, size_t acc_size) {
    Zeta_TreeNode* n = n_;
    ZETA_DebugAssert(n != NULL);

    n->acc_size = acc_size;
}

#endif

#pragma pop_macro("Zeta_TreeNode")
#pragma pop_macro("Zeta_TreeNode_")
