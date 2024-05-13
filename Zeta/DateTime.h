#pragma once

#include "define.h"

ZETA_ExternC_Beg;

typedef struct Zeta_DateTime Zeta_DateTime;

struct Zeta_DateTime {
    int year;
    unsigned int month;
    unsigned int day;

    unsigned int hour;
    unsigned int minute;
    unsigned int second;
    unsigned int millisecond;
};

ZETA_ExternC_End;
