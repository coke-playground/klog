#ifndef KLOG_FILE_LOGGER_H
#define KLOG_FILE_LOGGER_H

#include <cstdio>

#include "klog/logger_base.h"

namespace klog {

class FileLogger : public Logger {
public:
    FileLogger(std::FILE *file)
        : file(file)
    { }

    FileLogger(std::shared_ptr<std::FILE> file_ptr)
        : file(file_ptr.get())
    {
        ctx = std::move(file_ptr);
    }

    virtual ~FileLogger() = default;

protected:
    virtual void do_log(std::string_view sv, int) override {
        std::fwrite(sv.data(), sizeof(char), sv.size(), file);
    }

protected:
    std::FILE *file;
};

inline std::shared_ptr<FileLogger>
make_file_logger(std::FILE *file) {
    return std::make_shared<FileLogger>(file);
}

inline std::shared_ptr<FileLogger>
make_file_logger(std::string filepath, const char *mode = "a") {
    std::FILE *pfile = std::fopen(filepath.c_str(), mode);
    if (!pfile)
        return nullptr;

    std::shared_ptr<std::FILE> file_ptr(pfile, [](std::FILE *p) { std::fclose(p); });
    return std::make_shared<FileLogger>(std::move(file_ptr));
}

} // namespace klog

#endif // KLOG_FILE_LOGGER_H
