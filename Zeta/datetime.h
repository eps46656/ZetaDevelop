#pragma once

#include "define.h"

ZETA_ExternC_Beg;

ZETA_DeclareStruct(Zeta_Date);
ZETA_DeclareStruct(Zeta_DateTime);

#define ZETA_DateTime_min_year (-400 * 16)

#if ZETA_ULLONG_WIDTH == 32
#define ZETA_DateTime_max_year (ZETA_DateTime_min_year + 400 * 16)
#elif ZETA_ULLONG_WIDTH == 64
#define ZETA_DateTime_max_year (ZETA_DateTime_min_year + 400 * 1024)
#else
#error "Unsupported architecture."
#endif

ZETA_StaticAssert(ZETA_DateTime_min_year % 400 == 0);

struct Zeta_Date {
    int year;
    short month;
    short day;
};

struct Zeta_DateTime {
    int year;
    short month;
    short day;

    short hour;
    short min;
    short sec;
    int us;
};

unsigned long long Zeta_DateToAbsDay(Zeta_Date date);

Zeta_Date Zeta_AbsDayToDate(unsigned long long abs_day);

int Zeta_GetDayInWeek(Zeta_Date date);

unsigned long long Zeta_UTCDateTimeToAbsUs(Zeta_DateTime datetime);

Zeta_DateTime Zeta_AbsUsToUTCDateTime(unsigned long long abs_us);

unsigned long long Zeta_GMTDateTimeToAbsUs(Zeta_DateTime datetime);

Zeta_DateTime Zeta_AbsUsToGMTDateTime(unsigned long long abs_us);

ZETA_ExternC_End;
