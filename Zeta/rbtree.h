#pragma once

#include "bin_tree.h"
#include "mem_check_utils.h"

ZETA_ExternC_Beg;

void* Zeta_RBTree_InsertL(Zeta_BinTreeNodeOperator const* btn_opr, void* pos,
                          void* n);

void* Zeta_RBTree_InsertR(Zeta_BinTreeNodeOperator const* btn_opr, void* pos,
                          void* n);

void* Zeta_RBTree_Insert(Zeta_BinTreeNodeOperator const* btn_opr, void* pos_l,
                         void* pos_r, void* n);

void* Zeta_RBTree_GeneralInsertL(Zeta_BinTreeNodeOperator const* btn_opr,
                                 void* root, void* pos, void* n);

void* Zeta_RBTree_GeneralInsertR(Zeta_BinTreeNodeOperator const* btn_opr,
                                 void* root, void* pos, void* n);

void* Zeta_RBTree_Extract(Zeta_BinTreeNodeOperator const* btn_opr, void* n);

void Zeta_RBTree_Sanitize(Zeta_BinTreeNodeOperator const* btn_opr,
                          Zeta_MemRecorder* dst_mr, void* root);

ZETA_ExternC_End;
