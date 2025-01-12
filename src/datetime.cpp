#include "klog/datetime.h"

namespace klog {

template<typename T = int>
static T divide(int64_t &x, int64_t y) {
    auto z = x % y;
    x /= y;
    return static_cast<T>(z);
}

static bool is_leap(int year) {
    return (year % 4 == 0 && year % 100 != 0) || (year % 100 == 0);
}

static int count_leaps(int year) {
    return year / 4 - year / 100 + year / 400;
}

constexpr const int month_days[2][12] = {
    {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31},
    {31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31},
};

void to_datetime(int64_t nanoseconds, datetime &p) {
    int64_t t = nanoseconds;

    p.nanosecond = divide<long>(t, 1'000'000'000L);
    p.second = divide(t, 60);
    p.minute = divide(t, 60);
    p.hour = divide(t, 24);

    int days = static_cast<int>(t), year, guess;

    guess = days / 366;
    year = 1970 + guess;
    days -= guess * 365;
    days -= count_leaps(year-1) - count_leaps(1970-1);

    while (true) {
        int d = is_leap(year) ? 366 : 365;
        if (days < d)
            break;

        year++;
        days -= d;
    }

    const int *mdays = month_days[is_leap(year)];
    int month = 0;
    while (days >= mdays[month]) {
        days -= mdays[month];
        month++;
    }

    p.year = year;
    p.month = month + 1;
    p.day = days + 1;
}

} // namespace klog
