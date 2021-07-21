#include "can/format/dbc/node.hpp"

namespace can::format::dbc {

node::ptr node::from_ast(const ast::database& database, std::string name) {
    auto integer_attributes = database.get_integer_attributes(name);
    auto float_attributes   = database.get_float_attributes(name);
    auto string_attributes  = database.get_string_attributes(name);

    return std::make_shared<node>(std::move(name), integer_attributes, float_attributes, string_attributes);
}

node::node(std::string name, std::map<std::string, int64_t> integer_attributes,
           std::map<std::string, float> float_attributes, std::map<std::string, std::string> string_attributes)
    : object(std::move(integer_attributes), std::move(float_attributes), std::move(string_attributes)),
      name_(std::move(name)) {}

} /* namespace can::format::dbc */