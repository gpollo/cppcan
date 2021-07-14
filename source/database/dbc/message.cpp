#include "can/database/dbc/message.hpp"

namespace can::database::dbc {

message::ptr message::from_ast(const ast::database& database, const ast::message& message) {
    /* TODO: make sure signal bit ranges don't intersect each other unless they're multiplexed */

    std::map<std::string, signal::ptr> signals;
    for (const auto& signal : message.get_signals()) {
        auto signal_ptr = signal::from_ast(database, message, signal);
        signals.insert({signal_ptr->get_name(), signal_ptr});
    }

    auto integer_attributes = database.get_integer_attributes(message);
    auto float_attributes   = database.get_float_attributes(message);
    auto string_attributes  = database.get_string_attributes(message);

    return std::make_shared<dbc::message>(message, signals, integer_attributes, float_attributes, string_attributes);
}

message::message(const ast::message& message, std::map<std::string, signal::ptr> signals,
                 std::map<std::string, int64_t> integer_attributes, std::map<std::string, float> float_attributes,
                 std::map<std::string, std::string> string_attributes)
    : object(std::move(integer_attributes), std::move(float_attributes), std::move(string_attributes)),
      name_(message.get_name()),
      identifier_(message.get_identifier()),
      byte_count_(message.get_byte_count()),
      node_(message.get_node()),
      signals_(std::move(signals)) {}

std::vector<can::database::signal::const_ptr> message::get_signals() const {
    std::vector<can::database::signal::const_ptr> signals;
    for (auto [name, signal] : signals_) {
        signals.push_back(signal);
    }

    return signals;
}

can::database::signal::const_ptr message::get_signal(const std::string& name) const {
    return get_signal_dbc(name);
}

std::vector<dbc::signal::const_ptr> message::get_signals_dbc() const {
    std::vector<signal::const_ptr> signals;
    for (auto [name, signal] : signals_) {
        signals.push_back(signal);
    }

    return signals;
}

dbc::signal::const_ptr message::get_signal_dbc(const std::string& name) const {
    if (signals_.contains(name)) {
        return signals_.at(name);
    }

    return nullptr;
}

} /* namespace can::database::dbc */