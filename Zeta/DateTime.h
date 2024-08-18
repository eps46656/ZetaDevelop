#pragma once

#include "define.h"

ZETA_ExternC_Beg;

ZETA_DeclareStruct(Zeta_Date);
ZETA_DeclareStruct(Zeta_DateTime);

#define ZETA_DateTime_min_year (-400 * 64)
#define ZETA_DateTime_max_year (ZETA_DateTime_min_year + 1024 * 1024 * 16)

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

long long Zeta_DateToAbsDay(Zeta_Date date);

Zeta_Date Zeta_AbsDayToDate(long long abs_day);

int Zeta_GetDayInWeek(Zeta_Date date);

long long Zeta_UTCDateTimeToAbsUs(Zeta_DateTime datetime);

Zeta_DateTime Zeta_AbsUsToUTCDateTime(long long abs_us);

long long Zeta_GMTDateTimeToAbsUs(Zeta_DateTime datetime);

Zeta_DateTime Zeta_AbsUsToGMTDateTime(long long abs_us);

ZETA_ExternC_End;
