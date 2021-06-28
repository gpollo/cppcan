#include <algorithm>

#include "can/frame.hpp"

namespace can {

frame::ptr frame::create(uint32_t identifier, size_t length, uint8_t* bytes, uint64_t timestamp) {
    /* NOLINTNEXTLINE(cppcoreguidelines-pro-type-cstyle-cast): required for flexible array member */
    auto* ptr        = (frame*)(new uint8_t[sizeof(frame) + length]);
    ptr->timestamp_  = timestamp;
    ptr->identifier_ = identifier;
    ptr->length_     = length;
    /* NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic): much simpler */
    std::copy(bytes, bytes + length, ptr->bytes_);
    return frame::ptr(ptr);
}

} /* namespace can */