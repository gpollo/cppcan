#include "can/database/dbc/signal.hpp"

namespace can::database::dbc {

signal::ptr signal::from_ast(const ast::database& database, const ast::message& message, const ast::signal& signal) {
    auto values             = database.get_signal_values(message, signal);
    auto integer_attributes = database.get_integer_attributes(message, signal);
    auto float_attributes   = database.get_float_attributes(message, signal);
    auto string_attributes  = database.get_string_attributes(message, signal);

    return std::make_shared<dbc::signal>(signal, values, integer_attributes, float_attributes, string_attributes);
}

signal::signal(const ast::signal& sig, std::map<uint64_t, std::string> values,
               std::map<std::string, int64_t> integer_attributes, std::map<std::string, float> float_attributes,
               std::map<std::string, std::string> string_attributes)
    : object(std::move(integer_attributes), std::move(float_attributes), std::move(string_attributes)),
      name_(sig.get_name()),
      start_bit_(sig.get_start_bit()),
      bit_count_(sig.get_bit_count()),
      byte_order_(sig.get_byte_order()),
      is_integral_(false), /* FIXME */
      is_signed_(sig.is_signed()),
      scale_(sig.get_scale()),
      offset_(sig.get_offset()),
      min_(sig.get_min()),
      max_(sig.get_max()),
      unit_(sig.get_unit()),
      nodes_(sig.get_nodes()),
      multiplexing_(sig.get_multiplexing()),
      values_(std::move(values)) {}

} /* namespace can::database::dbc */