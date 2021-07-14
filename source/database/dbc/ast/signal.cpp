#include "can/database/dbc/ast/signal.hpp"

namespace can::database::dbc::ast {

signal::signal(std::string name, unsigned char start_bit, unsigned char bit_count, endian byte_order, bool is_signed,
               float scale, float offset, float min, float max, std::string unit, std::vector<std::string> nodes,
               std::variant<bool, unsigned short> multiplexing)
    : name_(std::move(name)),
      start_bit_(start_bit),
      bit_count_(bit_count),
      byte_order_(byte_order),
      is_signed_(is_signed),
      scale_(scale),
      offset_(offset),
      min_(min),
      max_(max),
      unit_(std::move(unit)),
      nodes_(std::move(nodes)),
      multiplexing_(multiplexing) {}

} /* namespace can::database::dbc::ast */