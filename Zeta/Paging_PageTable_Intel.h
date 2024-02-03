#pragma once

#include "define.h"

typedef struct Zeta_Pagin_PageTable_Intel_32 Zeta_Pagin_PageTable_Intel_32;

struct Zeta_Pagin_PageTable_Intel_32 {
    //
};

typedef struct Zeta_Pagin_PageTable_Intel_PAE Zeta_Pagin_PageTable_Intel_PAE;

struct Zeta_Pagin_PageTable_Intel_PAE {
    //
};

typedef struct Zeta_Pagin_PageTable_Intel_4Level
    Zeta_Pagin_PageTable_Intel_4Level;

struct Zeta_Pagin_PageTable_Intel_4Level {
    //
};

typedef struct Zeta_Pagin_PageTable_Intel_5Level
    Zeta_Pagin_PageTable_Intel_5Level;

struct Zeta_Pagin_PageTable_Intel_5Level {
    //
};

EXTERN_C void Zeta_Pagin_PageTable_Intel64_Insert(void* lin_addr,
                                                  void* phy_addr);

EXTERN_C void Zeta_Pagin_PageTable_Intel64_Erase(void* lin_addr,
                                                 void* phy_addr);
