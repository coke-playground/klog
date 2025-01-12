#ifndef KLOG_NULL_LOGGER_H
#define KLOG_NULL_LOGGER_H

#include "klog/logger_base.h"

namespace klog {

class NullLogger : public Logger {
public:
    NullLogger() = default;
    virtual ~NullLogger() = default;

protected:
    virtual void do_log(std::string_view sv, int) override { }
};

inline std::shared_ptr<NullLogger> make_null_logger() {
    return std::make_shared<NullLogger>();
}

} // namespace klog

#endif // KLOG_NULL_LOGGER_H
