// define TreeNode

#include "define.h"

#ifndef ZETA_BinTree_MacroGuard
#define ZETA_BinTree_MacroGuard

#define Zeta_BinTree_(tree_node, x) ZETA_Concat(Zeta_BinTree_, tree_node, _, x)

#endif

ZETA_ExternC_Beg;

#if !defined(TreeNode)
#error "TreeNode is not defined."
#endif

size_t Zeta_BinTree_(TreeNode, Count)(void* n);

#if defined(AccSizeType)

size_t Zeta_BinTree_(TreeNode, GetSize)(void* n);

void Zeta_BinTree_(TreeNode, SetSize)(void* n, size_t size);

void Zeta_BinTree_(TreeNode, SetDiffSize)(void* n, size_t diff_size);

#endif

void Zeta_BinTree_(TreeNode, AttatchL)(void* pos, void* n);

void Zeta_BinTree_(TreeNode, AttatchR)(void* pos, void* n);

void Zeta_BinTree_(TreeNode, Detach)(void* n);

void Zeta_BinTree_(TreeNode,
                   EraseAll)(void* root, void* callback_context,
                             void (*Callback)(void* callback_context, void* n));

void Zeta_BinTree_(TreeNode, Swap)(void* n, void* m);

void Zeta_BinTree_(TreeNode, RotateL)(void* n);

void Zeta_BinTree_(TreeNode, RotateR)(void* n);

void* Zeta_BinTree_(TreeNode, StepL)(void* n);

void* Zeta_BinTree_(TreeNode, StepR)(void* n);

#if defined(AccSizeType)

void Zeta_BinTree_(TreeNode, AccessL)(void** dst_n, size_t* dst_tail_idx,
                                      void* n, size_t idx);

void Zeta_BinTree_(TreeNode, AccessR)(void** dst_n, size_t* dst_tail_idx,
                                      void* n, size_t idx);

void Zeta_BinTree_(TreeNode, AdvanceL)(void** dst_n, size_t* dst_tail_idx,
                                       void* n, size_t step);

void Zeta_BinTree_(TreeNode, AdvanceR)(void** dst_n, size_t* dst_tail_idx,
                                       void* n, size_t step);

void Zeta_BinTree_(TreeNode, GetAccSize)(size_t* dst_l_acc_size,
                                         size_t* dst_r_acc_size, void* n);

#endif

void Zeta_BinTree_(TreeNode,
                   ErassAll)(void* root,
                             void (*Callback)(void* callback_context, void* n),
                             void* callback_context);

void Zeta_BinTree_(TreeNode, Sanitize)(void* root);

ZETA_ExternC_End;
