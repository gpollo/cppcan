#include "can/database/dbc/ast/attribute_definition.hpp"

namespace can::database::dbc::ast {

/* attribute_definition::string class */

attribute_definition::string::string(std::string value) : value_(std::move(value)) {}

/* attribute_definition::enumeration class */

attribute_definition::enumeration::enumeration(std::vector<std::string> values) : values_(std::move(values)) {}

std::string attribute_definition::enumeration::parse(const std::string& text) const {
    if (std::find(values_.begin(), values_.end(), text) != values_.end()) {
        return text;
    }

    throw std::string("invalid enumeration attribute specified");
}

/* attribute_definition class */

attribute_definition::attribute_definition(object_type object, std::string name, data_types data)
    : name_(std::move(name)), object_(object), data_(std::move(data)) {}

std::optional<attribute_definition::integer> attribute_definition::get_integer() const {
    if (!std::holds_alternative<integer>(data_)) {
        return {};
    }

    return std::get<integer>(data_);
}

std::optional<attribute_definition::floating_point> attribute_definition::get_floating_point() const {
    if (!std::holds_alternative<floating_point>(data_)) {
        return {};
    }

    return std::get<floating_point>(data_);
}

std::optional<attribute_definition::string> attribute_definition::get_string() const {
    if (!std::holds_alternative<string>(data_)) {
        return {};
    }

    return std::get<string>(data_);
}

std::optional<attribute_definition::enumeration> attribute_definition::get_enumeration() const {
    if (!std::holds_alternative<enumeration>(data_)) {
        return {};
    }

    return std::get<enumeration>(data_);
}

} /* namespace can::database::dbc::ast */