#ifndef KLOG_FD_LOGGER_H
#define KLOG_FD_LOGGER_H

#include <unistd.h>
#include <fcntl.h>

#include "klog/logger_base.h"

namespace klog {

class FdLogger : public Logger {
public:
    FdLogger(int fd)
        : fd(fd)
    { }

    FdLogger(std::shared_ptr<int> fd_ptr)
        : fd(*(fd_ptr.get()))
    {
        ctx = std::move(fd_ptr);
    }

protected:
    virtual void do_log(std::string_view sv, int) override {
        (void)write(fd, sv.data(), sv.size());
    }

protected:
    int fd;
};

inline std::shared_ptr<FdLogger>
make_fd_logger(int fd) {
    return std::make_shared<FdLogger>(fd);
}

inline std::shared_ptr<FdLogger>
make_fd_logger(std::string filepath, int flags = O_WRONLY|O_CREAT,
               mode_t mode = 0644) {
    int fd = open(filepath.c_str(), flags, mode);
    if (fd < 0)
        return nullptr;

    std::shared_ptr<int> fd_ptr(new int(fd), [](int *pfd) { close(*pfd); });
    return std::make_shared<FdLogger>(std::move(fd_ptr));
}

} // namespace klog

#endif // KLOG_FD_LOGGER_H
