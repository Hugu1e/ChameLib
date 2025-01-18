#include <utils/TimeUtils.h>

time_t TimeUtils::TimeCast(int year, int month, int day, int hour, int minute, int second)
{
    // time_t = 2025.12.31 0:00:00
    struct tm timeinfo = {};
    timeinfo.tm_year = year - 1900;
    timeinfo.tm_mon = month - 1;
    timeinfo.tm_mday = day;           
    timeinfo.tm_hour = hour;        
    timeinfo.tm_min = minute;             
    timeinfo.tm_sec = second;     
    timeinfo.tm_isdst = -1;  
    return mktime(&timeinfo);
}
