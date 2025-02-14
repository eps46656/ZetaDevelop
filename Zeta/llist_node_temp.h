// LListNode: The name of linked list node.

// RelLinkType: Relative addressing link type. If not defined, use directional
//              addressing.

// LColor: Enable the color bit of l link.

// RColor: Enable the color bit of r link.

#include "define.h"

ZETA_ExternC_Beg;

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

ZETA_DeclareStruct(Zeta_LListNode);

struct Zeta_LListNode {
#if defined(RelLinkType)
    RelLinkType l;
#else
    void* l;
#endif

#if defined(RelLinkType)
    RelLinkType r;
#else
    void* r;
#endif
} __attribute__((aligned(2)));

void Zeta_LListNode_(Init)(void* n);

void* Zeta_LListNode_(GetL)(void* n);
void* Zeta_LListNode_(GetR)(void* n);

void Zeta_LListNode_(SetL)(void* n, void* m);
void Zeta_LListNode_(SetR)(void* n, void* m);

#if LColor

int Zeta_LListNode_(GetLColor)(void* n);
void Zeta_LListNode_(SetLColor)(void* n, int color);

#endif

#if RColor

int Zeta_LListNode_(GetRColor)(void* n);
void Zeta_LListNode_(SetRColor)(void* n, int color);

#endif

#pragma pop_macro("Zeta_LListNode")
#pragma pop_macro("Zeta_LListNode_")

ZETA_ExternC_End;
