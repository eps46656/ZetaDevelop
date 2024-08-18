#include "DateTime.h"

#include "Debugger.h"

#define YearOffset_ (400 * 16)

static int const acc_days_in_month_comm[] = {
    0,
    0,
    31,                                                         // 1
    31 + 28,                                                    // 2
    31 + 28 + 31,                                               // 3
    31 + 28 + 31 + 30,                                          // 4
    31 + 28 + 31 + 30 + 31,                                     // 5
    31 + 28 + 31 + 30 + 31 + 30,                                // 6
    31 + 28 + 31 + 30 + 31 + 30 + 31,                           // 7
    31 + 28 + 31 + 30 + 31 + 30 + 31 + 31,                      // 8
    31 + 28 + 31 + 30 + 31 + 30 + 31 + 31 + 30,                 // 9
    31 + 28 + 31 + 30 + 31 + 30 + 31 + 31 + 30 + 31,            // 10
    31 + 28 + 31 + 30 + 31 + 30 + 31 + 31 + 30 + 31 + 30,       // 11
    31 + 28 + 31 + 30 + 31 + 30 + 31 + 31 + 30 + 31 + 30 + 31,  // 12
};

static int const acc_days_in_month_leap[] = {
    0,
    0,
    31,                                                         // 1
    31 + 29,                                                    // 2
    31 + 29 + 31,                                               // 3
    31 + 29 + 31 + 30,                                          // 4
    31 + 29 + 31 + 30 + 31,                                     // 5
    31 + 29 + 31 + 30 + 31 + 30,                                // 6
    31 + 29 + 31 + 30 + 31 + 30 + 31,                           // 7
    31 + 29 + 31 + 30 + 31 + 30 + 31 + 31,                      // 8
    31 + 29 + 31 + 30 + 31 + 30 + 31 + 31 + 30,                 // 9
    31 + 29 + 31 + 30 + 31 + 30 + 31 + 31 + 30 + 31,            // 10
    31 + 29 + 31 + 30 + 31 + 30 + 31 + 31 + 30 + 31 + 30,       // 11
    31 + 29 + 31 + 30 + 31 + 30 + 31 + 31 + 30 + 31 + 30 + 31,  // 12
};

static const Zeta_Date leap_sec_hists[] = {
    (Zeta_Date){ 1972, 6, 30 },  (Zeta_Date){ 1972, 12, 31 },  //
    (Zeta_Date){ 1973, 12, 31 },                               //
    (Zeta_Date){ 1974, 12, 31 },                               //
    (Zeta_Date){ 1975, 12, 31 },                               //
    (Zeta_Date){ 1976, 12, 31 },                               //
    (Zeta_Date){ 1977, 12, 31 },                               //
    (Zeta_Date){ 1978, 12, 31 },                               //
    (Zeta_Date){ 1979, 12, 31 },                               //

    (Zeta_Date){ 1981, 6, 30 },  //
    (Zeta_Date){ 1982, 6, 30 },  //
    (Zeta_Date){ 1983, 6, 30 },  //

    (Zeta_Date){ 1985, 6, 30 },  //

    (Zeta_Date){ 1987, 12, 31 },  //

    (Zeta_Date){ 1989, 12, 31 },  //
    (Zeta_Date){ 1990, 12, 31 },  //

    (Zeta_Date){ 1992, 6, 30 },   //
    (Zeta_Date){ 1993, 6, 30 },   //
    (Zeta_Date){ 1994, 6, 30 },   //
    (Zeta_Date){ 1995, 12, 31 },  //

    (Zeta_Date){ 1997, 6, 30 },   //
    (Zeta_Date){ 1998, 12, 31 },  //

    (Zeta_Date){ 2005, 12, 31 },  //

    (Zeta_Date){ 2008, 12, 31 },  //

    (Zeta_Date){ 2012, 6, 30 },  //

    (Zeta_Date){ 2015, 6, 30 },   //
    (Zeta_Date){ 2016, 12, 31 },  //
};

