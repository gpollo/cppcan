#ifndef INCLUDE_CAN_LOG_HPP
#define INCLUDE_CAN_LOG_HPP

#include <spdlog/spdlog.h>

namespace can {

extern std::shared_ptr<spdlog::logger> logger;

} /* namespace can */

#endif /* INCLUDE_CAN_LOG_HPP */