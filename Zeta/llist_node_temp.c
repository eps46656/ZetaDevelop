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
#define Zeta_LListNode_(func_name) \
    ZETA_Concat(ZETA_Concat(Zeta_LListNode, _), func_name)

void Zeta_LListNode_(Init)(void* n_) {
    Zeta_LListNode* n = n_;
    ZETA_DebugAssert(n != NULL);

#if LColor
    ZETA_ColorPtr_Set(&n->l, alignof(Zeta_LListNode), n, 0);
#else
    n->l = n;
#endif

#if RColor
    ZETA_ColorPtr_Set(&n->r, alignof(Zeta_LListNode), n, 0);
#else
    n->r = n;
#endif
}

void* Zeta_LListNode_(GetL)(void* n_) {
    Zeta_LListNode* n = n_;
    ZETA_DebugAssert(n != NULL);

#if LColor
    return ZETA_ColorPtr_GetPtr(&n->l, alignof(Zeta_LListNode));
#else
    return n->l;
#endif
}

void Zeta_LListNode_(SetL)(void* n_, void* m) {
    Zeta_LListNode* n = n_;
    ZETA_DebugAssert(n != NULL);

#if RColor
    ZETA_ColorPtr_Set(&n->l, alignof(Zeta_LListNode), m, 0);
#else
    n->l = m;
#endif
}

void* Zeta_LListNode_(GetR)(void* n_) {
    Zeta_LListNode* n = n_;
    ZETA_DebugAssert(n != NULL);

#if RColor
    return ZETA_ColorPtr_GetPtr(&n->r, alignof(Zeta_LListNode));
#else
    return n->r;
#endif
}

void Zeta_LListNode_(SetR)(void* n_, void* m) {
    Zeta_LListNode* n = n_;
    ZETA_DebugAssert(n != NULL);

#if RColor
    ZETA_ColorPtr_Set(&n->r, alignof(Zeta_LListNode), m, 0);
#else
    n->r = m;
#endif
}

#if LColor

int Zeta_LListNode_(GetLColor)(void* n_) {
    Zeta_LListNode* n = n_;
    ZETA_DebugAssert(n != NULL);

    return ZETA_ColorPtr_GetColor(&n->l, alignof(Zeta_LListNode));
}

void Zeta_LListNode_(SetLColor)(void* n_, int color) {
    Zeta_LListNode* n = n_;
    ZETA_DebugAssert(n != NULL);

    ZETA_ColorPtr_SetColor(&n->l, alignof(Zeta_LListNode), color);
}

#endif

#if RColor

int Zeta_LListNode_(GetRColor)(void* n_) {
    Zeta_LListNode* n = n_;
    ZETA_DebugAssert(n != NULL);

    return ZETA_ColorPtr_GetColor(&n->r, alignof(Zeta_LListNode));
}

void Zeta_LListNode_(SetRColor)(void* n_, int color) {
    Zeta_LListNode* n = n_;
    ZETA_DebugAssert(n != NULL);

    ZETA_ColorPtr_SetColor(&n->r, alignof(Zeta_LListNode), color);
}

#endif

#pragma pop_macro("Zeta_LListNode")
#pragma pop_macro("Zeta_LListNode_")
