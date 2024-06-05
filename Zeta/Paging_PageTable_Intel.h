#pragma once

#include "define.h"

ZETA_ExternC_Beg;

ZETA_DeclareStruct(Zeta_Pagin_PageTable_Intel_32);
ZETA_DeclareStruct(Zeta_Pagin_PageTable_Intel_PAE);
ZETA_DeclareStruct(Zeta_Pagin_PageTable_Intel_4Level);
ZETA_DeclareStruct(Zeta_Pagin_PageTable_Intel_5Level);

struct Zeta_Pagin_PageTable_Intel_32 {
    //
};

struct Zeta_Pagin_PageTable_Intel_PAE {
    //
};

struct Zeta_Pagin_PageTable_Intel_4Level {
    //
};

struct Zeta_Pagin_PageTable_Intel_5Level {
    //
};

void Zeta_Pagin_PageTable_Intel64_Insert(void* lin_addr, void* phy_addr);

void Zeta_Pagin_PageTable_Intel64_Erase(void* lin_addr, void* phy_addr);

ZETA_ExternC_End;
