#include <cassert>
#include <string>
#include <variant>

#include "lexy/action/parse.hpp"
#include "lexy/input/string_input.hpp"

#include "can/format/dbc/grammar/description.hpp"

static constexpr auto ignore_error = lexy::callback([](const auto& context, const auto& error) {});

static auto parse(auto input) {
    return lexy::parse<can::format::dbc::grammar::description>(input, ignore_error);
}

static void test_node_description() {
    char input[]         = "CM_ BU_ DRIVER \"test description of node\";\n";
    auto description_opt = parse(lexy::string_input<lexy::utf8_encoding>(input, sizeof(input)));
    assert(description_opt.has_value());
    auto description = description_opt.value();

    assert(std::holds_alternative<can::format::dbc::ast::node_description>(description));
    auto node_description = std::get<can::format::dbc::ast::node_description>(description);

    assert(node_description.get_name() == "DRIVER");
    assert(node_description.get_description() == "test description of node");
}

static void test_message_description() {
    char input[]         = "CM_ BO_ 1024 \"test description of message\";\n";
    auto description_opt = parse(lexy::string_input<lexy::utf8_encoding>(input, sizeof(input)));
    assert(description_opt.has_value());
    auto description = description_opt.value();

    assert(std::holds_alternative<can::format::dbc::ast::message_description>(description));
    auto message_description = std::get<can::format::dbc::ast::message_description>(description);

    assert(message_description.get_identifier() == 1024);
    assert(message_description.get_description() == "test description of message");
}

static void test_signal_description() {
    char input[]         = "CM_ SG_   1024  TEST_SIGNAL_NAME \"test description of signal\";\n";
    auto description_opt = parse(lexy::string_input<lexy::utf8_encoding>(input, sizeof(input)));
    assert(description_opt.has_value());
    auto description = description_opt.value();

    assert(std::holds_alternative<can::format::dbc::ast::signal_description>(description));
    auto signal_description = std::get<can::format::dbc::ast::signal_description>(description);

    assert(signal_description.get_message() == 1024);
    assert(signal_description.get_signal() == "TEST_SIGNAL_NAME");
    assert(signal_description.get_description() == "test description of signal");
}

static void test_database_description() {
    char input[]         = "CM_ \"test description of database\";\n";
    auto description_opt = parse(lexy::string_input<lexy::utf8_encoding>(input, sizeof(input)));
    assert(description_opt.has_value());
    auto description = description_opt.value();

    assert(std::holds_alternative<can::format::dbc::ast::database_description>(description));
    auto database_description = std::get<can::format::dbc::ast::database_description>(description);

    assert(database_description.get_description() == "test description of database");
}

int main() {
    test_node_description();
    test_message_description();
    test_signal_description();
    test_database_description();

    return 0;
}