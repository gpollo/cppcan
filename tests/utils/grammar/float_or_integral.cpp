#include <cassert>
#include <cmath>
#include <optional>

#include "lexy/input/string_input.hpp"
#include "lexy/parse.hpp"

#include "can/utils/grammar/float_or_integral.hpp"

static constexpr auto ignore_error = lexy::callback([](const auto& context, const auto& error) {});

template <typename T>
static std::optional<bool> test_integral(auto input, T expected = 0) {
    auto result_opt = lexy::parse<can::utils::grammar::float_or_integral>(input, ignore_error);
    if (!result_opt.has_value()) {
        return {};
    }
    auto result = result_opt.value();

    if (!std::holds_alternative<T>(result)) {
        return false;
    }

    return (std::get<T>(result) == expected);
}

template <typename T>
static std::optional<bool> test_float(auto input, T expected = 0.0F) {
    auto result_opt = lexy::parse<can::utils::grammar::float_or_integral>(input, ignore_error);
    if (!result_opt.has_value()) {
        return {};
    }
    auto result = result_opt.value();

    if (!std::holds_alternative<T>(result)) {
        return false;
    }

    constexpr float EPSILON = 0.00001F;
    return (std::fabs(std::get<T>(result) - expected) < EPSILON);
}

int main() {
    assert(test_integral<uint64_t>(lexy::zstring_input("1"), 1).value_or(false));
    assert(test_integral<uint64_t>(lexy::zstring_input("1."), 1).value_or(false));

    assert(test_integral<uint64_t>(lexy::zstring_input("1abc"), 1).value_or(false));
    assert(test_integral<uint64_t>(lexy::zstring_input("1.abc"), 1).value_or(false));

    assert(test_integral<int64_t>(lexy::zstring_input("-1"), -1).value_or(false));
    assert(test_integral<int64_t>(lexy::zstring_input("-1."), -1).value_or(false));

    assert(test_integral<int64_t>(lexy::zstring_input("-1abc"), -1).value_or(false));
    assert(test_integral<int64_t>(lexy::zstring_input("-1.abc"), -1).value_or(false));

    assert(test_float<float>(lexy::zstring_input("-1.23"), -1.23F).value_or(false));
    assert(test_float<float>(lexy::zstring_input("1.23"), 1.23F).value_or(false));

    assert(test_float<float>(lexy::zstring_input("-1.23abc"), -1.23F).value_or(false));
    assert(test_float<float>(lexy::zstring_input("1.23abc"), 1.23F).value_or(false));

    assert(test_integral<uint64_t>(lexy::zstring_input("abc")).value_or(true));
    assert(test_integral<int64_t>(lexy::zstring_input("abc")).value_or(true));
    assert(test_float<float>(lexy::zstring_input("abc")).value_or(true));

    return 0;
}