/*
static const Zeta_Date leap_sec_hists[] = {
    (Zeta_Date){ 1972, 7, 1 },  //
    (Zeta_Date){ 1973, 1, 1 },  //
    (Zeta_Date){ 1974, 1, 1 },  //
    (Zeta_Date){ 1975, 1, 1 },  //
    (Zeta_Date){ 1976, 1, 1 },  //
    (Zeta_Date){ 1977, 1, 1 },  //
    (Zeta_Date){ 1978, 1, 1 },  //
    (Zeta_Date){ 1979, 1, 1 },  //
    (Zeta_Date){ 1980, 1, 1 },  //
    (Zeta_Date){ 1981, 7, 1 },  //
    (Zeta_Date){ 1982, 7, 1 },  //
    (Zeta_Date){ 1983, 7, 1 },  //
    (Zeta_Date){ 1985, 7, 1 },  //
    (Zeta_Date){ 1988, 1, 1 },  //
    (Zeta_Date){ 1990, 1, 1 },  //
    (Zeta_Date){ 1991, 1, 1 },  //
    (Zeta_Date){ 1992, 7, 1 },  //
    (Zeta_Date){ 1993, 7, 1 },  //
    (Zeta_Date){ 1994, 7, 1 },  //
    (Zeta_Date){ 1996, 1, 1 },  //
    (Zeta_Date){ 1997, 7, 1 },  //
    (Zeta_Date){ 1999, 1, 1 },  //
    (Zeta_Date){ 2006, 1, 1 },  //
    (Zeta_Date){ 2009, 1, 1 },  //
    (Zeta_Date){ 2012, 7, 1 },  //
    (Zeta_Date){ 2015, 7, 1 },  //
    (Zeta_Date){ 2017, 1, 1 },  //
};
*/

#define IsLeapYear_(year)                                     \
    ({                                                        \
        long long tmp = (year);                               \
        (tmp % 4 == 0) && (tmp % 100 != 0 || tmp % 400 == 0); \
    })

#define AccDaysInYear_(abs_year)             \
    ({                                       \
        long long k = (abs_year) - 1;        \
        365 * k + k / 4 - k / 100 + k / 400; \
    })

long long Zeta_DateToAbsDay(Zeta_Date date) {
    ZETA_DebugAssert(ZETA_DateTime_min_year <= date.year);
    ZETA_DebugAssert(date.year <= ZETA_DateTime_max_year);

    ZETA_DebugAssert(1 <= date.month && date.month <= 12);

    bool_t is_leap_year = IsLeapYear_(date.year);

    int const* acc_days_in_month =
        is_leap_year ? acc_days_in_month_leap : acc_days_in_month_comm;

    ZETA_DebugAssert(1 <= date.day &&
                     date.day <= acc_days_in_month[date.month + 1] -
                                     acc_days_in_month[date.month]);

    return AccDaysInYear_(date.year - ZETA_DateTime_min_year) +
           acc_days_in_month[date.month] + date.day - 1;
}

static long long FindY_(long long x) {
    /*
        365*y + floor(y/4) - floor(y/100) + floor(y/400) <= x

        y = 400*a + 100*b + 4*c + d
        0 <= a
        0 <= b <= 3
        0 <= c <= 24
        0 <= d <= 3

        146097*a + 36524*b + 1461*c + 365*d <= x
    */

    long long a = x / 146097;
    x %= 146097;

    long long b = ZETA_GetMinOf(3, x / 36524);
    x -= 36524 * b;

    long long c = ZETA_GetMinOf(24, x / 1461);
    x -= 1461 * c;

    long long d = ZETA_GetMinOf(3, x / 365);

    return 400 * a + 100 * b + 4 * c + d;
}

Zeta_Date Zeta_AbsDayToDate(long long abs_day) {
    ZETA_DebugAssert(0 <= abs_day);

    long long abs_year = FindY_(abs_day) + 1;

    abs_day -= AccDaysInYear_(abs_year);

    bool_t is_leap_year = IsLeapYear_(abs_year);

    int month_lb = 1;
    int month_rb = 12;

    int const* acc_days_in_month =
        is_leap_year ? acc_days_in_month_leap : acc_days_in_month_comm;

    while (month_lb < month_rb) {
        int mid = (month_lb + month_rb + 1) / 2;

        if (acc_days_in_month[mid] <= abs_day) {
            month_lb = mid;
        } else {
            month_rb = mid - 1;
        }
    }

    abs_day -= acc_days_in_month[month_lb];

    return (Zeta_Date){ ZETA_DateTime_min_year + abs_year, month_lb,
                        abs_day + 1 };
}

