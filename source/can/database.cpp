#include "can/database.hpp"
#include <math.h>
#include "can/format/dbc/database.hpp"
#include "can/log.hpp"

namespace can {

/* can::database::signal class */

database::signal::signal() : quark_(utils::quark::get_next()) {}

quark database::signal::get_quark() const {
    return quark_;
}

uint64_t database::signal::extract(uint8_t* bytes, size_t length) const {
    const auto bit_count = get_bit_count();
    const auto start_bit = get_start_bit();
    const auto end_bit   = start_bit + bit_count;

    const auto byte_start = start_bit / 8U;
    const auto byte_end   = end_bit / 8U;

    if (byte_end >= length) {
        logger->error("frame of {} byte{} is too small to decode signal '{}'", length, (length > 1) ? "s" : "",
                      get_name());
        return 0;
    }

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

float database::signal::decode(uint64_t raw_value) const {
    const auto scale  = get_scale();
    const auto offset = get_offset();

    return (static_cast<float>(raw_value) * scale) + offset;
}

bool database::signal::is_multiplexer() const {
    auto multiplexing = get_multiplexing();

    auto* boolean = std::get_if<bool>(&multiplexing);
    if (boolean != nullptr) {
        return *boolean;
    }

    return false;
}

std::optional<unsigned short> database::signal::get_multiplexed_value() const {
    auto multiplexing = get_multiplexing();

    if (std::holds_alternative<unsigned short>(multiplexing)) {
        return std::get<unsigned short>(multiplexing);
    }

    return {};
}

/* can::database::message class */

database::message::message() : quark_(utils::quark::get_next()) {}

quark database::message::get_quark() const {
    return quark_;
}

std::vector<std::pair<database::signal::const_ptr, uint64_t>> database::message::extract(uint8_t* bytes,
                                                                                         size_t length) const {
    if (get_byte_count() < length) {
        logger->warn("frame of {} byte{} is too small to decode message '{}'", length, (length > 1) ? "s" : "",
                     get_name());
        return {};
    }

    auto signals = get_signals();

    signal::const_ptr multiplexer = nullptr;
    for (auto signal : signals) {
        if (!signal->is_multiplexer()) {
            continue;
        }

        if (multiplexer != nullptr) {
            logger->error("multiple multiplexer in signal '{}'", get_name());
            continue;
        }

        multiplexer = signal;
    }

    std::vector<std::pair<signal::const_ptr, uint64_t>> raw_values;

    std::optional<uint64_t> multiplexing_value;
    if (multiplexer != nullptr) {
        multiplexing_value = multiplexer->extract(bytes, length);
        raw_values.emplace_back(multiplexer, multiplexing_value.value());
    }

    for (auto signal : signals) {
        if (signal == multiplexer) {
            continue;
        }

        if (multiplexing_value.has_value()) {
            auto multiplexed_value = signal->get_multiplexed_value();
            if (multiplexed_value.has_value() && multiplexing_value != multiplexed_value) {
                continue;
            }
        }

        raw_values.emplace_back(signal, signal->extract(bytes, length));
    }

    return raw_values;
}

std::vector<std::pair<database::signal::const_ptr, float>> database::message::decode(uint8_t* bytes,
                                                                                     size_t length) const {
    std::vector<std::pair<signal::const_ptr, float>> decoded_values;

    auto raw_values = extract(bytes, length);
    for (auto [signal, raw_value] : raw_values) {
        decoded_values.emplace_back(signal, signal->decode(raw_value));
    }

    return decoded_values;
}

std::vector<std::pair<database::signal::const_ptr, std::variant<float, std::string>>> database::message::resolve(
    uint8_t* bytes, size_t length) const {
    std::vector<std::pair<signal::const_ptr, std::variant<float, std::string>>> resolved_values;

    auto raw_values = extract(bytes, length);
    for (auto [signal, raw_value] : raw_values) {
        auto resolved_value = signal->resolve(raw_value);
        if (!resolved_value.empty()) {
            resolved_values.emplace_back(signal, resolved_value);
        } else {
            resolved_values.emplace_back(signal, signal->decode(raw_value));
        }
    }

    return resolved_values;
}

/* can::database class */

database::ptr database::create(const std::filesystem::path& path) {
    if (!path.has_extension()) {
        logger->error("specified file '{}' does not have an extension", path.string());
        return nullptr;
    }

    if (path.extension().string() == ".dbc") {
        auto db = can::format::dbc::database::from_path(path);
        if (db == nullptr) {
            logger->error("failed to create database from file '{}'", path.string());
        }

        return db;
    }

    logger->error("invalid extension for file '{}', expected '*.dbc'", path.string());
    return nullptr;
}

} /* namespace can */