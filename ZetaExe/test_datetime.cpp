#include <deque>
#include <iomanip>
#include <map>
#include <random>

#include "../Zeta/datetime.h"

#define MAX (static_cast<long long>(1024) * 1024 * 1024 * 1024)

std::mt19937_64 en;

std::uniform_int_distribution<long long> size_generator{ 0, MAX };

std::ostream& operator<<(std::ostream& os, const Zeta_Date& date) {
    if (date.year <= 0) {
        os << "BC " << std::setw(4) << 1 - date.year;
    } else {
        os << "AC " << std::setw(4) << date.year;
    }

    os << "-" << std::setw(2) << date.month << "-" << std::setw(2) << date.day;
    return os;
}

std::ostream& operator<<(std::ostream& os, const Zeta_DateTime& datetime) {
    if (datetime.year <= 0) {
        os << "BC " << std::setw(4) << 1 - datetime.year;
    } else {
        os << "AC " << std::setw(4) << datetime.year;
    }

    os << "-" << std::setw(2) << datetime.month << "-" << std::setw(2)
       << datetime.day << "    " << std::setw(2) << datetime.hour << ":"
       << std::setw(2) << datetime.min << ":" << std::setw(2) << datetime.sec
       << ":" << std::setw(6) << datetime.us;

    return os;
}

void main1() {
    unsigned seed = time(NULL);
    // unsigned seed = 1717313193;

    ZETA_PrintVar(seed);

    en.seed(seed);

    long long test_num = ZETA_DateTime_max_year - ZETA_DateTime_min_year;

    for (long long test_i = 0; test_i <= test_num; ++test_i) {
        long long abs_day = test_i;
        Zeta_Date date = Zeta_AbsDayToDate(abs_day);

        ZETA_DebugAssert(Zeta_DateToAbsDay(date) == abs_day);

        if (test_i % (test_num / 100) == 0) {
            ZETA_PrintVar(test_i / (test_num / 100));
        }
    }
}

void main2() {
    unsigned seed = time(NULL);
    // unsigned seed = 1717313193;

    ZETA_PrintVar(seed);

    en.seed(seed);

    Zeta_DateTime beg_datetime = (Zeta_DateTime){ .year = 1970,
                                                  .month = 1,
                                                  .day = 1,
                                                  .hour = 0,
                                                  .min = 0,
                                                  .sec = 0,
                                                  .us = 0 };

    Zeta_DateTime end_datetime = (Zeta_DateTime){ .year = 2300,
                                                  .month = 1,
                                                  .day = 1,
                                                  .hour = 0,
                                                  .min = 0,
                                                  .sec = 0,
                                                  .us = 0 };

    long long beg_abs_sec = Zeta_UTCDateTimeToAbsUs(beg_datetime) / 1000000;
    long long end_abs_sec = Zeta_UTCDateTimeToAbsUs(end_datetime) / 1000000;

    long long test_num = end_abs_sec - beg_abs_sec + 1;

    for (long long test_i = beg_abs_sec; test_i <= end_abs_sec; ++test_i) {
        long long abs_sec = test_i;
        long long us = size_generator(en) % static_cast<long long>(1000000);

        long long abs_us = abs_sec * 1000000 + us;

        Zeta_DateTime datetime = Zeta_AbsUsToUTCDateTime(abs_us);

        long long re_abs_us = Zeta_UTCDateTimeToAbsUs(datetime);

        if (re_abs_us != abs_us) {
            ZETA_PrintVar(abs_sec);
            ZETA_PrintVar(abs_us);
            ZETA_PrintVar(datetime);
            ZETA_PrintVar(re_abs_us);

            ZETA_DebugAssert(FALSE);
        }

        if ((test_i - beg_abs_sec) % (test_num / 100) == 0) {
            ZETA_PrintVar((test_i - beg_abs_sec) / (test_num / 100));
            ZETA_PrintVar(datetime);
        }
    }
}

void main3() {
    unsigned seed = time(NULL);
    // unsigned seed = 1717313193;

    ZETA_PrintVar(seed);

    en.seed(seed);

    Zeta_DateTime beg_datetime = (Zeta_DateTime){ .year = 1970,
                                                  .month = 1,
                                                  .day = 1,
                                                  .hour = 0,
                                                  .min = 0,
                                                  .sec = 0,
                                                  .us = 0 };

    Zeta_DateTime end_datetime = (Zeta_DateTime){ .year = 2300,
                                                  .month = 1,
                                                  .day = 1,
                                                  .hour = 0,
                                                  .min = 0,
                                                  .sec = 0,
                                                  .us = 0 };

    long long beg_abs_sec = Zeta_GMTDateTimeToAbsUs(beg_datetime) / 1000000;
    long long end_abs_sec = Zeta_GMTDateTimeToAbsUs(end_datetime) / 1000000;

    long long test_num = end_abs_sec - beg_abs_sec + 1;

    ZETA_PrintVar(test_num);

    for (long long test_i = beg_abs_sec; test_i <= end_abs_sec; ++test_i) {
        long long abs_sec = test_i;
        long long us = size_generator(en) % static_cast<long long>(1000000);

        long long abs_us = abs_sec * 1000000 + us;

        Zeta_DateTime datetime = Zeta_AbsUsToGMTDateTime(abs_us);

        long long re_abs_us = Zeta_GMTDateTimeToAbsUs(datetime);

        if (re_abs_us != abs_us) {
            ZETA_PrintVar(abs_sec);
            ZETA_PrintVar(abs_us);
            ZETA_PrintVar(datetime);
            ZETA_PrintVar(re_abs_us);

            ZETA_DebugAssert(FALSE);
        }

        if ((test_i - beg_abs_sec) % (test_num / 100) == 0) {
            ZETA_PrintVar((test_i - beg_abs_sec) / (test_num / 100));
            ZETA_PrintVar(datetime);
        }
    }
}

void main4() {
    Zeta_Date today = {
        .year = 1950,
        .month = 12,
        .day = 15,
    };

    ZETA_PrintVar(Zeta_GetDayInWeek(today));
}

int main() {
    main2();
    printf("ok\n");
    return 0;
}
