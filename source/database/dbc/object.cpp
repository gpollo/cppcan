#include "can/database/dbc/object.hpp"

namespace can::database::dbc {

object::object(std::map<std::string, int64_t> integer_attributes, std::map<std::string, float> float_attributes,
               std::map<std::string, std::string> string_attributes)
    : integer_attributes_(std::move(integer_attributes)),
      float_attributes_(std::move(float_attributes)),
      string_attributes_(std::move(string_attributes)) {}

} /* namespace can::database::dbc */