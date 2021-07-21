#include <atomic>

#include "can/utils/quark.hpp"

static std::atomic<unsigned long> current_quark = 0;

namespace can::utils::quark {

unsigned long get_next() {
    return current_quark++;
}

} /* namespace can::utils::quark */