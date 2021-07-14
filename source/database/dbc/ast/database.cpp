#include "can/database/dbc/ast/database.hpp"

namespace can::database::dbc::ast {

bool database::add(const version& version) {
    if (version_.has_value()) {
        return false;
    }

    version_ = version;

    return true;
}

bool database::add(const requirements& requirements) {
    if (requirements_.has_value()) {
        return false;
    }

    requirements_ = requirements;

    return true;
}

bool database::add(const speed& speed) {
    if (speed_.has_value()) {
        return false;
    }

    speed_ = speed;

    return true;
}

bool database::add(const nodes& nodes) {
    if (nodes_.has_value()) {
        return false;
    }

    nodes_ = nodes;

    return true;
}

bool database::add(const message& message) {
    if (messages_.contains(message.get_identifier())) {
        return false;
    }

    messages_.insert({message.get_identifier(), message});

    return true;
}

bool database::add(const description_variant& description) {
    return std::visit([this](const auto& value) -> bool { return this->add(value); }, description);
}

bool database::add(const database_description& database_description) {
    if (database_description_.has_value()) {
        return false;
    }

    database_description_ = database_description;

    return true;
}

bool database::add(const node_description& node_description) {
    auto node = node_description.get_name();

    if (node_descriptions_.contains(node)) {
        return false;
    }

    node_descriptions_.insert({node, node_description});

    return true;
}

bool database::add(const message_description& message_description) {
    auto message = message_description.get_identifier();

    if (message_descriptions_.contains(message)) {
        return false;
    }

    message_descriptions_.insert({message, message_description});

    return true;
}

bool database::add(const signal_description& signal_description) {
    auto message = signal_description.get_message();
    auto signal  = signal_description.get_signal();

    if (signal_descriptions_.contains(message)) {
        if (signal_descriptions_.at(message).contains(signal)) {
            return false;
        }
    } else {
        signal_descriptions_.insert({message, {}});
    }

    signal_descriptions_.at(message).insert({signal, signal_description});

    return true;
}

bool database::add(const attribute_definition& attribute_definition) {
    auto attribute = attribute_definition.get_name();

    if (attribute_definitions_.contains(attribute)) {
        return false;
    }

    attribute_definitions_.insert({attribute, attribute_definition});

    return true;
}

bool database::add(const attribute_definition_default& attribute_definition_default) {
    auto attribute = attribute_definition_default.get_name();

    if (attribute_definition_defaults_.contains(attribute)) {
        return false;
    }

    attribute_definition_defaults_.insert({attribute, attribute_definition_default});

    return true;
}

bool database::add(const attribute_variant& attribute) {
    return std::visit([this](const auto& value) -> bool { return this->add(value); }, attribute);
}

bool database::add(const database_attribute& database_attribute) {
    auto attribute = database_attribute.get_name();

    if (database_attributes_.contains(attribute)) {
        return false;
    }

    database_attributes_.insert({attribute, database_attribute});

    return false;
}

bool database::add(const node_attribute& node_attribute) {
    auto attribute = node_attribute.get_name();
    auto node      = node_attribute.get_node();

    if (node_attributes_.contains(node)) {
        if (node_attributes_.at(node).contains(attribute)) {
            return false;
        }
    } else {
        node_attributes_.insert({node, {}});
    }

    node_attributes_.at(node).insert({attribute, node_attribute});

    return false;
}

bool database::add(const message_attribute& message_attribute) {
    auto attribute = message_attribute.get_name();
    auto message   = message_attribute.get_message();

    if (message_attributes_.contains(message)) {
        if (message_attributes_.at(message).contains(attribute)) {
            return false;
        }
    } else {
        message_attributes_.insert({message, {}});
    }

    message_attributes_.at(message).insert({attribute, message_attribute});

    return false;
}

bool database::add(const signal_attribute& signal_attribute) {
    auto attribute = signal_attribute.get_name();
    auto message   = signal_attribute.get_message();
    auto signal    = signal_attribute.get_signal();

    if (signal_attributes_.contains(message)) {
        if (signal_attributes_.at(message).contains(signal)) {
            if (signal_attributes_.at(message).at(signal).contains(attribute)) {
                return false;
            }
        } else {
            signal_attributes_.at(message).insert({signal, {}});
        }
    } else {
        signal_attributes_.insert({message, {}});
        signal_attributes_.at(message).insert({signal, {}});
    }

    signal_attributes_[message][signal].insert({attribute, signal_attribute});

    return false;
}

bool database::add(const value_definitions& value_definitions) {
    auto message = value_definitions.get_message();
    auto signal  = value_definitions.get_signal();

    if (value_definitions_.contains(message)) {
        if (value_definitions_.at(message).contains(signal)) {
            return false;
        }
    } else {
        value_definitions_.insert({message, {}});
    }

    value_definitions_.at(message).insert({signal, value_definitions});

    return true;
}

bool database::add(const value_table_definition& value_table_definition) {
    auto name = value_table_definition.get_name();

    if (value_table_definitions_.contains(name)) {
        return false;
    }

    value_table_definitions_.insert({name, value_table_definition});

    return true;
}

const std::optional<nodes>& database::get_nodes() const {
    return nodes_;
}

const std::map<unsigned int, message>& database::get_messages() const {
    return messages_;
}

const attribute_definition* database::get_attribute_definition(const std::string& name) const {
    if (!attribute_definitions_.contains(name)) {
        return nullptr;
    }

    return &attribute_definitions_.at(name);
}

const attribute_definition_default* database::get_attribute_definition_default(const std::string& name) const {
    if (!attribute_definition_defaults_.contains(name)) {
        return nullptr;
    }

    return &attribute_definition_defaults_.at(name);
}

void database::parse_attribute(std::map<std::string, int64_t>& attributes, const attribute_definition& attr_def,
                               const std::string& attr_text) const {
    const auto integer_attr_def = attr_def.get_integer();
    if (integer_attr_def.has_value()) {
        auto value = integer_attr_def.value().parse(attr_text);
        attributes.insert({attr_def.get_name(), value});
    }
}

void database::parse_attribute(std::map<std::string, float>& attributes, const attribute_definition& attr_def,
                               const std::string& attr_text) const {
    const auto floating_point_attr_def = attr_def.get_floating_point();
    if (floating_point_attr_def.has_value()) {
        auto value = floating_point_attr_def.value().parse(attr_text);
        attributes.insert({attr_def.get_name(), value});
    }
}

void database::parse_attribute(std::map<std::string, std::string>& attributes, const attribute_definition& attr_def,
                               const std::string& attr_text) const {
    const auto string_attr_def = attr_def.get_string();
    if (string_attr_def.has_value()) {
        auto value = string_attr_def.value().parse(attr_text);
        attributes.insert({attr_def.get_name(), value});
    }

    const auto enumeration_attr_def = attr_def.get_enumeration();
    if (enumeration_attr_def.has_value()) {
        auto value = enumeration_attr_def.value().parse(attr_text);
        attributes.insert({attr_def.get_name(), value});
    }
}

template <typename T, typename A>
std::map<std::string, T> database::get_attributes(const std::map<std::string, A>& attributes) const {
    /* TODO: add default values if attribute not found */

    std::map<std::string, T> attrs;
    for (const auto& attr_pair : attributes) {
        const auto& attr      = attr_pair.second;
        const auto& attr_name = attr.get_name();
        const auto& attr_text = attr.get_text();

        const auto* attr_def = get_attribute_definition(attr_name);
        if (attr_def == nullptr) {
            throw std::string("no attribute definition for '" + attr_name + "'");
        }

        ast::attribute_definition::object_type expected;
        if (std::is_same<A, ast::database_attribute>::value) {
            expected = ast::attribute_definition::object_type::DATABASE;
        } else if (std::is_same<A, ast::node_attribute>::value) {
            expected = ast::attribute_definition::object_type::NODE;
        } else if (std::is_same<A, ast::message_attribute>::value) {
            expected = ast::attribute_definition::object_type::MESSAGE;
        } else if (std::is_same<A, ast::signal_attribute>::value) {
            expected = ast::attribute_definition::object_type::SIGNAL;
        } else {
            throw std::string("unexpected attribute type");
        }

        if (attr_def->get_object() != expected) {
            continue;
        }

        parse_attribute(attrs, *attr_def, attr_text);
    }

    return attrs;
}

template <typename T>
std::map<std::string, T> database::get_database_attributes() const {
    return get_attributes<T>(database_attributes_);
}

template <typename T>
std::map<std::string, T> database::get_node_attributes(const std::string& node) const {
    if (!node_attributes_.contains(node)) {
        return {};
    }

    return get_attributes<T>(node_attributes_.at(node));
}

template <typename T>
std::map<std::string, T> database::get_message_attributes(unsigned int message) const {
    if (!message_attributes_.contains(message)) {
        return {};
    }

    return get_attributes<T>(message_attributes_.at(message));
}

template <typename T>
std::map<std::string, T> database::get_signal_attributes(unsigned int message, const std::string& signal) const {
    if (!signal_attributes_.contains(message)) {
        return {};
    }

    if (!signal_attributes_.at(message).contains(signal)) {
        return {};
    }

    return get_attributes<T>(signal_attributes_.at(message).at(signal));
}

std::map<std::string, int64_t> database::get_integer_attributes(/* database */) const {
    return get_database_attributes<int64_t>();
}

std::map<std::string, int64_t> database::get_integer_attributes(const std::string& node) const {
    return get_node_attributes<int64_t>(node);
}

std::map<std::string, int64_t> database::get_integer_attributes(const message& message) const {
    return get_message_attributes<int64_t>(message.get_identifier());
}

std::map<std::string, int64_t> database::get_integer_attributes(const message& message, const signal& signal) const {
    return get_signal_attributes<int64_t>(message.get_identifier(), signal.get_name());
}

std::map<std::string, float> database::get_float_attributes(/* database */) const {
    return get_database_attributes<float>();
}

std::map<std::string, float> database::get_float_attributes(const std::string& node) const {
    return get_node_attributes<float>(node);
}

std::map<std::string, float> database::get_float_attributes(const message& message) const {
    return get_message_attributes<float>(message.get_identifier());
}

std::map<std::string, float> database::get_float_attributes(const message& message, const signal& signal) const {
    return get_signal_attributes<float>(message.get_identifier(), signal.get_name());
}

std::map<std::string, std::string> database::get_string_attributes(/* database */) const {
    return get_database_attributes<std::string>();
}

std::map<std::string, std::string> database::get_string_attributes(const std::string& node) const {
    return get_node_attributes<std::string>(node);
}

std::map<std::string, std::string> database::get_string_attributes(const message& message) const {
    return get_message_attributes<std::string>(message.get_identifier());
}

std::map<std::string, std::string> database::get_string_attributes(const message& message, const signal& signal) const {
    return get_signal_attributes<std::string>(message.get_identifier(), signal.get_name());
}

std::map<uint64_t, std::string> database::get_signal_values(const message& message, const signal& signal) const {
    const auto identifier = message.get_identifier();
    const auto& name      = signal.get_name();

    if (!value_definitions_.contains(identifier)) {
        return {};
    }

    if (!value_definitions_.at(identifier).contains(name)) {
        return {};
    }

    auto value_definitions                         = value_definitions_.at(identifier).at(name).get_table();
    const ast::value_definitions::entries* entries = nullptr;
    if (std::holds_alternative<std::string>(value_definitions)) {
        auto& table_name = std::get<std::string>(value_definitions);
        if (value_table_definitions_.contains(table_name)) {
            entries = &value_table_definitions_.at(table_name).get_entries();
        }
    } else {
        entries = &std::get<ast::value_definitions::entries>(value_definitions);
    }

    if (entries == nullptr) {
        return {};
    }

    return std::map<uint64_t, std::string>(entries->begin(), entries->end());
}

} /* namespace can::database::dbc::ast */