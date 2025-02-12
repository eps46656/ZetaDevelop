// defined TreeNode

// defined PColor
// defined LColor
// defined RColor

// defined AccSize

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

#if !defined(AccSize)
#error "AccSize is not defined."
#endif

#pragma push_macro("Zeta_TreeNode")
#pragma push_macro("Zeta_TreeNode_")

#define Zeta_TreeNode ZETA_Concat(Zeta_, TreeNode)
#define Zeta_TreeNode_(func_name) \
    ZETA_Concat(ZETA_Concat(Zeta_TreeNode, _), func_name)

void Zeta_TreeNode_(Init)(void* n_) {
    Zeta_TreeNode* n = n_;
    ZETA_DebugAssert(n != NULL);

#if PColor
    n->p = (void*)n;
#else
    n->p = NULL;
#endif

#if LColor
    n->l = (void*)n;
#else
    n->l = NULL;
#endif

#if RColor
    n->r = (void*)n;
#else
    n->r = NULL;
#endif

#if AccSize
    n->acc_size = 0;
#endif
}

void* Zeta_TreeNode_(GetP)(void* n_) {
    Zeta_TreeNode* n = n_;
    ZETA_DebugAssert(n != NULL);

#if PColor
    void* m = ZETA_ColorPtr_GetPtr(&n->p, alignof(Zeta_TreeNode));
    return n == m ? NULL : m;
#else
    return n->p;
#endif
}

void* Zeta_TreeNode_(GetL)(void* n_) {
    Zeta_TreeNode* n = n_;
    ZETA_DebugAssert(n != NULL);

#if LColor
    void* m = ZETA_ColorPtr_GetPtr(&n->l, alignof(Zeta_TreeNode));
    return n == m ? NULL : m;
#else
    return n->l;
#endif
}

void* Zeta_TreeNode_(GetR)(void* n_) {
    Zeta_TreeNode* n = n_;
    ZETA_DebugAssert(n != NULL);

#if RColor
    void* m = ZETA_ColorPtr_GetPtr(&n->r, alignof(Zeta_TreeNode));
    return n == m ? NULL : m;
#else
    return n->r;
#endif
}

void Zeta_TreeNode_(SetP)(void* n_, void* m) {
    Zeta_TreeNode* n = n_;
    ZETA_DebugAssert(n != NULL);

#if PColor
    ZETA_ColorPtr_SetPtr(&n->p, alignof(Zeta_TreeNode), m == NULL ? n : m);
#else
    n->p = m;
#endif
}

void Zeta_TreeNode_(SetL)(void* n_, void* m) {
    Zeta_TreeNode* n = n_;
    ZETA_DebugAssert(n != NULL);

#if LColor
    ZETA_ColorPtr_SetPtr(&n->l, alignof(Zeta_TreeNode), m == NULL ? n : m);
#else
    n->l = m;
#endif
}

void Zeta_TreeNode_(SetR)(void* n_, void* m) {
    Zeta_TreeNode* n = n_;
    ZETA_DebugAssert(n != NULL);

#if RColor
    ZETA_ColorPtr_SetPtr(&n->r, alignof(Zeta_TreeNode), m == NULL ? n : m);
#else
    n->r = m;
#endif
}

#if PColor

int Zeta_TreeNode_(GetPColor)(void* n_) {
    Zeta_TreeNode* n = n_;
    ZETA_DebugAssert(n != NULL);

    return ZETA_ColorPtr_GetColor(&n->p, alignof(Zeta_TreeNode));
}

void Zeta_TreeNode_(SetPColor)(void* n_, int color) {
    Zeta_TreeNode* n = n_;
    ZETA_DebugAssert(n != NULL);

    ZETA_ColorPtr_SetColor(&n->p, alignof(Zeta_TreeNode), color);
}

#endif

#if LColor

int Zeta_TreeNode_(GetLColor)(void* n_) {
    Zeta_TreeNode* n = n_;
    ZETA_DebugAssert(n != NULL);

    return ZETA_ColorPtr_GetColor(&n->l, alignof(Zeta_TreeNode));
}

void Zeta_TreeNode_(SetLColor)(void* n_, int color) {
    Zeta_TreeNode* n = n_;
    ZETA_DebugAssert(n != NULL);

    ZETA_ColorPtr_SetColor(&n->l, alignof(Zeta_TreeNode), color);
}

#endif

#if RColor

int Zeta_TreeNode_(GetRColor)(void* n_) {
    Zeta_TreeNode* n = n_;
    ZETA_DebugAssert(n != NULL);

    return ZETA_ColorPtr_GetColor(&n->r, alignof(Zeta_TreeNode));
}

void Zeta_TreeNode_(SetRColor)(void* n_, int color) {
    Zeta_TreeNode* n = n_;
    ZETA_DebugAssert(n != NULL);

    ZETA_ColorPtr_SetColor(&n->r, alignof(Zeta_TreeNode), color);
}

#endif

#if AccSize

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
