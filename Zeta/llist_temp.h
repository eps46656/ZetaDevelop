// define LListNode

#include "define.h"

#ifndef ZETA_LListNode_MacroGuard
#define ZETA_LListNode_MacroGuard

#define Zeta_LList_(llist_node, x) ZETA_Concat(Zeta_LList_, llist_node, _, x)

#endif

ZETA_ExternC_Beg;

#if !defined(LListNode)
#error "LListNode is not defined."
#endif

size_t Zeta_LList_(LListNode, Count)(void* n);

void Zeta_LList_(LListNode, InsertL)(void* n, void* m);
void Zeta_LList_(LListNode, InsertR)(void* n, void* m);

void Zeta_LList_(LListNode, Extract)(void* n);

void Zeta_LList_(LListNode, InsertSegL)(void* n, void* m_beg, void* m_end);
void Zeta_LList_(LListNode, InsertSegR)(void* n, void* m_beg, void* m_end);

void Zeta_LList_(LListNode, ExtractSeg)(void* n_beg, void* n_end);

ZETA_ExternC_End;
