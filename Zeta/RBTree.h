#pragma once

#include "BinTree.h"

ZETA_extern_c_beg;

void* Zeta_RBTree_InsertL(Zeta_BinTreeNodeOperator const* btn_opr, void* n,
                          void* m);

void* Zeta_RBTree_InsertR(Zeta_BinTreeNodeOperator const* btn_opr, void* n,
                          void* m);

void* Zeta_RBTree_Extract(Zeta_BinTreeNodeOperator const* btn_opr, void* n);

void Zeta_RBTree_Check(Zeta_BinTreeNodeOperator const* btn_opr, void* n);

ZETA_extern_c_end;