int Zeta_GetDayInWeek(Zeta_Date date) {
    Zeta_Date base = { .year = 1970, .month = 1, .day = 1 };

    long long delta = Zeta_DateToAbsDay(date) - Zeta_DateToAbsDay(base);

    return ((delta % 7) + 11) % 7;
}

static int DateCompare_(Zeta_Date const* x, Zeta_Date const* y) {
    if (x->year < y->year) { return -1; }
    if (y->year < x->year) { return 1; }

    if (x->month < y->month) { return -1; }
    if (y->month < x->month) { return 1; }

    if (x->day < y->day) { return -1; }
    if (y->day < x->day) { return 1; }

    return 0;
}

long long Zeta_UTCDateTimeToAbsUs(Zeta_DateTime datetime) {
    Zeta_Date date = (Zeta_Date){ datetime.year, datetime.month, datetime.day };

    long long hist_lb = 0;
    long long hist_rb = sizeof(leap_sec_hists) / sizeof(leap_sec_hists[0]);

    while (hist_lb < hist_rb) {
        long long mid = (hist_lb + hist_rb) / 2;

        if (DateCompare_(leap_sec_hists + mid, &date) < 0) {
            hist_lb = mid + 1;
        } else {
            hist_rb = mid;
        }
    }

    long long abs_sec =
        ((Zeta_DateToAbsDay(date) * 24 + datetime.hour) * 60 + datetime.min) *
            60 +
        datetime.sec + hist_lb;

    return abs_sec * 1000000 + datetime.us;
}

Zeta_DateTime Zeta_AbsUsToUTCDateTime(long long abs_us) {
    ZETA_DebugAssert(0 <= abs_us);

    long long abs_sec = abs_us / 1000000;
    long long us = abs_us % 1000000;

    long long const leap_sec_hists_length =
        sizeof(leap_sec_hists) / sizeof(leap_sec_hists[0]);

    long long hist_lb = 0;
    long long hist_rb = leap_sec_hists_length;

    while (hist_lb < hist_rb) {
        long long mid = (hist_lb + hist_rb) / 2;

        long long cur_abs_sec =
            Zeta_DateToAbsDay(leap_sec_hists[mid]) * 86400 + mid + 86401;

        if (cur_abs_sec <= abs_sec) {
            hist_lb = mid + 1;
        } else {
            hist_rb = mid;
        }
    }

    if (hist_lb < leap_sec_hists_length) {
        long long hist_leap_abs_sec =
            Zeta_DateToAbsDay(leap_sec_hists[hist_lb]) * 86400 + hist_lb +
            86400;

        if (abs_sec == hist_leap_abs_sec) {
            Zeta_DateTime ret;
            ret.year = leap_sec_hists[hist_lb].year;
            ret.month = leap_sec_hists[hist_lb].month;
            ret.day = leap_sec_hists[hist_lb].day;
            ret.hour = 23;
            ret.min = 59;
            ret.sec = 60;
            ret.us = us;

            return ret;
        }
    }

    long long k = abs_sec - hist_lb;

    Zeta_DateTime ret;

    ret.us = us;

    ret.sec = k % 60;
    k /= 60;

    ret.min = k % 60;
    k /= 60;

    ret.hour = k % 24;
    k /= 24;

    Zeta_Date date = Zeta_AbsDayToDate(k);

    ret.year = date.year;
    ret.month = date.month;
    ret.day = date.day;

    return ret;
}

long long Zeta_GMTDateTimeToAbsUs(Zeta_DateTime datetime) {
    Zeta_Date date = (Zeta_Date){ datetime.year, datetime.month, datetime.day };

    long long ret = Zeta_DateToAbsDay(date);
    ret = ret * 24 + datetime.hour;
    ret = ret * 60 + datetime.min;
    ret = ret * 60 + datetime.sec;
    ret = ret * 1000000 + datetime.us;

    return ret;
}

Zeta_DateTime Zeta_AbsUsToGMTDateTime(long long abs_us) {
    ZETA_DebugAssert(0 <= abs_us);

    long long k = abs_us;

    Zeta_DateTime ret;

    ret.us = k % 1000000;
    k /= 1000000;

    ret.sec = k % 60;
    k /= 60;

    ret.min = k % 60;
    k /= 60;

    ret.hour = k % 24;
    k /= 24;

    Zeta_Date date = Zeta_AbsDayToDate(k);

    ret.year = date.year;
    ret.month = date.month;
    ret.day = date.day;

    return ret;
}
