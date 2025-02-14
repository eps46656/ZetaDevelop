// TreeNode: The name of tree node.

// RelLinkType: Relative addressing integer type. If not defined, use ordinary
//              addressing mode.

// PColor: Boolean. Enable if color bit at p link.

// LColor: Boolean. Enable if color bit at l link.

// RColor: Boolean. Enable if color bit at r link.

// AccSizeType: Accumulation size integer type. If not defined, not enable
//              accumulation size.

#include "define.h"

ZETA_ExternC_Beg;

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

ZETA_DeclareStruct(Zeta_TreeNode);

struct Zeta_TreeNode {
#if defined(RelLinkType)
    RelLinkType p;
#else
    void* p;
#endif

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

#if defined(AccSizeType)
    AccSizeType acc_size;
#endif
} __attribute__((aligned(2)));

void Zeta_TreeNode_(Init)(void* n);

void* Zeta_TreeNode_(GetP)(void* n);
void* Zeta_TreeNode_(GetL)(void* n);
void* Zeta_TreeNode_(GetR)(void* n);

void Zeta_TreeNode_(SetP)(void* n, void* m);
void Zeta_TreeNode_(SetL)(void* n, void* m);
void Zeta_TreeNode_(SetR)(void* n, void* m);

#if PColor

int Zeta_TreeNode_(GetPColor)(void* n);
void Zeta_TreeNode_(SetPColor)(void* n, int color);

#endif

#if LColor

int Zeta_TreeNode_(GetLColor)(void* n);
void Zeta_TreeNode_(SetLColor)(void* n, int color);

#endif

#if RColor

int Zeta_TreeNode_(GetRColor)(void* n);
void Zeta_TreeNode_(SetRColor)(void* n, int color);

#endif

#if defined(AccSizeType)

size_t Zeta_TreeNode_(GetAccSize)(void* n);
void Zeta_TreeNode_(SetAccSize)(void* n, size_t acc_size);

#endif

#pragma push_macro("Zeta_TreeNode")
#pragma push_macro("Zeta_TreeNode_")

ZETA_ExternC_End;
