#include <cassert>
#include <cmath>
#include <string>
#include <variant>

#include "lexy/input/string_input.hpp"
#include "lexy/parse.hpp"

#include "can/database/dbc/grammar/attribute_definition_default.hpp"

static constexpr auto ignore_error = lexy::callback([](const auto& context, const auto& error) {});

static auto parse(auto input) {
    return lexy::parse<can::database::dbc::grammar::attribute_definition_default>(input, ignore_error);
}

static void test_string_attribute_definition_default() {
    char input[] = "BA_DEF_DEF_ \"test attr\" \"string value\" ;  \n";

    auto attribute_definition_default_opt = parse(lexy::string_input<lexy::utf8_encoding>(input, sizeof(input)));
    assert(attribute_definition_default_opt.has_value());
    auto attribute_definition_default = attribute_definition_default_opt.value();

    assert(attribute_definition_default.get_name() == "test attr");
    assert(attribute_definition_default.get_text() == "string value");
}

static void test_positive_integer_attribute_definition_default() {
    char input[] = "BA_DEF_DEF_ \"test attr\" 123 ;  \n";

    auto attribute_definition_default_opt = parse(lexy::string_input<lexy::utf8_encoding>(input, sizeof(input)));
    assert(attribute_definition_default_opt.has_value());
    auto attribute_definition_default = attribute_definition_default_opt.value();

    assert(attribute_definition_default.get_name() == "test attr");
    assert(attribute_definition_default.get_text() == "123");
}

static void test_negative_integer_attribute_definition_default() {
    char input[] = "BA_DEF_DEF_ \"test attr\" -123 ;  \n";

    auto attribute_definition_default_opt = parse(lexy::string_input<lexy::utf8_encoding>(input, sizeof(input)));
    assert(attribute_definition_default_opt.has_value());
    auto attribute_definition_default = attribute_definition_default_opt.value();

    assert(attribute_definition_default.get_name() == "test attr");
    assert(attribute_definition_default.get_text() == "-123");
}

static void test_positive_float_attribute_definition_default() {
    char input[] = "BA_DEF_DEF_ \"test attr\" 123.45 ;  \n";

    auto attribute_definition_default_opt = parse(lexy::string_input<lexy::utf8_encoding>(input, sizeof(input)));
    assert(attribute_definition_default_opt.has_value());
    auto attribute_definition_default = attribute_definition_default_opt.value();

    assert(attribute_definition_default.get_name() == "test attr");
    assert(attribute_definition_default.get_text() == "123.45");
}

static void test_negative_float_attribute_definition_default() {
    char input[] = "BA_DEF_DEF_ \"test attr\" -123.45 ;  \n";

    auto attribute_definition_default_opt = parse(lexy::string_input<lexy::utf8_encoding>(input, sizeof(input)));
    assert(attribute_definition_default_opt.has_value());
    auto attribute_definition_default = attribute_definition_default_opt.value();

    assert(attribute_definition_default.get_name() == "test attr");
    assert(attribute_definition_default.get_text() == "-123.45");
}

int main() {
    test_string_attribute_definition_default();
    test_positive_integer_attribute_definition_default();
    test_negative_integer_attribute_definition_default();
    test_positive_float_attribute_definition_default();
    test_negative_float_attribute_definition_default();

    return 0;
}