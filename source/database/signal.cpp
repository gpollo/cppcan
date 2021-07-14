#include "can/database/signal.hpp"

namespace can::database {

signal::signal() : quark_(utils::quark::get_next()) {}

quark signal::get_quark() const {
    return quark_;
}

bool signal::is_multiplexer() const {
    auto multiplexing = get_multiplexing();

    auto* boolean = std::get_if<bool>(&multiplexing);
    if (boolean != nullptr) {
        return *boolean;
    }

    return false;
}

std::optional<unsigned short> signal::get_multiplexed_value() const {
    auto multiplexing = get_multiplexing();

    if (std::holds_alternative<unsigned short>(multiplexing)) {
        return std::get<unsigned short>(multiplexing);
    }

    return {};
}

float signal::decode(uint8_t* bytes, size_t length) const {
    auto raw_value = extract_raw_value(bytes, length);
    return convert_raw_value(raw_value);
}

std::variant<float, std::string> signal::decode_and_resolve(uint8_t* bytes, size_t length) const {
    auto raw_value = extract_raw_value(bytes, length);
    auto resolved  = resolve_value(raw_value);
    if (!resolved.empty()) {
        return resolved;
    }

    return convert_raw_value(raw_value);
}

uint64_t signal::extract_raw_value(uint8_t* bytes, size_t length) const {
    const auto bit_count = get_bit_count();
    const auto start_bit = get_start_bit();
    const auto end_bit   = start_bit + bit_count;

    const auto byte_start = start_bit / 8;
    const auto byte_end   = end_bit / 8;

    /* TODO: add support for big-endian packed message */

    unsigned int bit_packed = 0;
    unsigned long value     = 0;
    for (auto i = byte_start; i < byte_end + 1; i++) {
        if (i == byte_start) {
            value |= bytes[i] >> (start_bit - 8 * byte_start);
            bit_packed += (8 * byte_end - start_bit);
        } else {
            value |= bytes[i] << bit_packed;
            bit_packed += 8;
        }
    }

    return value & ((1 << bit_count) - 1);
}

float signal::convert_raw_value(unsigned long raw_value) const {
    const auto scale  = get_scale();
    const auto offset = get_offset();

    return (static_cast<float>(raw_value) * scale) + offset;
}

} /* namespace can::database */