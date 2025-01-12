#ifndef KLOG_DATETIME_H
#define KLOG_DATETIME_H

#include <cstdint>

namespace klog {

struct datetime {
    int year;
    int month;
    int day;
    int hour;
    int minute;
    int second;
    long nanosecond;
};

void to_datetime(int64_t nanoseconds, datetime &);

} // namespace klog

#endif // KLOG_DATETIME_H
