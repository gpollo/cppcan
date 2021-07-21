#include "can/format/dbc/ast/message.hpp"

namespace can::format::dbc::ast {

message::message(unsigned int identifier, std::string name, unsigned short byte_count, std::string node,
                 std::optional<std::vector<signal>> signals)
    : name_(std::move(name)),
      identifier_(identifier),
      byte_count_(byte_count),
      signals_(signals.value_or(std::vector<signal>{})),
      node_(std::move(node)) {}

} /* namespace can::format::dbc::ast */