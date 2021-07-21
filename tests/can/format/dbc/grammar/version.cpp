#include <cassert>
#include <cmath>
#include <string>

#include "lexy/action/parse.hpp"
#include "lexy/callback.hpp"
#include "lexy/input/string_input.hpp"

#include "can/format/dbc/grammar/version.hpp"

static constexpr auto ignore_error = lexy::callback([](const auto& context, const auto& error) {});

static auto parse(auto input) {
    return lexy::parse<can::format::dbc::grammar::version>(input, ignore_error);
}

static void test_empty_version() {
    char input[]     = "VERSION \"\"\n";
    auto version_opt = parse(lexy::string_input<lexy::utf8_encoding>(input, sizeof(input)));
    assert(version_opt.has_value());
    auto version = version_opt.value();

    assert(version.get_value().empty());
}

static void test_alphanumeric_version() {
    char input[]     = "VERSION \"hello world 123\"\n";
    auto version_opt = parse(lexy::string_input<lexy::utf8_encoding>(input, sizeof(input)));
    assert(version_opt.has_value());
    auto version = version_opt.value();

    assert(version.get_value() == "hello world 123");
}

static void test_alphanumeric_version_with_spaces() {
    char input[]     = "VERSION   \"  hello world 123  \"   \n";
    auto version_opt = parse(lexy::string_input<lexy::utf8_encoding>(input, sizeof(input)));
    assert(version_opt.has_value());
    auto version = version_opt.value();

    assert(version.get_value() == "  hello world 123  ");
}

int main() {
    test_empty_version();
    test_alphanumeric_version();
    test_alphanumeric_version_with_spaces();

    return 0;
}