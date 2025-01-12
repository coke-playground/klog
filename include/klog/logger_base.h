#ifndef KLOG_LOGGER_BASE_H
#define KLOG_LOGGER_BASE_H

#include <format>
#include <memory>
#include <memory_resource>
#include <source_location>
#include <string>
#include <string_view>

#ifndef KLOG_PATH_DELIMITER
#define KLOG_PATH_DELIMITER '/'
#endif

namespace klog {

enum : int {
    trace = 0,
    debug = 1,
    info  = 2,
    warn  = 3,
    error = 4,
    num_level = 5,
};

class Logger {
    static char *get_thread_local_buf(std::size_t &size);
    static constexpr const char *do_split_filename(const char *p);

    static const char *get_level_str(int level);
    static const char *get_aligned_level_str(int level);

public:
    Logger() = default;
    virtual ~Logger() = default;

    void set_log_level(int log_level) {
        this->log_level = log_level;
    }

    void set_display_level(bool display_level) {
        this->display_level = display_level;
    }

    void set_aligned_level(bool aligned_level) {
        this->aligned_level = aligned_level;
    }

    void set_display_time(bool display_time) {
        this->display_time = display_time;
    }

    void set_display_location(bool display_location) {
        this->display_location = display_location;
    }

    void set_split_filename(bool split_filename) {
        this->split_filename = split_filename;
    }

    void set_line_end(const std::string &line_end) {
        this->line_end = line_end;
    }

    template<typename... Args>
    void log(int level, const std::source_location &loc,
             std::format_string<Args...> str, Args&&... args) {
        if (level < log_level)
            return;

        std::size_t buf_size;
        char *local_buf = get_thread_local_buf(buf_size);
        std::pmr::monotonic_buffer_resource res(local_buf, buf_size);
        std::pmr::string buf(&res);

        // Avoid the impact of address alignment
        if (buf_size > 64) [[likely]]
            buf.reserve(buf_size - 64);

        if (display_time)
            format_time(buf);

        if (display_level) {
            if (aligned_level)
                buf.append(get_aligned_level_str(level));
            else
                buf.append(get_level_str(level));
        }

        if (display_location)
            format_location(buf, loc);

        std::format_to(std::back_inserter(buf), str, std::forward<Args>(args)...);
        buf.append(line_end);

        this->do_log(buf, 0);
    }

protected:
    virtual void do_log(std::string_view sv, int flags) = 0;

    void format_time(std::pmr::string &buf);

    void format_location(std::pmr::string &buf,
                         const std::source_location &loc) {
        const char *filename;

        if (split_filename)
            filename = do_split_filename(loc.file_name());
        else
            filename = loc.file_name();

        buf.append(filename).append(":")
            .append(std::to_string(loc.line()))
            .append(" ");
    }

protected:
    bool display_time{true};
    bool display_level{true};
    bool aligned_level{true};
    bool display_location{true};
    bool split_filename{true};

    int log_level{info};

    std::string line_end{"\n"};
    std::shared_ptr<void> ctx;
};

constexpr const char *
Logger::do_split_filename(const char *p) {
    const char *q = p;

    while (*p) {
        if (*p == KLOG_PATH_DELIMITER)
            q = p + 1;
        ++p;
    }

    return q;
}

Logger *get_default_logger_raw();

void set_default_logger(std::shared_ptr<Logger> logger);

inline const char *Logger::get_level_str(int level) {
    switch (level) {
    case trace: return "TRACE ";
    case debug: return "DEBUG ";
    case info:  return "INFO ";
    case warn:  return "WARN ";
    case error: return "ERROR ";
    default:    return "INVAL ";
    }
}

inline const char *Logger::get_aligned_level_str(int level) {
    switch (level) {
    case trace: return "TRACE ";
    case debug: return "DEBUG ";
    case info:  return "INFO  ";
    case warn:  return "WARN  ";
    case error: return "ERROR ";
    default:    return "INVAL ";
    }
}

} // namespace klog

#endif // KLOG_LOGGER_BASE_H
