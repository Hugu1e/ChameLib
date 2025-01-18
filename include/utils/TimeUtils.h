#ifndef CHAMELIB_TIMEUTILS_H
#define CHAMELIB_TIMEUTILS_H

#include <ctime>

class TimeUtils{
    public:
        static time_t TimeCast(int year, int month, int day, int hour, int minute, int second);
};

#endif  // CHAMELIB_TIMEUTILS_H