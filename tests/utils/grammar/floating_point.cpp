#include <cassert>
#include <cmath>
#include <optional>

#include "lexy/action/parse.hpp"
#include "lexy/input/string_input.hpp"

#include "can/utils/grammar/floating_point.hpp"

static constexpr auto ignore_error = lexy::callback([](const auto& context, const auto& error) {});

static std::optional<bool> test_floating_point_parsing(auto input, float expected = 0.0F) {
    auto result = lexy::parse<can::utils::grammar::floating_point>(input, ignore_error);
    if (!result.has_value()) {
        return {};
    }

    constexpr float EPSILON = 0.00001F;
    return (std::fabs(result.value() - expected) < EPSILON);
}

int main() {
    assert(test_floating_point_parsing(lexy::zstring_input("-1"), -1.0F).value_or(false));
    assert(test_floating_point_parsing(lexy::zstring_input("-1."), -1.0F).value_or(false));
    assert(test_floating_point_parsing(lexy::zstring_input("-1.23"), -1.23F).value_or(false));

    assert(test_floating_point_parsing(lexy::zstring_input("1"), 1.0F).value_or(false));
    assert(test_floating_point_parsing(lexy::zstring_input("1."), 1.0F).value_or(false));
    assert(test_floating_point_parsing(lexy::zstring_input("1.23"), 1.23F).value_or(false));

    assert(test_floating_point_parsing(lexy::zstring_input("-12"), -12.0F).value_or(false));
    assert(test_floating_point_parsing(lexy::zstring_input("-12."), -12.0F).value_or(false));
    assert(test_floating_point_parsing(lexy::zstring_input("-12.34"), -12.34F).value_or(false));

    assert(test_floating_point_parsing(lexy::zstring_input("12"), 12.0F).value_or(false));
    assert(test_floating_point_parsing(lexy::zstring_input("12."), 12.0F).value_or(false));
    assert(test_floating_point_parsing(lexy::zstring_input("12.34"), 12.34F).value_or(false));

    assert(!test_floating_point_parsing(lexy::zstring_input("")).has_value());
    assert(!test_floating_point_parsing(lexy::zstring_input("a")).has_value());
    assert(!test_floating_point_parsing(lexy::zstring_input("_")).has_value());
    assert(test_floating_point_parsing(lexy::zstring_input("111a"), 111.0F).value_or(false));

    return 0;
}