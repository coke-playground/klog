#ifndef KLOG_KLOG_H
#define KLOG_KLOG_H

#include "klog/logger_base.h"

namespace klog {

#define CURRENT_LOC std::source_location::current()

struct LogHelper {
    LogHelper(int level, const std::source_location &loc = CURRENT_LOC)
        : logger(get_default_logger_raw()), level(level), loc(loc)
    { }

    template<typename... Args>
    void operator()(std::format_string<Args...> str, Args&&... args) {
        logger->log(level, loc, str, std::forward<Args>(args)...);
    }

private:
    Logger *logger;
    int level;
    std::source_location loc;
};

struct LogToHelper {
    LogToHelper(int level, const std::source_location &loc = CURRENT_LOC)
        : level(level), loc(loc)
    { }

    template<typename... Args>
    void operator()(Logger *logger, std::format_string<Args...> str, Args&&... args) {
        logger->log(level, loc, str, std::forward<Args>(args)...);
    }

private:
    int level;
    std::source_location loc;
};

#undef CURRENT_LOC

} // namespace klog

#define KLOG_TRACE   ::klog::LogHelper{::klog::trace}
#define KLOG_DEBUG   ::klog::LogHelper{::klog::debug}
#define KLOG_INFO    ::klog::LogHelper{::klog::info}
#define KLOG_WARN    ::klog::LogHelper{::klog::warn}
#define KLOG_ERROR   ::klog::LogHelper{::klog::error}

#define KLOG_TRACE_TO   ::klog::LogToHelper{::klog::trace}
#define KLOG_DEBUG_TO   ::klog::LogToHelper{::klog::debug}
#define KLOG_INFO_TO    ::klog::LogToHelper{::klog::info}
#define KLOG_WARN_TO    ::klog::LogToHelper{::klog::warn}
#define KLOG_ERROR_TO   ::klog::LogToHelper{::klog::error}

#endif // KLOG_KLOG_H
