// define TreeNode
// define AccSize

#include "define.h"

ZETA_ExternC_Beg;

#if !defined(TreeNode)
#error "TreeNode is not defined."
#endif

#if !defined(AccSize)
#error "AccSize is not defined."
#endif

#pragma push_macro("Zeta_BinTree_TreeNode_")

#define Zeta_BinTree_TreeNode_(x) \
    ZETA_Concat(ZETA_Concat(ZETA_Concat(Zeta_BinTree_, TreeNode), _), x)

size_t Zeta_BinTree_TreeNode_(Count)(void* n);

#if AccSize

size_t Zeta_BinTree_TreeNode_(GetSize)(void* n);

void Zeta_BinTree_TreeNode_(SetSize)(void* n, size_t size);

void Zeta_BinTree_TreeNode_(SetDiffSize)(void* n, size_t diff_size);

#endif

void Zeta_BinTree_TreeNode_(AttatchL)(void* pos, void* n);

void Zeta_BinTree_TreeNode_(AttatchR)(void* pos, void* n);

void Zeta_BinTree_TreeNode_(Detach)(void* n);

void Zeta_BinTree_TreeNode_(EraseAll)(void* root, void* callback_context,
                                      void (*Callback)(void* callback_context,
                                                       void* n));

void Zeta_BinTree_TreeNode_(Swap)(void* n, void* m);

void Zeta_BinTree_TreeNode_(RotateL)(void* n);

void Zeta_BinTree_TreeNode_(RotateR)(void* n);

void* Zeta_BinTree_TreeNode_(StepL)(void* n);

void* Zeta_BinTree_TreeNode_(StepR)(void* n);

#if AccSize

void Zeta_BinTree_TreeNode_(AccessL)(void** dst_n, size_t* dst_tail_idx,
                                     void* n, size_t idx);

void Zeta_BinTree_TreeNode_(AccessR)(void** dst_n, size_t* dst_tail_idx,
                                     void* n, size_t idx);

void Zeta_BinTree_TreeNode_(AdvanceL)(void** dst_n, size_t* dst_tail_idx,
                                      void* n, size_t step);

void Zeta_BinTree_TreeNode_(AdvanceR)(void** dst_n, size_t* dst_tail_idx,
                                      void* n, size_t step);

void Zeta_BinTree_TreeNode_(GetAccSize)(size_t* dst_l_acc_size,
                                        size_t* dst_r_acc_size, void* n);

#endif

void Zeta_BinTree_TreeNode_(Sanitize)(void* root);

#pragma pop_macro("Zeta_BinTree_TreeNode_")

ZETA_ExternC_End;
