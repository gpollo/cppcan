#include <cassert>
#include <cmath>
#include <string>
#include <variant>

#include "lexy/action/parse.hpp"
#include "lexy/input/string_input.hpp"

#include "can/format/dbc/grammar/value_table_definition.hpp"

static constexpr auto ignore_error = lexy::callback([](const auto& context, const auto& error) {});

static auto parse(auto input) {
    return lexy::parse<can::format::dbc::grammar::value_table_definition>(input, ignore_error);
}

static void test_simple_value_table_definition() {
    char input[] =
        "VAL_TABLE_   FANS "
        "0 \"FORCE_FANS_STATE_FORCE_OFF\" "
        " 1  \"FORCE_FANS_STATE_FORCE_50_PERCENT\" "
        " 2 \"FORCE_FANS_STATE_FORCE_75_PERCENT\" "
        "3   \"FORCE_FANS_STATE_FORCE_100_PERCENT\" ; \n";
    auto value_table_definition_opt = parse(lexy::string_input<lexy::utf8_encoding>(input, sizeof(input)));
    assert(value_table_definition_opt.has_value());
    auto value_table_definition = value_table_definition_opt.value();

    assert(value_table_definition.get_name() == "FANS");
    assert(value_table_definition.get_entries().size() == 4);
    assert(value_table_definition.get_entries()[0].first == 0);
    assert(value_table_definition.get_entries()[0].second == "FORCE_FANS_STATE_FORCE_OFF");
    assert(value_table_definition.get_entries()[1].first == 1);
    assert(value_table_definition.get_entries()[1].second == "FORCE_FANS_STATE_FORCE_50_PERCENT");
    assert(value_table_definition.get_entries()[2].first == 2);
    assert(value_table_definition.get_entries()[2].second == "FORCE_FANS_STATE_FORCE_75_PERCENT");
    assert(value_table_definition.get_entries()[3].first == 3);
    assert(value_table_definition.get_entries()[3].second == "FORCE_FANS_STATE_FORCE_100_PERCENT");
}

int main() {
    test_simple_value_table_definition();

    return 0;
}