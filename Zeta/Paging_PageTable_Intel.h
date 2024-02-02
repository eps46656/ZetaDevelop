#pragma once

#include "define.h"

ZETA_DECL_STRUCT(Zeta_Pagin_PageTable_Intel_32){
    //
};

ZETA_DECL_STRUCT(Zeta_Pagin_PageTable_Intel_PAE){
    //
};

ZETA_DECL_STRUCT(Zeta_Pagin_PageTable_Intel_4Level){
    //
};

ZETA_DECL_STRUCT(Zeta_Pagin_PageTable_Intel_5Level){
    //
};

void Zeta_Pagin_PageTable_Intel64_Insert(void* lin_addr, void* phy_addr);

void Zeta_Pagin_PageTable_Intel64_Erase(void* lin_addr, void* phy_addr);
