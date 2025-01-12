#ifndef KLOG_STREAM_LOGGER_H
#define KLOG_STREAM_LOGGER_H

#include <ostream>
#include <fstream>
#include <mutex>

#include "klog/logger_base.h"

namespace klog {

template<typename Mutex>
class StreamLogger : public Logger {
public:
    StreamLogger(std::ostream *os)
        : os(os)
    { }

    StreamLogger(std::shared_ptr<std::ostream> os_ptr)
        : os(os_ptr.get())
    {
        ctx = std::move(os_ptr);
    }

    virtual ~StreamLogger() = default;

protected:
    virtual void do_log(std::string_view sv, int) override {
        std::lock_guard<Mutex> lg(mtx);
        os->write(sv.data(), (std::streamsize)sv.size());
    }

protected:
    Mutex mtx;
    std::ostream *os;
};

struct NullMutex {
    void lock() { }
    void unlock() { }
};

template<typename Mutex=std::mutex>
inline std::shared_ptr<StreamLogger<Mutex>>
make_stream_logger(std::ostream *os) {
    return std::make_shared<StreamLogger<Mutex>>(os);
}

template<typename Mutex=std::mutex>
inline std::shared_ptr<StreamLogger<Mutex>>
make_stream_logger(std::string filepath,
                   std::ios_base::openmode mode = std::ios_base::out | std::ios_base::app) {
    auto ofs_ptr = std::make_shared<std::ofstream>(filepath, mode);
    if (!ofs_ptr->is_open())
        return nullptr;

    return std::make_shared<StreamLogger<Mutex>>(std::move(ofs_ptr));
}

} // namespace klog

#endif // KLOG_STREAM_LOGGER_H
