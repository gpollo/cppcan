#include "spdlog/sinks/stdout_color_sinks.h"

#include "can/log.hpp"

namespace can {

std::shared_ptr<spdlog::logger> logger = spdlog::stdout_color_mt("libcan", spdlog::color_mode::automatic);

} /* namespace can */