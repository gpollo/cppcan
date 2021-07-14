#include "can/database/message.hpp"
#include "can/log.hpp"

namespace can::database {

message::message() : quark_(utils::quark::get_next()) {}

quark message::get_quark() const {
    return quark_;
}

std::vector<std::pair<signal::const_ptr, float>> message::decode(uint8_t* bytes, size_t length) const {
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

    std::optional<uint64_t> multiplexing_value;
    if (multiplexer != nullptr) {
        multiplexing_value = multiplexer->extract_raw_value(bytes, length);
    }

    std::vector<std::pair<signal::const_ptr, float>> values;
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

        values.emplace_back(signal, signal->decode(bytes, length));
    }

    return values;
}

} /* namespace can::database */