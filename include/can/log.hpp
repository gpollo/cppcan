#ifndef INCLUDE_CAN_LOG_HPP
#define INCLUDE_CAN_LOG_HPP

#include <cerrno>
#include <cstdio>

/* NOLINTNEXTLINE(cppcoreguidelines-macro-usage): required for __FILE__ and __LINE__ */
#define CAN_LOG_PERROR(func) fprintf(stderr, "%s:%d:" func ": %s\n", __FILE__, __LINE__, strerror(errno))

/* NOLINTNEXTLINE(cppcoreguidelines-macro-usage): required for __FILE__ and __LINE__ */
#define CAN_LOG_ERROR(func_n_reason, ...) \
    fprintf(stderr, "%s:%d:" func_n_reason "\n", __FILE__, __LINE__, ##__VA_ARGS__)

#endif /* INCLUDE_CAN_LOG_HPP */