#include "debugger.h"
#include "ptr_utils.h"

#if !defined(LListNode)
#error "LListNode is not defined."
#endif

#if !defined(LColor)
#error "LColor is not defined."
#endif

#if !defined(RColor)
#error "RColor is not defined."
#endif

#pragma push_macro("Zeta_LListNode")
#pragma push_macro("Zeta_LListNode_")

#define Zeta_LListNode ZETA_Concat(Zeta_, LListNode)
#define Zeta_LListNode_(x) ZETA_Concat(Zeta_LListNode, _, x)

void Zeta_LListNode_(Init)(void* n_) {
    Zeta_LListNode* n = n_;
    ZETA_DebugAssert(n != NULL);

#if defined(RelLinkType) && LColor
    ZETA_RelColorPtr_Set(&n->l, alignof(Zeta_LListNode), n, n, 0);
#elif defined(RelLinkType) && !LColor
    ZETA_RelPtr_SetPtr(&n->l, n, n);
#elif !defined(RelLinkType) && LColor
    ZETA_ColorPtr_SetPtr(&n->l, alignof(Zeta_LListNode), n);
#else
    n->l = n;
#endif

#if defined(RelLinkType) && RColor
    ZETA_RelColorPtr_Set(&n->r, alignof(Zeta_LListNode), n, n, 0);
#elif defined(RelLinkType) && !RColor
    ZETA_RelPtr_SetPtr(&n->r, n, n);
#elif !defined(RelLinkType) && RColor
    ZETA_ColorPtr_SetPtr(&n->r, alignof(Zeta_LListNode), n);
#else
    n->r = n;
#endif
}

void* Zeta_LListNode_(GetL)(void* n_) {
    Zeta_LListNode* n = n_;
    ZETA_DebugAssert(n != NULL);

#if defined(RelLinkType) && LColor
    return ZETA_RelColorPtr_GetPtr(&n->l, alignof(Zeta_LListNode), n);
#elif defined(RelLinkType) && !LColor
    return ZETA_RelPtr_GetPtr(&n->l, n);
#elif !defined(RelLinkType) && LColor
    return ZETA_ColorPtr_GetPtr(&n->l, alignof(Zeta_LListNode));
#else
    return n->l;
#endif
}

void* Zeta_LListNode_(GetR)(void* n_) {
    Zeta_LListNode* n = n_;
    ZETA_DebugAssert(n != NULL);

#if defined(RelLinkType) && RColor
    return ZETA_RelColorPtr_GetPtr(&n->r, alignof(Zeta_LListNode), n);
#elif defined(RelLinkType) && !RColor
    return ZETA_RelPtr_GetPtr(&n->r, n);
#elif !defined(RelLinkType) && RColor
    return ZETA_ColorPtr_GetPtr(&n->r, alignof(Zeta_LListNode));
#else
    return n->r;
#endif
}

void Zeta_LListNode_(SetL)(void* n_, void* m) {
    Zeta_LListNode* n = n_;
    ZETA_DebugAssert(n != NULL);

#if defined(RelLinkType) && LColor
    ZETA_RelColorPtr_SetPtr(&n->l, alignof(Zeta_LListNode), n, m);
#elif defined(RelLinkType) && !LColor
    ZETA_RelPtr_SetPtr(&n->l, n, m);
#elif !defined(RelLinkType) && LColor
    ZETA_ColorPtr_SetPtr(&n->l, alignof(Zeta_LListNode), m);
#else
    n->l = m;
#endif
}

void Zeta_LListNode_(SetR)(void* n_, void* m) {
    Zeta_LListNode* n = n_;
    ZETA_DebugAssert(n != NULL);

#if defined(RelLinkType) && RColor
    ZETA_RelColorPtr_SetPtr(&n->r, alignof(Zeta_LListNode), n, m);
#elif defined(RelLinkType) && !RColor
    ZETA_RelPtr_SetPtr(&n->r, n, m);
#elif !defined(RelLinkType) && RColor
    ZETA_ColorPtr_SetPtr(&n->r, alignof(Zeta_LListNode), m);
#else
    n->r = m;
#endif
}

#if LColor

int Zeta_LListNode_(GetLColor)(void* n_) {
    Zeta_LListNode* n = n_;
    ZETA_DebugAssert(n != NULL);

#if defined(RelLinkType)
    return ZETA_RelColorPtr_GetColor(&n->l, alignof(Zeta_LListNode), n);
#else
    return ZETA_ColorPtr_GetColor(&n->l, alignof(Zeta_LListNode));
#endif
}

void Zeta_LListNode_(SetLColor)(void* n_, int color) {
    Zeta_LListNode* n = n_;
    ZETA_DebugAssert(n != NULL);

#if defined(RelLinkType)
    ZETA_RelColorPtr_SetColor(&n->l, alignof(Zeta_LListNode), n, color);
#else
    ZETA_ColorPtr_SetColor(&n->l, alignof(Zeta_LListNode), color);
#endif
}

#endif

#if RColor

int Zeta_LListNode_(GetRColor)(void* n_) {
    Zeta_LListNode* n = n_;
    ZETA_DebugAssert(n != NULL);

#if defined(RelLinkType)
    return ZETA_RelColorPtr_GetColor(&n->r, alignof(Zeta_LListNode), n);
#else
    return ZETA_ColorPtr_GetColor(&n->r, alignof(Zeta_LListNode));
#endif
}

void Zeta_LListNode_(SetRColor)(void* n_, int color) {
    Zeta_LListNode* n = n_;
    ZETA_DebugAssert(n != NULL);

#if defined(RelLinkType)
    ZETA_RelColorPtr_SetColor(&n->r, alignof(Zeta_LListNode), n, color);
#else
    ZETA_ColorPtr_SetColor(&n->r, alignof(Zeta_LListNode), color);
#endif
}

#endif

#pragma pop_macro("Zeta_LListNode")
#pragma pop_macro("Zeta_LListNode_")
