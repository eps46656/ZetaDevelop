// define TreeNode

#include "mem_check_utils.h"

ZETA_ExternC_Beg;

#if !defined(TreeNode)
#error "TreeNode is not defined."
#endif

#pragma push_macro("Zeta_RBTree_TreeNode_")

#define Zeta_RBTree_TreeNode_(x) \
    ZETA_Concat(ZETA_Concat(ZETA_Concat(Zeta_RBTree_, TreeNode), _), x)

void* Zeta_RBTree_TreeNode_(InsertL)(void* pos, void* n);

void* Zeta_RBTree_TreeNode_(InsertR)(void* pos, void* n);

void* Zeta_RBTree_TreeNode_(Insert)(void* pos_l, void* pos_r, void* n);

void* Zeta_RBTree_TreeNode_(GeneralInsertL)(void* root, void* pos, void* n);

void* Zeta_RBTree_TreeNode_(GeneralInsertR)(void* root, void* pos, void* n);

void* Zeta_RBTree_TreeNode_(Extract)(void* n);

void Zeta_RBTree_TreeNode_(Sanitize)(Zeta_MemRecorder* dst_mr, void* root);

#pragma pop_macro("Zeta_RBTree_TreeNode_")

ZETA_ExternC_End;
