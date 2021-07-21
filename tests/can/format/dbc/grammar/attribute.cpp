#include <cassert>
#include <cmath>
#include <string>
#include <variant>

#include "lexy/action/parse.hpp"
#include "lexy/input/string_input.hpp"

#include "can/format/dbc/grammar/attribute.hpp"

static constexpr auto ignore_error = lexy::callback([](const auto& context, const auto& error) {});

static auto parse(auto input) {
    return lexy::parse<can::format::dbc::grammar::attribute>(input, ignore_error);
}

using db_attr   = can::format::dbc::ast::database_attribute;
using node_attr = can::format::dbc::ast::node_attribute;
using msg_attr  = can::format::dbc::ast::message_attribute;
using sig_attr  = can::format::dbc::ast::signal_attribute;

static void test_database_string_attribute() {
    char input[]       = "BA_ \"test attr\" \"string value\";\n";
    auto attribute_opt = parse(lexy::string_input<lexy::utf8_encoding>(input, sizeof(input)));
    assert(attribute_opt.has_value());
    auto attribute = attribute_opt.value();

    assert(std::holds_alternative<db_attr>(attribute));
    auto database_attribute = std::get<db_attr>(attribute);
    assert(database_attribute.get_name() == "test attr");
    assert(database_attribute.get_text() == "string value");
}

static void test_database_integer_attribute() {
    char input[]       = "BA_ \"test attr\" 123;  \n";
    auto attribute_opt = parse(lexy::string_input<lexy::utf8_encoding>(input, sizeof(input)));
    assert(attribute_opt.has_value());
    auto attribute = attribute_opt.value();

    assert(std::holds_alternative<db_attr>(attribute));
    auto database_attribute = std::get<db_attr>(attribute);
    assert(database_attribute.get_name() == "test attr");
    assert(database_attribute.get_text() == "123");
}

static void test_database_float_attribute() {
    char input[]       = "BA_ \"test attr\" 123.56;  \n";
    auto attribute_opt = parse(lexy::string_input<lexy::utf8_encoding>(input, sizeof(input)));
    assert(attribute_opt.has_value());
    auto attribute = attribute_opt.value();

    assert(std::holds_alternative<db_attr>(attribute));
    auto database_attribute = std::get<db_attr>(attribute);
    assert(database_attribute.get_name() == "test attr");
    assert(database_attribute.get_text() == "123.56");
}

static void test_node_string_attribute() {
    char input[]       = "BA_ \"test attr\" BU_ NODE_NAME \"string value\";\n";
    auto attribute_opt = parse(lexy::string_input<lexy::utf8_encoding>(input, sizeof(input)));
    assert(attribute_opt.has_value());
    auto attribute = attribute_opt.value();

    assert(std::holds_alternative<node_attr>(attribute));
    auto database_attribute = std::get<node_attr>(attribute);
    assert(database_attribute.get_name() == "test attr");
    assert(database_attribute.get_text() == "string value");
    assert(database_attribute.get_node() == "NODE_NAME");
}

static void test_node_integer_attribute() {
    char input[]       = "BA_ \"test attr\" BU_  NODE_NAME  123;  \n";
    auto attribute_opt = parse(lexy::string_input<lexy::utf8_encoding>(input, sizeof(input)));
    assert(attribute_opt.has_value());
    auto attribute = attribute_opt.value();

    assert(std::holds_alternative<node_attr>(attribute));
    auto database_attribute = std::get<node_attr>(attribute);
    assert(database_attribute.get_name() == "test attr");
    assert(database_attribute.get_text() == "123");
    assert(database_attribute.get_node() == "NODE_NAME");
}

static void test_node_float_attribute() {
    char input[]       = "BA_ \"test attr\" BU_  NODE_NAME  123.56  ;  \n";
    auto attribute_opt = parse(lexy::string_input<lexy::utf8_encoding>(input, sizeof(input)));
    assert(attribute_opt.has_value());
    auto attribute = attribute_opt.value();

    assert(std::holds_alternative<node_attr>(attribute));
    auto database_attribute = std::get<node_attr>(attribute);
    assert(database_attribute.get_name() == "test attr");
    assert(database_attribute.get_text() == "123.56");
    assert(database_attribute.get_node() == "NODE_NAME");
}

