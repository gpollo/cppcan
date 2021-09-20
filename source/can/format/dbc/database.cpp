#include <memory>

#include "lexy/action/parse.hpp"
#include "lexy/encoding.hpp"
#include "lexy/input/file.hpp"
#include "lexy_ext/report_error.hpp"

#include "can/format/dbc/database.hpp"
#include "can/format/dbc/grammar/database.hpp"
#include "can/log.hpp"

namespace can::format::dbc {

database::ptr database::from_path(const std::filesystem::path& path) {
    auto file = lexy::read_file<lexy::utf8_encoding>(path.string().c_str());
    if (!file) {
        logger->error("failed to open file '{}' for reading", path.string().c_str());
        return nullptr;
    }

    auto ast = lexy::parse<grammar::database>(file.buffer(), lexy_ext::report_error);
    if (!ast.has_value()) {
        logger->error("failed to parse file '{}'", path.string().c_str());
        return nullptr;
    }

    database::ptr new_database = nullptr;
    try {
        new_database = from_ast(*ast.value());
    } catch (const std::string& error) {
        logger->error("failed to create database from AST");
        logger->error("--> {}", error);
    }

    delete ast.value();
    return new_database;
}

database::ptr database::from_ast(const ast::database& database) {
    /* TODO: check for duplicate nodes, messages, signals, etc. */

    std::map<unsigned int, dbc::message::ptr> messages_by_identifier;
    std::map<std::string, dbc::message::ptr> messages_by_name;
    for (const auto& [identifier, message] : database.get_messages()) {
        auto message_ptr = dbc::message::from_ast(database, message);
        messages_by_identifier.insert({message_ptr->get_identifier(), message_ptr});
        messages_by_name.insert({message_ptr->get_name(), message_ptr});
    }

    std::map<std::string, node::ptr> nodes_dbc;
    auto nodes_opt = database.get_nodes();
    if (nodes_opt.has_value()) {
        for (const auto& node : nodes_opt.value().get_values()) {
            auto node_ptr = node::from_ast(database, node);
            nodes_dbc.insert({node_ptr->get_name(), node_ptr});
        }
    }

    auto integer_attributes = database.get_integer_attributes();
    auto float_attributes   = database.get_float_attributes();
    auto string_attributes  = database.get_string_attributes();

    return std::make_shared<dbc::database>(database, messages_by_identifier, messages_by_name, nodes_dbc,
                                           integer_attributes, float_attributes, string_attributes);
}

database::database(const ast::database& database, std::map<unsigned int, dbc::message::ptr> messages_by_identifier,
                   std::map<std::string, dbc::message::ptr> messages_by_name,
                   std::map<std::string, node::ptr> nodes_dbc, std::map<std::string, int64_t> integer_attributes,
                   std::map<std::string, float> float_attributes, std::map<std::string, std::string> string_attributes)
    : object(std::move(integer_attributes), std::move(float_attributes), std::move(string_attributes)),
      messages_by_identifier_(std::move(messages_by_identifier)),
      messages_by_name_(std::move(messages_by_name)),
      nodes_dbc_(std::move(nodes_dbc)) {}

std::vector<can::database::message::const_ptr> database::get_messages() const {
    std::vector<can::database::message::const_ptr> messages;
    for (auto [identifier, message] : messages_by_identifier_) {
        messages.push_back(message);
    }

    return messages;
}

can::database::message::const_ptr database::get_message(unsigned int identifier) const {
    return get_message_dbc(identifier);
}

can::database::message::const_ptr database::get_message(const std::string& name) const {
    return get_message_dbc(name);
}

std::vector<dbc::message::const_ptr> database::get_messages_dbc() const {
    std::vector<dbc::message::const_ptr> messages;
    for (auto [identifier, message] : messages_by_identifier_) {
        messages.push_back(message);
    }

    return messages;
}

dbc::message::const_ptr database::get_message_dbc(unsigned int identifier) const {
    if (messages_by_identifier_.contains(identifier)) {
        return messages_by_identifier_.at(identifier);
    }

    return nullptr;
}

dbc::message::const_ptr database::get_message_dbc(const std::string& name) const {
    if (messages_by_name_.contains(name)) {
        return messages_by_name_.at(name);
    }

    return nullptr;
}

} /* namespace can::format::dbc */