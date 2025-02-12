// define LListNode

#include "define.h"

ZETA_ExternC_Beg;

#if !defined(LListNode)
#error "LListNode is not defined."
#endif

#pragma push_macro("Zeta_LListNode")
#pragma push_macro("Zeta_LListNode_LListNode_")

#define Zeta_LListNode ZETA_Concat(Zeta_, LListNode)

#define Zeta_LListNode_LListNode_(func_name) \
    ZETA_Concat(ZETA_Concat(ZETA_Concat(Zeta_LList_, LListNode), _), func_name)

size_t Zeta_LListNode_LListNode_(Count)(void* n);

void Zeta_LListNode_LListNode_(InsertL)(void* n, void* m);
void Zeta_LListNode_LListNode_(InsertR)(void* n, void* m);

void Zeta_LListNode_LListNode_(Extract)(void* n);

void Zeta_LListNode_LListNode_(InsertSegL)(void* n, void* m_beg, void* m_end);
void Zeta_LListNode_LListNode_(InsertSegR)(void* n, void* m_beg, void* m_end);

void Zeta_LListNode_LListNode_(ExtractSeg)(void* n_beg, void* n_end);

#pragma pop_macro("Zeta_LListNode")
#pragma pop_macro("Zeta_LListNode_LListNode_")

ZETA_ExternC_End;
