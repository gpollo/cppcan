#include <cassert>
#include <string>

#include "lexy/input/string_input.hpp"
#include "lexy/parse.hpp"

#include "can/database/dbc/grammar/nodes.hpp"

static constexpr auto ignore_error = lexy::callback([](const auto& context, const auto& error) {});

static auto parse(auto input) {
    return lexy::parse<can::database::dbc::grammar::nodes>(input, ignore_error);
}

static void test_simple_nodes_list() {
    char input[]   = "BU_: DBG DRIVER IO MOTOR SENSOR\n";
    auto nodes_opt = parse(lexy::string_input<lexy::utf8_encoding>(input, sizeof(input)));
    assert(nodes_opt.has_value());
    auto nodes = nodes_opt.value();

    assert(nodes.get_values().size() == 5);
    assert(nodes.get_values()[0] == "DBG");
    assert(nodes.get_values()[1] == "DRIVER");
    assert(nodes.get_values()[2] == "IO");
    assert(nodes.get_values()[3] == "MOTOR");
    assert(nodes.get_values()[4] == "SENSOR");
}

static void test_simple_nodes_list_with_spaces() {
    char input[]   = "BU_  :  DBG    DRIVER  IO MOTOR   SENSOR   \n";
    auto nodes_opt = parse(lexy::string_input<lexy::utf8_encoding>(input, sizeof(input)));
    assert(nodes_opt.has_value());
    auto nodes = nodes_opt.value();

    assert(nodes.get_values().size() == 5);
    assert(nodes.get_values()[0] == "DBG");
    assert(nodes.get_values()[1] == "DRIVER");
    assert(nodes.get_values()[2] == "IO");
    assert(nodes.get_values()[3] == "MOTOR");
    assert(nodes.get_values()[4] == "SENSOR");
}

int main() {
    test_simple_nodes_list();
    test_simple_nodes_list_with_spaces();

    return 0;
}