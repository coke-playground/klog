#include <iostream>
#include <chrono>

#include "klog/logger_base.h"
#include "klog/stream_logger.h"
#include "klog/datetime.h"

namespace klog {

struct LoggerManager {
    std::shared_ptr<Logger> default_logger;
    Logger *default_logger_raw;
    std::chrono::seconds offset;

    LoggerManager() {
        static StreamLogger<NullMutex> logger(&std::cout);
        default_logger.reset(&logger, [](StreamLogger<NullMutex> *) {});
        default_logger_raw = default_logger.get();

        const std::chrono::time_zone *zone = std::chrono::current_zone();
        auto now = std::chrono::system_clock::now();
        std::chrono::sys_info info = zone->get_info(now);
        offset = info.offset;
    }

    ~LoggerManager() = default;

    void set_logger(std::shared_ptr<Logger> logger) {
        default_logger = std::move(logger);
        default_logger_raw = default_logger.get();
    }

    void set_offset(std::chrono::seconds offset) {
        this->offset = offset;
    }

    Logger *get_logger_raw() const {
        return default_logger_raw;
    }

    std::chrono::seconds get_offset() const {
        return this->offset;
    }
};

LoggerManager &get_logger_manager() {
    static LoggerManager m;
    return m;
}

char *Logger::get_thread_local_buf(std::size_t &size) {
    constexpr static int LOG_BUF_SIZE = 16 * 1024;
    static thread_local std::unique_ptr<char []> buf(new char[LOG_BUF_SIZE]);

    size = LOG_BUF_SIZE;
    return buf.get();
}

template<int N, typename Integer>
static void integer_to_chars(char *&p, Integer x, char c) {
    for (int i = 1; i <= N; i++) {
        p[N-i] = char('0' + (x % 10));
        x /= 10;
    }

    if (c) {
        p[N] = c;
        p += N + 1;
    }
    else
        p += N;
}

void Logger::format_time(std::pmr::string &buf) {
    std::chrono::seconds offset = get_logger_manager().get_offset();
    auto now = std::chrono::system_clock::now().time_since_epoch();
    auto nano = std::chrono::duration_cast<std::chrono::nanoseconds>(now);
    int64_t nsec = (int64_t)(nano + offset).count();
    datetime dt;
    char buffer[64];
    char *p = buffer;

    to_datetime(nsec, dt);

    *(p++) = '[';
    integer_to_chars<4>(p, dt.year, '-');
    integer_to_chars<2>(p, dt.month, '-');
    integer_to_chars<2>(p, dt.day, ' ');
    integer_to_chars<2>(p, dt.hour, ':');
    integer_to_chars<2>(p, dt.minute, ':');
    integer_to_chars<2>(p, dt.second, '.');
    integer_to_chars<9>(p, dt.nanosecond, ']');
    *(p++) = ' ';

    buf.append(buffer, p);
}

Logger *get_default_logger_raw() {
    return get_logger_manager().get_logger_raw();
}

void set_default_logger(std::shared_ptr<Logger> logger) {
    get_logger_manager().set_logger(std::move(logger));
}

} // namespace klog
