// define TreeNode

#include "mem_check_utils.h"

#ifndef ZETA_RBTree_MacroGuard
#define ZETA_RBTree_MacroGuard

#define Zeta_RBTree_(tree_name, x) ZETA_Concat(Zeta_RBTree_, tree_name, _, x)

#endif

ZETA_ExternC_Beg;

#if !defined(TreeNode)
#error "TreeNode is not defined."
#endif

void* Zeta_RBTree_(TreeNode, InsertL)(void* pos, void* n);

void* Zeta_RBTree_(TreeNode, InsertR)(void* pos, void* n);

void* Zeta_RBTree_(TreeNode, Insert)(void* pos_l, void* pos_r, void* n);

void* Zeta_RBTree_(TreeNode, GeneralInsertL)(void* root, void* pos, void* n);

void* Zeta_RBTree_(TreeNode, GeneralInsertR)(void* root, void* pos, void* n);

void* Zeta_RBTree_(TreeNode, Extract)(void* n);

void Zeta_RBTree_(TreeNode, Sanitize)(Zeta_MemRecorder* dst_mr, void* root);

ZETA_ExternC_End;
