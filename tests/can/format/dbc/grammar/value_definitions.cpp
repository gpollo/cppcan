#include <cassert>
#include <cmath>
#include <string>
#include <variant>

#include "lexy/action/parse.hpp"
#include "lexy/input/string_input.hpp"
#include "lexy_ext/report_error.hpp"

#include "can/format/dbc/grammar/value_definitions.hpp"

static constexpr auto ignore_error = lexy::callback([](const auto& context, const auto& error) {});

static auto parse(auto input) {
    return lexy::parse<can::format::dbc::grammar::value_definitions>(input, lexy_ext::report_error);
}

using namespace can::format::dbc::ast;

static void test_simple_value_definitions() {
    char input[] =
        "VAL_   1234   FORCE_FANS_STATE   "
        "0 \"FORCE_FANS_STATE_FORCE_OFF\" "
        " 1  \"FORCE_FANS_STATE_FORCE_50_PERCENT\" "
        " 2 \"FORCE_FANS_STATE_FORCE_75_PERCENT\" "
        "3   \"FORCE_FANS_STATE_FORCE_100_PERCENT\" ; \n";
    auto value_definitions_opt = parse(lexy::string_input<lexy::utf8_encoding>(input, sizeof(input)));
    assert(value_definitions_opt.has_value());
    auto value_definitions = value_definitions_opt.value();

    assert(value_definitions.get_message() == 1234);
    assert(value_definitions.get_signal() == "FORCE_FANS_STATE");
    assert(std::holds_alternative<value_definitions::entries>(value_definitions.get_table()));
    auto entries = std::get<value_definitions::entries>(value_definitions.get_table());
    assert(entries[0].first == 0);
    assert(entries[0].second == "FORCE_FANS_STATE_FORCE_OFF");
    assert(entries[1].first == 1);
    assert(entries[1].second == "FORCE_FANS_STATE_FORCE_50_PERCENT");
    assert(entries[2].first == 2);
    assert(entries[2].second == "FORCE_FANS_STATE_FORCE_75_PERCENT");
    assert(entries[3].first == 3);
    assert(entries[3].second == "FORCE_FANS_STATE_FORCE_100_PERCENT");
}

static void test_reference_value_definitions() {
    char input[]               = "VAL_   1234   FORCE_FANS_STATE  TABLE_NAME   ; \n";
    auto value_definitions_opt = parse(lexy::string_input<lexy::utf8_encoding>(input, sizeof(input)));
    assert(value_definitions_opt.has_value());
    auto value_definitions = value_definitions_opt.value();

    assert(value_definitions.get_message() == 1234);
    assert(value_definitions.get_signal() == "FORCE_FANS_STATE");
    assert(std::holds_alternative<std::string>(value_definitions.get_table()));
    assert(std::get<std::string>(value_definitions.get_table()) == "TABLE_NAME");
}

int main() {
    test_simple_value_definitions();
    test_reference_value_definitions();

    return 0;
}