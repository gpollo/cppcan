#include <cassert>
#include <optional>
#include <string>

#include "lexy/action/parse.hpp"
#include "lexy/input/string_input.hpp"

#include "can/utils/grammar/c_identifier.hpp"

static constexpr auto ignore_error = lexy::callback([](const auto& context, const auto& error) {});

static std::optional<bool> test_parsing(auto input, const std::string& expected = "") {
    auto result = lexy::parse<can::utils::grammar::c_identifier>(input, ignore_error);
    if (!result.has_value()) {
        return {};
    }

    return (result.value() == expected);
}

int main() {
    /* no capital letters */
    assert(test_parsing(lexy::zstring_input("hello"), "hello").value_or(false));
    assert(test_parsing(lexy::zstring_input("hello_world"), "hello_world").value_or(false));
    assert(test_parsing(lexy::zstring_input("hello_world_12"), "hello_world_12").value_or(false));

    assert(test_parsing(lexy::zstring_input("_hello"), "_hello").value_or(false));
    assert(test_parsing(lexy::zstring_input("_hello_world"), "_hello_world").value_or(false));
    assert(test_parsing(lexy::zstring_input("_hello_world_12"), "_hello_world_12").value_or(false));

    assert(!test_parsing(lexy::zstring_input("1hello")).has_value());
    assert(!test_parsing(lexy::zstring_input("1hello_world")).has_value());
    assert(!test_parsing(lexy::zstring_input("1hello_world_12")).has_value());

    /* all capital letters */
    assert(test_parsing(lexy::zstring_input("HELLO"), "HELLO").value_or(false));
    assert(test_parsing(lexy::zstring_input("HELLO_WORLD"), "HELLO_WORLD").value_or(false));
    assert(test_parsing(lexy::zstring_input("HELLO_WORLD_12"), "HELLO_WORLD_12").value_or(false));

    assert(test_parsing(lexy::zstring_input("_HELLO"), "_HELLO").value_or(false));
    assert(test_parsing(lexy::zstring_input("_HELLO_WORLD"), "_HELLO_WORLD").value_or(false));
    assert(test_parsing(lexy::zstring_input("_HELLO_WORLD_12"), "_HELLO_WORLD_12").value_or(false));

    assert(!test_parsing(lexy::zstring_input("1HELLO")).has_value());
    assert(!test_parsing(lexy::zstring_input("1HELLO_WORLD")).has_value());
    assert(!test_parsing(lexy::zstring_input("1HELLO_WORLD_12")).has_value());

    /* mixed cases */
    assert(test_parsing(lexy::zstring_input("HELlO"), "HELlO").value_or(false));
    assert(test_parsing(lexy::zstring_input("HELlO_WORLD"), "HELlO_WORLD").value_or(false));
    assert(test_parsing(lexy::zstring_input("HELlO_WORLD_12"), "HELlO_WORLD_12").value_or(false));

    assert(test_parsing(lexy::zstring_input("_HELlO"), "_HELlO").value_or(false));
    assert(test_parsing(lexy::zstring_input("_HELlO_WORLD"), "_HELlO_WORLD").value_or(false));
    assert(test_parsing(lexy::zstring_input("_HELlO_WORLD_12"), "_HELlO_WORLD_12").value_or(false));

    assert(!test_parsing(lexy::zstring_input("1HELlO")).has_value());
    assert(!test_parsing(lexy::zstring_input("1HELlO_WORLD")).has_value());
    assert(!test_parsing(lexy::zstring_input("1HELlO_WORLD_12")).has_value());

    return 0;
}