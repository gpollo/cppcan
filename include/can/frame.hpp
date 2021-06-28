#ifndef INCLUDE_CAN_MESSAGE_HPP
#define INCLUDE_CAN_MESSAGE_HPP

#include <cstdint>
#include <memory>

namespace can {

struct frame {
    using ptr = std::unique_ptr<frame>;

    uint32_t identifier_;
    uint64_t timestamp_;
    size_t length_;

    /* NOLINTNEXTLINE(modernize-avoid-c-arrays): flexible array member */
    uint8_t bytes_[];

    static ptr create(uint32_t identifier, size_t length, uint8_t* bytes, uint64_t timestamp = 0);
};

} /* namespace can */

#endif /* INCLUDE_CAN_MESSAGE_HPP */