static void test_message_string_attribute() {
    char input[]       = "BA_ \"test attr\"   BO_ 1234 \"string value\";\n";
    auto attribute_opt = parse(lexy::string_input<lexy::utf8_encoding>(input, sizeof(input)));
    assert(attribute_opt.has_value());
    auto attribute = attribute_opt.value();

    assert(std::holds_alternative<msg_attr>(attribute));
    auto database_attribute = std::get<msg_attr>(attribute);
    assert(database_attribute.get_name() == "test attr");
    assert(database_attribute.get_text() == "string value");
    assert(database_attribute.get_message() == 1234);
}

static void test_message_integer_attribute() {
    char input[]       = "BA_  \"test attr\" BO_  1234  123;  \n";
    auto attribute_opt = parse(lexy::string_input<lexy::utf8_encoding>(input, sizeof(input)));
    assert(attribute_opt.has_value());
    auto attribute = attribute_opt.value();

    assert(std::holds_alternative<msg_attr>(attribute));
    auto database_attribute = std::get<msg_attr>(attribute);
    assert(database_attribute.get_name() == "test attr");
    assert(database_attribute.get_text() == "123");
    assert(database_attribute.get_message() == 1234);
}

static void test_message_float_attribute() {
    char input[]       = "BA_   \"test attr\" BO_  1234  123.56  ;  \n";
    auto attribute_opt = parse(lexy::string_input<lexy::utf8_encoding>(input, sizeof(input)));
    assert(attribute_opt.has_value());
    auto attribute = attribute_opt.value();

    assert(std::holds_alternative<msg_attr>(attribute));
    auto database_attribute = std::get<msg_attr>(attribute);
    assert(database_attribute.get_name() == "test attr");
    assert(database_attribute.get_text() == "123.56");
    assert(database_attribute.get_message() == 1234);
}

static void test_signal_string_attribute() {
    char input[]       = "BA_ \"test attr\"   SG_ 1234 SIG_NAME  \"string value\";\n";
    auto attribute_opt = parse(lexy::string_input<lexy::utf8_encoding>(input, sizeof(input)));
    assert(attribute_opt.has_value());
    auto attribute = attribute_opt.value();

    assert(std::holds_alternative<sig_attr>(attribute));
    auto database_attribute = std::get<sig_attr>(attribute);
    assert(database_attribute.get_name() == "test attr");
    assert(database_attribute.get_text() == "string value");
    assert(database_attribute.get_message() == 1234);
    assert(database_attribute.get_signal() == "SIG_NAME");
}

static void test_signal_integer_attribute() {
    char input[]       = "BA_  \"test attr\" SG_   1234 SIG_NAME 123;  \n";
    auto attribute_opt = parse(lexy::string_input<lexy::utf8_encoding>(input, sizeof(input)));
    assert(attribute_opt.has_value());
    auto attribute = attribute_opt.value();

    assert(std::holds_alternative<sig_attr>(attribute));
    auto database_attribute = std::get<sig_attr>(attribute);
    assert(database_attribute.get_name() == "test attr");
    assert(database_attribute.get_text() == "123");
    assert(database_attribute.get_message() == 1234);
    assert(database_attribute.get_signal() == "SIG_NAME");
}

static void test_signal_float_attribute() {
    char input[]       = "BA_   \"test attr\" SG_  1234   SIG_NAME 123.56  ;  \n";
    auto attribute_opt = parse(lexy::string_input<lexy::utf8_encoding>(input, sizeof(input)));
    assert(attribute_opt.has_value());
    auto attribute = attribute_opt.value();

    assert(std::holds_alternative<sig_attr>(attribute));
    auto database_attribute = std::get<sig_attr>(attribute);
    assert(database_attribute.get_name() == "test attr");
    assert(database_attribute.get_text() == "123.56");
    assert(database_attribute.get_message() == 1234);
    assert(database_attribute.get_signal() == "SIG_NAME");
}

int main() {
    test_database_string_attribute();
    test_database_integer_attribute();
    test_database_float_attribute();

    test_node_string_attribute();
    test_node_integer_attribute();
    test_node_float_attribute();

    test_message_string_attribute();
    test_message_integer_attribute();
    test_message_float_attribute();

    test_signal_string_attribute();
    test_signal_integer_attribute();
    test_signal_float_attribute();

    return 0;
}