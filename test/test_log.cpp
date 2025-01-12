#include <chrono>
#include <iostream>
#include <thread>
#include <memory>
#include <vector>

#include "klog/klog.h"
#include "klog/file_logger.h"
#include "klog/null_logger.h"
#include "klog/stream_logger.h"
#include "klog/fd_logger.h"

long current_usec() {
    auto now = std::chrono::steady_clock::now().time_since_epoch();
    return std::chrono::duration_cast<std::chrono::microseconds>(now).count();
}

void test_klog() {
    KLOG_INFO("{0: <+#16X} {0:*^-#20o} {1:>-12.12f} {1:.4E} {2:-^12s} {2:_>16s}",
              2147483647, 3.1415926, "text");
    KLOG_WARN("{0: <+#16X} {0:*^-#20o} {1:>-12.12f} {1:.4E} {2:-^12s} {2:_>16s}",
              2147483647, 3.1415926, "text");
    KLOG_ERROR("{0: <+#16X} {0:*^-#20o} {1:>-12.12f} {1:.4E} {2:-^12s} {2:_>16s}",
               2147483647, 3.1415926, "text");
}

constexpr int total = 1000000;
void thread_test() {
    long start, cost;

    start = current_usec();
    for (int i = 0; i < total; i++)
        test_klog();
    cost = current_usec() - start;

    std::cout << cost << std::endl;
}

void init(int type) {
    std::shared_ptr<klog::Logger> logger;

    switch (type) {
    case 0: {
        std::cout << "stream logger" << std::endl;
        logger = klog::make_stream_logger("/dev/null", std::ios_base::out);
        break;
    }
    case 1: {
        std::cout << "file logger" << std::endl;
        logger = klog::make_file_logger("/dev/null", "w");
        break;
    }
    case 2: {
        std::cout << "fd logger" << std::endl;
        logger = klog::make_fd_logger("/dev/null", O_WRONLY|O_CREAT);
        break;
    }
    default: {
        std::cout << "null logger" << std::endl;
        logger = klog::make_null_logger();
        break;
    }
    }

    klog::set_default_logger(std::move(logger));
}

int main(int argc, char *argv[]) {
    int type = 0;
    if (argc > 1)
        type = std::atoi(argv[1]);

    init(type);

    int N = 4;
    std::vector<std::thread> vth;
    for (int i = 0; i < N; i++) {
        vth.emplace_back(thread_test);
    }

    for (int i = 0; i < N; i++) {
        vth[i].join();
    }

    return 0;
}
