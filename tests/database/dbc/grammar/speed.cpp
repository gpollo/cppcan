#include <cassert>
#include <cmath>
#include <string>

#include "lexy/callback.hpp"
#include "lexy/input/string_input.hpp"
#include "lexy/parse.hpp"

#include "can/database/dbc/grammar/speed.hpp"

static constexpr auto ignore_error = lexy::callback([](const auto& context, const auto& error) {});

static auto parse(auto input) {
    return lexy::parse<can::database::dbc::grammar::speed>(input, ignore_error);
}

static void test_empty_version() {
    char input[]   = "BS_ :\n";
    auto speed_opt = parse(lexy::string_input<lexy::utf8_encoding>(input, sizeof(input)));
    assert(speed_opt.has_value());
    auto speed = speed_opt.value();

    assert(!speed.has_value());
}

static void test_simple_version() {
    char input[]   = "BS_ :500000\n";
    auto speed_opt = parse(lexy::string_input<lexy::utf8_encoding>(input, sizeof(input)));
    assert(speed_opt.has_value());
    auto speed = speed_opt.value();

    assert(speed.has_value());
    assert(speed.value() == 500000U);
}

int main() {
    test_empty_version();
    test_simple_version();

    return 0;
}